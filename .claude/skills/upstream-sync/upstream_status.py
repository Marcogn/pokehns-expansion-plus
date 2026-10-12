#!/usr/bin/env python3
"""Mechanical half of the upstream sync: fetch both upstreams and say what changed.

It never merges, edits or pushes anything. It answers three questions so the
judgement part of the sync (see SKILL.md) starts from facts instead of a
`git log` scrolled by eye:

  1. HnS upstream -> Hasep: how many commits are missing, and which of the
     files they touch Hasep has also changed since the merge base (those are
     where a merge will conflict or, worse, silently mix two versions).
  2. Soulgold upstream -> Marcogn/soulgold: how many commits are missing.
  3. Soulgold upstream -> Hasep: which of the new SG commits touch code that
     was ported into Hasep, code that both games share, SG-only code, or only
     SG content (teams, learnsets, docs...), which is never ported.

The watermark for question 3 lives in state.json next to this file.
"""
import argparse
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

UPSTREAMS = {
    "hasep": ("upstream", "https://github.com/PokemonHnS-Development/pokehns-expansion", "master"),
    "soulgold": ("upstream", "https://github.com/Eemeliri/soulgold", "master"),
}

# Files whose SG version was ported into Hasep. A change here in SG is read in
# full, every time: it is either a fix Hasep needs or a divergence that must be
# kept on purpose. Source: docs/SOULGOLD_PORT_REPORT.md sections 1 and 4.
# Keep this list in step with that report when something new is ported.
PORTED = [
    "src/dexnav.c", "include/dexnav.h", "include/config/dexnav.h", "data/scripts/dexnav.inc",
    "src/candy_jar.c", "include/candy_jar.h", "src/item_use.c",
    "src/swsh_party_menu.c",
    "src/type_icons.c", "include/type_icons.h",
    "src/item_menu.c", "src/item_menu_icons.c", "graphics/bag/soulgold/",
    "src/overworld.c", "include/overworld.h", "src/battle_transition.c",
    "src/battle_main.c", "src/battle_controllers.c", "src/palette.c",
    "src/battle_bg.c", "src/battle_interface.c", "src/battle_gfx_sfx_util.c",
    "src/menu.c", "src/start_menu.c", "src/pokemon_summary_screen.c",
    "src/daycare.c", "src/datetime.c", "src/rtc.c",
    # Fast HMs (SG 1.2): the hunks are ported by hand, so read these diffs whole.
    "src/field_effect.c", "src/fldeff_flash.c", "src/fldeff_rocksmash.c",
    "data/scripts/field_move_scripts.inc", "data/scripts/surf.inc",
]

# SG content: its own game, never ported (report section 5). Prefix match.
CONTENT_PREFIXES = [
    "docs/", "data/maps/", "data/layouts/", "data/tilesets/", "sound/",
    "src/data/trainers", "src/data/battle_partners", "src/data/pokemon/",
    "src/data/text/", "graphics/object_events/", "graphics/pokemon/",
    "graphics/trainers/", "test/", "migration_scripts/", "dev_scripts/",
    "src/data/battle_frontier/",
    # SG rebalances these wholesale (innates, move changes): balance, not fixes.
    "src/data/abilities.h", "src/data/moves_info.h", "src/data/items.h",
]
CONTENT_SUFFIXES = (".md", ".txt", ".png.backup", ".party", ".backup")
CODE_PREFIXES = ("src/", "include/", "asm/", "data/", "tools/", "Makefile", "ld_script")


def git(repo, *args, check=True):
    r = subprocess.run(["git", "-C", repo, *args], capture_output=True, text=True)
    if check and r.returncode != 0:
        sys.exit(f"git {' '.join(args)} failed in {repo}:\n{r.stderr}")
    return r.stdout.strip()


def ensure_upstream(repo, key, no_fetch):
    name, url, branch = UPSTREAMS[key]
    if name not in git(repo, "remote").split():
        git(repo, "remote", "add", name, url)
    if not no_fetch:
        git(repo, "fetch", "--quiet", name, branch)
        git(repo, "fetch", "--quiet", "origin", "master")
    return f"{name}/{branch}"


def classify(path, hasep):
    if any(path == p or (p.endswith("/") and path.startswith(p)) for p in PORTED):
        return "ported"
    if path.startswith(tuple(CONTENT_PREFIXES)) or path.endswith(CONTENT_SUFFIXES):
        return "content"
    if path.startswith(CODE_PREFIXES):
        return "shared" if os.path.exists(os.path.join(hasep, path)) else "sg-only"
    return "content"


