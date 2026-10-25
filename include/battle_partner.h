#ifndef BATTLE_PARTNER_H
#define BATTLE_PARTNER_H

#include "difficulty.h"
#include "event_data.h"
#include "constants/battle_partner.h"

void FillPartnerParty(u16 trainerId);
void FillRedsPathParty(u32 challenge);

// What the quest book reads (src/reds_path.c).
bool32 RedsPath_IsIntroSeen(void);
bool32 RedsPath_IsChallengeWon(u32 challenge);
u32 RedsPath_GetPrizeWaiting(void);
u32 RedsPath_GetNextChallenge(void);

// The eight Leaders of Red's Path, TRAINER_REDS_PATH_BROCK_HNS onwards. They
// are fought as written: no level scaling, no TcloneR.
static inline bool32 IsRedsPathTrainer(u16 trainerId)
{
#if IS_HNS
    return trainerId >= TRAINER_REDS_PATH_BROCK_HNS && trainerId <= TRAINER_REDS_PATH_GIOVANNI_HNS;
#else
    return FALSE;
#endif
}

// TRUE from just before Red's team is loaded until the player's own party is
// back (FLAG_REDS_PATH_BATTLE, set and cleared by RedsPath_EventScript_Battle).
// The challenge is a replay of Red's journey as it was, so the randomizer and
// the challenge menu's rules stay out of it: the player's settings apply to
// the player's own Pokemon, and these are not theirs.
static inline bool32 IsRedsPathBattle(void)
{
#if IS_HNS
    return FlagGet(FLAG_REDS_PATH_BATTLE);
#else
    return FALSE;
#endif
}

#endif // BATTLE_PARTNER_H
