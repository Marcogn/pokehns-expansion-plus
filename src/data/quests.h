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


#include "battle_partner.h"
#include "tcloner.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/vars.h"

// ---- Badges: one quest per region, one subquest per Gym ----
//
// The step is the first Gym still to do, in the order below. The game does
// not force that order (Kanto's is free, Johto's gates are per city), so it
// is a suggestion: beat them in another order and the step just moves on to
// whichever comes first in this list.

static const u8 sText_Won[] = _("Won");

static const u8 sText_JohtoName[] = _("Johto Badges");
static const u8 sText_JohtoDone[] = _("All eight Johto Badges are\nyours. Kanto is open now!");

static const u8 sText_JohtoStep0[] = _("Challenge Falkner at the\nViolet City Gym.");
static const u8 sText_JohtoStep1[] = _("Challenge Bugsy at the\nAzalea Town Gym.");
static const u8 sText_JohtoStep2[] = _("Challenge Whitney at the\nGoldenrod City Gym.");
static const u8 sText_JohtoStep3[] = _("Challenge Morty at the\nEcruteak City Gym.");
static const u8 sText_JohtoStep4[] = _("Challenge Chuck at the\nCianwood City Gym.");
static const u8 sText_JohtoStep5[] = _("Challenge Jasmine at the\nOlivine City Gym.");
static const u8 sText_JohtoStep6[] = _("Challenge Pryce at the\nMahogany Town Gym.");
static const u8 sText_JohtoStep7[] = _("The Gym opens with the Cianwood,\nOlivine and Mahogany Badges.");
static const u8 sText_JohtoStep8[] = _("Clair sends you to the\nDragon's Den.");

static const u8 sText_Violet[] = _("Violet City");
static const u8 sText_Azalea[] = _("Azalea Town");
static const u8 sText_Goldenrod[] = _("Goldenrod City");
static const u8 sText_Ecruteak[] = _("Ecruteak City");
static const u8 sText_Cianwood[] = _("Cianwood City");
static const u8 sText_Olivine[] = _("Olivine City");
static const u8 sText_Mahogany[] = _("Mahogany Town");
static const u8 sText_Blackthorn[] = _("Blackthorn City");
static const u8 sText_DragonsDen[] = _("Dragon's Den");

static const struct QuestStep sJohtoSteps[] =
{
    { sText_JohtoStep0, sText_Violet,     OBJ_EVENT_GFX_FALKNER_HNS,  QUEST_SPRITE_OBJECT },
    { sText_JohtoStep1, sText_Azalea,     OBJ_EVENT_GFX_BUGSY_HNS,    QUEST_SPRITE_OBJECT },
    { sText_JohtoStep2, sText_Goldenrod,  OBJ_EVENT_GFX_WHITNEY_HNS,  QUEST_SPRITE_OBJECT },
    { sText_JohtoStep3, sText_Ecruteak,   OBJ_EVENT_GFX_MORTY_HNS,    QUEST_SPRITE_OBJECT },
    { sText_JohtoStep4, sText_Cianwood,   OBJ_EVENT_GFX_CHUCK_HNS,    QUEST_SPRITE_OBJECT },
    { sText_JohtoStep5, sText_Olivine,    OBJ_EVENT_GFX_JASMINE_HNS,  QUEST_SPRITE_OBJECT },
    { sText_JohtoStep6, sText_Mahogany,   OBJ_EVENT_GFX_PRYCE_HNS,    QUEST_SPRITE_OBJECT },
    { sText_JohtoStep7, sText_Blackthorn, OBJ_EVENT_GFX_CLAIR_HNS,    QUEST_SPRITE_OBJECT },
    { sText_JohtoStep8, sText_DragonsDen, OBJ_EVENT_GFX_CLAIR_HNS,    QUEST_SPRITE_OBJECT },
};

// Clair's badge is not given by the Gym: beating her there sends the player
// to the Dragon's Den, where FLAG_BADGE08_GET is set. The Gym's own flag
// tells which of the two steps this is.
#define JOHTO_STEP_CLAIR_GYM   7
#define JOHTO_STEP_CLAIR_DEN   8

