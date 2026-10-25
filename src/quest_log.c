#include "global.h"
#include "event_data.h"
#include "quest_book.h"
#include "quest_log.h"
#include "script.h"
#include "string_util.h"
#include "strings.h"

// Quest book state (docs/plans/quest-log.md, phase 1). Most quests read the
// state the game already keeps - badges, Pokedex, legendaries, Red's Path,
// the TcloneR vars - so they work on old saves and cost no save space beyond
// their favourite and seen bits. Only quests the game has no state for keep
// one of their own, in gSaveBlock3Ptr->questLog.

#include "data/quests.h"

STATIC_ASSERT(QUEST_COUNT <= QUEST_SAVE_MAX_QUESTS, QuestLogQuestsFitSave);
STATIC_ASSERT(QUEST_OWN_SLOT_COUNT <= QUEST_SAVE_MAX_OWN_SLOTS, QuestLogOwnSlotsFitSave);
STATIC_ASSERT(QUEST_OWN_SUBQUEST_COUNT <= QUEST_SAVE_MAX_SUBQUESTS, QuestLogSubquestsFitSave);
// An own slot is a nibble.
STATIC_ASSERT(QUEST_STATE_COUNT <= 16, QuestLogStateFitsNibble);

static bool32 GetBit(const u8 *bits, u32 bit)
{
    return (bits[bit / 8] >> (bit % 8)) & 1;
}

static void SetBit(u8 *bits, u32 bit, bool32 set)
{
    if (set)
        bits[bit / 8] |= 1 << (bit % 8);
    else
        bits[bit / 8] &= ~(1 << (bit % 8));
}

// Seen levels: see QuestLog_IsNew.
enum
{
    SEEN_NEVER,
    SEEN_ACTIVE,    // unlocked or active
    SEEN_REWARD,
    SEEN_COMPLETED,
};

static void SetSeenLevel(u32 quest, u32 level);

static const struct QuestInfo *GetQuest(u32 quest)
{
    if (quest >= QUEST_COUNT)
        return NULL;
    return &sQuests[quest];
}

static u32 GetOwnState(u32 slot)
{
    u32 state;

    if (slot >= QUEST_OWN_SLOT_COUNT)
        return QUEST_STATE_LOCKED;
    state = (gSaveBlock3Ptr->questLog.ownStates[slot / 2] >> ((slot % 2) * 4)) & 0xF;
    return state < QUEST_STATE_COUNT ? state : QUEST_STATE_LOCKED;
}

static void SetOwnState(u32 slot, u32 state)
{
    u8 *byte = &gSaveBlock3Ptr->questLog.ownStates[slot / 2];
    u32 shift = (slot % 2) * 4;

    *byte = (*byte & ~(0xF << shift)) | (state << shift);
}

// The own subquest bit of a quest's subquest, or QUEST_OWN_SUBQUEST_COUNT if
// it has none.
static u32 GetOwnSubquestBit(const struct QuestInfo *info, u32 subquest)
{
    u32 bit;

    if (info->subquestSource != QUEST_SOURCE_OWN || subquest >= info->numSubquests)
        return QUEST_OWN_SUBQUEST_COUNT;
    bit = info->subquestBitStart + subquest;
    return bit < QUEST_OWN_SUBQUEST_COUNT ? bit : QUEST_OWN_SUBQUEST_COUNT;
}

void QuestLog_GetProgress(u32 quest, struct QuestProgress *progress)
{
    const struct QuestInfo *info = GetQuest(quest);
    u32 value;

    progress->state = QUEST_STATE_LOCKED;
    progress->step = 0;
    if (info == NULL)
        return;

    switch (info->source)
    {
    case QUEST_SOURCE_VAR:
        value = VarGet(info->var);
        if (value == 0)
            progress->state = QUEST_STATE_LOCKED;
        else if (value >= info->doneValue)
            progress->state = QUEST_STATE_COMPLETED;
        else
            progress->state = QUEST_STATE_ACTIVE;
        progress->step = min(value, 0xFF);
        break;
    case QUEST_SOURCE_FUNC:
        if (info->getProgress != NULL)
            info->getProgress(progress);
        break;
    case QUEST_SOURCE_OWN:
        progress->state = GetOwnState(info->ownSlot);
        break;
    }

    // The texts are indexed by the step, and a var can hold anything: PSF
    // read desc[VarGet(...)] unchecked, past the end of its array.
    if (progress->state >= QUEST_STATE_COUNT)
        progress->state = QUEST_STATE_LOCKED;
    if (info->numSteps == 0)
        progress->step = 0;
    else if (progress->step >= info->numSteps)
        progress->step = info->numSteps - 1;
}

