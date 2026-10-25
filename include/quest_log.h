#ifndef GUARD_QUEST_LOG_H
#define GUARD_QUEST_LOG_H

#include "constants/quests.h"

// Quest book state (docs/plans/quest-log.md). The screen comes later; this is
// what it and the scripts read.

struct QuestProgress
{
    u8 state; // QUEST_STATE_*
    u8 step;  // what to do now: indexes the quest's texts, clamped to numSteps
};

typedef void (*QuestProgressFunc)(struct QuestProgress *progress);
typedef bool32 (*SubquestDoneFunc)(u32 subquest);

struct QuestInfo
{
    const u8 *name;
    u8 source;               // QUEST_SOURCE_*
    u8 numSteps;
    // QUEST_SOURCE_VAR: 0 is locked, doneValue and above completed, anything
    // between is active with that value as the step.
    u16 var;
    u16 doneValue;
    QuestProgressFunc getProgress; // QUEST_SOURCE_FUNC
    u8 ownSlot;                    // QUEST_SOURCE_OWN
    u8 numSubquests;
    u8 subquestSource;             // QUEST_SOURCE_FUNC or QUEST_SOURCE_OWN
    u8 subquestBitStart;           // QUEST_SOURCE_OWN subquests
    SubquestDoneFunc isSubquestDone; // QUEST_SOURCE_FUNC subquests
};

void QuestLog_GetProgress(u32 quest, struct QuestProgress *progress);
u32 QuestLog_GetState(u32 quest);
u32 QuestLog_GetStep(u32 quest);
bool32 QuestLog_SetState(u32 quest, u32 state);
u32 QuestLog_GetSubquestCount(u32 quest);
bool32 QuestLog_IsSubquestDone(u32 quest, u32 subquest);
bool32 QuestLog_SetSubquestDone(u32 quest, u32 subquest);
u32 QuestLog_CountSubquestsDone(u32 quest);
bool32 QuestLog_IsFavorite(u32 quest);
void QuestLog_SetFavorite(u32 quest, bool32 favorite);
bool32 QuestLog_IsNew(u32 quest);
void QuestLog_MarkSeen(u32 quest);
const u8 *QuestLog_GetName(u32 quest);

#endif // GUARD_QUEST_LOG_H
