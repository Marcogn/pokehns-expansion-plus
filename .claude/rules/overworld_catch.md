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

- **`autoRun` 0 = acceso** (prima voce di `sDesc_Autorun`). Partita nuova
  acceso dal 16/11 (`SetDefaultChallengeSettings`, solo a salvataggio vuoto o
  corrotto): i salvataggi tengono il loro valore, **anche una New Game sopra
  un salvataggio**; spento = niente passo lento (B corre, P20).
- Passo lento = `PlayerWalkSlow` + `gPlayerAvatar.creeping`: B tenuto con
  AUTORUN acceso, oppure B tenuto durante una ricerca DexNav (a terra e in
  Surf) qualunque sia AUTORUN. **A non fa più il creeping.** Va provato
  **prima** del blocco della corsa, che fa sempre `return` con AUTORUN acceso.
- Il salto da una sporgenza spegne `creeping` (se no il salto era muto).
- In Surf: velocità uguali, udito da camminata.
- `PLAYER_AVATAR_FLAG_DASH` resta acceso per tutto il passo di corsa
  (`npc_clear_strange_bits` lo spegne solo fra due passi).

## Percezione (`CanAwareOWESeePlayer`, `src/wild_encounter_ow.c`)

- **Udito**, in ogni direzione, solo con `runningState == MOVING`: corsa o bici
  entro `activeDistance` (5), camminata entro `OWE_HEARING_WALK` (2), passo
  lento mai. Metrica di `IsPlayerWithinOWEDistance` (quadrato tagliato a
  `|dx|+|dy| <= d*362>>8`). Il suono passa gli ostacoli.
- **Vista**: cono davanti, metà larghezza 0 a distanza 1, 1 a 2-3, 2 da 4,
  limitata da `(viewWidth-1)/2`, profondità `viewDistance`. Fermata da
  collisione o da un oggetto non OWE, non follower, non `invisible` (NPC e
  Pokémon decorativi coprono). Giocatore in erba alta o lunga: visto solo
  dalla casella davanti. Il dislivello resta (`CanOWEReachPlayer`).
- **Invariante: `viewDistance <= activeDistance`.** Con `returnToIdle =
  PLAYER_OUTSIDE_ACTIVE_RANGE` un Pokémon che ti vede oltre l'`activeDistance`
  torna a riposo e ti rinota: "!" in loop.
- **`IsOWEAwareOfPlayer()`** = `OWE_SAVED_MOVEMENT_STATE_FLAG` (bit 7 di
  `warpArrowSpriteId`): alzato in `Common_Step7`, tolto a riposo, sopravvive
  al ricaricamento. Il "vaga" (`WANDER_AROUND_OWE`) non nota mai.
- Il controllo gira solo in `WanderAround_Step3` (fra due passi).

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
  record non è libero** (`OverworldCatch_IsBallOut`), in silenzio.
- **Bersaglio**: prima cosa sulla linea entro 5. Collisione, dislivello e
  oggetti visibili non-OWE fermano la Ball (cade e torna); un oggetto a
  un'altra altezza è saltato (`AreElevationsApart`). **Acqua (1) = terreno
  (3)** (`GetThrowLevel`): senza, ogni lancio fra riva e acqua cadeva alla
  prima casella. Le sporgenze non sono dislivelli. **La Ball non insegue**
  (utente, 18/11): colpisce se all'arrivo `currentCoords` è la casella del
  lancio (`targetX/Y`, `IsOWEAtLanding`); i salti sul posto non contano
  (`GetObjectGroundPos` senza `y2`).
- **Sprite**: fogli delle Ball dei follower (`gPokeballGraphics`, frame
  0-4), `coordOffsetEnabled`, posizioni dagli sprite (`GetObjectGroundPos`).
  **Mai un assert**: senza sprite il volo prosegue invisibile e `FinishThrow`
  chiude all'impatto. `IsBallSpriteValid` (callback + indice + magic) copre
  i campi ricostruiti (menu, warp).
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
  "vaga"** (il bit resterebbe). Il bit salta distanza e vista, non l'altezza
  (`CanOWEReachPlayer`), e si consuma al primo controllo. Il "vaga" tiene il ×2.
- **Coda** (`results`, 3): un lancio parte se c'è un record libero e Ball in
  volo + catture in coda < 3. Banner e soprannomi da
  `OverworldCatch_TryStartResults` a record tutti liberi, **dopo** il passo.
  Palette dell'icona: `FieldEffectFreePaletteIfUnused`. Dopo un `callnative`
  che ferma lo script serve `waitstate`.
- **Pokémon lanciato**: `gOverworldThrowLeadSlot` (letto e azzerato da
  `SetBattlePartyIds`, e a ogni frame a controlli liberi). Roamer e sciami:
  R avvia la lotta. Spento in Safari, Gara, Pike, Piramide, Trainer Hill.

## Pannello della scelta (D18, `UpdatePanel`)

- Finestra BG0 12x3 a (2,16), base 8, pal 15, cornice pal 14 come il banner
  (tema scuro da sé, **dopo** che le pal 14/15 sono ricaricate).
  **`SetStandardWindowBorderStyle` riempie con 1: `PIXEL_FILL(0)` dopo rende
  il fondo trasparente.**
- **Tile e posto condivisi**: base 8 come banner, descrizione oggetto, menu
  Start e **finestra della ricerca DexNav** (stessa base e (1,16)): spento con
  `DN_FLAG_SEARCHING`, se no la cancellava. Si aggiorna e si nasconde
  **all'inizio del CB2** (`OverworldCatch_UpdatePanel` in `OverworldBasic`,
  prima di `ScriptContext_RunScript`): i controlli si bloccano nel CB1, quindi
  sparisce prima che uno script o un task disegni nello stesso frame.
- Su con L tenuto o `panelTimer` (120 frame, riarmato da cambio, scorrimento,
  lancio); giù a controlli bloccati o con `L=A`. Il rilascio di L senza
  scorrere **cambia modalità** (D1). Solo testo (`FONT_SMALL`, niente sprite).

## Prove

Ricette in §5.2-5.5 del piano e `docs/claude/emulator.md`. `PendingThrow` =
36 B. `gSaveblock3 + 0x13` = `autoRun`; `FLAG_SYS_B_DASH` = byte `0x118` bit 0
di SaveBlock1. Un OWE col bit 7 di `+0x1D` non viene ricambiato.