u32 QuestLog_GetState(u32 quest)
{
    struct QuestProgress progress;

    QuestLog_GetProgress(quest, &progress);
    return progress.state;
}

u32 QuestLog_GetStep(u32 quest)
{
    struct QuestProgress progress;

    QuestLog_GetProgress(quest, &progress);
    return progress.step;
}

// Only own quests have a state to set: the others follow the game's state.
// Any change shows the quest as new again, even one inside the same seen level.
bool32 QuestLog_SetState(u32 quest, u32 state)
{
    const struct QuestInfo *info = GetQuest(quest);

    if (info == NULL || info->source != QUEST_SOURCE_OWN
     || info->ownSlot >= QUEST_OWN_SLOT_COUNT || state >= QUEST_STATE_COUNT)
        return FALSE;
    if (GetOwnState(info->ownSlot) != state)
    {
        SetOwnState(info->ownSlot, state);
        SetSeenLevel(quest, SEEN_NEVER);
    }
    return TRUE;
}

u32 QuestLog_GetSubquestCount(u32 quest)
{
    const struct QuestInfo *info = GetQuest(quest);

    return info != NULL ? info->numSubquests : 0;
}

bool32 QuestLog_IsSubquestDone(u32 quest, u32 subquest)
{
    const struct QuestInfo *info = GetQuest(quest);
    u32 bit;

    if (info == NULL || subquest >= info->numSubquests)
        return FALSE;
    if (info->subquestSource == QUEST_SOURCE_FUNC)
        return info->isSubquestDone != NULL && info->isSubquestDone(subquest);
    bit = GetOwnSubquestBit(info, subquest);
    if (bit >= QUEST_OWN_SUBQUEST_COUNT)
        return FALSE;
    return GetBit(gSaveBlock3Ptr->questLog.subquests, bit);
}

bool32 QuestLog_SetSubquestDone(u32 quest, u32 subquest)
{
    const struct QuestInfo *info = GetQuest(quest);
    u32 bit;

    if (info == NULL)
        return FALSE;
    bit = GetOwnSubquestBit(info, subquest);
    if (bit >= QUEST_OWN_SUBQUEST_COUNT)
        return FALSE;
    SetBit(gSaveBlock3Ptr->questLog.subquests, bit, TRUE);
    return TRUE;
}

// The x of the x/y counter.
u32 QuestLog_CountSubquestsDone(u32 quest)
{
    u32 i, count = 0;

    for (i = 0; i < QuestLog_GetSubquestCount(quest); i++)
    {
        if (QuestLog_IsSubquestDone(quest, i))
            count++;
    }
    return count;
}

bool32 QuestLog_IsFavorite(u32 quest)
{
    if (quest >= QUEST_COUNT)
        return FALSE;
    return GetBit(gSaveBlock3Ptr->questLog.favorite, quest);
}

void QuestLog_SetFavorite(u32 quest, bool32 favorite)
{
    if (quest < QUEST_COUNT)
        SetBit(gSaveBlock3Ptr->questLog.favorite, quest, favorite);
}

// NEW is a seen level against the level of the quest's state: a quest is new
// when it has reached a level the player has not looked at yet. Stored as
// "seen", so that zero - a new game, or an old save after the migration -
// shows every quest the player can see as new. Two bits per quest: seen holds
// bit 0, seenHigh (SAVE_VERSION 13) bit 1.
static u32 GetSeenLevel(u32 quest)
{
    return GetBit(gSaveBlock3Ptr->questLog.seen, quest)
         | (GetBit(gSaveBlock3Ptr->questLog.seenHigh, quest) << 1);
}

