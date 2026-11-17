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

## Lancio (`src/overworld_catch.c`, fasi 3-4)

- **Tasti**: R = `OverworldCatch_TryThrow` (`ProcessPlayerFieldInput`; in bici
  Mach/Acro). R ricordato 16 frame (`sRPressBuffer`), **mai a controlli
  bloccati**. L = `OverworldCatch_HandleLButton` da `DoCB1_Overworld` (toglie
  il D-pad finché è tenuto); L tenuto attraverso un blocco conta come usato.
  Con `L=A` main.c **aggiunge** A ma lascia L: l'opzione va letta.
- **Un lancio è un record** (`sCatch.throws`, 3) con stato e timer: `FLYING`
  (8 + 8 a casella) → `OPENING` 16 (Pokémon nascosto dal frame 4, il raggio)
  → `CLOSING` 8 (cattura: qui l'OWE è tolto) → `FALLING` 8 → `SHAKING` (16 +
  32 a scossa, 0-3 da `RollShakes`) → `CAUGHT` 40 / `BREAKOUT` 12 (l'OWE esce,
  poi `ForceOWEToNoticePlayer`). A vuoto: `FLYING` → `FALLING` → `DROPPED`
  24, poi la Ball torna. **Esito deciso in `ResolveImpact`** (D15), il resto
  è animazione. Timer fermi a controlli bloccati. **Start bloccato finché un
  record non è libero** (`OverworldCatch_IsBallOut`): la Ball è fuori borsa.
- **Bersaglio**: prima cosa sulla linea entro 5. Collisione e oggetti
  visibili non-OWE fermano la Ball (cade e torna); il follower davanti: nessun
  lancio; niente sulla linea: vola 5 e torna. Acqua: niente collisione. Un
  bersaglio che si muove è inseguito (fine dell'arco = suo sprite).
- **Sprite**: fogli delle Ball dei follower (`gPokeballGraphics`, frame 0-4,
  Poké Ball = `OBJ_EVENT_GFX_POKE_BALL`), `coordOffsetEnabled`, posizioni dagli
  sprite (`GetObjectGroundPos`). **Mai un assert**: senza sprite il volo
  prosegue invisibile e dall'impatto `FinishThrow` chiude subito con l'esito.
  `IsBallSpriteValid` (callback + indice + magic) basta per i campi
  ricostruiti (menu, warp): niente aggancio sul reset degli sprite.
- **Pokémon nella Ball** (dall'impatto, `holdsOWE`): congelato e, dal raggio,
  `invisible`, **riasserito a ogni frame** (`KeepOWEInBall`): il `release` di
  uno script scongela tutti. Ha il flag "non sparire" (`HoldOWEForThrow`,
  rimesso com'era all'uscita): un ricambio della stessa specie riusa slot e
  `localId` e passerebbe per lui. Non è bersaglio (`FindThrowTarget`), né
  contatto (`TryTriggerOverworldWildEncounter`), e **una seconda Ball in volo
  verso di lui diventa un lancio a vuoto**: senza, il Pokémon veniva dato due
  volte. Lo sprite da nuoto legge `owe->invisible`.
- **Lotta che parte** (`CreateBattleStartTask` e `_Debug` → `OverworldCatch_OnBattleStart`):
  le Ball a terra finiscono prima della transizione (chi è dentro esce), quelle
  in volo restano e atterrano dopo, invisibili fino all'impatto.
- **Posa**: frame della corsa fermo, tenuto da un'azione `DELAY_16` sul
  giocatore (senza, `PlayerFaceDirection` la rifà ogni frame). **Trappola**:
  `ObjectEventIsMovementOverridden` dà occupato anche un'azione finita e non
  tolta (il giocatore fermo ne ha sempre una): `ClearHeldMovementIfFinished`
  prima di `ObjectEventSetHeldMovement`.
- **Il Pokémon nasce sullo stack** (`CreateMonFromOWE`, comune con la lotta),
  **mai in `gEnemyParty`**: lo spawn lo usa nello stesso frame.
- **Formula**: `ComputeCaptureOddsFromParams` (`battle_script_commands.c`).
  ×2 se `!IsOWEAwareOfPlayer`; Quick Ball solo se non allertato; Timer mai.
  Sfida: `GetCaptureChallengeChecks`. **Nuzlocke: solo una cattura riuscita
  segna la zona** (utente, 17/11, P3).
- **Fallito**: `ForceOWEToNoticePlayer`, **mai su un OWE già allertato o
  "vaga"** (il bit resterebbe). Il "vaga" non reagisce e tiene il ×2.
- **Coda** (`results`, 3): un lancio parte se c'è un record libero e Ball in
  volo + catture in coda < 3. Un lancio a vuoto senza sprite finisce subito. Banner e soprannomi da `OverworldCatch_TryStartResults` quando tutti i
  record sono liberi, **dopo** la gestione del passo. Palette dell'icona
  liberata solo se nessuno la usa. Dopo un `callnative` che ferma lo script
  serve `waitstate`.
- **Pokémon lanciato**: `gOverworldThrowLeadSlot` (letto e azzerato da
  `SetBattlePartyIds`). Roamer e sciami: R avvia la lotta. Spento in Safari,
  Gara, Pike, Piramide, Trainer Hill.

## Prove

Ricette in §5.2-5.4 del piano e `docs/claude/emulator.md`. APCS: struct
arrotondate a 4 byte, `PendingThrow` = 32 (modalità a `sCatch+136` bit 0).
`gSaveblock3 + 0x13` = `autoRun`; `FLAG_SYS_B_DASH` = byte `0x118` bit 0 dei
flag di SaveBlock1. Un OWE col bit 7 di `+0x1D` non viene ricambiato.
