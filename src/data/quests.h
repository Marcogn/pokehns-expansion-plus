// Quest book entries, indexed by QUEST_* (include/constants/quests.h).
// Append only: the saved favourite and seen bits follow the id.
//
// A quest reads its progress from one of three places (QUEST_SOURCE_*):
//   .source = QUEST_SOURCE_VAR,  .var = VAR_X, .doneValue = N
//   .source = QUEST_SOURCE_FUNC, .getProgress = SomeFunc
//   .source = QUEST_SOURCE_OWN,  .ownSlot = QUEST_OWN_SLOT_X
// and its subquests, if any, from a function or from their own bits:
//   .numSubquests = N, .subquestSource = QUEST_SOURCE_FUNC, .isSubquestDone = SomeFunc
//   .numSubquests = N, .subquestSource = QUEST_SOURCE_OWN,  .subquestBitStart = QUEST_X_SUBQUEST_START


// Throwaway quests for looking at the screen; see QUEST_BOOK_TEST_QUESTS.
// Their data comes first because the table points at it.
#if QUEST_BOOK_TEST_QUESTS
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/species.h"
#include "constants/vars.h"

static const u8 sText_TestStepsName[] = _("Test: Scientists");
static const u8 sText_TestStepsDesc0[] = _("Nothing to do yet.\nThis is step 0.");
static const u8 sText_TestStepsDesc1[] = _("Find the first scientist.\nHe is an object sprite.");
static const u8 sText_TestStepsDesc2[] = _("Bring a Poké Ball.\nThis step shows an item.");
static const u8 sText_TestStepsDesc3[] = _("Look for the Ralts.\nThis step shows a Pokémon.");
static const u8 sText_TestStepsDesc4[] = _("Last step before the end\nof the test quest.");
static const u8 sText_TestStepsDone[] = _("All done. The quest book\nshows this once completed.");
static const u8 sText_TestMapNewBark[] = _("New Bark Town");
static const u8 sText_TestMapRoute29[] = _("Route 29");

static const struct QuestStep sTestStepsSteps[] =
{
    [0] = { sText_TestStepsDesc0, sText_TestMapNewBark, OBJ_EVENT_GFX_PROF_OAK, QUEST_SPRITE_OBJECT },
    [1] = { sText_TestStepsDesc1, sText_TestMapNewBark, OBJ_EVENT_GFX_BILL, QUEST_SPRITE_OBJECT },
    [2] = { sText_TestStepsDesc2, sText_TestMapRoute29, ITEM_POKE_BALL, QUEST_SPRITE_ITEM },
    [3] = { sText_TestStepsDesc3, sText_TestMapRoute29, SPECIES_RALTS, QUEST_SPRITE_MON },
    [4] = { sText_TestStepsDesc4, NULL, OBJ_EVENT_GFX_RED, QUEST_SPRITE_OBJECT },
    [5] = { sText_TestStepsDesc4, NULL, OBJ_EVENT_GFX_RED, QUEST_SPRITE_OBJECT },
};

static const u8 sText_TestBadgesName[] = _("Test: Badges");
static const u8 sText_TestBadgesDesc[] = _("Collect the badges.\nPress {A_BUTTON} for the list.");
static const u8 sText_TestBadgesDone[] = _("Every badge found.");
static const u8 sText_TestBadgesMap[] = _("Johto");
static const u8 sText_TestFound[] = _("Won");

static const u8 sText_TestBadgeName0[] = _("Zephyr Badge");
static const u8 sText_TestBadgeName1[] = _("Hive Badge");
static const u8 sText_TestBadgeName2[] = _("Plain Badge");
static const u8 sText_TestBadgeName3[] = _("Fog Badge");
static const u8 sText_TestBadgeName4[] = _("Storm Badge");
static const u8 sText_TestBadgeName5[] = _("Mineral Badge");
static const u8 sText_TestBadgeName6[] = _("Glacier Badge");
static const u8 sText_TestBadgeName7[] = _("Rising Badge");
static const u8 sText_TestBadgeDesc[] = _("Beaten and won.\nNo more to say.");

static const struct QuestStep sTestBadgesSteps[] =
{
    { sText_TestBadgesDesc, sText_TestBadgesMap, OBJ_EVENT_GFX_LITTLE_GIRL, QUEST_SPRITE_OBJECT },
};

#define TEST_BADGE(n) { sText_TestBadgeName##n, sText_TestBadgeDesc, sText_TestBadgesMap, ITEM_POKE_BALL, QUEST_SPRITE_ITEM, sText_TestFound }
static const struct QuestSubquestInfo sTestBadgesSubquests[] =
{
    TEST_BADGE(0), TEST_BADGE(1), TEST_BADGE(2), TEST_BADGE(3),
    TEST_BADGE(4), TEST_BADGE(5), TEST_BADGE(6), TEST_BADGE(7),
};

static u32 CountTestBadges(void)
{
    u32 i, count = 0;

    for (i = 0; i < 8; i++)
    {
        if (FlagGet(FLAG_BADGE01_GET + i))
            count++;
    }
    return count;
}

static void GetTestBadgesProgress(struct QuestProgress *progress)
{
    u32 count = CountTestBadges();

    progress->step = 0;
    progress->state = count == 0 ? QUEST_STATE_UNLOCKED : count < 8 ? QUEST_STATE_ACTIVE : QUEST_STATE_COMPLETED;
}

static bool32 IsTestBadgeDone(u32 subquest)
{
    return FlagGet(FLAG_BADGE01_GET + subquest);
}
#endif // QUEST_BOOK_TEST_QUESTS

static const struct QuestInfo sQuests[QUEST_COUNT] =
{
#if QUEST_BOOK_TEST_QUESTS
    [QUEST_TEST_STEPS] =
    {
        .name = sText_TestStepsName,
        .source = QUEST_SOURCE_VAR,
        .numSteps = NELEMS(sTestStepsSteps),
        .var = VAR_TCLONER_STAGE,
        .doneValue = 5,
        .steps = sTestStepsSteps,
        .doneDesc = sText_TestStepsDone,
    },
    [QUEST_TEST_BADGES] =
    {
        .name = sText_TestBadgesName,
        .source = QUEST_SOURCE_FUNC,
        .numSteps = NELEMS(sTestBadgesSteps),
        .getProgress = GetTestBadgesProgress,
        .numSubquests = 8,
        .subquestSource = QUEST_SOURCE_FUNC,
        .isSubquestDone = IsTestBadgeDone,
        .steps = sTestBadgesSteps,
        .doneDesc = sText_TestBadgesDone,
        .subquestInfo = sTestBadgesSubquests,
    },
#endif
};
