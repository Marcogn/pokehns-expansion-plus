---
paths:
  - "data/scripts/reds_path.inc"
  - "src/battle_partner.c"
  - "include/battle_partner.h"
  - "include/constants/battle_partner.h"
  - "src/data/battle_partners.party"
  - "src/reds_path.c"
  - "include/constants/reds_path.h"
  - "data/maps/*_Gym_hns/**"
---

# Red's Path (piano in `docs/plans/reds-path.md`)

Il giocatore rigioca le otto palestre di Kanto con la squadra di Red del
momento, contro i capi di FRLG. Sessioni A1, A2 e B fatte; resta la C
(taratura). **Le prove in emulatore (A2 e B) e le motivazioni lunghe stanno
nell'appendice del piano.** Le lotte partono dagli aiutanti delle palestre o
dal debug (`Red's Path… → N`, senza scena).

## Pezzi e dove stanno

| Cosa | Dove |
| --- | --- |
| otto capi, id 653-660 | `TRAINER_REDS_PATH_*_HNS` (`opponents_hns.h`), squadre in `trainers_hns.party`, `AI: Smart Trainer` (senza Prediction) |
| otto squadre di Red | `PARTNER_RED_*` in `battle_partners.party`, negli slot 0-5 via `FillRedsPathParty` |
| la lotta | `RedsPath_EventScript_Battle` (`reds_path.inc`), `VAR_0x8004` = sfida 0-7 |
| ripristino squadra | `special RedsPath_RestorePlayerParty` (`LoadPlayerParty` + `UpdateFollowingPokemon`) |
| "lotta in corso" | `FLAG_REDS_PATH_BATTLE` (ex `FLAG_UNUSED_EXTENDED_86`), `IsRedsPathBattle()` |
| "è un capo" | `IsRedsPathTrainer(id)`: level scaling e TcloneR |
| "sfida vinta" | flag "allenatore battuto" del capo, acceso solo alla vittoria |
| premio ritirato, introduzione | `FLAG_REDS_PATH_PRIZE_*`, `FLAG_REDS_PATH_INTRO_SEEN` (ex `EXTENDED_87..95`, migrazione 11) |
| aiutante, scena, premi | `src/reds_path.c` (tabella `sGyms`), `reds_path.inc`, `constants/reds_path.h` |

## Agganci di lotta, tutti dietro `IsRedsPathBattle()`

Dorso di Red FRLG (`PlayerGetTrainerBackPicId`), mugshot (`Mugshot_SetGfx` e lo
sprite in `battle_transition.c`), niente fuga (`CanPlayerForfeitNormalTrainerBattle`),
niente esperienza (`Cmd_getexp`; senza esperienza niente Candy Jar), niente
evoluzioni (`TryEvolvePokemon`), niente premio in soldi (`jumpifredspathbattle`
in `BattleScript_LocalBattleWonReward`), RED nei testi (`BattleStringGetPlayerName`,
`GetRedsPathBattleString`). **`B_FLAG_NO_WHITEOUT` è `FLAG_REDS_PATH_BATTLE`**
(`config/battle.h`, solo HnS): niente bit nuovo non dimostrabile a zero nei
salvataggi vecchi. Prezzo: un futuro `setflag B_FLAG_NO_WHITEOUT` spegnerebbe
anche randomizer e sfide; allora separarli.

- **Sconfitta**: `CB2_EndTrainerBattle` torna allo script senza il flag del
  capo; lo script dopo `trainerbattle_no_intro` riprende sia dopo vittoria sia
  dopo sconfitta, e la B li distingue con il flag del capo.
- **Evoluzioni**: `EVO_MODE_BATTLE_SPECIAL` evolve anche senza esperienza e
  segna la specie come catturata nel Pokédex: per questo il giro si salta.
- **RED nei testi**: tre righe ("You are challenged by…", "You used…", "You
  defeated…") sono scambiate per puntatore da `GetRedsPathBattleString`; un
  testo nuovo con "You" che compaia in queste lotte va aggiunto lì. Le domande
  restano in seconda persona. La cornice è un **visore VR**: niente viaggio
  nel tempo nei testi.
- OT dei Pokémon di Red `RED`, ID dal nome (`GetPartnerOtId`); con meno di 8
  medaglie i livelli alti disobbediscono (solo da debug). Randomizer, sfide
  del challenge menu e Nuzlocke esclusi.

## Trappole

- **Il follower**: `ReturnToFieldLocal` chiama `UpdateFollowingPokemon` con
  ancora la squadra di Red (sconfitta: tutti KO, follower tolto). `LoadPlayerParty`
  da solo non lo aggiorna: per questo `RedsPath_RestorePlayerParty`.
- **Non cambiare la grafica del giocatore nella scena**:
  `SetPlayerAvatarObjectEventIdAndObjectId` la rimette (e una giocatrice
  diventa maschio). Red è un **oggetto a parte** (`LOCALID_REDS_PATH_RED` 241);
  Janine e Blue prendono lo sprite di Koga e Giovanni.
- **Il capo va cercato dopo lo spostamento** (dalla porta è fuori schermo,
  e un oggetto fuori vista non esiste); `FreezeObjectEvents()` a fine
  `RedsPath_EnterScene`, o chi entra in vista non è congelato dal `lockall`.
- **Script delle righe d'innesco**: expansion li prova "a secco"
  (`RunScriptImmediatelyUntilEffect`); uno special non dichiarato fa partire
  lo script vero e, se finisce con `end` senza `releaseall`, il giocatore
  resta bloccato. `RedsPath_IsGymLeaderBeaten` è `requests_effects=TRUE`.
- `msgbox` scrive `VAR_RESULT`: salva l'esito del premio prima del messaggio.
  `CreateMon` non dà mosse: `GiveMonInitialMoveset`.
- I cammini dell'aiutante sono in RAM (`gRedsPathAideApproach`) con
  `applymovement` (non `applymovement2`: il follower rientra, voluto).
- **Seafoam**: si esce da una scala e sopra c'è un muro; innesco solo a
  (19,22) e (20,22), dal lato dell'aiutante. OT dei premi di 7 caratteri
  (`SURGE`, `GIOVANN`).
- Stati dell'aiutante: `RedsPath_GetAideState`; "Beat the LEADER there first!"
  se il capo di HnS della prossima sfida non è battuto.