static const u16 sJohtoBadgeFlags[] =
{
    FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
    FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_BADGE07_GET, FLAG_BADGE08_GET,
};

static const u8 sText_JohtoBadge0[] = _("Zephyr Badge");
static const u8 sText_JohtoBadge1[] = _("Hive Badge");
static const u8 sText_JohtoBadge2[] = _("Plain Badge");
static const u8 sText_JohtoBadge3[] = _("Fog Badge");
static const u8 sText_JohtoBadge4[] = _("Storm Badge");
static const u8 sText_JohtoBadge5[] = _("Mineral Badge");
static const u8 sText_JohtoBadge6[] = _("Glacier Badge");
static const u8 sText_JohtoBadge7[] = _("Rising Badge");
static const u8 sText_JohtoBadgeDesc0[] = _("Falkner's Flying-type Gym.");
static const u8 sText_JohtoBadgeDesc1[] = _("Bugsy's Bug-type Gym.");
static const u8 sText_JohtoBadgeDesc2[] = _("Whitney's Normal-type Gym.");
static const u8 sText_JohtoBadgeDesc3[] = _("Morty's Ghost-type Gym.");
static const u8 sText_JohtoBadgeDesc4[] = _("Chuck's Fighting-type Gym.");
static const u8 sText_JohtoBadgeDesc5[] = _("Jasmine's Steel-type Gym.");
static const u8 sText_JohtoBadgeDesc6[] = _("Pryce's Ice-type Gym.");
static const u8 sText_JohtoBadgeDesc7[] = _("Clair's Dragon-type Gym.");

static const struct QuestSubquestInfo sJohtoSubquests[] =
{
    { sText_JohtoBadge0, sText_JohtoBadgeDesc0, sText_Violet,     OBJ_EVENT_GFX_FALKNER_HNS, QUEST_SPRITE_OBJECT, sText_Won },
    { sText_JohtoBadge1, sText_JohtoBadgeDesc1, sText_Azalea,     OBJ_EVENT_GFX_BUGSY_HNS,   QUEST_SPRITE_OBJECT, sText_Won },
    { sText_JohtoBadge2, sText_JohtoBadgeDesc2, sText_Goldenrod,  OBJ_EVENT_GFX_WHITNEY_HNS, QUEST_SPRITE_OBJECT, sText_Won },
    { sText_JohtoBadge3, sText_JohtoBadgeDesc3, sText_Ecruteak,   OBJ_EVENT_GFX_MORTY_HNS,   QUEST_SPRITE_OBJECT, sText_Won },
    { sText_JohtoBadge4, sText_JohtoBadgeDesc4, sText_Cianwood,   OBJ_EVENT_GFX_CHUCK_HNS,   QUEST_SPRITE_OBJECT, sText_Won },
    { sText_JohtoBadge5, sText_JohtoBadgeDesc5, sText_Olivine,    OBJ_EVENT_GFX_JASMINE_HNS, QUEST_SPRITE_OBJECT, sText_Won },
    { sText_JohtoBadge6, sText_JohtoBadgeDesc6, sText_Mahogany,   OBJ_EVENT_GFX_PRYCE_HNS,   QUEST_SPRITE_OBJECT, sText_Won },
    { sText_JohtoBadge7, sText_JohtoBadgeDesc7, sText_Blackthorn, OBJ_EVENT_GFX_CLAIR_HNS,   QUEST_SPRITE_OBJECT, sText_Won },
};

static u32 CountBadgeFlags(const u16 *flags, u32 count)
{
    u32 i, done = 0;

    for (i = 0; i < count; i++)
    {
        if (FlagGet(flags[i]))
            done++;
    }
    return done;
}

// The first Gym of the list without its badge, count if every one is won.
static u32 FirstMissingBadge(const u16 *flags, u32 count)
{
    u32 i;

    for (i = 0; i < count; i++)
    {
        if (!FlagGet(flags[i]))
            break;
    }
    return i;
}

