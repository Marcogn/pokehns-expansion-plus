---
name: doc-sync
description: Check that the documentation of pokehns-expansion-plus is up to date with the last commit of the session (CHANGELOG, area rules in .claude/rules, the plan's phase section, port report, CLAUDE.md index, README) and fix what is missing. Use when the user asks whether the docs are updated, before closing a session or a PR, or invokes /doc-sync.
---

# Docs in step with the code

The question "hai aggiornato la documentazione con l'ultimo commit?" needs
a checked answer, not a "yes". Talk to the user in Italian; docs keep their
language (CHANGELOG and README in English, rules and plans in Italian).

## 1. What changed

```sh
git fetch origin master
git log --oneline origin/master..HEAD          # the session's commits
git diff --stat origin/master...HEAD           # files touched
```

If the branch is already merged, use the commits of this session instead
(`git log --since=<session start>`). Note the **last commit** sha: the
answer is about that one.

## 2. Checklist (each item: needed? done? where?)

| Doc | Needed when | Check |
| --- | --- | --- |
| `CHANGELOG.md` | anything a player can see or that changes a save | section `## <next version>` exists (the release stops without it), the change is there under `Added`/`Changed`/`Fixed`, written for players, every sentence true in the code |
| `.claude/rules/<area>.md` | code touched in an area with a rule | find it: `grep -l "<path>" .claude/rules/*.md` against each touched file's `paths:`; traps, measurements and verification found this session are written; nothing stale left ("provvisorio", old offsets, old counts) |
| new rule + CLAUDE.md §7 | a new area with no rule | rule with `paths:` front matter, row in the §7 table |
| `docs/plans/<plan>.md` | the session did a phase of a plan | "Fase N, com'è stata fatta" + handoff, status line at the top |
| `docs/SOULGOLD_PORT_REPORT.md` | anything ported from Soulgold (or explicitly not SG) | row added / updated (CLAUDE.md §6: "tienilo in pari a ogni port") |
| `README.md` | a new player-facing feature | its row in the feature tables |
| `.claude/rules/save.md` | `SAVE_VERSION` or SaveBlock layout changed | section for the new version: offsets, sizes, how it was verified |
| `CLAUDE.md` | only general lessons valid everywhere | area notes go in the rule, not here (CLAUDE.md, top) |

Read each doc **against the code** of the last commit: a doc that describes
an earlier attempt is worse than none.

## 3. Fix and answer

- Fix what is missing in one commit `Docs: <what>` (or fold it into the
  last commit's PR), push.
- Answer the user with a short table: doc → "aggiornato in `<sha>`" /
  "non serve, perché ..." / "aggiornato ora". Name the commit the docs are
  now in step with. If something cannot be checked (e.g. a claim that needs
  the emulator), say so instead of ticking it.
