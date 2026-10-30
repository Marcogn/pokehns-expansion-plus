---
paths:
  - "src/wild_encounter_ow.c"
  - "include/wild_encounter_ow.h"
  - "include/config/wild_encounter.h"
  - "src/data/pokemon/wild_encounter_ow_behavior.h"
  - "src/data/pokemon/owe_temperaments.h"
  - "tools/owe_temperaments.py"
  - "src/event_object_movement.c"
  - "src/field_effect_helpers.c"
  - "include/field_effect_helpers.h"
  - "src/trainer_see.c"
  - "src/surfable.c"
---

# Pokémon visibili nell'overworld (OWE)

Piano, fatti, decisioni **e il diario di verifica di tutte le fasi** (prove,
misure, ricette GDB) in [`docs/plans/overworld-encounters.md`](../../docs/plans/overworld-encounters.md),
appendice in fondo. Qui restano solo trappole e invarianti.
Port della PR #8434 di expansion (dal diff del merge `10ae7f9`, più le correzioni
fino a `expansion/1.16.4`; **non** da `master`, che dipende da `mass_outbreak.h`)
con un tiro pesato per indole (`docs/plans/overworld-encounters/temperaments.json`).

## Interruttori e numeri

- **`AreOverworldMonsEnabled()`** (`wild_encounter_ow.c`) è l'unico interruttore:
  falso con WILD BATTLES OFF e durante la Gara Pigliamosche, altrimenti sempre
  vero. Lo usano lo spawn e `ShouldDisableRandomEncounters()`
  (`field_control_avatar.c`). `WE_VANILLA_RANDOM` resta TRUE (serve alla Gara).
- Il bit `unusedOverworldMonsBit` (SaveBlock3, byte 56 bit 5) è **ignorato ma
  non libero**: nei salvataggi passati per le build con la voce `WILD POKéMON`
  del PC può valere 1.
- Tetti 6/3/3 (terra/acqua/grotta; grotta = `MAP_TYPE_UNDERGROUND` o
  `INDOOR`), Safari 4 (`OWE_SPAWNS_MAX_SAFARI`), 6 caselle per tentativo
  (`OWE_SPAWN_TILE_TRIES`). A tetto pieno il ricambio è uno per uno.
- `localId` degli OWE: da `LOCALID_OW_ENCOUNTER_END` (252) in giù, 6 posti =
  247-252, sotto il follower (253). Le mappe HnS arrivano a 64 oggetti.
- **Misurare in frame, non con l'orologio** (`gMain.vblankCounter1`): ogni
  lettura GDB ferma il gioco ~0,7 s.
- Il menu debug (R + START) fa sparire gli OWE: R avvia il DexNav sulla specie
  registrata. La ricerca DexNav li despawna anche da `InitDexNavSearch`.

## Trappole

- **Un `assertf` su una condizione che il gioco può raggiungere è una schermata
  blu** (`make hns` non è RELEASE). VRAM/palette piene o tabella oggetti piena
  sono `if` che saltano lo spawn (`CheckCanLoadOWE`). Con `OW_GFX_COMPRESS`
  ogni oggetto tiene in VRAM tutto il suo foglio: nel Safari Low Mid restavano
  10 tile su 1024; `OWE_SPRITE_TILE_HEADROOM` 32 le riserva a icone ed effetti.
- **Uno sprite di icona che non si crea blocca lo script del follower**:
  `waitfieldeffect FLDEFF_EMOTE` non finisce mai. `IconSpriteNotCreated()`
  (`trainer_see.c`) toglie l'effetto dalla lista; vale per tutte le icone.
- **Lo spawn usa `gEnemyParty[0]`** e poi `ZeroEnemyPartyMons()`: ogni
  percorso che crea un selvatico e **sblocca i controlli prima che la lotta
  inizi** la fa partire con specie 0 (successo con la Testata, `field.md`).
- **Indici "non trovato" su `gObjectEvents`** (`OBJECT_EVENTS_COUNT`) leggono
  e scrivono i `.sbss` seguenti: usa `GetOWEBySpawnSlot()` (NULL per slot
  vuoto) e le guardie di `DespawnOWEOnBattleStart`, `SortOWEAges`. Con OWE
  spenti `TryAndDespawnOldestGeneratedOWE_ToFreeObject` deve fare `return TRUE`.
  Non prendere il refactor 1.16.4 di `GetAvailableObjectEventId` (cambia la
  firma e l'ordine degli slot).
- **Gli hunk entrati "con fuzz" vanno riletti** (`patch -F3` ha messo una
  funzione dentro uno `switch`): compilano comunque.
- **Despawn "entrando in città"**: `TryDespawnOWEsCrossingMapConnection` gira a
  ogni passo e upstream toglie tutto in `MAP_TYPE_CITY/TOWN`, ma 15 città di
  HnS hanno selvatici. Si tolgono solo gli OWE di un'altra mappa.
- **Tag di palette e riflessi**: i tag del Surf vanno da `0x3001` a `0x308B`;
  `PAL_RAW_REFLECTION_OFFSET` è `0x3100` (`STATIC_ASSERT` in `surfable.c`);
  riflesso `0x0800`, shiny `+0x200`. Il vecchio `HIGH_BRIDGE_PAL_TAG 0x4010`
  collideva con la palette di Pidgey.
- **OWE in acqua**: sprite da nuoto (`UpdateOWESwimSprites`) con lo sprite
  vero nascosto da `ST_OAM_AFFINE_ERASE`, **non** `invisible` (`facetogether`
  salta gli invisibili). Un ricambio riusa slot e `localId`: confronta anche
  il `graphicsId`. Occupa 2 palette e 64 tile in più; senza palette libera
  il Pokémon resta in piedi.
- **`trainer_type` dei massi di HnS**: il campo ospita un flag (Ice Path B1F,
  `0x2F7-0x2FA`) ma `trainerType` è `u8` e un OWE è `0xFF`: un flag nuovo
  `0x?FF` usato così diventa un selvatico.
- `GetAvailableObjectEventId` a tabella piena con un `localId` di OWE cade su
  `*objectEventId = 16` e restituisce `FALSE`: tienilo presente se cambi lo spawn.
- Comparsa su una casella in uscita: `IsTileBeingLeftByObject` guarda anche
  le `previousCoords`. Un NPC che cammina su un OWE lo fa sparire.
- La lotta con un OWE usa `PickWildMonNature` (Sincronismo e Pokéblock) e
  `TryApplyPokeblockBonus`, come `CreateWildMon`.
- `UpdateOverworldWildEncounter` (ogni frame) costa anche a feature spenta:
  primo sospetto se il frame rate cala.
- `ComputePlayerShinyOdds()` usa `GetShinyOdds()`, non `SHINY_ODDS`: senza,
  SHINY CHANCE del challenge menu sparirebbe. Le sei `MOVEMENT_TYPE_*_OWE`
  partono da `0x54` (`0x53` è `TOWER_BEAM` di HnS).
- `tools/owe_temperaments.py` **si rilancia a mano** dopo ogni modifica di
  `temperaments.json`; il file generato è nel git.
- Force Shiny resta nel salvataggio e gli shiny occupano i posti; uscire dal
  Safari col warp di debug lascia `FLAG_SYS_SAFARI_MODE` acceso.
