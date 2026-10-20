---
name: upstream-sync
description: Periodic sync of Hasep (pokehns-expansion-plus) and Marcogn/soulgold with their upstreams. Merges PokemonHnS-Development/pokehns-expansion into Hasep, merges Eemeliri/soulgold into the Marcogn/soulgold mirror, then triages every new Soulgold commit for what Hasep should port. Use when the user asks to check or integrate upstream updates of Heart and Soul or Soulgold, or invokes /upstream-sync.
---

# Upstream sync: HnS → Hasep, Soulgold → fork, Soulgold → Hasep

Three jobs, in this order. The first two are mechanical merges; the third is
judgement and is where the value (and the risk) is. Talk to the user in
Italian, write commits and code comments in English (CLAUDE.md §9).

| job | from | into | how |
| --- | --- | --- | --- |
| A | `PokemonHnS-Development/pokehns-expansion` master | Hasep | `git merge`, Hasep wins on its own features |
| B | `Eemeliri/soulgold` master | `Marcogn/soulgold` | `git merge`; the fork is a pure mirror, so it cannot conflict |
| C | new SG commits since `state.json` | Hasep | triage, then port **only fixes**, never content |

## 0. Prerequisites

- **Both** repos cloned in the session: `/home/user/pokehns-expansion-plus` and
  `/home/user/soulgold`. If one is missing, attach it with `add_repo` and clone it;
  the job needs both.
- Read `CLAUDE.md` (all of it: the traps in §2, §4, §6 are exactly the ones a
  sync walks into), the `.claude/rules/` file of every area the sync touches
  (they load on their own only when a matching file is opened, and triage
  mostly greps) and `docs/SOULGOLD_PORT_REPORT.md` §0, §4, §5.
- Work on the branch the session assigned, in both repos. Never on `master`.
- If the fork `Marcogn/soulgold` ever gets commits of its own
  (`git log upstream/master..origin/master --no-merges` non-empty), job B stops
  being trivial: say so instead of assuming the mirror rule.

## 1. Facts first

```sh
python3 /home/user/pokehns-expansion-plus/.claude/skills/upstream-sync/upstream_status.py \
  | tee <scratchpad>/upstream_status.md
```

It fetches both upstreams (adds the `upstream` remote if missing), touches
nothing else, and prints: missing commits for A and B, the files A will touch
that Hasep also changed since the merge base, and every new SG commit for C
with its files classified as `ported` / `shared` / `sg-only` / `content`.

**If all three counts are 0: tell the user, in two lines, and stop.** No
branch, no commit, no PR.

## 2. Job A — HnS upstream into Hasep

1. `git merge upstream/master` on the dev branch. Message:
   `Merge HnS upstream <version or short sha>`.
2. **Stop and ask the user before resolving** if upstream touches any of these,
   because each one silently rewrites existing saves:
   - `include/global.h` (`SaveBlock*`, `struct ChallengeSettings`, pinned to 32 bytes),
   - the item enum in `include/constants/items.h` (Hasep's `ITEM_CANDY_JAR` must
     stay last before `ITEMS_COUNT`, §6),
   - `SAVE_VERSION` in `include/save.h` or the migration steps in `src/save.c`
     (upstream and Hasep might both claim the same `saveVersion < N`),
   - flags/vars Hasep reuses (`FLAG_FORCE_SHINY`, `FLAG_RECEIVED_FIRST_BALLS`, DexNav vars).
3. Conflicts in the "touched by both" list: keep Hasep's features and options,
   take upstream for content and fixes. When both sides changed the same logic
   and either choice loses behaviour, ask; don't pick.
4. A clean merge is not proof: for every file in the "touched by both" list,
   read the merged result around Hasep's changes. Watch `#if IS_HNS / #else`
   (§6: that is where features get switched off without any flag saying so) and
   re-run `python3 /tmp/ppcheck.py` on files with preprocessor conflicts if the
   script exists, otherwise check `#if`/`#endif` balance by hand.

## 3. Job B — Soulgold upstream into the fork