static void SetSeenLevel(u32 quest, u32 level)
{
    SetBit(gSaveBlock3Ptr->questLog.seen, quest, level & 1);
    SetBit(gSaveBlock3Ptr->questLog.seenHigh, quest, level >> 1);
}

static u32 GetStateLevel(u32 state)
{
    switch (state)
    {
    case QUEST_STATE_LOCKED:
        return SEEN_NEVER;
    case QUEST_STATE_REWARD:
        return SEEN_REWARD;
    case QUEST_STATE_COMPLETED:
        return SEEN_COMPLETED;
    default:
        return SEEN_ACTIVE;
    }
}

// Only a step up is new. A quest can step down - the TcloneR goes back from
// Reward to Active once the prize is collected - and that is not news;
// QuestLog_SyncSeen lowers the stored level so that the next Reward is.
// state is the quest's QuestLog_GetState, which the book already has: a
// function quest would run its progress function again for it.
bool32 QuestLog_IsNew(u32 quest, u32 state)
{
    if (quest >= QUEST_COUNT)
        return FALSE;
    return GetStateLevel(state) > GetSeenLevel(quest);
}

void QuestLog_MarkSeen(u32 quest)
{
    if (quest < QUEST_COUNT)
        SetSeenLevel(quest, GetStateLevel(QuestLog_GetState(quest)));
}

// Called when the book opens. A Reward collected and the next one due before
// the book is opened again stays at the same level, and shows no NEW.
void QuestLog_SyncSeen(void)
{
    u32 quest, level;

    for (quest = 0; quest < QUEST_COUNT; quest++)
    {
        level = GetStateLevel(QuestLog_GetState(quest));
        if (level < GetSeenLevel(quest))
            SetSeenLevel(quest, level);
    }
}

const u8 *QuestLog_GetName(u32 quest)
{
    const struct QuestInfo *info = GetQuest(quest);

    if (info == NULL || info->name == NULL)
        return gText_EmptyString2;
    return info->name;
}

const struct QuestInfo *QuestLog_GetInfo(u32 quest)
{
    return GetQuest(quest);
}

// questlog action, quest, arg (asm/macros/event.inc). Quest and arg may be vars.
bool8 ScrCmd_questlog(struct ScriptContext *ctx)
{
    u32 action = ScriptReadByte(ctx);
    u32 quest = VarGet(ScriptReadHalfword(ctx));
    u32 arg = VarGet(ScriptReadHalfword(ctx));

    switch (action)
    {
    case QUEST_LOG_SET_STATE:
    case QUEST_LOG_SET_SUBQUEST:
        Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
        break;
    case QUEST_LOG_OPEN:
        // Fades the screen and switches to the book, like fadescreen.
        Script_RequestEffects(SCREFF_V1 | SCREFF_HARDWARE);
        break;
    default:
        Script_RequestEffects(SCREFF_V1);
        break;
    }

    switch (action)
    {
    case QUEST_LOG_GET_STATE:
        gSpecialVar_Result = QuestLog_GetState(quest);
        break;
    case QUEST_LOG_SET_STATE:
        gSpecialVar_Result = QuestLog_SetState(quest, arg);
        break;
    case QUEST_LOG_GET_STEP:
        gSpecialVar_Result = QuestLog_GetStep(quest);
        break;
    case QUEST_LOG_GET_SUBQUEST:
        gSpecialVar_Result = QuestLog_IsSubquestDone(quest, arg);
        break;
    case QUEST_LOG_SET_SUBQUEST:
        gSpecialVar_Result = QuestLog_SetSubquestDone(quest, arg);
        break;
    case QUEST_LOG_BUFFER_NAME:
        StringCopy(gStringVar1, QuestLog_GetName(quest));
        break;
    case QUEST_LOG_OPEN:
        QuestBook_OpenFromScript();
        break;
    }
    return FALSE;
}
