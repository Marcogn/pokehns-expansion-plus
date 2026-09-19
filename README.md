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

# About this fork: `pokehns-expansion-plus`

This repository is a personal fork of **`pokemonHnS-expansion`** that back-ports a
set of features from [**Soulgold**](https://eemeliri.github.io/soulgold/), plus a
few changes of its own. Everything below is additive: no existing HnS feature was
removed, and the save layout is unchanged, so saves from the upstream release keep
working.

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
| Compact start menu | — | The menu window grows with the number of entries, so nine fit. |

### Added here

| Feature | Option | Notes |
| --- | --- | --- |
| Pokédex from the party menu | — | New entry in the party menu action list. |
| Guaranteed capture | `EASY CATCH` OFF/ON | Every Ball catches without fail while it is on. |
| Nickname prompts | `NICKNAMES` ON/OFF | When off, catches, gift Pokémon and hatched eggs never ask. |
| Wild encounters | `WILD BATTLES` ON/OFF | When off, grass, surfing, Rock Smash, Sweet Scent and fishing never trigger a wild battle. |
| DexNav: show all | `DEXNAV SHOW ALL` OFF/ON | Lists every species in the area, not only the ones the Pokédex has seen. |
| DexNav: cave fix | `DEXNAV CAVE FIX` OFF/ON | Uses Soulgold's tile picking in caves, water and indoor maps, so a search always finds a spot, and stops the target relocating as you approach. Off leaves the behaviour HnS ships. |
| Follower toggle in both party menus | — | The classic menu already had it; the SwSh menu now does too. |

### Switched on and finished here

These were already in the codebase, disabled or half-wired. Nothing below was
written for this fork; the work was turning it on and making it behave.

| Feature | Where it came from | What was done here |
| --- | --- | --- |
| DexNav | `src/dexnav.c`, already present with `DEXNAV_ENABLED FALSE` | Turned on, given R on foot, made the creeping reachable, and reconciled with Soulgold: search levels, SELECT to unbind, per-row caught counts, and a pile of Soulgold's robustness fixes. It opens on maps with no wild encounters instead of silently refusing. |
| Move relearner from the summary | `P_ENABLE_MOVE_RELEARNERS`, a pokeemerald-expansion feature | HnS drew the prompt but nothing handled START. Wired up, and kept to the Battle Moves page the way Soulgold does. |
| Types shown in battle | `src/type_icons.c`, with `B_SHOW_TYPES SHOW_TYPES_NEVER` | Turned on and aligned to Soulgold: the art is Soulgold's and is asymmetric, so the mirroring, the 4px stagger and the slide directions all had to match. |
| HGSS Pokédex | `src/pokedex_plus_hgss.c`, already present | Used as-is. The party-menu entry point above is what was added. |

Nature Mints cost 100 each. They were already stocked at the Goldenrod flower shop,
behind the third badge and the challenge menu's `MINTS` toggle.

Shop and price changes: Poké Ball 10, Great Ball 15, Ultra Ball 30, Quick Ball 50,
Ability Patch 100, Ability Capsule 75, and the six vitamins 700. Azalea Town stocks
the Quick Ball, Violet City the Ability Patch and Capsule, and Goldenrod 4F now also
sells the six EV-lowering Berries. Town-specific stock is appended to the usual
badge-scaled inventory rather than replacing it, so those towns keep their normal
progression.

### Credits for this fork

The ported features are the work of the Soulgold project and the people it credits
in turn. In particular:

- [**Soulgold**](https://github.com/eemeliri) — the overworld and battle speed-ups,
  the bag screen, the dark UI and the shiny healthbox all come from there.
- [**Mont**](https://github.com/montmoguri/pokeemerald-expansion/) — the SwSh party
  menu, which reached this fork by way of Soulgold.
- The full Soulgold credits list is worth reading on its own: see the
  [Soulgold repository](https://eemeliri.github.io/soulgold/).

The features in *Switched on and finished here* came with the
**`pokemonHnS-expansion`** / **`pokeemerald-expansion`** codebase and are the
work of their authors, not of this fork. Their contributors are listed in
[`CREDITS.md`](CREDITS.md), which is inherited unchanged from upstream.

The upstream chain below — **`pokemonHnS-expansion`**, RHH's
**`pokeemerald-expansion`**, **`Modern Emerald`** and pret's **`pokeemerald`** —
applies to this fork unchanged.

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