static void GetJohtoProgress(struct QuestProgress *progress)
{
    u32 next = FirstMissingBadge(sJohtoBadgeFlags, NELEMS(sJohtoBadgeFlags));

    if (next == NELEMS(sJohtoBadgeFlags))
    {
        progress->state = QUEST_STATE_COMPLETED;
        return;
    }
    progress->state = QUEST_STATE_ACTIVE;
    progress->step = next;
    if (next == JOHTO_STEP_CLAIR_GYM && FlagGet(FLAG_DEFEATED_BLACKTHORN_GYM))
        progress->step = JOHTO_STEP_CLAIR_DEN;
}

static bool32 IsJohtoBadgeDone(u32 subquest)
{
    return subquest < NELEMS(sJohtoBadgeFlags) && FlagGet(sJohtoBadgeFlags[subquest]);
}

// Kanto: the Gyms in the order of the list below. Blue's badge (16) is last
// because he only takes challengers with the other fifteen: he is at
// Cinnabar Island until the player has talked to him, then at Viridian.
static const u8 sText_KantoName[] = _("Kanto Badges");
static const u8 sText_KantoDone[] = _("All eight Kanto Badges are\nyours. Nothing left to win.");

static const u8 sText_KantoStep0[] = _("Challenge Brock at the\nPewter City Gym.");
static const u8 sText_KantoStep1[] = _("Challenge Misty at the\nCerulean City Gym.");
static const u8 sText_KantoStep2[] = _("Challenge Lt. Surge at the\nVermilion City Gym.");
static const u8 sText_KantoStep3[] = _("Challenge Erika at the\nCeladon City Gym.");
static const u8 sText_KantoStep4[] = _("Challenge Janine at the\nFuchsia City Gym.");
static const u8 sText_KantoStep5[] = _("Challenge Sabrina at the\nSaffron City Gym.");
static const u8 sText_KantoStep6[] = _("Challenge Blaine at the\nSeafoam Islands Gym.");
static const u8 sText_KantoStep7[] = _("Find Blue on Cinnabar Island.\nHe wants all the other badges.");
static const u8 sText_KantoStep8[] = _("Blue is waiting at the\nViridian City Gym.");

static const u8 sText_Pewter[] = _("Pewter City");
static const u8 sText_Cerulean[] = _("Cerulean City");
static const u8 sText_Vermilion[] = _("Vermilion City");
static const u8 sText_Celadon[] = _("Celadon City");
static const u8 sText_Fuchsia[] = _("Fuchsia City");
static const u8 sText_Saffron[] = _("Saffron City");
static const u8 sText_Seafoam[] = _("Seafoam Islands");
static const u8 sText_Cinnabar[] = _("Cinnabar Island");
static const u8 sText_Viridian[] = _("Viridian City");

static const struct QuestStep sKantoSteps[] =
{
    { sText_KantoStep0, sText_Pewter,     OBJ_EVENT_GFX_BROCK_HNS,   QUEST_SPRITE_OBJECT },
    { sText_KantoStep1, sText_Cerulean,   OBJ_EVENT_GFX_MISTY_HNS,   QUEST_SPRITE_OBJECT },
    { sText_KantoStep2, sText_Vermilion,  OBJ_EVENT_GFX_SURGE_HNS,   QUEST_SPRITE_OBJECT },
    { sText_KantoStep3, sText_Celadon,    OBJ_EVENT_GFX_ERIKA_HNS,   QUEST_SPRITE_OBJECT },
    { sText_KantoStep4, sText_Fuchsia,    OBJ_EVENT_GFX_JANINE_HNS,  QUEST_SPRITE_OBJECT },
    { sText_KantoStep5, sText_Saffron,    OBJ_EVENT_GFX_SABRINA_HNS, QUEST_SPRITE_OBJECT },
    { sText_KantoStep6, sText_Seafoam,    OBJ_EVENT_GFX_BLAINE_HNS,  QUEST_SPRITE_OBJECT },
    { sText_KantoStep7, sText_Cinnabar,   OBJ_EVENT_GFX_BLUE_HNS,    QUEST_SPRITE_OBJECT },
    { sText_KantoStep8, sText_Viridian,   OBJ_EVENT_GFX_BLUE_HNS,    QUEST_SPRITE_OBJECT },
};

