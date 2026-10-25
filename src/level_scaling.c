#include "global.h"
#include "battle_partner.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "bug_contest.h"
#include "caps.h"
#include "event_data.h"
#include "level_scaling.h"
#include "pokemon.h"
#include "random.h"
#include "constants/battle.h"
#include "constants/battle_pyramid.h"
#include "constants/trainers.h"

// Engine ported from Soulgold (src/level_scaling.c), adapted to the decisions
// in docs/plans/level-scaling.md. What differs from Soulgold and why:
// - no mon is ever scaled below its authored level, so the devolution helpers
//   and their override table are not needed and not ported;
// - evolutions stop at any species with more than one evolution target, not
//   only more than one level-up target (Slowpoke stays Slowpoke);
// - no party level cache: Soulgold only refreshed it on trainer battles, so a
//   wild encounter could read the party as it was at the last trainer;
// - wild Pokémon stop at the level cap and at B_WILD_SCALING_MAX_ABOVE_TABLE
//   above their table, so routes left behind do not follow the party.

#if B_LEVEL_SCALING_ENABLED

bool32 IsTrainerLevelScalingEnabled(void)
{
    return gSaveBlock3Ptr->soundAndFieldOptions.levelScaling;
}

bool32 IsWildLevelScalingEnabled(void)
{
    if (!gSaveBlock3Ptr->soundAndFieldOptions.levelScaling)
        return FALSE;
    if (GetBugContestFlag())
        return FALSE;
    if (InBattlePike() || CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
        return FALSE;
    return TRUE;
}

// ============================================================================
// Reference level
// ============================================================================

static bool32 IsPartySlotCounted(struct Pokemon *mon)
{
    u32 species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);

    return species != SPECIES_NONE && species != SPECIES_EGG;
}

static u32 GetPlayerPartyHighestLevel(void)
{
    u32 i, level, highest = 0;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (!IsPartySlotCounted(&gPlayerParty[i]))
            continue;
        level = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
        if (level > highest)
            highest = level;
    }

    return highest;
}

static u32 GetPlayerPartyLowestLevel(void)
{
    u32 i, level, lowest = 0;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (!IsPartySlotCounted(&gPlayerParty[i]))
            continue;
        level = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
        if (lowest == 0 || level < lowest)
            lowest = level;
    }

    return lowest;
}

// Mons B_LEVEL_SCALING_PASSENGER_GAP or more levels below the strongest are
// left out: party 41, 40, 40, 39, 38 and a fresh Lv5 Abra averages 39, not 33.
static u32 GetPlayerPartyAverageLevel(void)
{
    u32 i, level, total = 0, count = 0;
    u32 highest = GetPlayerPartyHighestLevel();

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (!IsPartySlotCounted(&gPlayerParty[i]))
            continue;
        level = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
        if (highest - level >= B_LEVEL_SCALING_PASSENGER_GAP)
            continue;
        total += level;
        count++;
    }

    if (count == 0)
        return 0;
    return total / count;
}

// 0 means "nothing to scale to", and the caller keeps the authored level.
static u32 GetPlayerReferenceLevel(void)
{
    switch (B_LEVEL_SCALING_MODE)
    {
    case LEVEL_SCALING_TO_LEVEL_CAP:
        return GetLevelScalingCap();
    case LEVEL_SCALING_PARTY_AVG:
        return GetPlayerPartyAverageLevel();
    case LEVEL_SCALING_PARTY_HIGHEST:
        return GetPlayerPartyHighestLevel();
    case LEVEL_SCALING_PARTY_LOWEST:
        return GetPlayerPartyLowestLevel();
    default:
        return 0;
    }
}

static u32 ClampToValidLevel(s32 level)
{
    if (level < MIN_LEVEL)
        return MIN_LEVEL;
    if (level > MAX_LEVEL)
        return MAX_LEVEL;
    return level;
}

// ============================================================================
// Trainers
// ============================================================================

// Same seed as Soulgold: fixed per save and trainer, so the same trainer always
// brings the same levels for the same party.
static u32 GetDeterministicTrainerVariation(u16 trainerId, u32 variation)
{
    u32 seed;

    if (variation == 0)
        return 0;

    seed = T1_READ_32(gSaveBlock2Ptr->playerTrainerId);
    seed ^= trainerId * 1103515245;
    seed ^= seed >> 16;
    seed *= 2246822519U;
    seed ^= seed >> 13;

    return seed % (variation + 1);
}

static bool32 IsTrainerExcludedFromScaling(u16 trainerId)
{
    switch (trainerId)
    {
    case TRAINER_NONE:
#if IS_HNS
    // The first rival battle, in Cherrygrove: his starter is a fixed Lv5.
    case TRAINER_RIVAL_CHIKORITA_1_HNS:
    case TRAINER_RIVAL_CYNDAQUIL_1_HNS:
    case TRAINER_RIVAL_TOTODILE_1_HNS:
#endif
        return TRUE;
    }
    // Red's Path replays the Leaders as Red met them, levels included.
    if (IsRedsPathTrainer(trainerId))
        return TRUE;

    return FALSE;
}

