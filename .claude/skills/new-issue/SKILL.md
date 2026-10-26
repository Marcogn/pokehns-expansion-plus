---
name: new-issue
description: Open a GitHub issue on Marcogn/pokehns-expansion-plus for a bug, feature request or task, in English, with context checked against the code and acceptance criteria. Use when the user asks to create, open or file an issue, or invokes /new-issue.
---

# New issue

Issues are written in **English** (the user's choice); talk to the user in
Italian. The repo has no labels of its own and the templates in
`.github/ISSUE_TEMPLATE/` are inherited from expansion: ignore them.

## 1. Before writing

1. **Duplicates**: `search_issues` on `repo:Marcogn/pokehns-expansion-plus`
   with the key words, open and closed. If one exists, show it and ask
   whether to comment there instead.
2. **Context from the code**: find the files, functions and constants
   involved and cite them as paths (`src/strip_start_menu.c`,
   `UseStripStartMenu()`). Only facts you checked; anything you did not
   verify goes under "Open points" or is marked "(to verify)".
3. For a bug: the version (shown on the title screen and in the save menu),
   steps, expected vs actual. If the user has a screenshot or a `.srm`, say
   in the issue that a save reproduces it (do not attach the save).
4. The relevant `.claude/rules/` file, if any: it often already names the
   trap.

## 2. Body (model: issues #7 and #39)

```markdown
## Context            (or "Request" for a feature, "Bug" with steps for a bug)
...facts with file paths...

## Plan              (when the approach is known; checkboxes)
- [ ] ...

## Open points       (decisions not taken, things to verify)
- **<point>**: ...

## Acceptance criteria
- [ ] observable checks (emulator screenshot, light and dark theme, old save loads, ...)
- [ ] Noted in `CHANGELOG.md` under the next version.
```

Title: short, imperative, in English ("Show the hack's version in the
pause menu"). If it is big enough to need phases, add one line suggesting
`/new-plan`.

## 3. Create and report

Create it with `issue_write` (GitHub MCP), no labels, no assignee unless
asked. Footer: the Claude Code attribution footer required for every
GitHub post. Reply with the link and one line on what it says. A commit
that resolves it later uses `(closes #N)` in its subject, like
`5bb3d3f2`.
