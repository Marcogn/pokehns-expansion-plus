#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_main.h"
#include "braille_puzzles.h"
#include "data.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_camera.h"
#include "fieldmap.h"
#include "field_effect.h"
#include "field_weather.h"
#include "fldeff.h"
#include "hasep_symbols.h"
#include "heal_location.h"
#include "item.h"
#include "main.h"
#include "move.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "region_map.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "wild_encounter.h"
#include "constants/difficulty.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/item.h"
#include "constants/items.h"
#include "constants/map_groups.h"
#include "constants/metatile_behaviors.h"
#include "constants/event_object_movement.h"
#include "constants/songs.h"
#include "constants/pokemon.h"
#include "constants/region_map_sections.h"
#include "constants/vars.h"

// See include/hasep_symbols.h. The statics the app needs are exported from
// their own files: battle_controller_player.c, event_object_movement.c,
// field_camera.c, field_move.c, fieldmap.c, fldeff_flash.c, heal_location.c,
// pokemon.c, region_map.c, script.c and sprite.c.

extern const struct MapHeader *const *const gMapGroups[];
typedef u16 (*SpecialFunc)(void);
extern const SpecialFunc gSpecials[]; // data/specials.inc
extern const struct SpriteTemplate gFieldEffectObjectTemplate_Bird; // data/field_effects/field_effect_objects.h

// MAP_GROUP_COUNT[]: the number of maps in each group, as mapjson writes it.
#include "data/map_group_count.h"

struct HasepSymbolsHeader
{
    char magic[8];       // "HASEPSYM"
    u16 formatVersion;   // HASEP_SYMBOLS_FORMAT_VERSION: this header and struct HasepSymbol
    u16 layoutVersion;   // HASEP_SYMBOLS_LAYOUT_VERSION: the structs the app reads
    u16 symbolSize;      // sizeof(struct HasepSymbol)
    u16 padding;
    const struct HasepSymbol *end; // one past the last entry; they start right after this header
};

// Placed by the linker script at its own symbol, so the entries from every
// object file sit between this header and __hasep_symbols_end.
extern const struct HasepSymbol __hasep_symbols_end[];

__attribute__((section(".text.header_hasep"), used)) static const struct HasepSymbolsHeader sHasepSymbolsHeader =
{
    .magic = { 'H', 'A', 'S', 'E', 'P', 'S', 'Y', 'M' },
    .formatVersion = HASEP_SYMBOLS_FORMAT_VERSION,
    .layoutVersion = HASEP_SYMBOLS_LAYOUT_VERSION,
    .symbolSize = sizeof(struct HasepSymbol),
    .end = __hasep_symbols_end,
};

// RAM the app may write while the game runs (an argument for a call, a short
// script for the PC): Heart and Soul's app support used the unused end of
// EWRAM for this, a buffer of its own can't collide with a new variable.
EWRAM_DATA u8 gHasepAppScratch[32] = {0};

// Party, save blocks, field.
HASEP_EXPORT(gPlayerParty);
HASEP_EXPORT(gPlayerPartyCount);
HASEP_EXPORT(gSaveBlock1Ptr);
HASEP_EXPORT(gSaveBlock2Ptr);
HASEP_EXPORT(gMain);
HASEP_EXPORT(gObjectEvents);
HASEP_EXPORT(gMapHeader);
HASEP_EXPORT(gBagPockets);
HASEP_EXPORT(gPartyMenu);
HASEP_EXPORT(gHasepAppScratch);

HASEP_EXPORT(gBackupMapLayout);
HASEP_EXPORT(gPlayerAvatar); // bike / surf: HnS keeps it before gObjectEvents, not after

