#ifndef GUARD_CONSTANTS_QUESTS_H
#define GUARD_CONSTANTS_QUESTS_H

// Quest book (docs/plans/quest-log.md). Shared by C and the event scripts, so
// only #defines here.

// Quest ids. They index the saved favourite and seen bits, so a new quest is
// appended, never inserted: inserting one moves every later quest's bits onto
// the wrong quest in existing saves.
//
// QUEST_BOOK_TEST_QUESTS adds two throwaway quests (src/data/quests.h) that
// exercise every path of the screen: a var quest with sprites of the three
// kinds, and a function quest with function subquests. Off in every build
// that ships; build with -DQUEST_BOOK_TEST_QUESTS=1 to look at the screen
// before the real quests exist.
#ifndef QUEST_BOOK_TEST_QUESTS
#define QUEST_BOOK_TEST_QUESTS 0
#endif

#define QUEST_JOHTO_BADGES 0
#define QUEST_KANTO_BADGES 1
#define QUEST_TCLONER      2
#define QUEST_REDS_PATH    3

#if QUEST_BOOK_TEST_QUESTS
#define QUEST_TEST_STEPS  4
#define QUEST_TEST_BADGES 5
#define QUEST_COUNT       6
#else
#define QUEST_COUNT       4
#endif

// Quests that keep their state in the quest book because the game has it
// nowhere else take one of these slots (QUEST_SOURCE_OWN). Same rule: append.
#define QUEST_OWN_SLOT_COUNT 0

// Subquests that keep their own done bit (QUEST_SOURCE_OWN subquests): each
// quest that has them takes a run of bits starting at its subquestBitStart.
// Same rule: append.
#define QUEST_OWN_SUBQUEST_COUNT 0

// What struct QuestLogSaveData has room for. Raising any of these grows the
// struct, which needs a new save migration for the bytes it adds.
#define QUEST_SAVE_MAX_QUESTS    48
#define QUEST_SAVE_MAX_OWN_SLOTS 16
#define QUEST_SAVE_MAX_SUBQUESTS 32

// Where a quest reads its progress from.
#define QUEST_SOURCE_VAR  0 // a script var the game already has (TcloneR)
#define QUEST_SOURCE_FUNC 1 // a function over state the game already has (badges, Pokedex, legendaries, Red's Path)
#define QUEST_SOURCE_OWN  2 // a state of its own, saved in the quest book, set by script

// Quest states. 0 is LOCKED, which is what an own slot reads in a save written
// before the quest existed.
#define QUEST_STATE_LOCKED    0 // shown as ????
#define QUEST_STATE_UNLOCKED  1 // known, nothing to do yet (PSF's "inactive")
#define QUEST_STATE_ACTIVE    2
#define QUEST_STATE_REWARD    3 // done, the reward is still to be collected
#define QUEST_STATE_COMPLETED 4
#define QUEST_STATE_COUNT     5

// questlog script command (asm/macros/event.inc). Every query writes VAR_RESULT.
#define QUEST_LOG_GET_STATE     0 // questlog QUEST_LOG_GET_STATE, quest
#define QUEST_LOG_SET_STATE     1 // questlog QUEST_LOG_SET_STATE, quest, state (own quests only)
#define QUEST_LOG_GET_STEP      2 // questlog QUEST_LOG_GET_STEP, quest
#define QUEST_LOG_GET_SUBQUEST  3 // questlog QUEST_LOG_GET_SUBQUEST, quest, subquest
#define QUEST_LOG_SET_SUBQUEST  4 // questlog QUEST_LOG_SET_SUBQUEST, quest, subquest (own subquests only)
#define QUEST_LOG_BUFFER_NAME   5 // questlog QUEST_LOG_BUFFER_NAME, quest: name into gStringVar1
#define QUEST_LOG_OPEN          6 // questlog QUEST_LOG_OPEN: opens the quest book; follow it with waitstate

// The picture shown beside a quest's text (struct QuestStep, QuestSubquestInfo).
#define QUEST_SPRITE_NONE   0
#define QUEST_SPRITE_OBJECT 1 // sprite = OBJ_EVENT_GFX_*
#define QUEST_SPRITE_ITEM   2 // sprite = ITEM_*
#define QUEST_SPRITE_MON    3 // sprite = SPECIES_*

#endif // GUARD_CONSTANTS_QUESTS_H
