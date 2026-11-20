---
paths:
  - "src/move_relearner.c"
  - "src/pokemon_summary_screen*.c"
  - "include/config/pokemon.h"
  - "src/data/pokemon/level_up_learnsets/**"
---

# Ricorda mosse e learnset

- **Ricorda mosse dal riassunto**: è una feature di expansion 1.15.2
  (`P_ENABLE_MOVE_RELEARNERS`), non di Soulgold. Hasep ne aveva però solo metà:
  `ShowRelearnPrompt()` disegnava il prompt e **nessuno gestiva START**.
  **Egg e tutor a pagamento (fase 7 di `overworld-catch`, 20/11)**:
  `P_ENABLE_MOVE_RELEARNERS` **TRUE** come in SG (verificato su `310bf6a9`: SG la
  tiene TRUE e chiude egg/tutor con i suoi `AreEggMovesUnlocked/AreTutorMovesUnlocked`;
  le MT restano libere, l'ho letto sul suo `CheckRelearnerStateFlag`). Con FALSE la
  finestra del tag (`tilemapLeft` 22 invece di 18) esce dallo schermo: "TM START"
  tagliato in "STAR", visto in emulatore, non da codice. Qui egg/tutor sono
  `P_FLAG_EGG_MOVES` = `FLAG_EGG_MOVES_UNLOCKED` (0x304, ex `FLAG_UNUSED_36`,
  `flags_hns.h`) e `P_FLAG_TUTOR_MOVES` = `FLAG_TUTOR_MOVES_UNLOCKED` (0x305),
  controllati **da soli** (senza `|| P_ENABLE`) in `move_relearner.c`, in
  `CheckRelearnerStateFlag` e in `move_relearner.inc`. Zero nei salvataggi vecchi
  = chiusi. Ordine del ciclo come SG: LEVEL, TUTOR, EGG, TM.
  `include/config/summary_screen.h` è incluso **prima** di `IS_HNS`: la guardia è
  `#ifdef POKEMON_HNS`. Maestri, script e testi di SG: Egg Move Master in
  `BlackthornCity_House3_hns` (NPC nuovo a (2,6)), Tutor Move Master in
  `OlivineCity_House3_hns` (NPC nuovo a (7,4): SG ha una House4 che qui non
  esiste). Prezzi (utente, 20/11): metà di SG, 44.444 / 22.222, metà ancora con
  `FLAG_IS_CHAMPION` (Lance battuto), 22.222 / 11.111. `checkmoney` vuole una
  costante: due rami, prezzo nel testo con `bufferstring`. Pagano e
  alzano il flag; la scelta della mossa resta nel riassunto. Il ricorda mosse a
  Squama Cuore di Blackthorn non c'entra (solo mosse di livello).
  Prova (guarda anche il tag intero, non solo "LEVEL"): pagina mosse, L/R sul tag "LEVEL": prima del pagamento il ciclo è
  LEVEL↔TM, dopo EGG si aggiunge, dopo TUTOR pure. Il sorgente di SG non era
  accessibile in quella sessione: posizioni, prezzi e testi dei maestri sono
  ricavati dal piano e dalle note, non copiati.
  Soulgold apre il relearner **solo dalla pagina Battle Moves** e forza
  `showContestInfo` a FALSE.
  Attenzione prima di gridare al bug sulla categoria LEVEL: `P_LVL_UP_LEARNSETS`
  qui è **GEN_7**, e con "modern moves" attivo il gioco usa quel learnset, non
  il gen 3 di `gLevelUpLearnsets_Gen3`. Le due tabelle non coincidono: Cyndaquil
  impara SMOKESCREEN a 6 in gen 1-6/8-9 ma EMBER a **8** in gen 7. Un Cyndaquil
  Lv6 che conosce solo Tackle e Leer quindi non ha davvero niente da ricordare,
  e `HasRelearnerLevelUpMoves` risponde giusto. Verificato con una build
  strumentata che stampava cache, specie, numero di voci del learnset e livello
  (`0101 0 155 18 1 6`): 18 voci = gen 7.

- **Learnset e relearner, da dove pesca**: `GetSpeciesLevelUpLearnset()` sceglie
  fra `gLevelUpLearnsets_Gen3` e `gSpeciesInfo[].levelUpLearnset` (gen 7) in base
  a `tx_Mode_Modern_Moves`, che si imposta **solo a inizio partita** — il
  challenge menu è raggiungibile unicamente da `oak_speech_hns.c`. Creazione e
  relearner passano entrambi da lì, senza bypass: dentro una stessa partita non
  possono divergere. Verificato in emulatore: un Pidgey Lv7 creato ora nasce con
  TACKLE + PECK + SAND ATTACK, cioè il gen 7.
  Da ricordare: **le mosse di livello 1 arrivano solo alla creazione**.
  `MonTryLearningNewMoveAtLevel` cerca solo entry con `level ==` il livello
  appena guadagnato, quindi un livello 1 non è mai raggiungibile salendo. E
  `GiveBoxMonInitialMoveset` tiene le **ultime quattro** mosse disponibili al
  livello di cattura, scartando le prime: per un selvatico di livello alto è
  normale che il relearner offra mosse che non ha mai avuto.
