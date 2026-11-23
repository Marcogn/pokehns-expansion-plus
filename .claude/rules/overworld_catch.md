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

Piano: [`docs/plans/overworld-catch.md`](../../docs/plans/overworld-catch.md); prove nel diario
`docs/claude/overworld-catch-history.md`. OWE in generale: `overworld_encounters.md`.

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
- Il salto da una sporgenza spegne `creeping` (se no era muto).
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

- **Tasti**: R = `OverworldCatch_TryThrow` (in bici Mach/Acro; **R+START del
  debug lancia anche**). R ricordato 16 frame, **mai a controlli bloccati**.
  L = `OverworldCatch_HandleLButton` da `DoCB1_Overworld` (toglie il D-pad
  finché è tenuto); L tenuto attraverso un blocco conta come usato.
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
- **Sprite: mai un assert**: senza sprite il volo prosegue invisibile.
- **Pokémon nella Ball** (`holdsOWE`): congelato e `invisible`, **riasserito
  a ogni frame** (`KeepOWEInBall`: il `release` di uno script scongela tutti);
  flag "non sparire" (`HoldOWEForThrow`), se no il ricambio riusa slot e
  `localId`. Non è bersaglio né contatto: **una seconda Ball verso di lui è
  un lancio a vuoto** (se no veniva dato due volte).
- **Lotta che parte** (`OverworldCatch_OnBattleStart`): le Ball a terra
  finiscono prima (chi è dentro esce), quelle in volo atterrano dopo.
- **Posa**: tenuta da `DELAY_16`; `ClearHeldMovementIfFinished` prima di
  `ObjectEventSetHeldMovement` (un'azione finita e non tolta dà occupato).
- **Il Pokémon nasce sullo stack** (`CreateMonFromOWE`), **mai in
  `gEnemyParty`**: lo spawn lo usa nello stesso frame.
- **Formula**: `ComputeCaptureOddsFromParams` (`battle_script_commands.c`;
  ARCEUS FORMULAS: `arceus.md`). ×2 se `!IsOWEAwareOfPlayer`; Quick Ball
  solo se non allertato; Timer mai.
  Sfida: `GetCaptureChallengeChecks`. **Nuzlocke: solo una cattura riuscita
  segna la zona** (P3).
- **Fallito**: `ForceOWEToNoticePlayer`, **mai su un OWE già allertato o
  "vaga"** (il "vaga" tiene il ×2). Usa il livello del lancio (acqua ≠ riva).
- **Ball tolta solo a cattura riuscita** (`GiveCaughtMon`): al lancio la
  borsa deve averne più di quelle in volo; all'impatto, se non c'è più (usata
  in una lotta partita durante il volo), è un lancio a vuoto.
- **Ball a terra = ostacolo** (`OverworldCatch_IsBallOnGroundAt` in
  `GetVanillaCollision`): `landX/Y` della mappa del lancio, da `OPENING`.
- **Foglietti, senza script**: il più nuovo è una finestra (BG0 da 92;
  DexNav 8-91), i vecchi 2 righe di tilemap ciascuno (cornice 3/4/5 + 6/7/8).
  **Il bordo 6/7/8 finisce con 1-2 righe trasparenti** (13 cornici su 20):
  sopra ogni foglietto sotto va la giunzione (7 + carta 3/4/5,
  `LoadPileJoinTiles`, BG0 0x23D-0x23F, dopo il popup del nome mappa e prima
  della Pokégear 0x270; issue #82). Max 3, 180 frame, esce il più vecchio.
  Lotta, messaggio, menu o dissolvenza: nascosti (`pileHidden`), poi
  ridisegnati; il popup del nome mappa li cancella; in ricerca DexNav la pila
  scende. **Pal 14 caricata da noi** (`LoadCatchFrameGfx`): dopo il Continue
  c'è quella del titolo. Soprannome solo con la Nuzlocke.
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
  `DN_FLAG_SEARCHING`. Pannello e foglietti si nascondono **all'inizio del
  CB2** (`OverworldCatch_UpdatePanel`, prima di `ScriptContext_RunScript`),
  prima che uno script disegni. **Menu Start e debug si aprono nel CB1 e
  disegnano subito**: `OverworldCatch_ClearScreenForMenu()` prima di
  `ShowStartMenu`/`Debug_ShowMainMenu`, se no ci aprivano buchi.
- Su con L tenuto o `panelTimer` (120 frame); giù a controlli bloccati, con
  `L=A` o a cattura spenta. **Il rilascio di L senza scorrere cambia
  modalità** (D1), anche in una prova.

## Prove

Ricette: diario (fasi 2-9) e `docs/claude/emulator.md`. `gSaveblock3 + 0x13` =
`autoRun`, `+0x2F` bit 7 = OVERWORLD CATCH spenta.
