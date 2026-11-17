#ifndef GUARD_NUZLOCKE_H
#define GUARD_NUZLOCKE_H

#include "global.h"

#define NUZLOCKE_NUM_ZONES 96
#define NUZLOCKE_FLAG_BYTES ((NUZLOCKE_NUM_ZONES + 7) / 8)

bool8 IsNuzlockeActive(void);
bool8 IsNuzlockeCaptureSuspended(void);
bool8 IsNuzlockeEasyActive(void);
bool8 IsNuzlockeNicknamingActive(void);

u8 NuzlockeFlagSet(u16 mapsec);
u8 NuzlockeFlagClear(u16 mapsec);
u8 NuzlockeFlagGet(u16 mapsec);

void NuzlockeDeletePartyMon(u8 position);
void NuzlockeDeletePartyMonOption(u8 position);
void NuzlockeDeleteFaintedPartyPokemon(void);

u8 NuzlockeGetCurrentRegionMapSectionId(void);

extern u8 NuzlockeIsCaptureBlocked;
extern u8 NuzlockeIsSpeciesClauseActive;
extern u8 OneTypeChallengeCaptureBlocked;

void SetNuzlockeChecks(void);
struct CaptureChallengeChecks
{
    u8 oneTypeBlocked;  // off-type under the One Type Challenge
    u8 zoneBlocked;     // Nuzlocke: this area's encounter is used
    u8 speciesClause;   // Nuzlocke Species Clause: 1 = evolution line caught, 2 = this species caught
};
void GetCaptureChallengeChecks(u16 species, bool32 isShiny, bool32 isFateful, struct CaptureChallengeChecks *checks);
u8 NuzlockeIsCaptureBlockedBySpeciesClause(u16 species);

#endif // GUARD_NUZLOCKE_H
