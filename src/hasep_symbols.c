#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_main.h"
#include "braille_puzzles.h"
#include "data.h"
#include "event_data.h"
#include "field_effect.h"
#include "field_weather.h"
#include "fldeff.h"
#include "hasep_symbols.h"
#include "item.h"
#include "main.h"
#include "move.h"
#include "overworld.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "region_map.h"
#include "script.h"
#include "wild_encounter.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/region_map_sections.h"
#include "constants/vars.h"

// See include/hasep_symbols.h. The statics the app needs are exported from
// their own files: battle_controller_player.c, field_move.c, fldeff_flash.c,
// pokemon.c and region_map.c.

extern const struct MapHeader *const *const gMapGroups[];
typedef u16 (*SpecialFunc)(void);
extern const SpecialFunc gSpecials[]; // data/specials.inc

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

// Battle.
HASEP_EXPORT(gBattleMons);
HASEP_EXPORT(gBattlerPositions);
HASEP_EXPORT(gBattlersCount);
HASEP_EXPORT(gBattleTypeFlags);
HASEP_EXPORT(gBattlerControllerFuncs);
HASEP_EXPORT(gActionSelectionCursor);
HASEP_EXPORT(gMoveSelectionCursor);
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

// Constants the app checks or passes to the functions above.
HASEP_EXPORT_VALUE(NUM_SPECIES, "NUM_SPECIES", NUM_SPECIES);
HASEP_EXPORT_VALUE(ITEMS_COUNT, "ITEMS_COUNT", ITEMS_COUNT);
HASEP_EXPORT_VALUE(VAR_REPEL_STEP_COUNT, "VAR_REPEL_STEP_COUNT", VAR_REPEL_STEP_COUNT);
HASEP_EXPORT_VALUE(FLAG_SYS_USE_FLASH, "FLAG_SYS_USE_FLASH", FLAG_SYS_USE_FLASH);
HASEP_EXPORT_VALUE(FLAG_SYS_POKEDEX_GET, "FLAG_SYS_POKEDEX_GET", FLAG_SYS_POKEDEX_GET);
HASEP_EXPORT_VALUE(MON_DATA_MOVE1, "MON_DATA_MOVE1", MON_DATA_MOVE1);
HASEP_EXPORT_VALUE(MON_DATA_IS_EGG, "MON_DATA_IS_EGG", MON_DATA_IS_EGG);
HASEP_EXPORT_VALUE(MAPSEC_NONE, "MAPSEC_NONE", MAPSEC_NONE);

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
STATIC_ASSERT(offsetof(struct SaveBlock2, encryptionKey) == 0xAC, HasepLayoutSaveBlock2Key);
STATIC_ASSERT(offsetof(struct PartyMenu, slotId) == 0x09, HasepLayoutPartyMenuSlot);
