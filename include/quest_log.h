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

// What the quest book shows for one step of a quest: the text saying what to
// do now, where, and a picture of who or what to look for.
struct QuestStep
{
    const u8 *desc;
    const u8 *map;     // where; NULL falls back to step 0's
    u16 sprite;        // QUEST_SPRITE_* decides what it is
    u8 spriteType;
};

// A subquest line. Name, text and picture show once it is done, "??????" before.
struct QuestSubquestInfo
{
    const u8 *name;
    const u8 *desc;
    const u8 *map;
    u16 sprite;
    u8 spriteType;
    const u8 *type;    // the tag printed at the right of the row ("Found", "Caught")
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
    // The screen's texts. steps is indexed by QuestLog_GetStep, so it has
    // numSteps entries; step 0 is also what a quest that is not yet active shows.
    const struct QuestStep *steps;
    // Shown once completed instead of a step: steps[] are what to do, and the
    // step a completed quest is left on (often 0) points to the wrong place.
    const struct QuestStep *done;
    const struct QuestSubquestInfo *subquestInfo; // numSubquests entries
    // Called before the quest's texts are printed, to fill gStringVar1 and
    // gStringVar3 for them ({STR_VAR_2} is the map). NULL if they use neither.
    void (*bufferText)(void);
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
bool32 QuestLog_IsNew(u32 quest, u32 state);
void QuestLog_MarkSeen(u32 quest);
void QuestLog_SyncSeen(void);
const u8 *QuestLog_GetName(u32 quest);
const struct QuestInfo *QuestLog_GetInfo(u32 quest);

#endif // GUARD_QUEST_LOG_H
