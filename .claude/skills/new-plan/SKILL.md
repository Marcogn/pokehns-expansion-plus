---
name: new-plan
description: Write a new feature plan for pokehns-expansion-plus in docs/plans/, split into phases (one per session) with the most suitable Claude model assigned to each, in the shape the next-phase skill can execute. Use when the user wants to plan a new feature, port or side project, asks to split work into phases or to assign models, or invokes /new-plan.
---

# New plan

A plan is what lets a future session, with no memory of this one, do one
phase and stop. Plans are written in **Italian** (like the existing ones);
talk to the user in Italian.

## 1. Before writing anything

1. **Is it already there?** CLAUDE.md §6: DexNav, type icons and the move
   relearner were in the code, switched off behind `#if IS_HNS`. Grep the
   feature's names, check `docs/SOULGOLD_PORT_REPORT.md`, `docs/plans/`
   (including the backlog sections, e.g. `quest-log.md` §7) and
   `docs/analysis/`.
2. **Where does it come from?** Soulgold (`/home/user/soulgold`, attach it
   with `add_repo` if the feature is a port), another public repo, or new.
   Read the source; never write "identical in SG" without opening it.
3. **Measure the costs** that can sink it, with numbers in the plan:
   - ROM space: the build is near the CI limit (warning at 98 %, error at
     99.5 %, `.claude/rules/megas.md`); measure free space on a fresh
     `make hns` the way `build.yml` does;
   - save space: `nm -S pokehns.elf` on `gSaveblock1/2/3` (method and current
     margins in `docs/plans/quest-log.md` §4 and `.claude/rules/save.md`);
   - VRAM / palettes if it is a screen or overworld sprites.
4. **Decisions are the user's.** List the real choices (behaviour, where it
   shows, what is cut) with a recommendation each, and ask with
   `AskUserQuestion` before writing the phases. Do not invent decisions.

## 2. The file: `docs/plans/<feature>.md`

Follow `docs/plans/quest-log.md`, the most complete example:

- `# Piano: <feature>` and a **status line** (`**Stato (<dd/mm>): ...**`)
  that `next-phase` keeps updated.
- `## 1. Cosa si fa` — what the player gets, in a few lines.
- `## 2. Decisioni prese` — the user's answers, final.
- `## 3. Fatti verificati` — file:line facts the phases rely on, so no
  session rediscovers them. Long research (comparisons, discarded sources)
  goes in `docs/analysis/<feature>-research.md`, linked, not inline.
- `## 4. Spazio misurato` — the numbers from step 1.3.
- `## 5. Fasi` — the table below, then nothing: the "com'è stata fatta"
  sections are written by the phases themselves.
- `## 6. Scartato, e perché`.

## 3. Phases and models

Table: `| # | Fase | Modello | Cancello per passare oltre |`.

- **One phase = one session.** It must end with a build that works and a
  gate that can be checked; never a phase that leaves `master` broken.
- **Gate** = an observable test: "build verde; un `.sav` vecchio carica e il
  campo nuovo legge zero (emulatore)", "screenshot in tutti e due i temi",
  "ogni voce aperta e chiusa in emulatore". Not "works".
- Write in the gate whether the user's `.srm` is needed (rules in the
  `next-phase` skill, §3), so the user knows in advance.
- Typical order: foundations (save, data, API) → port/screen → graphics →
  content → documentation → final review (`/code-review` at medium, open
  points tested in the emulator, ROM delivered).

**Model per phase** — the user's standing constraints: **no model above
Opus 5.5, medium effort, few tokens a month.**

| Model | Give it | Why |
| --- | --- | --- |
| **Opus 5.5** | save layout and migrations, engine/battle code, anything with a hidden invariant (VRAM, palettes shared across screens, callbacks), the final review | an error here costs a corrupted save or a week of debugging |
| **Sonnet 5.5** | ports with a clear source, screens, graphics, scripts and maps, content (quests, teams, texts) | well-specified work with a gate that catches mistakes |
| **Haiku 5.5** | documentation, CHANGELOG, rules re-read against the code, mechanical data entry where the condition already exists | low risk, the text is checked against the code |

When in doubt between two, pick the cheaper one and make the gate stricter.
Check the current model names before writing them (the session's
environment lists the current family); do not copy names from old plans if
they changed.

## 4. Finish

- Link the plan from the related rule or from CLAUDE.md §7 only if it adds
  a new area (`.claude/rules/<area>.md` with `paths:`).
- Add the plan to `docs/SUMMARY.md` (with the other plans): the
  `docs_validate` check fails on any `docs/**/*.md` it does not list.
- Commit `Plan: <feature>` on the session branch, push, draft PR.
- Tell the user: phases with model and gate, which ones will need their
  `.srm`, and the command to start: `/next-phase <feature>`.
