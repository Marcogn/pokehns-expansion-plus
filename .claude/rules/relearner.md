---
paths:
  - "src/move_relearner.c"
  - "src/pokemon_summary_screen*.c"
  - "include/config/pokemon.h"
  - "src/data/pokemon/level_up_learnsets/**"
  - "src/pokemon.c"
---

# Ricorda mosse e learnset

- **Ricorda mosse dal riassunto**: è una feature di expansion 1.15.2
  (`P_ENABLE_MOVE_RELEARNERS`), non di Soulgold. Hasep ne aveva però solo metà:
  `ShowRelearnPrompt()` disegnava il prompt e **nessuno gestiva START**.
  Come lo configura Soulgold, verificato: `P_TM_MOVES_RELEARNER` **FALSE** (MT
  mai disponibili), mosse uovo dietro `FLAG_EGG_MOVES_UNLOCKED` (Egg Move Master,
  Blackthorn City House 3, ¥88.888) e tutor dietro `FLAG_TUTOR_MOVES_UNLOCKED`
  (Tutor Move Master, Olivine City House 4, ¥44.444). È per questo che lì un
  Pokémon di livello basso non vede mosse fuori scala: qui quei due cancelli non
  ci sono per scelta.
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
