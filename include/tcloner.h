#ifndef GUARD_TCLONER_H
#define GUARD_TCLONER_H

bool32 TClonerIsRewardDue(void);
bool32 TClonerAllRewardsClaimed(void);
u32 TClonerGetNextRewardClones(void);
void TClonerRecordCatch(u16 trainerId);
void TClonerGetNextReward(void);
void TClonerClaimItemReward(void);
void TClonerGiveRewardMon(void);

#endif // GUARD_TCLONER_H