`git merge upstream/master` in `/home/user/soulgold`, message
`Merge upstream Eemeliri/soulgold master (<N> commits, up to <sha>)` (same form
as the previous merges). Build with `make modern -j$(nproc) -O`; the fork is
not Hasep, `make hns` does not exist there.

## 4. Job C — Soulgold → Hasep triage

Go through the script's classification, **reading the diffs**, not the
commit subjects ("various fixes" hides real fixes and balance changes alike):

```sh
git -C /home/user/soulgold show <sha> -- <file>
```

- `ported` — read every hunk. Each one is either a fix Hasep needs or a
  divergence Hasep keeps on purpose (report §6: dark UI healthbox, DexNav,
  type icons). Check §6 of the report before "fixing" a divergence.
- `shared` — look for bug fixes to engine code both games have. Before applying
  one, **find the same bug in Hasep's code**: SG's fix often targets SG's own
  systems (innates, multi-item, level scaling) and does not apply — the report
  §4 has three such cases. Also compare the signatures of every function the
  hunk calls (§6: a dropped argument compiles and changes behaviour).
- `sg-only` — new SG features. **Never port them in this skill.** List them
  as candidates in report §5 with one line each; the user decides.
- `content` — SG's own game (teams, learnsets, innates, docs, maps). Not ported.
  The script already puts `abilities.h`, `moves_info.h`, `items.h` here because
  SG rebalances them wholesale; skim them only for an obvious engine-level bug.

Rule number one (CLAUDE.md §2) holds for every port: **never copy SG values**
(palettes, coordinates, ids). Derive them from Hasep's own files.

Record **every** commit you looked at in report §4 (applied / applied, adapted /
not applied + why), including the ones you decided against: next time that is
what stops someone from re-evaluating them.

## 5. Verify

- Toolchain if missing:
  `apt-get install -y --no-install-recommends binutils-arm-none-eabi gcc-arm-none-eabi libnewlib-arm-none-eabi libpng-dev`
- Hasep: `make hns -j$(nproc) -O` must pass. Report how much of the 32 MB is
  used before and after (§7bis: ~95% before megas; anything that pushes past
  ~99% is a stop-and-ask).
- mGBA is **not** part of the default run: driving it costs time and a
  screenshot per step. Instead, write in the PR and in the final message a
  short "cosa provare" list: for each behaviour change from job A or C, where
  to go in the game and what should happen. The user tests on their device.
- Use mGBA (CLAUDE.md §3) only when the user asks, or for a change whose
  failure would damage saves (anything near save layout or migration): there,
  load the user's save and check that it still loads and the party is intact.
- Never call a change "verified" when it was only built. Say "compila, non
  provato in gioco".
- `make check` and the CI are already broken upstream (§8): their failure is
  not a sync regression, and passing them is not a verification either.

## 6. Record and deliver

1. `docs/SOULGOLD_PORT_REPORT.md`: §0 table (date, both upstream SHAs, SG
   evaluated up to), §4 rows, §5 candidates.
2. `state.json`: set `soulgold_evaluated_upto` to the last SG commit you
   **actually evaluated** (full SHA) and the date. Same commit as the report.
   If job C was cut short, set it to where you stopped, not to upstream's tip.
3. If something new was ported, add its files to `PORTED` in
   `upstream_status.py`, so the next run reads their diffs in full.
4. `CHANGELOG.md`: under the next unreleased `## <version>` heading (add it
   if the top section is already released, i.e. has a `v<version>` tag), one
   line per fix or feature that came in, plus the `### Upstream` line with
   both upstream positions. The release workflow publishes that section and
   refuses to run without it.
5. Commit, push, open draft PRs (one per repo that changed), as per the
   session's git rules.
6. Hasep ROM: `zip -9 pokemonHnS_upstream-sync_<sha>.zip pokehns.gba`, full
   32 MB ROM (CLAUDE.md §1), sent to the user.
7. Final message to the user, in Italian, short: what came in from each
   upstream, what was ported into Hasep and how it was verified, what was
   deliberately not ported, what needs their decision.
