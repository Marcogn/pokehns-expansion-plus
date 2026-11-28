#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "side_quests.h"
#include "constants/flags.h"
#include "constants/species.h"
#include "constants/vars.h"

// Side quests (docs/plans/side-quests.md). Phase 1 lays the ground the
// quests count on: one entry point for every catch, called by the battle
// (Cmd_givecaughtmon) and by the field throw (overworld_catch.c) once the
// Pokémon is really the player's, and the Unown letters caught.

// The letter mask has a bit per letter, and the quests read it as one.
STATIC_ASSERT(NUM_UNOWN_FORMS <= 32, SideQuestsUnownLettersFitMask);
STATIC_ASSERT(SPECIES_UNOWN_QUESTION - SPECIES_UNOWN_B + 2 == NUM_UNOWN_FORMS, SideQuestsUnownFormsContiguous);

#define NOT_UNOWN 0xFF

#if IS_HNS
// Old Emerald vars no HnS build writes (vars_hns.h). Seven of them are the
// Mystery Gift block, which ClearMysteryGiftVars zeroes when Mystery Gift is
// built in: that would wipe the quests.
#if FREE_MYSTERY_GIFT == FALSE
#error "The side quests use VAR_GIFT_UNUSED_1..7 (0x40DE-0x40E4): move them before turning Mystery Gift on"
#endif

static const u16 sSideQuestVars[] =
{
    VAR_SQ_THROW_MASTER,
    VAR_SQ_NINJA,
    VAR_SQ_OAK_STARTERS,
    VAR_SQ_KURT_BALLS,
    VAR_SQ_JOHTO_DEX,
    VAR_SQ_TYPE_COLLECTOR,
    VAR_SQ_UNOWN_REPORT,
    VAR_SQ_DAY_CARE_BASE,
    VAR_SQ_REMATCHES,
    VAR_SQ_DEXNAV_CENSUS,
};
#endif

void SideQuests_ClearVarsAndFlags(void)
{
#if IS_HNS
    for (u32 i = 0; i < ARRAY_COUNT(sSideQuestVars); i++)
        VarSet(sSideQuestVars[i], 0);
    for (u32 i = 0; i < SIDE_QUEST_FLAGS_COUNT; i++)
        FlagClear(SIDE_QUEST_FLAGS_START + i);
#endif
}

// Wild Unown are created as SPECIES_UNOWN with the letter in the personality
// (RANDOM_UNOWN_LETTER, wild_encounter.c), the way the Pokédex and the icons
// read it; a Pokémon given as one of the form species (SPECIES_UNOWN_B...) is
// that letter whatever its personality.
u32 SideQuests_GetUnownLetter(u32 species, u32 personality)
{
    if (species == SPECIES_UNOWN)
        return GetUnownLetterByPersonality(personality);
    if (species >= SPECIES_UNOWN_B && species <= SPECIES_UNOWN_QUESTION)
        return species - SPECIES_UNOWN_B + 1;
    return NOT_UNOWN;
}

static void RecordUnownLetter(u32 species, u32 personality)
{
    u32 letter = SideQuests_GetUnownLetter(species, personality);

    if (letter < NUM_UNOWN_FORMS)
        gSaveBlock3Ptr->unownLettersCaught |= 1u << letter;
}

bool32 SideQuests_IsUnownLetterCaught(u32 letter)
{
    return letter < NUM_UNOWN_FORMS && (gSaveBlock3Ptr->unownLettersCaught >> letter) & 1;
}

u32 SideQuests_CountUnownLetters(void)
{
    u32 count = 0;

    for (u32 letter = 0; letter < NUM_UNOWN_FORMS; letter++)
        count += SideQuests_IsUnownLetterCaught(letter);
    return count;
}

// The mask is new in SAVE_VERSION 16: a save from before has caught Unown
// the mask never saw. The ones the player still has, in the party and in
// the PC, are the best record there is of them (save.c's migration).
void SideQuests_SeedUnownLetters(void)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            RecordUnownLetter(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES), GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY));
    }
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (u32 pos = 0; pos < IN_BOX_COUNT; pos++)
        {
            struct BoxPokemon *boxMon = GetBoxedMonPtr(box, pos);

            if (!GetBoxMonData(boxMon, MON_DATA_IS_EGG))
                RecordUnownLetter(GetBoxMonData(boxMon, MON_DATA_SPECIES), GetBoxMonData(boxMon, MON_DATA_PERSONALITY));
        }
    }
}

void SideQuests_OnCatch(const struct SideQuestCatch *caught)
{
    RecordUnownLetter(caught->species, caught->personality);
}
