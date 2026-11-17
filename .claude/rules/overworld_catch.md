---
paths:
  - "src/field_player_avatar.c"
  - "src/wild_encounter_ow.c"
  - "include/wild_encounter_ow.h"
  - "src/data/pokemon/wild_encounter_ow_behavior.h"
  - "src/dexnav.c"
  - "src/overworld_catch.c"
  - "include/overworld_catch.h"
  - "data/scripts/overworld_catch.inc"
  - "src/field_control_avatar.c"
---

# Cattura nell'overworld: andature, percezione, lancio

Piano, decisioni e diario delle verifiche:
[`docs/plans/overworld-catch.md`](../../docs/plans/overworld-catch.md) (§5.2
per la fase 2). Gli OWE in generale: `overworld_encounters.md`.

## Andature (`PlayerNotOnBikeMoving`, `src/field_player_avatar.c`)

- **`autoRun` 0 = acceso** (`sDesc_Autorun`: la prima voce è "Run without
  pressing B"). Fino a PLUS.11 una partita nuova partiva **spento** (`= 1`
  in `SetDefaultChallengeSettings`); dal 16/11 parte acceso (`= 0`, scelta
  dell'utente). Quella funzione gira solo a salvataggio vuoto o corrotto:
  i salvataggi esistenti tengono il loro valore, **anche una New Game sopra
  un salvataggio** (le opzioni del titolo si ricopiano, `oak_speech_hns.c`),
  e chi li ha spenti non ha il passo lento (B corre, P20).
- Passo lento = `PlayerWalkSlow` + `gPlayerAvatar.creeping`: B tenuto con
  AUTORUN acceso, oppure B tenuto durante una ricerca DexNav (a terra e in
  Surf) qualunque sia AUTORUN. **A non fa più il creeping.** Va provato
  **prima** del blocco della corsa, che fa sempre `return` con AUTORUN acceso.
- Il salto da una sporgenza spegne `creeping` (esce prima del reset comune):
  senza, un salto dopo il passo lento era silenzioso.
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
  un OWE né il follower né `invisible` (NPC e Pokémon decorativi coprono),
  lungo la retta arrotondata lontano da zero. Giocatore in erba alta o lunga
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

## Lancio (`src/overworld_catch.c`, fase 3: senza animazione)

- **Tasti**: R = `OverworldCatch_TryThrow` (`ProcessPlayerFieldInput`; in bici
  R resta Mach/Acro). R è ricordato 16 frame (`sRPressBuffer`): premuto a metà
  passo parte a fine passo; **mai a controlli bloccati** (un R dato a un
  messaggio tornerebbe come lancio). Uno stato di L tenuto attraverso un
  blocco conta come già usato (`OverworldCatch_Update`). L = `OverworldCatch_HandleLButton` da
  `DoCB1_Overworld`, che toglie il D-pad ai tasti finché L è tenuto. Con
  `L=A` main.c **aggiunge** A ma lascia L nei tasti: l'opzione va letta.
- **Un lancio è un record** (`sCatch.throws`, 3): `FLYING` col timer
  (8 + 8 a casella), poi `SETTLING` 60 frame (il posto delle scosse della
  fase 4). La Ball esce dalla borsa al lancio e **torna** (`ReturnBall`) se
  il bersaglio non c'è più o la cattura è bloccata all'impatto. Tutto si
  decide in `ResolveImpact`. **I timer sono fermi a controlli bloccati**:
  uno script può essere la lotta con quel Pokémon. **Niente menu Start con
  una Ball in volo** (`OverworldCatch_IsBallInFlight`): la Ball è già fuori
  dalla borsa e un salvataggio la perderebbe.
- **Bersaglio**: prima cosa sulla linea entro 5 caselle. Fermano il lancio
  (niente Ball usata) collisione di mappa e ogni oggetto visibile che non è
  un OWE generato: NPC, decorativi, follower, OWE di mappa. L'acqua non ha
  il bit di collisione: non ferma.
- **Il Pokémon nasce sullo stack** con `CreateMonFromOWE` (la stessa della
  lotta), **mai in `gEnemyParty`**: lo spawn lo usa nello stesso frame.
- **Formula**: `ComputeCaptureOddsFromParams(struct CaptureParams)` in
  `battle_script_commands.c`; la lotta la riempie dai battler, il lancio da
  specie e squadra. ×2 se `!IsOWEAwareOfPlayer`; Quick Ball solo se non
  allertato (`quickBallSpent`); Timer mai. Regole di sfida:
  `GetCaptureChallengeChecks` (`nuzlocke.c`), senza globali. **Nuzlocke:
  solo una cattura riuscita segna la zona, un lancio fallito no** (scelta
  dell'utente, 17/11, P3), anche se in lotta ogni esito la segna.
- **Fallito**: `ForceOWEToNoticePlayer` alza un bit per slot letto da
  `CanAwareOWESeePlayer`; lo toglie `SetSavedOWEMovementState`. **Mai su un
  OWE già allertato o "vaga"**: il bit resterebbe (visto in emulatore) e lo
  rifarebbe notare appena calmo. Il "vaga" non reagisce e tiene il ×2.
- **Coda delle catture** (`results`, 3): un lancio parte se Ball in volo +
  catture in coda < 3 (i record `SETTLING` non contano: sono già in coda o
  falliti). Banner e soprannomi partono da `OverworldCatch_TryStartResults`
  quando non resta nessun record, **dopo** la gestione del passo in
  `ProcessPlayerFieldInput` (prima, un `return TRUE` saltava contapassi e
  script della casella); sopravvivono a una lotta. La palette dell'icona
  si libera solo se nessun altro sprite la usa (6 tag condivisi).
- **`callnative` + `ScriptContext_Stop()` non ferma i comandi del frame**:
  lo script proseguiva fino a `end` sotto il banner. Dopo il `callnative`
  serve un `waitstate` nello script.
- **Pokémon lanciato**: `gOverworldThrowLeadSlot`, letto e azzerato da
  `SetBattlePartyIds` (solo lotte selvatiche): cambia l'indice del primo
  battler, non l'ordine della squadra. 0 = comportamento di sempre.
- Roamer e sciami (`IsOWEBattleOnly`): R avvia la lotta, niente Ball.
  Spento (messaggio) in Safari, Gara, Pike, Piramide, Trainer Hill.
- Le struct in EWRAM sono arrotondate a 4 byte (APCS): `PendingThrow` è 12,
  non 10, quando si legge `sCatch` da GDB.

## Prove

ROM strumentata e ricetta GDB in §5.2 del piano. Da GDB: `gSaveblock3 + 0x13`
= `autoRun` (bitfield prima: l'`offsetof` compilato a mano dà 0x14, sbagliato),
`FLAG_SYS_B_DASH` = byte `0x118` bit 0 dei flag di SaveBlock1 (`+0x198C`),
dopo "Cheat start" del debug leggeva 0 (16/11): accendilo a mano. Un OWE col bit 7 di
`trainerRange_berryTreeId` (+0x1D) non viene ricambiato.