// Battle.
HASEP_EXPORT(gBattleMons);
HASEP_EXPORT(gBattlerPositions);
HASEP_EXPORT(gBattlersCount);
HASEP_EXPORT(gBattleTypeFlags);
HASEP_EXPORT(gBattlerControllerFuncs);
HASEP_EXPORT(gActionSelectionCursor);
HASEP_EXPORT(gMoveSelectionCursor);
HASEP_EXPORT(gEnemyParty);          // the FOE TEAM and SHINY ODDS
HASEP_EXPORT(gBattlerPartyIndexes);
HASEP_EXPORT(gBattleStruct);        // a pointer: monToSwitchIntoId behind it
HASEP_EXPORT(HandleInputChooseMove);
HASEP_EXPORT(HandleInputChooseTarget);

// ROM tables.
HASEP_EXPORT(gSpeciesInfo);
HASEP_EXPORT(gItemsInfo);
HASEP_EXPORT(gMovesInfo);
HASEP_EXPORT(gAbilitiesInfo);
HASEP_EXPORT(gMonIconPaletteTable);
HASEP_EXPORT(gTypeEffectivenessTable);
HASEP_EXPORT(gWildMonHeaders);
HASEP_EXPORT(gMapGroups);
HASEP_EXPORT(gRegionMapInfos);
// The guide reads the trainers at the difficulty the game uses by default.
HASEP_EXPORT_VALUE(gTrainers, "gTrainers[DIFFICULTY_NORMAL]", gTrainers[DIFFICULTY_NORMAL]);

// Functions the app calls between frames (field items, PC, FLY, FLASH).
HASEP_EXPORT(VarGet);
HASEP_EXPORT(VarSet);
HASEP_EXPORT(FlagGet);
HASEP_EXPORT(CheckBagHasItem);
HASEP_EXPORT(RemoveBagItem);
HASEP_EXPORT(AddBagItem);
HASEP_EXPORT(GetMonData3);
HASEP_EXPORT(SetMonData);
HASEP_EXPORT(ArePlayerFieldControlsLocked);
HASEP_EXPORT(LockPlayerFieldControls);
HASEP_EXPORT(UnlockPlayerFieldControls);
HASEP_EXPORT(ScriptContext_SetupScript);
HASEP_EXPORT(gSpecials);
HASEP_EXPORT(ShowPokemonStorageSystemPC);
HASEP_EXPORT(SetUpFieldMove_Fly);
HASEP_EXPORT(FadeScreen);
HASEP_EXPORT(SetWarpDestinationToHealLocation);
HASEP_EXPORT(SetWarpDestinationToMapWarp);
HASEP_EXPORT(ReturnToFieldFromFlyMapSelect);
HASEP_EXPORT(SetUpFieldMove_Flash);
HASEP_EXPORT(SetUpPuzzleEffectRegisteel);
HASEP_EXPORT(gFieldCallback2);
HASEP_EXPORT(gPostMenuFieldCallback);

// FREE FLIGHT and the scenic FLY (the fork's pk_flight.c, Heart and Soul's FLIGHT_HEART_AND_SOUL record at
// these addresses). sMapConnectionFlags and sHealLocations are exported from fieldmap.c / heal_location.c.
HASEP_EXPORT(CB2_Overworld);
HASEP_EXPORT(gPaletteFade);
HASEP_EXPORT(CreateSprite);
HASEP_EXPORT(DestroySprite);
HASEP_EXPORT(StartSpriteAnim);
HASEP_EXPORT(SpriteCallbackDummy);
HASEP_EXPORT(gFieldEffectObjectTemplate_Bird);
HASEP_EXPORT(LoadPlayerObjectEventPalette);
HASEP_EXPORT(MapGridGetMetatileBehaviorAt);
HASEP_EXPORT(MapGridGetCollisionAt);
HASEP_EXPORT(MapGridGetElevationAt);
HASEP_EXPORT(GetObjectEventIdByPosition);
HASEP_EXPORT(PlaySE);
HASEP_EXPORT(CurrentMapDrawMetatileAt);
HASEP_EXPORT(DrawWholeMapView);
HASEP_EXPORT(RemoveFollowingPokemon);
HASEP_EXPORT(UpdateFollowingPokemon);
// An address under an UPPER_CASE name (tools/hasep_symbols.py reads those as constants): the u8 count per map group.
HASEP_EXPORT_VALUE(MAP_GROUP_COUNT, "MAP_GROUP_COUNT", MAP_GROUP_COUNT);
HASEP_EXPORT_VALUE(MAP_GROUPS_COUNT, "MAP_GROUPS_COUNT", MAP_GROUPS_COUNT);

