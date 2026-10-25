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

static const struct QuestInfo sQuests[QUEST_COUNT] =
{
};