#define KANTO_STEP_BLUE_CINNABAR 7
#define KANTO_STEP_BLUE_VIRIDIAN 8

static const u16 sKantoBadgeFlags[] =
{
    FLAG_BADGE09_GET, FLAG_BADGE10_GET, FLAG_BADGE11_GET, FLAG_BADGE12_GET,
    FLAG_BADGE14_GET, FLAG_BADGE13_GET, FLAG_BADGE15_GET, FLAG_BADGE16_GET,
};

static const u8 sText_KantoBadge0[] = _("Boulder Badge");
static const u8 sText_KantoBadge1[] = _("Cascade Badge");
static const u8 sText_KantoBadge2[] = _("Thunder Badge");
static const u8 sText_KantoBadge3[] = _("Rainbow Badge");
static const u8 sText_KantoBadge4[] = _("Soul Badge");
static const u8 sText_KantoBadge5[] = _("Marsh Badge");
static const u8 sText_KantoBadge6[] = _("Volcano Badge");
static const u8 sText_KantoBadge7[] = _("Earth Badge");
static const u8 sText_KantoBadgeDesc0[] = _("Brock's Rock-type Gym.");
static const u8 sText_KantoBadgeDesc1[] = _("Misty's Water-type Gym.");
static const u8 sText_KantoBadgeDesc2[] = _("Lt. Surge's Electric Gym.");
static const u8 sText_KantoBadgeDesc3[] = _("Erika's Grass-type Gym.");
static const u8 sText_KantoBadgeDesc4[] = _("Janine's Poison-type Gym.");
static const u8 sText_KantoBadgeDesc5[] = _("Sabrina's Psychic Gym.");
static const u8 sText_KantoBadgeDesc6[] = _("Blaine's Fire-type Gym.");
static const u8 sText_KantoBadgeDesc7[] = _("Blue's Gym, the last one.");

static const struct QuestSubquestInfo sKantoSubquests[] =
{
    { sText_KantoBadge0, sText_KantoBadgeDesc0, sText_Pewter,    OBJ_EVENT_GFX_BROCK_HNS,   QUEST_SPRITE_OBJECT, sText_Won },
    { sText_KantoBadge1, sText_KantoBadgeDesc1, sText_Cerulean,  OBJ_EVENT_GFX_MISTY_HNS,   QUEST_SPRITE_OBJECT, sText_Won },
    { sText_KantoBadge2, sText_KantoBadgeDesc2, sText_Vermilion, OBJ_EVENT_GFX_SURGE_HNS,   QUEST_SPRITE_OBJECT, sText_Won },
    { sText_KantoBadge3, sText_KantoBadgeDesc3, sText_Celadon,   OBJ_EVENT_GFX_ERIKA_HNS,   QUEST_SPRITE_OBJECT, sText_Won },
    { sText_KantoBadge4, sText_KantoBadgeDesc4, sText_Fuchsia,   OBJ_EVENT_GFX_JANINE_HNS,  QUEST_SPRITE_OBJECT, sText_Won },
    { sText_KantoBadge5, sText_KantoBadgeDesc5, sText_Saffron,   OBJ_EVENT_GFX_SABRINA_HNS, QUEST_SPRITE_OBJECT, sText_Won },
    { sText_KantoBadge6, sText_KantoBadgeDesc6, sText_Seafoam,   OBJ_EVENT_GFX_BLAINE_HNS,  QUEST_SPRITE_OBJECT, sText_Won },
    { sText_KantoBadge7, sText_KantoBadgeDesc7, sText_Viridian,  OBJ_EVENT_GFX_BLUE_HNS,    QUEST_SPRITE_OBJECT, sText_Won },
};