// Constants the app checks or passes to the functions above.
HASEP_EXPORT_VALUE(NUM_SPECIES, "NUM_SPECIES", NUM_SPECIES);
HASEP_EXPORT_VALUE(ITEMS_COUNT, "ITEMS_COUNT", ITEMS_COUNT);
HASEP_EXPORT_VALUE(VAR_REPEL_STEP_COUNT, "VAR_REPEL_STEP_COUNT", VAR_REPEL_STEP_COUNT);
HASEP_EXPORT_VALUE(FLAG_SYS_USE_FLASH, "FLAG_SYS_USE_FLASH", FLAG_SYS_USE_FLASH);
HASEP_EXPORT_VALUE(FLAG_SYS_POKEDEX_GET, "FLAG_SYS_POKEDEX_GET", FLAG_SYS_POKEDEX_GET);
HASEP_EXPORT_VALUE(MON_DATA_MOVE1, "MON_DATA_MOVE1", MON_DATA_MOVE1);
HASEP_EXPORT_VALUE(MON_DATA_IS_EGG, "MON_DATA_IS_EGG", MON_DATA_IS_EGG);
HASEP_EXPORT_VALUE(MAPSEC_NONE, "MAPSEC_NONE", MAPSEC_NONE);
HASEP_EXPORT_VALUE(NUM_OBJ_EVENT_GFX, "NUM_OBJ_EVENT_GFX", NUM_OBJ_EVENT_GFX);
// What tools/hasep_symbols.py checks the gTrainers row against.
HASEP_EXPORT_VALUE(DIFFICULTY_NORMAL, "DIFFICULTY_NORMAL", DIFFICULTY_NORMAL);
HASEP_EXPORT_VALUE(TRAINERS_COUNT, "TRAINERS_COUNT", TRAINERS_COUNT);
// The Repels the app uses from the bag (expansion's ids; a Lure sets REPEL_LURE_MASK in VAR_REPEL_STEP_COUNT).
HASEP_EXPORT_VALUE(ITEM_REPEL, "ITEM_REPEL", ITEM_REPEL);
HASEP_EXPORT_VALUE(ITEM_SUPER_REPEL, "ITEM_SUPER_REPEL", ITEM_SUPER_REPEL);
HASEP_EXPORT_VALUE(ITEM_MAX_REPEL, "ITEM_MAX_REPEL", ITEM_MAX_REPEL);
HASEP_EXPORT_VALUE(REPEL_LURE_MASK, "REPEL_LURE_MASK", REPEL_LURE_MASK);
// What the game draws as shiny (MON_DATA_IS_SHINY): Hasep's challenge setting changes the odds, not this.
HASEP_EXPORT_VALUE(SHINY_ODDS, "SHINY_ODDS", SHINY_ODDS);

