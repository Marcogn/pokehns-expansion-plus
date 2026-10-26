---
name: next-phase
description: Carry out the next phase of a plan in docs/plans/ for pokehns-expansion-plus, one phase per session, and tell the user up front whether they need to upload their save (.srm) for the verification. Use when the user asks to go on with the plan, do the next phase, continue from the handoff, or invokes /next-phase (optionally with the plan name, e.g. /next-phase quest-log).
---

# Next phase of a plan

The plans in `docs/plans/` are the memory between sessions: each one has a
phase table (`# | Fase | Modello | Cancello`), and every phase that is done
gets a section "Fase N, com'è stata fatta" ending with a handoff for the
next one. This skill picks up from there. Talk to the user in Italian;
code comments and commits in English (CLAUDE.md §9).

## 1. Find the plan and the phase (before touching code)

1. Which plan: the one the user named; otherwise the plans whose status line
   (first lines of the file) is not "tutte le fasi fatte", newest by
   `git log -1 --format=%cs -- docs/plans/<file>`. If more than one is open,
   **ask** which one, listing them with their next phase.
2. Which phase: the first row of the phase table without a matching
   "com'è stata fatta" section. Read the **handoff of the previous phase**
   and any "punti aperti" list: they override the table when they disagree.
3. Check it is really not done: `git log --oneline -20` and
   `git log origin/master --oneline --grep "phase <N>"` (commits are named
   `<Area> phase N: ...`). A phase may be merged without its doc section.
4. Read what the plan says to read: the sections the phase refers to, the
   `.claude/rules/` of every area it touches (they only auto-load when a
   matching file is opened, CLAUDE.md §7), and `docs/claude/emulator.md`
   if the gate needs the emulator.

## 2. Model check

The table assigns a model to the phase. Call `get_session` (claude-code-remote
MCP server, `session_id` omitted) and compare `session_context.model` with the
assigned one. If they differ, say so in one line before starting
(e.g. "la fase 3 è assegnata a Sonnet 5.5, questa sessione è su Opus 5.5")
and let the user decide; do not refuse to work. Never write a model id in
commits, code or the PR (only in the plan's table, which already has them).

## 3. Does the user need to upload their save? Say it first.

Decide from the phase's gate **before writing code**, and put the answer in
your first message, so the upload can happen while you work:

**The `.srm` is needed** when the verification needs a state a new game
cannot reach cheaply:
- the phase changes the save layout (`include/global.h` SaveBlock structs,
  `SAVE_VERSION`, a migration step in `src/save.c`): a real old save is the
  only proof the migration works (`.claude/rules/save.md`, every past
  `SAVE_VERSION` was proved this way);
- the gate needs mid/late-game progress (badges, Champion, Kanto, TcloneR
  stages, Red's Path, a specific theme or option the user plays with) and
  setting it by hand would take many flags/vars via GDB or the debug menu;
- the plan or handoff explicitly says "`.sav` dell'utente".

**Not needed** when a new game, the debug menu (hold R + START), or a few
GDB writes reach the state (offsets in the area's rule), or the phase is
docs/graphics only.

Say it in one or two lines: "Serve il tuo `.srm`: <why>" or "Non serve il
salvataggio: <how you will verify instead>". If it is needed and does not
arrive, do the parts that do not depend on it and stop at the gate; do not
claim the gate passed. The save is used as `pokehns.sav` next to the ROM
(`docs/claude/emulator.md`); never commit it.

## 4. Do the phase

- Only this phase. Things found for later phases go in the handoff, not in
  this diff.
- Follow CLAUDE.md (traps in §2, §4, §6) and the area rules.
- `make hns -j$(nproc) -O`, then the phase's gate exactly as the table
  states it. Screenshots looked at, pixels measured; a case that fails
  without the change.
- A save layout change follows `.claude/rules/save.md` (field at the end,
  `STATIC_ASSERT`, `saveVersion < N` step, zero = old behaviour).

## 5. Close the phase

1. In the plan: a section `### 5.N Fase N, com'è stata fatta (<dd/mm>)` in the
   plan's style (Italian, short bullets): what was done, where the details
   live (which rule), what was verified and how, then
   **"Handoff per la fase N+1 (<model>)"** with what the next session must
   know and not rediscover. Update the status line at the top.
2. Area rule in `.claude/rules/`: traps and measurements found (the plan
   holds decisions, the rule holds how the code behaves).
3. `CHANGELOG.md` under the next version if the phase changes something a
   player sees.
4. Commit `<Area> phase N: <what>` with the "why" and "how verified" in the
   body; push to the session branch; draft PR as the session rules say.
5. If the phase changes what ships, deliver the ROM zipped (CLAUDE.md §1).
6. Final message to the user: what was done, the gate result (passed /
   not passed and why), what the next phase is and which model it wants.