static void GetKantoProgress(struct QuestProgress *progress)
{
    u32 next = FirstMissingBadge(sKantoBadgeFlags, NELEMS(sKantoBadgeFlags));

    if (next == NELEMS(sKantoBadgeFlags))
    {
        progress->state = QUEST_STATE_COMPLETED;
        return;
    }
    // Unknown until the Johto league is done (the Reception Gate wants the
    // eighth badge), or a Kanto badge turns up some other way.
    if (next == 0 && !FlagGet(FLAG_BADGE08_GET))
    {
        progress->state = QUEST_STATE_LOCKED;
        return;
    }
    progress->state = QUEST_STATE_ACTIVE;
    progress->step = next;
    if (next == KANTO_STEP_BLUE_CINNABAR && !FlagGet(FLAG_HIDE_VIRIDIAN_BLUE) && FlagGet(FLAG_HIDE_CINNABAR_BLUE))
        progress->step = KANTO_STEP_BLUE_VIRIDIAN;
}

static bool32 IsKantoBadgeDone(u32 subquest)
{
    return subquest < NELEMS(sKantoBadgeFlags) && FlagGet(sKantoBadgeFlags[subquest]);
}

// ---- TcloneR ----
//
// Hidden until the first scientist has been met (VAR_TCLONER_STAGE, set by
// the scientist's script). The step is that stage. The text has no clone
// count: the book prints static strings, and VAR_TCLONER_COUNT is not one.

static const u8 sText_TcloneName[] = _("The TcloneR");
static const u8 sText_TcloneMap[] = _("Johto and Kanto");
static const u8 sText_TcloneDone[] = _("Every prize collected.\nThe TcloneR has no more to give.");
static const u8 sText_TcloneStep0[] = _("Nothing to do yet.");
static const u8 sText_TcloneStep1[] = _("Look for more scientists in\nMahogany, Goldenrod, Cerulean.");
static const u8 sText_TcloneStep4[] = _("A scientist now works at Oak's\nLab. Clone Pokémon for prizes.");
static const u8 sText_TcloneStep5[] = _("Clone Trainers' Pokémon. Every ten\nclones, a prize awaits.");
static const u8 sText_TcloneReward[] = _("the TcloneR scientist");
static const u8 sText_PalletTown[] = _("Pallet Town");

static const struct QuestStep sTcloneSteps[] =
{
    [0] = { sText_TcloneStep0, sText_TcloneMap,   OBJ_EVENT_GFX_SCIENTIST_M_HNS, QUEST_SPRITE_OBJECT },
    [1] = { sText_TcloneStep1, sText_TcloneMap,   OBJ_EVENT_GFX_SCIENTIST_M_HNS, QUEST_SPRITE_OBJECT },
    [2] = { sText_TcloneStep1, sText_TcloneMap,   OBJ_EVENT_GFX_SCIENTIST_M_HNS, QUEST_SPRITE_OBJECT },
    [3] = { sText_TcloneStep1, sText_TcloneMap,   OBJ_EVENT_GFX_SCIENTIST_M_HNS, QUEST_SPRITE_OBJECT },
    [4] = { sText_TcloneStep4, sText_PalletTown,  OBJ_EVENT_GFX_SCIENTIST_M_HNS, QUEST_SPRITE_OBJECT },
    [5] = { sText_TcloneStep5, sText_PalletTown,  OBJ_EVENT_GFX_SCIENTIST_M_HNS, QUEST_SPRITE_OBJECT },
    [6] = { NULL,              sText_TcloneReward, OBJ_EVENT_GFX_SCIENTIST_M_HNS, QUEST_SPRITE_OBJECT },
};

#define TCLONE_STEP_REWARD 6

static void GetTclonerProgress(struct QuestProgress *progress)
{
    u32 stage = VarGet(VAR_TCLONER_STAGE);

    if (TClonerAllRewardsClaimed())
    {
        progress->state = QUEST_STATE_COMPLETED;
        return;
    }
    if (stage == 0)
    {
        progress->state = QUEST_STATE_LOCKED;
        return;
    }
    if (TClonerIsRewardDue())
    {
        progress->state = QUEST_STATE_REWARD;
        progress->step = TCLONE_STEP_REWARD;
        return;
    }
    progress->state = QUEST_STATE_ACTIVE;
    progress->step = stage;
}

// ---- Red's Path ----
//
// Hidden until the aide's introduction has been seen. Steps 0-7 are the next
// challenge, 8-15 a prize waiting at that challenge's Gym (only the map is
// used there: the book's own "your reward is waiting" text covers the rest).

