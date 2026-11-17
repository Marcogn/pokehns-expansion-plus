#ifndef GUARD_OVERWORLD_CATCH_H
#define GUARD_OVERWORLD_CATCH_H

// Hasep: throwing a Ball (or a party Pokémon) at a visible wild Pokémon in the
// overworld. Plan and decisions: docs/plans/overworld-catch.md.

#define CAPTURE_GUARANTEED -1

// What the capture formula needs to know. In battle it is filled from the
// battlers (ComputeCaptureOdds, battle_script_commands.c), in the overworld
// from the object and the party (overworld_catch.c).
struct CaptureParams
{
    u16 species;
    u16 hp;
    u16 maxHP;
    u8 level;
    u8 gender;
    u32 status1;
    enum Type types[3];
    bool8 comatose;         // Dream Ball
    u16 playerSpecies;      // Love Ball
    u8 playerLevel;         // Level Ball, missing badge malus
    u8 playerGender;        // Love Ball
    u16 ballItem;
    u16 turnCounter;        // Timer and Quick Ball
    bool8 quickBallSpent;   // overworld only: no Quick Ball bonus although no turn has passed
    bool8 fishing;          // Lure and Dive Ball
    bool8 surfing;          // Dive Ball
    bool8 useSafariFactor;
    u8 safariCatchFactor;
};

u32 ComputeCaptureOddsFromParams(const struct CaptureParams *p);
u32 ComputeBallShakeOdds(u32 odds);

// Party slot that leads the next wild battle, PARTY_SIZE for none. Set by a
// thrown Pokémon, read and cleared by SetBattlePartyIds.
extern u8 gOverworldThrowLeadSlot;

void OverworldCatch_Update(void);
bool32 OverworldCatch_HandleLButton(u16 newKeys, u16 heldKeys);
bool32 OverworldCatch_TryThrow(void);
bool32 OverworldCatch_TryStartResults(void);

#endif // GUARD_OVERWORLD_CATCH_H
