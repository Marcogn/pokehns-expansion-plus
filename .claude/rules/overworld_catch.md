---
paths:
  - "src/field_player_avatar.c"
  - "src/wild_encounter_ow.c"
  - "include/wild_encounter_ow.h"
  - "src/data/pokemon/wild_encounter_ow_behavior.h"
  - "src/dexnav.c"
---

# Cattura nell'overworld: andature e percezione

Piano, decisioni e diario delle verifiche:
[`docs/plans/overworld-catch.md`](../../docs/plans/overworld-catch.md) (§5.2
per la fase 2). Gli OWE in generale: `overworld_encounters.md`.

## Andature (`PlayerNotOnBikeMoving`, `src/field_player_avatar.c`)

- **`autoRun` 0 = acceso** (`sDesc_Autorun`: la prima voce è "Run without
  pressing B"). Fino a PLUS.11 una partita nuova partiva **spento** (`= 1`
  in `SetDefaultChallengeSettings`); dal 16/11 parte acceso (`= 0`, scelta
  dell'utente). Quella funzione gira solo a salvataggio vuoto o corrotto:
  i salvataggi esistenti tengono il loro valore, e chi li ha spenti non ha
  il passo lento (B corre, P20).
- Passo lento = `PlayerWalkSlow` + `gPlayerAvatar.creeping`: B tenuto con
  AUTORUN acceso, oppure B tenuto durante una ricerca DexNav (a terra e in
  Surf) qualunque sia AUTORUN. **A non fa più il creeping.** Va provato
  **prima** del blocco della corsa, che fa sempre `return` con AUTORUN acceso.
- In Surf le velocità non cambiano (B sceglie l'altra velocità); il Surf
  conta come camminata per l'udito (non alza `PLAYER_AVATAR_FLAG_DASH`).
- `PLAYER_AVATAR_FLAG_DASH` resta acceso per tutto il passo di corsa: lo
  spegne `npc_clear_strange_bits` solo al confine fra due passi, e la corsa
  lo riaccende nello stesso frame.

## Percezione (`CanAwareOWESeePlayer`, `src/wild_encounter_ow.c`)

- **Udito**, in ogni direzione, solo con `runningState == MOVING`: corsa o bici
  entro `activeDistance` (5), camminata entro `OWE_HEARING_WALK` (2), passo
  lento mai. Metrica di `IsPlayerWithinOWEDistance` (quadrato tagliato a
  `|dx|+|dy| <= d*362>>8`). Il suono passa gli ostacoli.
- **Vista**: cono davanti, metà larghezza 0 a distanza 1, 1 a 2-3, 2 da 4,
  limitata da `(viewWidth-1)/2`, profondità `viewDistance`. Si ferma su una
  casella con collisione (`MapGridGetCollisionAt`) o su un oggetto che non è
  un OWE né il follower (gli NPC e i Pokémon decorativi coprono), lungo la
  retta arrotondata lontano da zero. Giocatore in erba alta o lunga
  (`MetatileBehavior_IsTallGrass/IsLongGrass`): visto solo dalla casella
  subito davanti. Il dislivello resta (`CanOWEReachPlayer`).
- **Invariante: `viewDistance <= activeDistance`.** Con `returnToIdle =
  PLAYER_OUTSIDE_ACTIVE_RANGE` un Pokémon che ti vede oltre l'`activeDistance`
  torna a riposo al passo dopo e ti rinota: "!" in loop. L'angolo lontano del
  cono (5 avanti, 2 di lato) vale 7 = la diagonale di 5.
- **`IsOWEAwareOfPlayer()`** = OWE con `OWE_SAVED_MOVEMENT_STATE_FLAG`
  (bit 7 di `warpArrowSpriteId`): alzato in `Common_Step7` quando nota,
  tolto al ritorno a riposo, sopravvive al ricaricamento della mappa. Il
  tipo "vaga" (`WANDER_AROUND_OWE`) non nota mai.
- Il controllo gira solo in `WanderAround_Step3` (attesa fra due passi), ogni
  frame: un OWE mentre cammina non nota.

## Prove

ROM strumentata e ricetta GDB in §5.2 del piano. Da GDB: `gSaveblock3 + 0x13`
= `autoRun` (bitfield prima: l'`offsetof` compilato a mano dà 0x14, sbagliato),
`FLAG_SYS_B_DASH` = byte `0x118` bit 0 dei flag di SaveBlock1 (`+0x198C`),
dopo "Cheat start" del debug leggeva 0 (16/11): accendilo a mano. Un OWE col bit 7 di
`trainerRange_berryTreeId` (+0x1D) non viene ricambiato.
