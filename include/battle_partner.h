#ifndef BATTLE_PARTNER_H
#define BATTLE_PARTNER_H

#include "difficulty.h"
#include "constants/battle_partner.h"

void FillPartnerParty(u16 trainerId);
void FillRedsPathParty(u32 challenge);

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

#endif // BATTLE_PARTNER_H
