<p align="center"><img src="HnS_Plus_Logo.png" alt="Pokémon Heart &amp; Soul +" width="640"></p>

# `pokehns-expansion-plus`

**Pokémon Heart & Soul +** is a personal fork of
[**`pokemonHnS-expansion`**](https://github.com/PokemonHnS-Development/pokehns-expansion)
(Heart & Soul 2.0.6, built on pokeemerald-expansion 1.15.2). It back-ports a set
of features from [**Soulgold**](https://eemeliri.github.io/soulgold/), switches
on and finishes a few that were already in the code, and adds some of its own.
The original README of the project this fork comes from is kept, unchanged,
[at the bottom of this page](#original-readme-pokemonhns-expansion).

Everything below is additive: no existing HnS feature was removed, and saves from
the upstream release keep working. Nothing in the save ever moves - new fields
are only ever appended, and a save written before a field existed is migrated on
load (`SAVE_VERSION` in `include/save.h`).

- [AI disclosure](#ai-disclosure)
- [How to play](#how-to-play)
- [New features](#new-features)
- [Credits](#credits)
- [Original README](#original-readme-pokemonhns-expansion)

---

## AI disclosure

This fork is written by an AI, **Claude Code** (Anthropic), **working under my
direction**. I want that stated plainly rather than buried.

- **I decide.** Which features exist, which Soulgold ones are worth bringing
  over, how each one should behave and look, what is kept off by default, what
  is left out. I read the results, play the builds, report what is wrong, and
  say what to change.
- **Claude does the work.** It reads the codebases, writes the C, scripts and
  build tooling, ports code from Soulgold, builds the ROM, and checks the
  result in an emulator. When I steer it away from a first guess, it is
  usually because something it believed turned out to be wrong, and the
  history of this repo has plenty of those corrections.
- **Neither of us is the source of the game.** The game, its art, its music
  and most of its code come from the projects credited below. Nothing here is
  generated art or music. The "+" in the logo and on the title screen is drawn
  by script.

The AI disclosure of the original project, which covers *their* code and not
this fork's, is kept in the original README at the bottom.

---

## How to play

Download the `.bps` patch from the
[latest release](https://github.com/Marcogn/pokehns-expansion-plus/releases/latest)
and apply it to a clean **Pokémon Emerald (USA)** ROM with any BPS patcher, such
as [Flips](https://github.com/Alcaro/Flips) or
[Rom Patcher JS](https://www.marcrobledo.com/RomPatcher.js/). The result is a
32 MiB ROM.

The patch only works on the exact clean ROM: check that yours has SHA-1
`f3ae088181bf583e55daf962a92bb46f4f1d07b7` before patching. A save from the
upstream HnS release can be carried over: rename it to match the new ROM.

The title screen reads **HEART & SOUL +**, with this fork's version under PRESS
START, so it is easy to tell which game is running. What changed in each
release is in [`CHANGELOG.md`](CHANGELOG.md).

---

## New features

Four groups: what was **ported from Soulgold**, what was **already in the
codebase and was switched on or reworked**, what was **ported from other
projects**, and what was **written from scratch** for this fork. Where a feature mixes origins, the notes say which half
is whose.

### Ported from Soulgold

| Feature | Option | Notes |
| --- | --- | --- |
| Overworld speed-up | `OW SPEED` 1x–4x | Cutscenes always run at 1x. R is reserved for the DexNav. |
| Battle speed-up | `BATTLE SPEED` 1x–3x | Runs several software ticks per frame; animations keep their timing. |
| SwSh party menu | `PARTY MENU` CLASSIC/SWSH | Both menus are built and selected at runtime. |
| Soulgold bag screen | — | Replaces the bag graphics and layout. |
| Dark UI | `DARK UI` LIGHT/DARK | Darkens the bag and the whole battle HUD. |
| Shiny healthbox | — | A shiny Pokémon gets a gold battle box, in both themes. |
| Shiny Genome | — | Turns a Pokémon shiny. Sold at the Viridian City Mart for 1000. |
| Compact start menu | — | The menu window grows with the number of entries, so nine fit. Since the strip pause menu (below) it is the list of the places that keep it: Safari Zone, Bug-Catching Contest, Battle Pike and Pyramid, Union Room, link and the Multi Partner Room. |
| Candy Jar | — | A Key Item that skims 90% of the Exp from every battle and turns it into Exp. Candies. Elm's Aide hands it over in the Violet City Pokémon Center along with the Togepi Egg. |
| Shiny parents | — | A Day-Care Egg is 4x as likely to be shiny with one shiny parent, 8x with two. |
| Exp. Candy order | — | Sorting the bag by type lists the Exp. Candies by size, biggest first. |
| Fast HMs | `FAST HMS` OFF/ON | Skips the yes/no prompt, the "used X" message and the field-move pose. Cut, Rock Smash, Strength, Waterfall and Headbutt already had no prompt or animation in HnS, so for them it only drops the message. |
| Battle music theme | `BATTLE MUSIC` DEFAULT / HG JOHTO / HG KANTO / FRLG / RSE / DPPT / RANDOM | Wild, Trainer, Gym Leader and Elite Four battles. DEFAULT keeps each region's own themes. Bosses, rivals, Team Rocket, champions, legendaries, link and facility battles never change. Unlike Soulgold: the default is regional, HG JOHTO is an option, and the Elite Four follows the theme. |
| Night music in Pokémon Centers | `NIGHT CENTER` ON/OFF | At night the Center plays the DPPt night theme. On by default here; always on in Soulgold. |
| Level scaling | `LEVEL SCALING` OFF/ON | Follows your team's average level (Pokémon 7+ levels below your strongest don't count). Trainers: ace one level above it, never below the level HnS gives them, plain level evolutions applied; prize money on the real level plus 1% per 5 levels of the level cap. Wild: 2-4 below it, never below the area's own level, never more than 15 above it and never above the level cap. Scripted battles (red Gyarados, Snorlax…) at the level cap; legendaries keep their level. Engine from Soulgold, rules of HnS. |

### Reworked from existing code

These were already in the codebase, disabled or half-wired, or exist elsewhere
in a different form. The work was turning them on, making them behave, and
reconciling them with Soulgold where it had something better.

| Feature | Option | Notes |
| --- | --- | --- |
| Enhanced DexNav | `ENHANCED DEXNAV` ON/OFF | One switch, **on by default**, over two things that are never wanted apart. The search half is Soulgold's: it always finds a spot, the target neither relocates nor flees, and there is no timer, so creeping stops mattering exactly as it already does in Soulgold. The listing half is not Soulgold's — it gates the area list on the Pokédex the same way HnS does — and shows every species in the area, seen or not. Off leaves the HGSS-style behaviour HnS ships. |
| Follower toggle in both party menus | — | The classic menu already had it; the SwSh menu now does too. |
| Bag icons readable in both themes | — | The registered-item `SEL` badge and the HM badge sat on palette entries that flip with the theme, so each came out as a bright box over the dark bag. Both now use entries that mean the same colour in either theme. Not a port: Soulgold's own HM badge has the same problem. |

Switched on or finished, with nothing written for this fork except the wiring:


| Feature | Where it came from | What was done here |
| --- | --- | --- |
| DexNav | `src/dexnav.c`, already present with `DEXNAV_ENABLED FALSE` | Turned on, given R on foot, made the creeping reachable, and reconciled with Soulgold: search levels, SELECT to unbind, per-row caught counts, and a pile of Soulgold's robustness fixes. It opens on maps with no wild encounters instead of silently refusing. It also searches in Dark Cave once Flash is used: the stock check treated a Flash-lit cave as too dark. Elm's Aide hands it over with the first five Poké Balls, so it appears in the Start menu only from that scene on — by which point the player already has both the Pokégear and the Pokédex. |
| Move relearner from the summary | `P_ENABLE_MOVE_RELEARNERS`, a pokeemerald-expansion feature | HnS drew the prompt but nothing handled START. Wired up, and kept to the Battle Moves page the way Soulgold does. |
| Types shown in battle | `src/type_icons.c`, with `B_SHOW_TYPES SHOW_TYPES_NEVER` | Turned on and aligned to Soulgold: the art is Soulgold's and is asymmetric, so the mirroring, the 4px stagger and the slide directions all had to match. |
| HGSS Pokédex | `src/pokedex_plus_hgss.c`, already present | Used as-is. The party-menu entry point (see *Written from scratch*) is what was added. |
| Wild Pokémon in the overworld | pokeemerald-expansion PR #8434 (expansion 1.16), ported with its fixes up to 1.16.4 | Always on; `WILD BATTLES` OFF turns them off too. Up to 6 on land, 4 in the Safari Zone, 3 on water, in caves and indoors; with it on, tall grass, surfing and caves stop starting random battles, while fishing, Rock Smash, Headbutt, Sweet Scent and the DexNav stay. Written here: a **temperament per species family** rolled at each spawn (wander, watch, come closer, chase, flee, vanish; weights modelled on the Wilds of Kanto mod), lower caps on water, in caves and in the Safari Zone, the Bug-Catching Contest left classic, the Safari Pokéblock bonus, faster spawning where grass is sparse, and fixes for full sprite memory (no crash screen, room kept for icons). On water they swim, with HnS's own surf-mount sprites. Shiny ones sparkle and stay on the map; roamers can appear among them. |

### Ported from other projects

| Feature | Where it came from | Notes |
| --- | --- | --- |
| Quest book | PokemonSanFran's *Unbound Quest Menu* (`complex_quests` branch) | Opened from the Quests icon of the pause menu. For each quest: its state (locked, active, reward waiting, done), what to do next and where, with a picture; A opens its list (the badges, the prizes, the Leaders). R filters by state, Start sorts A-Z, Select marks a favourite; **NEW** marks a quest you have not looked at since it appeared, became "Reward" or was completed. The quests read the state the game already has, so they work on old saves: **Johto Badges** and **Kanto Badges** (the next Gym, one line per badge; Kanto shows up once you are Champion), **The TcloneR** (clones so far, the next prize, one line per prize) and **Red's Path** (the next challenge, one line per Leader). Rewritten on this fork's save data and state sources; follows the Dark UI. |
| Pause menu strip | pollythadon's Unbound-style start menu (`unbound_start_menu.c`) | Start opens a strip of icons over the map instead of the list: Pokédex, DexNav, Party, Bag, Quests, Options, with the name of the entry and the day and time. **Start** saves, **L** opens the Trainer Card, **R** the Pokégear, B closes. **Select** picks up an icon: move it with left and right, put it down with A or Select (B undoes); the order is kept with your save, and an icon you do not have yet keeps its place. The special places listed under *Compact start menu* keep the list. Changed from the original: one page instead of several, Select instead of R to reorder, icons visible under Flash; the DexNav and Quests icons are new, and it follows the Dark UI. |

### Written from scratch

| Feature | Option | Notes |
| --- | --- | --- |
| Pokédex from the party menu | — | New entry in the party menu action list. |
| Guaranteed capture | `EASY CATCH` OFF/ON | Every Ball catches without fail while it is on. |
| Nickname prompts | `NICKNAMES` ON/OFF | When off, catches, gift Pokémon and hatched eggs never ask. |
| Wild encounters | `WILD BATTLES` ON/OFF | When off, grass, surfing, Rock Smash, Sweet Scent and fishing never trigger a wild battle. |
| The TcloneR (side quest) | Starts in Azalea Town; replaced the `TRAINER CATCH` option | **An original side quest of this fork, with its own story and dialogue.** One of Prof. Elm's former assistants went over to Team Rocket and built the TcloneR, a device that clones an opponent's Pokémon in battle on tachyon particles drawn from a tuft of Celebi's hair. You meet him five times, following Team Rocket's story: in Azalea, in Mahogany (in the Rocket hideout, or outside the shop once it is cleared), outside the Goldenrod Radio Tower, outside the Cerulean Pokémon Center, and at last in Prof. Oak's lab, where he stays. Each meeting tells a little more (who he is, Team Rocket, the particles, Celebi, why he did it), and the first one, wherever it is, gives you the TcloneR. **The device**: a key item that stays in the Bag; a Ball thrown at a Trainer's Pokémon then works like on a wild one, and the battle ends with the catch. The Trainer does not count as beaten (no badge, no story step), stops noticing you until you leave the map, and has the full team back every battle. Facility, link and partner battles and Mirror Mode are left out. **Prizes**: each Trainer counts once, and every 10 clones the scientist pays out at your next meeting: 20 Quick Balls, 10 Nuggets, 20 Ultra Balls, 15 Max Repels, 10 Rare Candies, 10 Big Nuggets, then a Totodile, a Chikorita and a Cyndaquil at Lv5, and at 100 clones a Weezing, an Arbok and a Meowth at Lv50; the six Pokémon are shiny and go to the PC. The quest book follows it as **The TcloneR**: clones so far, the next prize, one line per prize. |
| Full party editor (debug) | Debug menu → Party → Edit Pokemon → Edit All… | Level, shininess, nature (the stat one, as a Mint sets it), ability, Tera type, IVs, EVs and moves of a party Pokémon, each screen starting from its current values. |
| Force Shiny (debug) | Debug menu → Flags & Vars → Toggle Force Shiny | Every wild, gift and hatched Pokémon comes out shiny while it is on. |
| Title screen and version | — | A "+" after the HEART & SOUL banner and the fork's version under PRESS START. The save window shows the same version under the play time, so it can be read off when reporting a bug ("dev build" on a build without a release tag). |
| Red's Path | The guide at the entrance of each Kanto Gym, once its Leader is beaten | The eight Kanto Gyms replayed in order with Red's team of the time against the Leaders as he met them, in a VR headset. You play as Red, cannot run, gain no Exp. and lose nothing; each win gives that Leader's ace at Lv70 as a prize (kept by the guide if your party is full). |
| Box sort and search | A on the box name → `SORT` / `SEARCH` | **Sort** this box or all boxes, by Pokédex number or by species name. Numbers follow the Johto Pokédex until the National Dex is unlocked (Prof. Oak says so when he hands it over), then the national one; within a species the highest level comes first, Eggs and empty slots go last. Sorting all boxes asks first. **Search** picks a letter group, then a species present in the boxes, then A jumps to each one box after box, with a shiny sparkle on shiny ones. Both are hidden while holding a Pokémon. Only slots change: nothing new in the save. |

### Shops and prices

Nature Mints cost 100 each. They were already stocked at the Goldenrod flower shop,
behind the third badge and the challenge menu's `MINTS` toggle.

Shop and price changes: Poké Ball 10, Great Ball 15, Ultra Ball 30, Quick Ball 50,
Ability Patch 100, Ability Capsule 75, and the six vitamins 700. Azalea Town stocks
the Quick Ball, Violet City the Ability Patch and Capsule, and Goldenrod 4F now also
sells the six EV-lowering Berries. Town-specific stock is appended to the usual
badge-scaled inventory rather than replacing it, so those towns keep their normal
progression.

What came over from Soulgold, where it lives, what did not and why:
[`docs/SOULGOLD_PORT_REPORT.md`](docs/SOULGOLD_PORT_REPORT.md) (in Italian).

---

## Credits

**This fork** is directed by [**Marcogn**](https://github.com/Marcogn) and
written by **Claude Code** (Anthropic), as described in the
[AI disclosure](#ai-disclosure).
The **TcloneR** side quest is Marcogn's own: the idea, the story, the
scientist and his dialogue.

**Soulgold.** Every feature in *Ported from Soulgold* is the work of the
[Soulgold](https://eemeliri.github.io/soulgold/) project and its author
[**Eemeliri**](https://github.com/eemeliri), and of the people Soulgold credits
in turn: the overworld and battle speed-ups, the bag screen, the dark UI, the
shiny healthbox, the Candy Jar, the Fast HMs, the battle-music theme and the
Pokémon Center night music, the art of the in-battle type icons, the four
Diamond/Pearl tracks, and the search half of the Enhanced DexNav. The bug fixes
listed in the changelog as coming from Soulgold are theirs too. The full
Soulgold credits are worth reading on their own: see the
[Soulgold repository](https://github.com/eemeliri/soulgold) and its site.

- [**Mont**](https://github.com/montmoguri/pokeemerald-expansion/) - the SwSh
  party menu, which reached this fork by way of Soulgold.

**Heart & Soul.** Everything in *Reworked from existing code* came with
**`pokemonHnS-expansion`**, **`pokeemerald-expansion`** or both, and is the work
of their authors, not of this fork: the DexNav, the HGSS Pokédex, the in-battle
type icon code, the move relearner and the overworld wild Pokémon (by
**Bivurnum** and **HashtagMarky**, who credit Pokabbie's *Emerald Rogue*). The
temperament weights are modelled on
[*Wilds of Kanto*](https://github.com/YoDrehDenSwagAuf/overworld-spawn-mod)
(MIT). The game itself, Johto, the story, the
art and the music are the work of the **Pokémon Heart & Soul** team and its
contributors, listed in [`CREDITS.md`](CREDITS.md), which is inherited
unchanged from upstream.

**Other projects.** The quest book screen is PokemonSanFran's
[*Unbound Quest Menu*](https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu),
itself built on the Unbound quest menu, and the work of the people its
documentation credits. The pause menu strip, its band and its Pokédex,
Party, Bag and Options icons are
[**pollythadon**](https://github.com/pollythadon/pokeemerald-expansion)'s
Unbound-style start menu. Both were ported by hand and rewritten where this
fork needed it (save data, state sources, one page, Flash, dark theme).

**The chain below** applies to this fork unchanged: **`pokemonHnS-expansion`**,
RHH's **`pokeemerald-expansion`**, resetes12's **`Modern Emerald`** and pret's
**`pokeemerald`** decompilation. If you use this fork, credit **Pokémon Heart &
Soul** and keep that chain, as the original README asks.

*Pokémon is a trademark of Nintendo, Game Freak and Creatures Inc. This is an
unofficial fan project and is not affiliated with or endorsed by them. It
distributes only a patch: you need your own clean Pokémon Emerald ROM.*

---

<a id="original-readme-pokemonhns-expansion"></a>

# Original README (`pokemonHnS-expansion`)

*Everything below is the README of the project this fork comes from, kept
exactly as it was. "We" and "our" there mean the Heart & Soul team, not this
fork.*

---

![HnS Logo](HnS_Logo.png)

# About `pokemonHnS-expansion`

<!-- If you want to re-record or change these gifs, here are some notes that I used: https://files.catbox.moe/05001g.md -->
<!-- TODO: Actually change these gifs, and generally update contents to convey HnS-specific information -->
![HnS Collage](HnS_Collage_YourAdventure.png)

**`pokemonHnS-expansion`**, aka Pokémon Heart and Soul 2.0, is a GBA ROM hack that is both a remake of GSC and demake of HGSS, with added quality-of-life, customization, and more.  
Originally built on top of [resetes12's **`Modern Emerald`**](https://github.com/resetes12/pokeemerald).  
Now additionally built on top of [RHH's **`pokeemerald-expansion`**](https://github.com/rh-hideout/pokeemerald-expansion) GBA ROM hack base.  
Finally, all of these projects are built on top of [pret's **`pokeemerald`**](https://github.com/pret/pokeemerald) decompilation project.

> Pokémon Heart & Soul brings the classic Johto Region and its iconic story to the world of modern GBA decomp hacking. Built on Modern Emerald and pokeemerald-expansion, this project offers a fresh take on the GSC/HGSS experience, blending key aspects of the Gen 2 and Gen 4 games, while incorporating many modern QoL features, as well as some familiar mechanics from Gen 3 to Gen 9. Not only is Heart & Soul (HnS) a first-of-its-kind, fully completed, playtested, and largely faithful GSC remake / HGSS demake, it's also completely open source, and is intended to be a base for a new generation of Johto rom hacks.

Unfortunately, saves from before 2.0 will not be compatible moving forward.

2.0.1 will be the last "official" release of Pokémon Heart and Soul, after which any bug fixes, content updates, or any propogated updates from **`pokeemerald`** or **`pokeemerald-expansion`** will only be available via community forks of the project.

# [Features](FEATURES.md)

**`pokemonHnS-expansion`** includes a mix of vanilla Emerald/FRLG features, re/de-made implementations of GSC/HGSS features, custom **`Modern Emerald`** features, and both features from [core series Pokémon games](https://bulbapedia.bulbagarden.net/wiki/Core_series) and popular QOL enhancements made available by **`pokeemerald-expansion`**.  
A full list of the features present in Pokémon Heart & Soul 2.0 can be found in [`FEATURES.md`](FEATURES.md)
A full list of the features made available by **`pokeemerald-expansion`** can be found in [`AVAILABLE_FEATURES.md`](AVAILABLE_FEATURES.md).

# [Credits](CREDITS.md)

<!-- TODO: update .all-contributorsrc and CREDITS.md to match https://pokemonhns-development.github.io/pokehns-expansion-documentation/credits.html -->
<!-- [![](https://img.shields.io/github/all-contributors/pokemonHnS-Development/pokemonHnS-expansion/upcoming)](CREDITS.md) -->

<!-- TODO: confirm our actual crediting policy and how best to respect our upstreams -->
If you use **`pokemonHnS-expansion`**, please credit **Pokemon Heart and Soul**, and retain the full chain of credits as best possible.  
If you additionally use a more updated version of **`pokeemerald-expansion`**, please *specifically* credit **RHH (Rom Hacking Hideout)** and include the version number for clarity.
For example:

<!-- TODO: confirm the closest applicable expansion version number -->
```
pokemonHnS-expansion 2.0 is Based off RHH's pokeemerald-expansion version 1.15.1 https://github.com/rh-hideout/pokemonHnS-expansion/
```

Finally, please consider [crediting all contributors](CREDITS.md) involved in the project!

# **`pokemonHnS-expansion`** multiplayer compatibility

- **`pokemonHnS-expansion`** supports trade and link battle multiplayer functionality, which *should* extend to forks built on **`pokemonHnS-expansion`** but cannot be guaranteed.
- **`pokemonHnS-expansion`** is not compatible with official Pokémon games, **`pokemonHnS 1.X`**, **`Modern Emerald`**, or other **`pokeemerald-expansion`** projects.

# [Getting Started](INSTALL.md)

❗❗ **Important**: Do not use GitHub's "Download Zip" option as it will not include commit history. This is necessary if you want to update or merge other feature branches from **`pokeemerald-expansion`**.

If you're new to git and GitHub, [Team Aqua's Asset Repo](https://github.com/Pawkkie/Team-Aquas-Asset-Repo/) has a [guide to forking and cloning the repository](https://github.com/Pawkkie/Team-Aquas-Asset-Repo/wiki/The-Basics-of-GitHub). Then you can follow one of the following guides:

<!-- TODO: update INSTALL.md to refer to HnS-specific things -->
## 📥 [Installing **`pokemonHnS-expansion`**](INSTALL.md)
## 🏗️ [Building **`pokemonHnS-expansion`**](INSTALL.md#Building-pokemonHnS-expansion)

# [Documentation](https://pokemonhns-development.github.io/pokehns-expansion-documentation/)

For our player-facing documentation, visit the [**`pokemonHnS-expansion`** documentation page](https://pokemonhns-development.github.io/pokehns-expansion-documentation/).

# [Contributions and Community](https://discord.gg/ksNTFNSBj)

[![](https://dcbadge.limes.pink/api/server/ksNTFNSBj)](https://discord.gg/ksNTFNSBj)

If - in the window between 2.0 release and 2.0.1 release - you are looking to report a bug, make a suggestion, or give feedback, please join the [Pokémon Heart and Soul Discord server](https://discord.gg/ksNTFNSBj). You are also welcome to join just to participate in the community, including pinging our @guides (and only our guides) for help answering questions not sufficiently covered by our documentation or in-game resources.

# AI Disclosure
Since this is a controversial topic at the moment, we'd like to be transparent about use of AI for this project.

Every line of code written for the game is either hand-written or manually reviewed by a member of the team. However, it is still important to point out that LLMs like Claude Code and GitHub Copilot have been used for some tasks.

Here is what AI has been used for:
- Code Reviews of hand-written code
- Debugging more complex scenarios
- Auto Completion (stuff like repeating lists, DebugPrints, etc.)
- Creating Python Scripts for I/O procedures (like downloading/writing list data, I/O data with Excel, etc. namely for documentation)

AI has not been used for:
- Generating assets of any kind; Art or Music