// Returns the level of the ace (the party's highest authored level), or 0 to
// leave the whole party as authored. Never below authoredAceLevel.
u32 GetTrainerScaledAceLevel(u16 trainerId, struct Pokemon *party, u32 battleTypeFlags, u32 authoredAceLevel)
{
    u32 reference;
    s32 level;

    if (!IsTrainerLevelScalingEnabled())
        return 0;
    // The in-game partner of a multi battle is built into the player's party.
    if (party >= gPlayerParty && party < gPlayerParty + PARTY_SIZE)
        return 0;
    if (battleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED | BATTLE_TYPE_FIRST_BATTLE | BATTLE_TYPE_CATCH_TUTORIAL))
        return 0;
    if (IsTrainerExcludedFromScaling(trainerId))
        return 0;

    reference = GetPlayerReferenceLevel();
    if (reference == 0)
        return 0;

    level = (s32)reference + B_TRAINER_SCALING_LEVEL_AUGMENT;
    level -= GetDeterministicTrainerVariation(trainerId, B_TRAINER_SCALING_LEVEL_VARIATION);
    if (level < (s32)authoredAceLevel)
        level = authoredAceLevel;

    return ClampToValidLevel(level);
}

// The other mons keep their authored distance from the ace, up to
// B_TRAINER_SCALING_MAX_AUTHORED_GAP, and never drop below their own level.
u32 GetTrainerScaledMonLevel(u32 scaledAceLevel, u32 authoredAceLevel, u32 authoredLevel)
{
    u32 gap = authoredAceLevel - authoredLevel;
    s32 level;

    if (scaledAceLevel == 0)
        return authoredLevel;

    if (gap > B_TRAINER_SCALING_MAX_AUTHORED_GAP)
        gap = B_TRAINER_SCALING_MAX_AUTHORED_GAP;
    level = (s32)scaledAceLevel - (s32)gap;
    if (level < (s32)authoredLevel)
        level = authoredLevel;

    return ClampToValidLevel(level);
}

// Evolve through plain level evolutions only while the species has exactly one
// place to go: Pidgey Lv40 becomes Pidgeot, but Slowpoke (Slowbro by level,
// Slowking by trade), Kirlia, Eevee and Poliwhirl stay as written.
u16 EvolveSpeciesForLevel(u16 species, u8 targetLevel)
{
    u32 iterations, i;

    if (!B_TRAINER_SCALING_EVOLVE)
        return species;

    for (iterations = 0; iterations < 10; iterations++)
    {
        const struct Evolution *evolutions = GetSpeciesEvolutions(species);
        u16 target = SPECIES_NONE;
        bool32 levelReached = FALSE;

        if (evolutions == NULL)
            break;

        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            if (!IsSpeciesEnabled(evolutions[i].targetSpecies))
                continue;
            if (target != SPECIES_NONE && target != evolutions[i].targetSpecies)
                return species;
            if ((evolutions[i].method != EVO_LEVEL && evolutions[i].method != EVO_LEVEL_BATTLE_ONLY)
             || evolutions[i].param == 0
             || evolutions[i].params != NULL)
                return species;

            target = evolutions[i].targetSpecies;
            if (targetLevel >= evolutions[i].param)
                levelReached = TRUE;
        }

        if (target == SPECIES_NONE || !levelReached)
            break;
        species = target;
    }

    return species;
}

u32 GetTrainerScalingMoneyBonusPercent(void)
{
    if (!IsTrainerLevelScalingEnabled())
        return 0;
    return GetLevelScalingCap() / B_TRAINER_SCALING_MONEY_CAP_STEP;
}

// ============================================================================
// Wild Pokémon
// ============================================================================

u8 GetWildScaledLevel(u8 tableLevel)
{
    u32 reference, cap;
    s32 level;

    if (!IsWildLevelScalingEnabled())
        return tableLevel;

    reference = GetPlayerReferenceLevel();
    if (reference == 0)
        return tableLevel;
    // Only an overlevelled party (LEVEL CAP off, or a mon received above the
    // cap) gets past the cap: with LEVEL CAP on this never triggers.
    cap = GetLevelScalingCap();
    if (reference > cap)
        reference = cap;

    level = (s32)reference + B_WILD_SCALING_LEVEL_AUGMENT;
    level -= Random() % (B_WILD_SCALING_LEVEL_VARIATION + 1);
    if (B_WILD_SCALING_MAX_ABOVE_TABLE > 0 && level > tableLevel + B_WILD_SCALING_MAX_ABOVE_TABLE)
        level = tableLevel + B_WILD_SCALING_MAX_ABOVE_TABLE;
    if (level < tableLevel)
        level = tableLevel;

    return ClampToValidLevel(level);
}

u8 GetScriptedWildScaledLevel(u16 species, u8 level)
{
    u32 cap;

    if (!IsWildLevelScalingEnabled())
        return level;
    if (gSpeciesInfo[species].isRestrictedLegendary
     || gSpeciesInfo[species].isSubLegendary
     || gSpeciesInfo[species].isMythical
     || gSpeciesInfo[species].isUltraBeast)
        return level;
    // Uncatchable set pieces: the Saffron Dojo EV trainers (Lv1) and the
    // Route 41 Tentacruel attacks (Lv100) are not meant to be fair fights.
    if (FlagGet(FLAG_NO_WILD_CATCHING))
        return level;

    cap = GetLevelScalingCap();
    if (cap < level)
        return level;
    return ClampToValidLevel(cap);
}

#endif // B_LEVEL_SCALING_ENABLED
