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

## Interruttore OVERWORLD CATCH (D20, fase 9)

- Solo `IsOverworldCatchEnabled()`. **Spenta = gioco di prima del piano
  salvo prezzi e negozi**: tetti `*_NO_CATCH` 6/3/3, Safari 4, ricambio 240;
  `CanAwareOWESeePlayer_NoCatch` (4x3 per tutti, solo corsa/bici sentite,
  niente copertura); B con AUTORUN acceso = camminata (il passo lento resta
  nella ricerca DexNav); R = `TryStartDexNavSearch`; L, pannello e
  `OverworldCatch_TryThrow` spenti.
- Si cambia dal menu Start, chiuso con una Ball fuori o banner in coda:
  nessun lancio vivo al cambio. Riaccesa, `TryRemoveOWEAboveCap` toglie
  l'eccesso. **A cattura spenta R+START del debug avvia prima il DexNav.**

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

## Lancio (`src/overworld_catch.c`, fasi 3-4)

- **Tasti**: R = `OverworldCatch_TryThrow` (`ProcessPlayerFieldInput`; in bici
  Mach/Acro). R ricordato 16 frame (`sRPressBuffer`), **mai a controlli
  bloccati**. L = `OverworldCatch_HandleLButton` da `DoCB1_Overworld` (toglie
  il D-pad finché è tenuto); L tenuto attraverso un blocco conta come usato.
  Con `L=A` main.c **aggiunge** A ma lascia L: l'opzione va letta.
- **Un lancio è un record** (`sCatch.throws`, 3): `FLYING` → `OPENING` →
  `CLOSING` (cattura: qui l'OWE è tolto) → `FALLING` → `SHAKING` →
  `CAUGHT`/`BREAKOUT`; a vuoto `FLYING` → `FALLING` → `DROPPED`, poi la Ball
  torna. **Esito deciso in `ResolveImpact`** (D15), il resto è animazione.
  Timer fermi a controlli bloccati. **Start bloccato finché un record non è
  libero** (`OverworldCatch_IsBallOut`), in silenzio.
- **Bersaglio**: prima cosa sulla linea entro 5; il follower è saltato e
  rientra nella Ball (`SendFollowerIntoPokeball`). Muri, dislivelli, oggetti
  non-OWE fermano la Ball. **Acqua (1) = terreno (3)** (`GetThrowLevel`).
  Colpito se all'arrivo `currentCoords` è `targetX/Y` (niente `y2`).
- **Sprite**: **mai un assert**: senza sprite il volo prosegue invisibile e
  `FinishThrow` chiude all'impatto. `IsBallSpriteValid` (callback + indice +
  magic) copre i campi ricostruiti (menu, warp).
- **Pokémon nella Ball** (dall'impatto, `holdsOWE`): congelato e, dal raggio,
  `invisible`, **riasserito a ogni frame** (`KeepOWEInBall`): il `release` di
  uno script scongela tutti. Ha il flag "non sparire" (`HoldOWEForThrow`,
  rimesso com'era all'uscita): un ricambio riuserebbe slot e `localId`. Non è
  bersaglio né contatto; **una seconda Ball verso di lui è un lancio a
  vuoto** (se no veniva dato due volte).
- **Lotta che parte** (`CreateBattleStartTask` e `_Debug` →
  `OverworldCatch_OnBattleStart`): le Ball a terra finiscono prima della
  transizione (chi è dentro esce), quelle in volo atterrano dopo, invisibili.
- **Posa**: tenuta da `DELAY_16`. `ObjectEventIsMovementOverridden` dà
  occupato anche un'azione finita e non tolta: `ClearHeldMovementIfFinished`
  prima di `ObjectEventSetHeldMovement`.
- **Il Pokémon nasce sullo stack** (`CreateMonFromOWE`), **mai in
  `gEnemyParty`**: lo spawn lo usa nello stesso frame.
- **Formula**: `ComputeCaptureOddsFromParams` (`battle_script_commands.c`).
  ×2 se `!IsOWEAwareOfPlayer`; Quick Ball solo se non allertato; Timer mai.
  Sfida: `GetCaptureChallengeChecks`. **Nuzlocke: solo una cattura riuscita
  segna la zona** (P3).
- **Fallito**: `ForceOWEToNoticePlayer`, **mai su un OWE già allertato o
  "vaga"** (il bit resterebbe). Il bit salta distanza e vista e usa il livello
  del lancio (non `CanOWEReachPlayer`, acqua ≠ riva): su un altro livello si
  consuma. Il "vaga" tiene il ×2.
- **Ball tolta solo a cattura riuscita** (`GiveCaughtMon`, 20/11): al lancio
  la borsa deve averne più di quelle in volo (`CountFlyingBalls`).
- **Ball a terra = ostacolo** (`OverworldCatch_IsBallOnGroundAt` in
  `GetVanillaCollision`): casella `landX/Y`, da `OPENING` a fine record.
- **Banner a foglietti, senza script** (20-21/11): una finestra (il più
  nuovo, tile BG0 da 92; DexNav 8-91) e sotto i vecchi, 2 righe ciascuno
  (cornice 3/4/5 e 6/7/8: il solo bordo sembrava trasparente) nel tilemap,
  con `CopyBgTilemapBufferToVram`. Max 3, 180 frame, esce il più vecchio.
  `pileUp` come `panelUp`. Il popup del nome mappa (scorre BG0) li cancella;
  in ricerca DexNav la pila scende. Soprannome solo con la Nuzlocke.
- **Pokémon lanciato**: `gOverworldThrowLeadSlot` (azzerato da
  `SetBattlePartyIds` e a controlli liberi). Roamer e sciami: lotta. Spento
  in Safari, Gara, Pike, Piramide, Trainer Hill.

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
- Su con L tenuto o `panelTimer` (120 frame); giù a controlli bloccati, con
  `L=A` o a cattura spenta. **Il rilascio di L senza scorrere cambia
  modalità** (D1): anche in una prova che tiene L solo per guardare.

## Prove

Ricette nel piano (§5.2-5.9) e `docs/claude/emulator.md`. `gSaveblock3 +
0x13` = `autoRun`, `+0x2F` bit 7 = OVERWORLD CATCH spenta.