def commits(repo, rng):
    out = git(repo, "log", "--reverse", "--format=%h%x09%ad%x09%s", "--date=short", rng)
    return [line.split("\t", 2) for line in out.splitlines() if line]


def files_of(repo, sha):
    return [f for f in git(repo, "show", "--format=", "--name-only", sha).splitlines() if f]


def section(title):
    print(f"\n## {title}\n")


def report_hns(hasep, up):
    section("1. HnS upstream -> Hasep")
    base = git(hasep, "merge-base", "origin/master", up)
    missing = commits(hasep, f"origin/master..{up}")
    print(f"upstream tip `{git(hasep, 'rev-parse', '--short', up)}`, merge base `{base[:9]}`, "
          f"**{len(missing)} commit mancanti** in origin/master")
    if not missing:
        return
    for sha, date, subj in missing:
        print(f"- `{sha}` {date} {subj}")
    theirs = set(git(hasep, "diff", "--name-only", base, up).splitlines())
    ours = set(git(hasep, "diff", "--name-only", base, "origin/master").splitlines())
    both = sorted(theirs & ours)
    print(f"\nFile toccati da upstream: {len(theirs)}. Toccati **anche** da Hasep dopo la base: {len(both)}")
    for f in both:
        print(f"- `{f}`")


def report_sg_fork(sg, up):
    section("2. Soulgold upstream -> Marcogn/soulgold")
    missing = commits(sg, f"origin/master..{up}")
    print(f"upstream tip `{git(sg, 'rev-parse', '--short', up)}`, **{len(missing)} commit mancanti** in origin/master")
    for sha, date, subj in missing:
        print(f"- `{sha}` {date} {subj}")


def report_sg_hasep(sg, hasep, up, since):
    section("3. Soulgold upstream -> Hasep (cosa valutare per il port)")
    if git(sg, "cat-file", "-t", since, check=False) != "commit":
        sys.exit(f"watermark {since} not found in {sg}: fix state.json")
    new = commits(sg, f"{since}..{up}")
    print(f"watermark `{since}` (state.json), **{len(new)} commit nuovi** da valutare")
    if not new:
        return
    by_cat = {"ported": {}, "shared": {}, "sg-only": {}, "content": {}}
    for sha, date, subj in new:
        cats = {}
        for f in files_of(sg, sha):
            c = classify(f, hasep)
            cats.setdefault(c, []).append(f)
            by_cat[c].setdefault(f, []).append(sha)
        tag = " ".join(f"{c}:{len(v)}" for c, v in sorted(cats.items()))
        print(f"- `{sha}` {date} {subj}  [{tag}]")
    labels = {
        "ported": "Codice portato in Hasep: leggere OGNI diff",
        "shared": "Codice presente in entrambi: cercare bug fix applicabili",
        "sg-only": "Codice solo di SG: feature, da classificare (non portare in automatico)",
    }
    for c, label in labels.items():
        if by_cat[c]:
            print(f"\n### {label} ({len(by_cat[c])} file)\n")
            for f, shas in sorted(by_cat[c].items()):
                print(f"- `{f}` <- {', '.join(shas)}")
    print(f"\nSolo contenuto di SG (non si porta): {len(by_cat['content'])} file")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--hasep", default="/home/user/pokehns-expansion-plus")
    ap.add_argument("--soulgold", default="/home/user/soulgold")
    ap.add_argument("--no-fetch", action="store_true", help="use the refs already fetched")
    args = ap.parse_args()
    for p in (args.hasep, args.soulgold):
        if not os.path.isdir(os.path.join(p, ".git")):
            sys.exit(f"{p} is not a clone: both repos are needed (add_repo + clone)")
    with open(os.path.join(HERE, "state.json")) as fh:
        state = json.load(fh)

    print("# Upstream sync status")
    report_hns(args.hasep, ensure_upstream(args.hasep, "hasep", args.no_fetch))
    sg_up = ensure_upstream(args.soulgold, "soulgold", args.no_fetch)
    report_sg_fork(args.soulgold, sg_up)
    report_sg_hasep(args.soulgold, args.hasep, sg_up, state["soulgold_evaluated_upto"])


if __name__ == "__main__":
    main()
