#include "global.h"
#include "tcloner.h"
#include "event_data.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/items.h"
#include "constants/opponents.h"
#include "constants/species.h"

// TcloneR (docs/plans/tcloner.md). Carrying the key item lets a Ball work on a
// Trainer's Pokemon (CanCatchTrainerMon); this file keeps the score and pays
// it out. The scientist's scripts call the specials below through
// EventScript_TClonerRewards, which shows one message per reward.

struct TClonerReward
{
    u16 clones;   // VAR_TCLONER_COUNT needed
    u16 item;     // ITEM_NONE for a Pokemon
    u16 quantity;
    u16 species;
    u8 level;
};

// The reward table of the plan. The last tier is three steps, so that full PC
// Boxes hold back only the Pokemon that does not fit and the others arrive.
static const struct TClonerReward sRewards[TCLONER_REWARD_STEPS] =
{
    { .clones =  10, .item = ITEM_QUICK_BALL, .quantity = 20 },
    { .clones =  20, .item = ITEM_NUGGET,     .quantity = 10 },
    { .clones =  30, .item = ITEM_ULTRA_BALL, .quantity = 20 },
    { .clones =  40, .item = ITEM_MAX_REPEL,  .quantity = 15 },
    { .clones =  50, .item = ITEM_RARE_CANDY, .quantity = 10 },
    { .clones =  60, .item = ITEM_BIG_NUGGET, .quantity = 10 },
    { .clones =  70, .species = SPECIES_TOTODILE,  .level = 5 },
    { .clones =  80, .species = SPECIES_CHIKORITA, .level = 5 },
    { .clones =  90, .species = SPECIES_CYNDAQUIL, .level = 5 },
    { .clones = 100, .species = SPECIES_WEEZING,   .level = 50 },
    { .clones = 100, .species = SPECIES_ARBOK,     .level = 50 },
    { .clones = 100, .species = SPECIES_MEOWTH,    .level = 50 },
};

#if IS_HNS
STATIC_ASSERT(TRAINERS_COUNT <= TCLONER_TRAINER_FLAGS_BYTES * 8, TClonerTrainerFlagsFitHnsTrainers);
#endif

// Called when a Trainer battle ends with a catch, with the Trainer the caught
// Pokemon belonged to. Each Trainer counts once: their team comes back every
// battle, so counting every catch would let one Youngster pay out everything.
// Catching from them again still works, it just scores nothing. A rematch tier
// is a separate Trainer id with a new team, so it counts on its own.
void TClonerRecordCatch(u16 trainerId)
{
    u8 *flags = gSaveBlock2Ptr->pokedex.tclonerTrainerFlags;
    u16 count;

    if (trainerId == TRAINER_NONE || trainerId >= TCLONER_TRAINER_FLAGS_BYTES * 8)
        return;
    if (flags[trainerId / 8] & (1 << (trainerId % 8)))
        return;

    flags[trainerId / 8] |= 1 << (trainerId % 8);
    count = VarGet(VAR_TCLONER_COUNT);
    if (count < USHRT_MAX)
        VarSet(VAR_TCLONER_COUNT, count + 1);
}

static const struct TClonerReward *GetDueReward(void)
{
    u16 step = VarGet(VAR_TCLONER_REWARDS);

    if (step >= TCLONER_REWARD_STEPS || VarGet(VAR_TCLONER_COUNT) < sRewards[step].clones)
        return NULL;
    return &sRewards[step];
}

static void AdvanceRewardStep(void)
{
    VarSet(VAR_TCLONER_REWARDS, VarGet(VAR_TCLONER_REWARDS) + 1);
}

// Quest book (src/data/quests.h): is a reward due, and have all been handed out.
bool32 TClonerIsRewardDue(void)
{
    return GetDueReward() != NULL;
}

bool32 TClonerAllRewardsClaimed(void)
{
    return VarGet(VAR_TCLONER_REWARDS) >= TCLONER_REWARD_STEPS;
}

// The clone count the next reward wants, 0 once every reward is claimed (the
// quest book's TcloneR text).
u32 TClonerGetNextRewardClones(void)
{
    u16 step = VarGet(VAR_TCLONER_REWARDS);

    return step < TCLONER_REWARD_STEPS ? sRewards[step].clones : 0;
}

// special: what the next reward is, in VAR_RESULT (TCLONER_REWARD_*). An item
// comes back in VAR_0x8000/VAR_0x8001 for giveitem, which shows the standard
// "Obtained" message; the script then claims it with TClonerClaimItemReward,
// so a full Bag leaves the step pending.
void TClonerGetNextReward(void)
{
    const struct TClonerReward *reward = GetDueReward();

    if (VarGet(VAR_TCLONER_REWARDS) >= TCLONER_REWARD_STEPS)
    {
        gSpecialVar_Result = TCLONER_REWARD_ALL_CLAIMED;
    }
    else if (reward == NULL)
    {
        gSpecialVar_Result = TCLONER_REWARD_NOT_YET;
    }
    else if (reward->item != ITEM_NONE)
    {
        gSpecialVar_0x8000 = reward->item;
        gSpecialVar_0x8001 = reward->quantity;
        gSpecialVar_Result = TCLONER_REWARD_ITEM;
    }
    else
    {
        gSpecialVar_Result = TCLONER_REWARD_MON;
    }
}

// special: the item of the current step went into the Bag.
void TClonerClaimItemReward(void)
{
    const struct TClonerReward *reward = GetDueReward();

    if (reward != NULL && reward->item != ITEM_NONE)
        AdvanceRewardStep();
}

// special: sends the Pokemon of the current step, shiny, to the PC, even with
// room in the party (the plan: rewards always go to the Boxes). VAR_RESULT is
// TCLONER_MON_SENT_TO_PC with the species name in STR_VAR_1, or
// TCLONER_MON_BOXES_FULL with the step left pending.
void TClonerGiveRewardMon(void)
{
    const struct TClonerReward *reward = GetDueReward();
    struct Pokemon mon;
    bool32 isShiny = TRUE;

    gSpecialVar_Result = TCLONER_MON_BOXES_FULL;
    if (reward == NULL || reward->item != ITEM_NONE)
        return;

    CreateRandomMon(&mon, reward->species, reward->level);
    SetMonData(&mon, MON_DATA_IS_SHINY, &isShiny);
    if (CopyMonToPC(&mon) == MON_CANT_GIVE)
        return;

    HandleSetPokedexFlagFromMon(&mon, FLAG_SET_SEEN);
    HandleSetPokedexFlagFromMon(&mon, FLAG_SET_CAUGHT);
    StringCopy(gStringVar1, GetSpeciesName(reward->species));
    AdvanceRewardStep();
    gSpecialVar_Result = TCLONER_MON_SENT_TO_PC;
}
