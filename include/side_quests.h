#ifndef GUARD_SIDE_QUESTS_H
#define GUARD_SIDE_QUESTS_H

// Side quests (docs/plans/side-quests.md). What every catch tells them, in a
// battle and with a Ball thrown in the field: the quests count from here, so
// both ways of catching agree on what a catch is.

struct SideQuestCatch
{
    u16 species;          // as stored: SPECIES_UNOWN for every wild Unown letter
    u16 ball;             // ITEM_*
    u32 personality;
    u8 overworld:1;       // a Ball thrown in the field (overworld_catch.c)
    u8 unnoticed:1;       // field: the Pokémon was not aware of the player
    u8 firstTurn:1;       // battle: thrown on the first turn
    u8 trainerMon:1;      // battle: a Trainer's Pokémon taken with the TcloneR
    u8 thrownFromLand:1;  // field: the player was not surfing
    u8 targetOnWater:1;   // field: the Pokémon was on a water tile
    u8 distance;          // field: tiles from the player to the Pokémon (1-5); battle: 0
};

void SideQuests_OnCatch(const struct SideQuestCatch *caught);

// Unown letters caught (quest H): one bit per letter, A = bit 0.
u32 SideQuests_GetUnownLetter(u32 species, u32 personality);
bool32 SideQuests_IsUnownLetterCaught(u32 letter);
u32 SideQuests_CountUnownLetters(void);
void SideQuests_SeedUnownLetters(void);

// The SAVE_VERSION 16 migration: the vars and flags the quests took.
void SideQuests_ClearVarsAndFlags(void);

#endif // GUARD_SIDE_QUESTS_H
