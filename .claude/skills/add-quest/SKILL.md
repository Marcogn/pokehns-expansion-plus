---
name: add-quest
description: Add a side quest to the quest book of pokehns-expansion-plus (src/data/quests.h, include/constants/quests.h), with its in-game trigger, texts, picture and emulator verification. Use when the user wants a new quest or side quest, a new entry in the quest book or "Missioni", or invokes /add-quest.
---

# Add a quest

The recipe already exists: `docs/plans/quest-log.md` §6 (how to add one),
§7 (backlog of quests already discussed) and `.claude/rules/quest_book.md`
(screen limits, verification). Read all three first; this skill is the
order of work and the checks that are easy to forget. Talk to the user in
Italian; quest texts are in **English** (the game is in English).

## 0. Size it

- **Entry only** (the condition exists in the game: a flag, var, badge,
  Pokédex count, a trainer beaten): this skill, Sonnet or Haiku is enough.
- **Entry + a little script** (one NPC line, setting a free flag): this skill.
- **New content** (new maps, trainers, battles, rewards, story): stop and
  use `/new-plan` instead; Red's Path and the TcloneR were plans, not quests.

## 1. Design with the user

Ask (with `AskUserQuestion` when there are real options) and do not invent:
name (**≤ ~20 characters**), what unlocks it (from LOCKED/`??????` to
visible), the steps (what to do now, where, who to look for), subquests if
any, when it is done, reward and where it is collected (`REWARD` state),
the picture of each step (`QUEST_SPRITE_OBJECT/ITEM/MON`). If it is in the
§7 backlog, start from what is written there.

## 2. Find the real conditions

- `grep` the `_hns` scripts (`data/maps/*_hns/`, `data/scripts/`) and
  `include/constants/flags_hns.h`, `vars_hns.h`. **Check where each flag is
  set**, not its name: `FLAG_VISITED_KANTO` is set on Route 27, not in
  Vermilion (§6).
- A new flag/var for an invented quest: take a free one from
  `flags_hns.h`/`vars_hns.h`. A `FLAG_UNUSED_*` may have been written by an
  older build: if you cannot show it was always 0, it needs clearing in a
  migration step (`.claude/rules/save.md`, `FLAG_FORCE_SHINY` case). Ask
  before adding a migration.

## 3. Pick the source (it decides the save cost)

| Source | When | Save cost |
| --- | --- | --- |
| `QUEST_SOURCE_FUNC` | state the game already has; preferred | 3 bits (favourite + seen) |
| `QUEST_SOURCE_VAR` | one var already walks the steps (0 locked, `doneValue` done) | 3 bits |
| `QUEST_SOURCE_OWN` | nothing in the game holds the state | 7 bits: bump `QUEST_OWN_SLOT_COUNT`, set by script with `questlog QUEST_LOG_SET_STATE` |

Own subquests cost 1 bit each (`QUEST_OWN_SUBQUEST_COUNT`, a
`QUEST_X_SUBQUEST_START`). Within `QUEST_SAVE_MAX_*` (48 quests, 16 own
slots, 32 own subquests) **no migration is needed**: the struct is already
that size and an old save reads 0 = LOCKED / not seen. Going past a max
grows the struct: that is a migration, so a `/new-plan` phase on Opus.
Reuse existing C logic for progress (e.g. `RedsPath_GetAideState`,
`TClonerGetNextRewardClones`) instead of rewriting it.

## 4. Write it

1. `include/constants/quests.h`: id **appended** after the last real quest,
   `QUEST_COUNT` +1 in **both** branches of `#if QUEST_BOOK_TEST_QUESTS`
   (the test ids move after it). Never insert: saved bits follow the id.
2. `src/data/quests.h`: copy the closest existing quest (badges = func +
   subquests, TcloneR = `bufferText` with counts, Red's Path = reward
   state). Texts: footer **max two lines, ~38 characters each** (`\n`),
   `{STR_VAR_2}` = the step's map, `{STR_VAR_1}`/`{STR_VAR_3}` only with a
   `bufferText`. A `done` step for the completed state. `REWARD` with
   `desc` NULL prints the fixed "Return to {map}" text.
3. Script lines, if any, in the map's `_hns` script (`.claude/rules/field.md`).
4. Two quests whose pictures share a palette are fine (`ShowPicture`
   destroys the old one first); do not change that order.

## 5. Verify (in mGBA, `docs/claude/emulator.md`)

- `make hns -j$(nproc) -O`.
- **A case that fails without the change**: the quest is `??????` before the
  unlock condition, appears with NEW after it.
- Every state the quest can be in: locked, active (each step text fits),
  reward if any, completed (shows `done`), subquest lines. Screenshot each,
  **light and dark theme** (dark via GDB: rule, "Tema via GDB").
- States are set via GDB (flags at SaveBlock1 + 6540, vars + 6844; re-read
  `gSaveBlock1Ptr` right before writing, it moves) or the debug menu; close
  and reopen the book after each write. If the states need real progress,
  ask the user for their `.srm` at the start, not at the end.

## 6. Close

- `CHANGELOG.md`, next version, `### Added`: the quest from the player's
  side (what it asks, where it starts), no internals.
- `.claude/rules/quest_book.md`: add the quest to the "Missioni reali"
  notes (flags/vars used, the lists to keep aligned by hand).
- `docs/plans/quest-log.md` §7: mark it done if it came from the backlog.
- Commit `Quest book: <quest name>`, push, draft PR, ROM zipped to the user.
