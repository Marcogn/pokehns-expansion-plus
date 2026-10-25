#ifndef GUARD_TCLONER_H
#define GUARD_TCLONER_H

bool32 TClonerIsRewardDue(void);
bool32 TClonerAllRewardsClaimed(void);
void TClonerRecordCatch(u16 trainerId);
void TClonerGetNextReward(void);
void TClonerClaimItemReward(void);
void TClonerGiveRewardMon(void);

#endif // GUARD_TCLONER_H
