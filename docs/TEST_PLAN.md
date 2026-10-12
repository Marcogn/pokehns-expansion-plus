# Piano: far girare i test che servono a Hasep

Stato: **da fare**. Misure del 12/10/2026 su `668cfc6d`.

## Da dove si parte

`make check` non è mai passato, e non per colpa di Hasep:

- **In HnS upstream i test non compilano.** `test/test_runner.c` include
  `fake_rtc.h` prima di `global.h`, e `fake_rtc.h` (modificato da HnS per
  `tx_Features_RTCType`) usa `gSaveBlock3Ptr` senza includere la struct.
  Entrambi i file sono identici a `PokemonHnS-Development/pokehns-expansion`
  master (`167aa6d`), la cui CI lancia `make check` sul target Emerald, che
  fallisce già prima (CLAUDE.md §8). Gli autori originali non li hanno mai
  fatti passare.
- **In Hasep c'è un errore in più, nostro:** `test/overworld_speedup.c` chiama
  `OverworldSpeedup_AdditionalIterations(speed, overworld)`, ma da `cf6c286b`
  (R al DexNav) la funzione ha un solo parametro, e il test "Holding R drops
  the overworld back to 1x" descrive un comportamento che non esiste più.
- `test/save.c` fissa dimensioni del salvataggio non più vere (§8).

Con le due correzioni di compilazione applicate a mano, la suite completa
(`make check BUILD=hns`) gira: in 30 minuti su 4 core, prima di essere
interrotta, **3 849 risultati, 1 856 PASS, ~1 480 FAIL**, 168 TODO, 12
INVALID, 1 CRASH. Quasi tutti i fallimenti sono test di lotta.

Esempio analizzato: "Will-O-Wisp burns target" fallisce con `Unmatched
MESSAGE` a `test/battle/status1/burn.c:56`. Il testo del messaggio nel codice
è identico all'atteso, quindi la causa è nel flusso della lotta di HnS (o in
una sua configurazione), non nella stringa. **Non ancora trovata.**

## Cosa interessa davvero

| gruppo | file | test (circa) | tocca Hasep? |
| --- | --- | --- | --- |
| non di lotta | `test/*.c` | 145 | **sì**: salvataggi, Pokémon, daycare (uova shiny), borsa, overworld, testo, random |
| lotta | `test/battle/**` | 4 900 | in parte: meccaniche di expansion, che Hasep tocca poco (Court Change, Trainer Catch) |
| compressione | `test/compression/**` | — | no |

## Piano

1. **Compilare.** `#include "global.h"` in `include/fake_rtc.h`; aggiornare
   `test/overworld_speedup.c` alla firma attuale e togliere il test su R;
   sistemare `test/save.c` alle dimensioni vere, ricavate dagli
   `STATIC_ASSERT` di `src/save.c` (non copiate a mano).
2. **Solo i test non di lotta.** Il Makefile prende i test da
   `TEST_SRCS_IN := $(wildcard test/*.c test/*/*.c test/*/*/*.c)`; una
   variabile (es. `TEST_SCOPE=core`) che lo restringa a `test/*.c`. Da
   verificare che il runner di lotta (`test/test_runner_battle.c`) linki
   senza i test di lotta. Obiettivo: **tutti verdi**, e ogni fallimento
   capito (meccanica cambiata apposta → si aggiorna il test; bug → si
   corregge nel gioco, in una PR a parte).
3. **CI.** Il sottoinsieme non di lotta in `build.yml` a ogni PR, se dura
   pochi minuti (da misurare). La suite di lotta solo a mano
   (`workflow_dispatch`), mai a ogni PR: dura più di 30 minuti.
4. **Lotta, se e quando servirà.** Raggruppare i fallimenti per causa (uno
   per gruppo analizzato a fondo: probabilmente poche cause comuni, non 1 500
   bug). Non prima che serva: per ora nemmeno upstream lo fa.

Regola che vale per tutto il piano: **nessun test va disattivato o saltato
per far tornare il verde.** Un test che non ha più senso (come quello su R)
si toglie con il commit che spiega perché.