// The layouts the app reads, as Heart and Soul 2.0.6 has them (measured on
// both builds). A change here means the app reads garbage: bump
// HASEP_SYMBOLS_LAYOUT_VERSION and update the app with the new values.
STATIC_ASSERT(HASEP_SYMBOLS_LAYOUT_VERSION == 1, HasepLayoutVersionMatchesTheAssertsBelow);
STATIC_ASSERT(sizeof(struct Pokemon) == 0x64, HasepLayoutPokemon);
STATIC_ASSERT(sizeof(struct BattlePokemon) == 0x88, HasepLayoutBattlePokemon);
STATIC_ASSERT(offsetof(struct BattlePokemon, ability) == 0x20, HasepLayoutBattlePokemonAbility);
STATIC_ASSERT(offsetof(struct BattlePokemon, hp) == 0x2A, HasepLayoutBattlePokemonHp);
STATIC_ASSERT(offsetof(struct BattlePokemon, item) == 0x30, HasepLayoutBattlePokemonItem);
STATIC_ASSERT(offsetof(struct BattlePokemon, status1) == 0x50, HasepLayoutBattlePokemonStatus1);
STATIC_ASSERT(sizeof(struct SpeciesInfo) == 0x10C, HasepLayoutSpeciesInfo);
STATIC_ASSERT(offsetof(struct SpeciesInfo, types) == 0x06, HasepLayoutSpeciesTypes);
STATIC_ASSERT(offsetof(struct SpeciesInfo, genderRatio) == 0x12, HasepLayoutSpeciesGender);
STATIC_ASSERT(offsetof(struct SpeciesInfo, speciesName) == 0x2C, HasepLayoutSpeciesName);
STATIC_ASSERT(offsetof(struct SpeciesInfo, iconSprite) == 0x6C, HasepLayoutSpeciesIcon);
STATIC_ASSERT(sizeof(struct ItemInfo) == 0x2C, HasepLayoutItemInfo);
STATIC_ASSERT(offsetof(struct ItemInfo, description) == 0x0C, HasepLayoutItemDescription);
STATIC_ASSERT(offsetof(struct ItemInfo, name) == 0x14, HasepLayoutItemName);
STATIC_ASSERT(offsetof(struct ItemInfo, iconPic) == 0x24, HasepLayoutItemIcon);
STATIC_ASSERT(offsetof(struct ItemInfo, iconPalette) == 0x28, HasepLayoutItemPalette);
STATIC_ASSERT(sizeof(struct MoveInfo) == 0x44, HasepLayoutMoveInfo);
STATIC_ASSERT(sizeof(struct Trainer) == 0x34, HasepLayoutTrainer);
STATIC_ASSERT(sizeof(struct TrainerMon) == 0x24, HasepLayoutTrainerMon);
STATIC_ASSERT(offsetof(struct SaveBlock1, pos) == 0x04, HasepLayoutSaveBlock1Pos);
STATIC_ASSERT(offsetof(struct SaveBlock1, playerParty) == 0x23C, HasepLayoutSaveBlock1Party);
STATIC_ASSERT(offsetof(struct SaveBlock1, money) == 0x494, HasepLayoutSaveBlock1Money);
STATIC_ASSERT(offsetof(struct SaveBlock1, flags) == 0x198C, HasepLayoutSaveBlock1Flags);
STATIC_ASSERT(offsetof(struct SaveBlock1, vars) == 0x1ABC, HasepLayoutSaveBlock1Vars);
STATIC_ASSERT(offsetof(struct SaveBlock1, objectEventTemplates) == 0x138C, HasepLayoutSaveBlock1Templates);
STATIC_ASSERT(sizeof(struct ObjectEvent) == 0x24, HasepLayoutObjectEvent);
STATIC_ASSERT(sizeof(struct Sprite) == 0x44, HasepLayoutSprite);
STATIC_ASSERT(sizeof(struct BagPocket) == 8, HasepLayoutBagPocket);
STATIC_ASSERT(sizeof(struct ItemSlot) == 4, HasepLayoutItemSlot);
STATIC_ASSERT(offsetof(struct MapHeader, regionMapSectionId) == 0x14, HasepLayoutMapHeaderMapsec);
STATIC_ASSERT(offsetof(struct ItemInfo, holdEffectParam) == 0x1D, HasepLayoutItemHoldEffectParam);
STATIC_ASSERT(offsetof(struct BattleStruct, monToSwitchIntoId) == 0xE5, HasepLayoutMonToSwitchInto);
STATIC_ASSERT(offsetof(struct PlayerAvatar, objectEventId) == 5, HasepLayoutAvatarObject);
STATIC_ASSERT(offsetof(struct PlayerAvatar, gender) == 7, HasepLayoutAvatarGender);
// The forced-shiny bit the app reads: `u16 hpLost:14, shinyModifier:1` right after the checksum, so bit 6 of +0x1F.
STATIC_ASSERT(offsetof(struct BoxPokemon, checksum) == 0x1C, HasepLayoutBoxMonChecksum);
STATIC_ASSERT(VAR_LAST_REPEL_LURE_USED == 0, HasepRepelNoLastUsedVar); // the app doesn't restore one
STATIC_ASSERT(offsetof(struct SaveBlock2, encryptionKey) == 0xAC, HasepLayoutSaveBlock2Key);
STATIC_ASSERT(offsetof(struct PartyMenu, slotId) == 0x09, HasepLayoutPartyMenuSlot);
// FREE FLIGHT: the movements, sounds and ground the app's flight uses, as Heart and Soul 2.0.6 has them.
STATIC_ASSERT(MOVEMENT_ACTION_WALK_FAST_DOWN == 0x15 && MOVEMENT_ACTION_WALK_FASTER_DOWN == 0x2D, HasepFlightWalk);
STATIC_ASSERT(MOVEMENT_ACTION_RIDE_WATER_CURRENT_DOWN == 0x29 && MOVEMENT_ACTION_SLIDE_DOWN == 0x31, HasepFlightDiagonal);
STATIC_ASSERT(SE_M_FLY == 158 && SE_FAILURE == 32, HasepFlightSounds);
STATIC_ASSERT(MB_NORMAL == 0x00 && MB_TALL_GRASS == 0x02 && MB_LONG_GRASS == 0x03 && MB_DEEP_SAND == 0x06
           && MB_SHORT_GRASS == 0x07 && MB_CAVE == 0x08 && MB_NO_RUNNING == 0x0A && MB_MOUNTAIN_TOP == 0x0C
           && MB_PUDDLE == 0x16 && MB_SAND == 0x21 && MB_ASHGRASS == 0x24 && MB_FOOTPRINTS == 0x25
           && MB_HOT_SPRINGS == 0x28, HasepFlightLanding);
