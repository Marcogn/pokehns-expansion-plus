#ifndef GUARD_QUEST_BOOK_H
#define GUARD_QUEST_BOOK_H

// The quest book screen (docs/plans/quest-log.md, phase 2). Ported from
// PokemonSanFran's complex_quests branch (Unbound quest menu); the state it
// shows comes from quest_log.c.

#include "main.h"

void QuestBook_Init(MainCallback callback);
void Task_OpenQuestBookFromStartMenu(u8 taskId);
void QuestBook_OpenFromScript(void);

#endif // GUARD_QUEST_BOOK_H
