---
paths:
  - "src/party_menu.c"
  - "src/swsh_party_menu.c"
  - "src/party_menu_dispatch.c"
  - "include/party_menu*.h"
  - "tools/gen_party_menu_variant.py"
  - "src/item_use.c"
---

# Menu squadra (classico e SwSh)

- **Menu squadra SwSh**: due varianti compilate insieme. `tools/gen_party_menu_variant.py`
  rigenera `include/party_menu_variant.h` e `src/party_menu_dispatch.c`. Lo script è
  idempotente (toglie i prefissi prima di riapplicarli) e verifica che ogni
  sostituzione compaia esattamente una volta.

- **Toggle del follower nel menu squadra**: esisteva solo in `party_menu.c`
  (`MENU_PKMN_FOLLOWER`, `CursorCb_PkmnFollower`, `followerEnable`). In
  `swsh_party_menu.c` non c'era **niente**: svista, non scelta. Portato. Il
  codice di disegno delle voci è identico fra i due file, quindi il port è una
  copia di cinque pezzi: enum (prima di `MENU_FIELD_MOVES`, che indicizza le
  mosse campo come `MENU_FIELD_MOVES + j`), voce in `sCursorOptions`, colore
  nel loop di disegno, append in `SetPartyMonFieldSelectionActions`, e copie
  `static` di `GetFirstLiveMonIndex` e `CursorCb_PkmnFollower` (static, quindi
  nessuna collisione di link con la variante classica).

- **`swsh_party_menu.c`** si è portato dietro roba di Soulgold mai agganciata
  (`Task_ShinGenome` era già lì). Controlla con
  `nm --defined-only build/hns/src/swsh_party_menu.o` prima di scrivere un doppione:
  un simbolo senza prefisso di variante è una collisione di link che aspetta.
- **Caramelle e vitamine applicate DUE volte dal menu SwSh.** Il bug più
  istruttivo trovato finora, perché **compilava**.
  `ExecuteTableBasedItemEffect` in Soulgold ha **sei** parametri, qui ne aveva
  **quattro**: mancavano `modifyStats` e `itemCount`. Chi ha portato
  `swsh_party_menu.c` ha adattato le chiamate **cancellando i due argomenti in
  più**. Il compilatore è contento e il comportamento cambia in silenzio:
  - `ItemUseCB_Medicine` e `ItemUseCB_RareCandy` chiamano la funzione una prima
    volta per decidere «si può usare?». In SG quella è una prova a vuoto
    (`modifyStats = 0`); qui **applicava l'oggetto davvero**. Poi
    `ItemUse_Apply*` lo applicava una seconda volta, e ne veniva tolto **uno
    solo** dalla borsa;
  - il prompt «quanti?» passa `tItemCount` all'applicazione. Senza quel
    parametro ne applicava 1 e ne toglieva N.
  **Il sintomo segnalato era però un altro**: la finestra dei passaggi di
  livello mostrava **+0 su tutte e sei le statistiche**. Motivo: la prima
  applicazione (la "prova") faceva salire di livello, e le due istantanee
  before/after dentro `ItemUse_ApplyExpCandy` venivano prese **entrambe dopo**.
  Con una Caramella Rara lo stesso bug si vedeva invece come **+2 livelli**.
  **Il menu classico non è mai stato toccato**: `party_menu.c` non ha né la
  prova né il flusso multi-oggetto, chiama una volta sola con le istantanee
  attorno. Solo `swsh_party_menu.c` era rotto.
  Attenzione a cosa `modifyStats` sospende davvero: **non** è un dry run
  generale. HP, PP e status vengono applicati dalla prova e non sono mai
  riapplicati — è per questo che la seconda chiamata nel menu esiste solo per
  EV e caramelle. Rimandati sono solo EXP/livello, EV e affetto.
  Verificato in emulatore sul salvataggio dell'utente: Caramella Rara su
  Larvitar Lv20 → **Lv21** (non 22), differenze reali (+2 HP, +2 ATT, +1 DIF,
  +0 ATT.SP, +1 DIF.SP, +2 VEL); 3 Caramelle Esp. XS → esp. da 11 576 a
  **11 876**, e una quarta singola → **11 976**. Prima ne sarebbero arrivati
  100 in tutti e tre i casi.
  **La regola generale, che vale per ogni prossimo port:** quando porti un file
  da Soulgold, **confronta le firme delle funzioni che chiama**, non solo il
  file. Un argomento in meno compila; una feature in meno no.
