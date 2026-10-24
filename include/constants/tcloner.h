#ifndef GUARD_CONSTANTS_TCLONER_H
#define GUARD_CONSTANTS_TCLONER_H

// TcloneR (docs/plans/tcloner.md). Shared by src/tcloner.c and the scripts.

// A reward step comes due every TCLONER_CLONES_PER_TIER clones. The last tier
// hands out three Pokemon, one step each, so a full PC can hold back just the
// one that does not fit.
#define TCLONER_CLONES_PER_TIER 10
#define TCLONER_REWARD_STEPS    12

// Result of TClonerGetNextReward, in VAR_RESULT.
#define TCLONER_REWARD_ITEM        0 // VAR_0x8000 = item, VAR_0x8001 = quantity: giveitem, then TClonerClaimItemReward
#define TCLONER_REWARD_MON         1 // call TClonerGiveRewardMon
#define TCLONER_REWARD_NOT_YET     2 // not enough clones for the next step
#define TCLONER_REWARD_ALL_CLAIMED 3

// Result of TClonerGiveRewardMon, in VAR_RESULT.
#define TCLONER_MON_SENT_TO_PC 0 // STR_VAR_1 = species name
#define TCLONER_MON_BOXES_FULL 1

// One bit per HnS Trainer id (1-660, TRAINERS_COUNT_HNS 661) in the space the
// Pokedex struct kept from Emerald's dex flags. See struct Pokedex. Was 82
// bytes until Red's Path added Trainers 653-660; the SAVE_VERSION 10 migration
// zeroes the 83rd. 22 bytes of the old filler were free, 21 still are.
#define TCLONER_TRAINER_FLAGS_BYTES 83

#endif // GUARD_CONSTANTS_TCLONER_H