static const u8 sText_RedsName[] = _("Red's Path");
static const u8 sText_RedsMap[] = _("Kanto Gyms");
static const u8 sText_RedsDone[] = _("Red's journey is finished:\nall eight Gyms and prizes.");

static const u8 sText_RedsStep0[] = _("Win the Gym, then ask the aide\nto play Red against Brock.");
static const u8 sText_RedsStep1[] = _("Win the Gym, then ask the aide\nto play Red against Misty.");
static const u8 sText_RedsStep2[] = _("Win the Gym, then ask the aide\nto play Red against Surge.");
static const u8 sText_RedsStep3[] = _("Win the Gym, then ask the aide\nto play Red against Erika.");
static const u8 sText_RedsStep4[] = _("Win the Gym, then ask the aide\nto play Red against Koga.");
static const u8 sText_RedsStep5[] = _("Win the Gym, then ask the aide\nto play Red against Sabrina.");
static const u8 sText_RedsStep6[] = _("Win the Gym, then ask the aide\nto play Red against Blaine.");
static const u8 sText_RedsStep7[] = _("Win the Gym, then ask the aide\nto play Red against Giovanni.");

#define REDS_STEP(n, map, gfx) { sText_RedsStep##n, map, gfx, QUEST_SPRITE_OBJECT }
#define REDS_PRIZE_STEP(map, gfx) { NULL, map, gfx, QUEST_SPRITE_OBJECT }
static const struct QuestStep sRedsSteps[] =
{
    REDS_STEP(0, sText_Pewter,    OBJ_EVENT_GFX_BROCK_HNS),
    REDS_STEP(1, sText_Cerulean,  OBJ_EVENT_GFX_MISTY_HNS),
    REDS_STEP(2, sText_Vermilion, OBJ_EVENT_GFX_SURGE_HNS),
    REDS_STEP(3, sText_Celadon,   OBJ_EVENT_GFX_ERIKA_HNS),
    REDS_STEP(4, sText_Fuchsia,   OBJ_EVENT_GFX_KOGA_HNS),
    REDS_STEP(5, sText_Saffron,   OBJ_EVENT_GFX_SABRINA_HNS),
    REDS_STEP(6, sText_Seafoam,   OBJ_EVENT_GFX_BLAINE_HNS),
    REDS_STEP(7, sText_Viridian,  OBJ_EVENT_GFX_GIOVANNI_HNS),
    REDS_PRIZE_STEP(sText_Pewter,    OBJ_EVENT_GFX_BROCK_HNS),
    REDS_PRIZE_STEP(sText_Cerulean,  OBJ_EVENT_GFX_MISTY_HNS),
    REDS_PRIZE_STEP(sText_Vermilion, OBJ_EVENT_GFX_SURGE_HNS),
    REDS_PRIZE_STEP(sText_Celadon,   OBJ_EVENT_GFX_ERIKA_HNS),
    REDS_PRIZE_STEP(sText_Fuchsia,   OBJ_EVENT_GFX_KOGA_HNS),
    REDS_PRIZE_STEP(sText_Saffron,   OBJ_EVENT_GFX_SABRINA_HNS),
    REDS_PRIZE_STEP(sText_Seafoam,   OBJ_EVENT_GFX_BLAINE_HNS),
    REDS_PRIZE_STEP(sText_Viridian,  OBJ_EVENT_GFX_GIOVANNI_HNS),
};

static const u8 sText_RedsLeader0[] = _("Brock");
static const u8 sText_RedsLeader1[] = _("Misty");
static const u8 sText_RedsLeader2[] = _("Lt. Surge");
static const u8 sText_RedsLeader3[] = _("Erika");
static const u8 sText_RedsLeader4[] = _("Koga");
static const u8 sText_RedsLeader5[] = _("Sabrina");
static const u8 sText_RedsLeader6[] = _("Blaine");
static const u8 sText_RedsLeader7[] = _("Giovanni");
static const u8 sText_RedsWonDesc[] = _("Beaten with Red's team,\nas in his journey.");

