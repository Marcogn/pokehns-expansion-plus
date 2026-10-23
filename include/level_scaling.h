#ifndef GUARD_LEVEL_SCALING_H
#define GUARD_LEVEL_SCALING_H

#include "config/level_scaling.h"

enum LevelScalingMode
{
    LEVEL_SCALING_NONE,
    LEVEL_SCALING_TO_LEVEL_CAP,
    LEVEL_SCALING_PARTY_AVG,
    LEVEL_SCALING_PARTY_HIGHEST,
    LEVEL_SCALING_PARTY_LOWEST,
};

#if B_LEVEL_SCALING_ENABLED

bool32 IsTrainerLevelScalingEnabled(void);
bool32 IsWildLevelScalingEnabled(void);

// Trainer parties (battle_main.c). Returns 0 when this party is not scaled.
u32 GetTrainerScaledAceLevel(u16 trainerId, struct Pokemon *party, u32 battleTypeFlags, u32 authoredAceLevel);
u32 GetTrainerScaledMonLevel(u32 scaledAceLevel, u32 authoredAceLevel, u32 authoredLevel);
u16 EvolveSpeciesForLevel(u16 species, u8 targetLevel);
u32 GetTrainerScalingMoneyBonusPercent(void);

// Wild Pokémon: tableLevel is what the encounter table rolled. Never below
// it, never above the level cap nor B_WILD_SCALING_MAX_ABOVE_TABLE above it.
u8 GetWildScaledLevel(u8 tableLevel);
// setwildbattle: the level cap, never below the authored level; legendaries,
// training dummies and the Bug-Catching Contest keep the authored level.
u8 GetScriptedWildScaledLevel(u16 species, u8 level);

#else

static inline bool32 IsTrainerLevelScalingEnabled(void) { return FALSE; }
static inline bool32 IsWildLevelScalingEnabled(void) { return FALSE; }
static inline u32 GetTrainerScaledAceLevel(u16 trainerId, struct Pokemon *party, u32 battleTypeFlags, u32 authoredAceLevel) { return 0; }
static inline u32 GetTrainerScaledMonLevel(u32 scaledAceLevel, u32 authoredAceLevel, u32 authoredLevel) { return authoredLevel; }
static inline u16 EvolveSpeciesForLevel(u16 species, u8 targetLevel) { return species; }
static inline u32 GetTrainerScalingMoneyBonusPercent(void) { return 0; }
static inline u8 GetWildScaledLevel(u8 tableLevel) { return tableLevel; }
static inline u8 GetScriptedWildScaledLevel(u16 species, u8 level) { return level; }

#endif // B_LEVEL_SCALING_ENABLED

#endif // GUARD_LEVEL_SCALING_H
