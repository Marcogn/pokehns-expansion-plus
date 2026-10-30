---
paths:
  - "src/tcloner.c"
  - "include/tcloner.h"
  - "include/constants/tcloner.h"
  - "data/scripts/tcloner.inc"
  - "data/scripts/trainer_battle.inc"
  - "src/battle_setup.c"
  - "src/trainer_see.c"
  - "data/maps/AzaleaTown_hns/**"
  - "data/maps/Mahoganytown_hns/**"
  - "data/maps/RocketHideout_B1F_hns/**"
  - "data/maps/GoldenrodCity_hns/**"
  - "data/maps/CeruleanCity_hns/**"
  - "data/maps/PalletTown_Lab_hns/**"
---

# Cattura dei Pokémon degli allenatori (TcloneR, ex `TRAINER CATCH`)

Piano e dialoghi: `docs/plans/tcloner.md`. **Le prove in emulatore, i
censimenti e la cronaca delle sessioni A e B stanno nella sua appendice**
("regola dell'area com'era prima dello sfoltimento"). Qui restano trappole e
fatti.

**Il permesso è l'oggetto chiave `ITEM_TCLONER` in borsa** (dal 23/10; l'opzione
`TRAINER CATCH` è tolta), riga in `CanCatchTrainerMon()` (`battle_util.c`).

## Contatore e premi

- **Contatore** `VAR_TCLONER_COUNT` (0x40E5): `RecordTrainerMonCaught(battler)`
  (`battle_setup.c`) è chiamata da `FinalizeCapture`, perché a lotta finita i
  battler non dicono più di chi era il Pokémon. Con **due allenatori** la
  squadra del secondo sta in `gEnemyParty[3..5]`: decide l'indice, non il lato.
- **Un allenatore conta una volta**: bitfield `pokedex.tclonerTrainerFlags`
  (83 byte dal `SAVE_VERSION 10`) in SaveBlock2, ricavato dal `filler[0x68]`
  di `struct Pokedex` (104 byte misurati, non 108 come dice `config/save.h`);
  offset inchiodati da `STATIC_ASSERT` in `save.c`. **Trappola**:
  `NewGameInitData` non chiama `ClearSav2()`, quindi `ResetPokedex()` azzera
  il bitfield a mano. Un rematch è un altro id e conta da sé; gli allenatori
  653-660 (Red's Path) non si catturano.
- **Premi**: `VAR_TCLONER_REWARDS` (0x40FC) conta i **passi** 0-12, non gli
  scaglioni; tabella `sRewards` (`tcloner.c`). `EventScript_TClonerRewards`
  controlla `checkitemspace` prima di `giveitem` **e il risultato dopo**
  (col randomizer l'oggetto può cambiare). Gli shiny vanno sempre nel PC.
- `VAR_TCLONER_STAGE` (0x40C2) = ultima tappa dello scienziato, 0-5. **0x40FA e
  0x40FB non sono liberi**: li usa il randomizer.
- Nuzlocke: con un allenatore `SetNuzlockeChecks` non gira, il clone non
  consuma l'incontro della zona. Level cap: nessuno.

## Lo scienziato

Sei oggetti `OBJ_EVENT_GFX_SCIENTIST_M_HNS`, uno per mappa, ognuno col suo
flag `FLAG_HIDE_TCLONER_*` (`FLAG_UNUSED_EXTENDED_80..85`). Le mappe chiamano
solo `TCloner_EventScript_UpdateNpcs` da `ON_TRANSITION`. **La visibilità si
ricalcola a ogni ingresso** (tappa *s* visibile se `STAGE < s` più la
condizione di mappa), quindi nessuna migrazione. Condizioni: covo se
`VAR_MAHOGANY_TOWN_STATE < 14`, negozio se `>= 14`; Goldenrod da
`VAR_GOLDENROD_CITY_STATE >= 6`; Pallet da tappa `>= 4`, per sempre. Il font
non ha la lineetta lunga ("stea… ahem"). Per provarlo: script di debug che
imposta tappa e stato della storia e teletrasporta; per Cerulean e Pallet una
build con lo scienziato fermo (vagando non si pilota).

## Motore della cattura

`MIRROR THIEF` sembra la stessa cosa e **non lo è** (Mirror Mode combatti con
una copia della squadra, "thief" te la lascia).

- Blocco in `Cmd_handleballthrow` (`BALL_TRAINER_BLOCK`); permesso in
  `CanCatchTrainerMon()`, che esclude strutture, link, partner, primo
  tutorial e **Mirror Mode** (a fine lotta ripristina la squadra dal backup).
  `item_use.c` lo consulta: la ball non si consuma solo se bloccata.
- **La trappola vera è lo script dopo la lotta.** Dopo un `trainerbattle`
  lo script di mappa riprende come se avessi vinto: per un capopalestra
  (`trainerbattle_no_intro`) è la **medaglia**. `B_OUTCOME_CAUGHT` in
  `CB2_EndTrainerBattle` accende `sTrainerMonCaught` e
  `BattleSetup_GetScriptAddrAfterBattle` / `GetTrainerPostBattleScript`
  restituiscono `EventScript_TrainerMonCaught` (`releaseall; end`).
- **Niente cattura nelle lotte di scena**: quel `releaseall; end` salta il resto
  della scena (grunt in mappa, statua che non avanza). `IsCutsceneTrainerBattle()`
  blocca `SINGLE_NO_INTRO_TEXT`, `TWO_TRAINERS_NO_INTRO` ed `EARLY_RIVAL`,
  tranne Leader/Leader Kanto/Elite Four/Champion. Accettato: i quattro
  allenatori finti della palestra di Fuchsia non si clonano.
- **Vista sospesa fino al cambio mappa** (`sSightSuspendedTrainer`,
  `CheckTrainer` in `trainer_see.c`): senza, l'allenatore ti risfidava subito.
  La squadra si rigenera a ogni lotta e si può ricatturare: voluto.
- **Il Pokémon catturato diventava un Bad Egg**: `SetMonData(MON_DATA_OT_ID)`
  scrive in chiaro ma l'OT ID è metà della chiave dei substruct
  (`personality ^ otId`). `SetMonOtIdRekeyed` (`pokemon.c`) decifra, scrive e
  ricifra, e conserva la shininess (relativa all'OT ID). **Lezione: il prodotto
  della feature va guardato, apri la squadra dopo la cattura.**
- Icona: arte dello Scanner con `graphics/items/icon_palettes/tcloner.pal`
  (originali nel commento di `src/data/graphics/items.h`).