#define REDS_SUB(n, map, gfx) { sText_RedsLeader##n, sText_RedsWonDesc, map, gfx, QUEST_SPRITE_OBJECT, sText_Won }
static const struct QuestSubquestInfo sRedsSubquests[] =
{
    REDS_SUB(0, sText_Pewter,    OBJ_EVENT_GFX_BROCK_HNS),
    REDS_SUB(1, sText_Cerulean,  OBJ_EVENT_GFX_MISTY_HNS),
    REDS_SUB(2, sText_Vermilion, OBJ_EVENT_GFX_SURGE_HNS),
    REDS_SUB(3, sText_Celadon,   OBJ_EVENT_GFX_ERIKA_HNS),
    REDS_SUB(4, sText_Fuchsia,   OBJ_EVENT_GFX_KOGA_HNS),
    REDS_SUB(5, sText_Saffron,   OBJ_EVENT_GFX_SABRINA_HNS),
    REDS_SUB(6, sText_Seafoam,   OBJ_EVENT_GFX_BLAINE_HNS),
    REDS_SUB(7, sText_Viridian,  OBJ_EVENT_GFX_GIOVANNI_HNS),
};

static void GetRedsPathProgress(struct QuestProgress *progress)
{
    u32 prize, next;

    if (!RedsPath_IsIntroSeen())
    {
        progress->state = QUEST_STATE_LOCKED;
        return;
    }
    prize = RedsPath_GetPrizeWaiting();
    if (prize < REDS_PATH_CHALLENGE_COUNT)
    {
        progress->state = QUEST_STATE_REWARD;
        progress->step = REDS_PATH_CHALLENGE_COUNT + prize;
        return;
    }
    next = RedsPath_GetNextChallenge();
    if (next >= REDS_PATH_CHALLENGE_COUNT)
    {
        progress->state = QUEST_STATE_COMPLETED;
        return;
    }
    progress->state = QUEST_STATE_ACTIVE;
    progress->step = next;
}

static bool32 IsRedsPathChallengeDone(u32 subquest)
{
    return RedsPath_IsChallengeWon(subquest);
}

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
    [QUEST_JOHTO_BADGES] =
    {
        .name = sText_JohtoName,
        .source = QUEST_SOURCE_FUNC,
        .numSteps = NELEMS(sJohtoSteps),
        .getProgress = GetJohtoProgress,
        .numSubquests = NELEMS(sJohtoSubquests),
        .subquestSource = QUEST_SOURCE_FUNC,
        .isSubquestDone = IsJohtoBadgeDone,
        .steps = sJohtoSteps,
        .doneDesc = sText_JohtoDone,
        .subquestInfo = sJohtoSubquests,
    },
    [QUEST_KANTO_BADGES] =
    {
        .name = sText_KantoName,
        .source = QUEST_SOURCE_FUNC,
        .numSteps = NELEMS(sKantoSteps),
        .getProgress = GetKantoProgress,
        .numSubquests = NELEMS(sKantoSubquests),
        .subquestSource = QUEST_SOURCE_FUNC,
        .isSubquestDone = IsKantoBadgeDone,
        .steps = sKantoSteps,
        .doneDesc = sText_KantoDone,
        .subquestInfo = sKantoSubquests,
    },
    [QUEST_TCLONER] =
    {
        .name = sText_TcloneName,
        .source = QUEST_SOURCE_FUNC,
        .numSteps = NELEMS(sTcloneSteps),
        .getProgress = GetTclonerProgress,
        .steps = sTcloneSteps,
        .doneDesc = sText_TcloneDone,
    },
    [QUEST_REDS_PATH] =
    {
        .name = sText_RedsName,
        .source = QUEST_SOURCE_FUNC,
        .numSteps = NELEMS(sRedsSteps),
        .getProgress = GetRedsPathProgress,
        .numSubquests = NELEMS(sRedsSubquests),
        .subquestSource = QUEST_SOURCE_FUNC,
        .isSubquestDone = IsRedsPathChallengeDone,
        .steps = sRedsSteps,
        .doneDesc = sText_RedsDone,
        .subquestInfo = sRedsSubquests,
    },
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