// The fields the fork's pk_flight.c reads and writes by offset (its FL_OBJ_* / FL_SPR_* defines); the bitfields
// (elevations, inanimate / invisible, the sprite's flags) can't be pinned by offsetof: struct sizes above cover them.
STATIC_ASSERT(offsetof(struct ObjectEvent, spriteId) == 0x23, HasepFlightObjectSprite);
STATIC_ASSERT(offsetof(struct ObjectEvent, localId) == 0x08, HasepFlightObjectLocalId);
STATIC_ASSERT(offsetof(struct ObjectEvent, currentCoords) == 0x10, HasepFlightObjectCoords);
STATIC_ASSERT(offsetof(struct ObjectEvent, fieldEffectSpriteId) == 0x1A, HasepFlightObjectEffectSprite);
STATIC_ASSERT(offsetof(struct ObjectEvent, movementActionId) == 0x1C, HasepFlightObjectAction);
STATIC_ASSERT(offsetof(struct Sprite, x) == 0x20 && offsetof(struct Sprite, y) == 0x22, HasepFlightSpritePos);
STATIC_ASSERT(offsetof(struct Sprite, animNum) == 0x2A && offsetof(struct Sprite, data) == 0x2E, HasepFlightSpriteAnimData);
STATIC_ASSERT(offsetof(struct PlayerAvatar, flags) == 0, HasepFlightAvatarFlags);
STATIC_ASSERT(offsetof(struct MapHeader, connections) == 0x0C && sizeof(struct MapConnection) == 12, HasepFlightConnections);
STATIC_ASSERT(offsetof(struct HealLocation, x) == 2 && offsetof(struct HealLocation, y) == 4, HasepFlightHealLocation);
STATIC_ASSERT(MAPGRID_UNDEFINED == 0x3FF, HasepFlightMapGridUndefined);
STATIC_ASSERT(MOVEMENT_ACTION_WALK_FAST_UP == 0x16 && MOVEMENT_ACTION_WALK_FAST_LEFT == 0x17
           && MOVEMENT_ACTION_WALK_FAST_RIGHT == 0x18 && MOVEMENT_ACTION_WALK_FASTER_RIGHT == 0x30, HasepFlightWalkDirections);
