---
name: pr-review
description: Review a pull request of pokehns-expansion-plus once it is marked ready for review, with the generic code review plus the repo's own checks (preprocessor balance, save polarity and migrations, palettes derived not copied, assertf on reachable states, docs), post the findings on the PR and fix the blocking ones. Use when the user says a PR is ready, asks whether the review was done, or invokes /pr-review [PR number].
---

# Review of a PR marked ready

Talk to the user in Italian; comments on GitHub in English, with the
Claude Code attribution footer.

## 1. Which PR, in which state

- The number the user gave; otherwise the open PR of the session's branch
  (`list_pull_requests` with `head`).
- `pull_request_read`: it must be **ready for review** (not draft). If it is
  still a draft, say so and ask whether to review anyway.
- Has it been reviewed already on its current head? Look at the reviews and
  comments: a review on an older commit covers only that commit. Answer
  the user's question ("hai fatto la review?") with that fact first.
- CI: read the `build-hns` check on the head (`build` is only the gate
  that follows it). Red CI comes before the review
  (it is work now).

## 2. Generic review

Invoke the `code-review` skill on the PR at **medium** effort (the level
the plans use for final reviews), without `--comment`: findings are
checked first, then posted.

## 3. This repo's checks (the generic review does not know them)

Go through the diff for each; every hit is a finding with file:line.

- **Preprocessor**: `#if`/`#ifdef` balanced with `#endif` in every touched
  file (an unclosed one shows up as "used but never defined" elsewhere).
  `#if IS_HNS` / `#else`: is the HnS branch the one changed?
- **Save** (`.claude/rules/save.md`): new field at the **end** of the
  SaveBlock, `STATIC_ASSERT` on its offset, a `saveVersion < N` step;
  **zero = old behaviour** (polarity, label of value zero listed first);
  title options kept in both `oak_speech_hns.c` and `NewGameInitData`;
  SaveBlock2 fields cleared by hand on a new game; reused `FLAG_UNUSED_*`
  cleared in a migration.
- **Quest ids / appended enums**: new ids at the end, never inserted
  (`include/constants/quests.h`, the item enum with `ITEM_CANDY_JAR` last).
- **Palettes**: values derived from this repo's `.4bpp`/`.gbapal`, with
  `// was RGB(...)` beside each change; a shared entry checked against all
  its users (`.claude/rules/battle_ui.md`). Values copied from Soulgold
  are a finding (CLAUDE.md §2).
- **`assertf`** only for programming errors, never for states the game
  reaches (VRAM full, object table full): `make hns` shows a blue screen.
- **Ported code**: signatures of the functions it calls compared with this
  repo's (one argument fewer compiles and drops a feature).
- **New symbols**: no collision (`nm --defined-only build/hns/src/<file>.o`).
- **`LoadPalette` between a fade to black and the next screen**, screens
  without their own `SetVBlankCallback`.
- **Test switches left on**: `QUEST_BOOK_TEST_QUESTS 0`, `CB2_TestBoot`
  removed, no instrumented code.
- **Docs**: run the `doc-sync` checklist; CHANGELOG section for the next
  version present (the release needs it).

## 4. Verify, post, fix

1. Each finding: trace a real path from a caller or input to the failure;
   drop the ones that do not hold.
2. Post them as one review on the PR (`pull_request_review_write`
   create → `add_comment_to_pending_review` per finding →
   `submit_pending`), each with the failure scenario, marked blocking or
   optional. No findings: one comment saying what was checked and on which
   commit.
3. On a PR of this session: fix the blocking findings, `make hns` green,
   push, reply on each thread with the commit. Never resolve a human's
   larger ask on your own (session rules).
4. Tell the user: reviewed commit, findings (blocking / optional / dropped),
   what was fixed, and whether the PR is ready to merge.

## Note on "automatic" reviews

Marking a PR "Ready for review" sends a `pull_request.ready_for_review`
event to the session subscribed to that PR (seen on #45, 26/10). So the
session that opened the PR, if it is still subscribed, gets woken: on that
event, run this skill without waiting to be asked. A session that is no
longer subscribed (PR opened elsewhere, subscription dropped) gets nothing;
then the review starts only from `/pr-review`. A review that starts on its
own regardless of sessions is a GitHub Actions workflow (a separate change,
it needs an API key in the repo secrets): propose it, do not add it
silently.
