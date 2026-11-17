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
Cattura con la Ball nell'overworld (in corso): [`docs/plans/overworld-catch.md`](../../docs/plans/overworld-catch.md).
Andature, udito e cono di vista (fase 2 di quel piano): `overworld_catch.md`.

## Interruttori e numeri

- **`AreOverworldMonsEnabled()`** (`wild_encounter_ow.c`) è l'unico interruttore:
  falso con WILD BATTLES OFF e durante la Gara Pigliamosche, altrimenti sempre
  vero. Lo usano lo spawn e `ShouldDisableRandomEncounters()`
  (`field_control_avatar.c`). `WE_VANILLA_RANDOM` resta TRUE (serve alla Gara).
- Il bit `unusedOverworldMonsBit` (SaveBlock3, byte 56 bit 5) è **ignorato ma
  non libero**: nei salvataggi passati per le build con la voce `WILD POKéMON`
  del PC può valere 1.
- Tetti **4/2/2** (terra/acqua/grotta; grotta = `MAP_TYPE_UNDERGROUND` o
  `INDOOR`), Safari **3** (`OWE_SPAWNS_MAX_SAFARI`), 6 caselle per tentativo
  (`OWE_SPAWN_TILE_TRIES`), ricambio a tetto pieno ogni 480 frame
  (`OWE_SPAWN_TIME_REPLACEMENT`; erano 6/3/3, Safari 4, 240: piano
  `overworld-catch` fase 1). `OWE_SPAWNS_MAX` resta 6: **non è un tetto**,
  dimensiona gli array e i `localId` (`STATIC_ASSERT` sui tetti in
  `wild_encounter_ow.c`). A tetto pieno il ricambio è uno per uno, quindi
  **sopra il tetto** (salvataggio fatto con i tetti vecchi: gli oggetti stanno
  in SaveBlock1; Pokémon di terra quando si inizia il Surf) ci pensa
  `TryRemoveOWEAboveCap`, che toglie il più vecchio non shiny ogni 30 frame.
  Con OWE dell'altra zona (acqua/terra) il ricambio scende a 30 frame
  (`IsAnyOWEFromOtherArea`), **shiny esclusi**: non vengono mai tolti, e
  contarli farebbe girare gli altri ogni 30 frame finché restano. `GetOWEBySpawnSlot(OWE_INVALID_SPAWN_SLOT)`
  **punta al follower** (252 - 0xFF = 253 in `u8`): controlla lo slot prima.
- `localId` degli OWE: da `LOCALID_OW_ENCOUNTER_END` (252) in giù, 6 posti =
  247-252, sotto il follower (253). Le mappe HnS arrivano a 64 oggetti.
- **Misurare in frame, non con l'orologio** (`gMain.vblankCounter1`): ogni
  lettura GDB ferma il gioco ~0,7 s.
- La ricerca DexNav despawna gli OWE (`InitDexNavSearch`). R non la avvia
  più (lancia la Ball, `overworld_catch.md`): R + START del debug non li tocca.

## Trappole

- **Il conto alla rovescia dello spawn è `u16`** (`sOWESpawnCountdown`) e il
  segnale "nessun incontro" è `0xFFFF`: con `u8` e `0xFF` 480 frame non ci
  stanno (errore `-Woverflow`) e il massimo sarebbe 254.
- **Le mappe di HnS hanno Pokémon decorativi** (`OBJ_EVENT_GFX_MON_BASE+…`,
  `trainerType` 0, `localId` bassi: la Grotta dell'Unione 1F ne ha 16). Non
  sono OWE, non contano nei tetti, ma **occupano slot di oggetti e VRAM** e
  stanno sul percorso di un lancio: da distinguere con `IsOverworldWildEncounter`.
- **Contare gli OWE da GDB**: `gObjectEvents`
  da `nm`, 16 oggetti da 0x24 byte, `active` = bit 0 del byte 0,
  `trainerType` a +7 (0xFF = OWE), `localId` a +8 (generati = 247-252),
  `gMain` + 0x20 = `vblankCounter1`. Dettagli e ricetta nell'appendice di
  `docs/plans/overworld-catch.md` §5.1.
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
