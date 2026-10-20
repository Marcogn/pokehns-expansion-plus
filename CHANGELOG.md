# Changelog of `pokehns-expansion-plus`

One section per release, newest first. The release workflow publishes the
section whose heading is the version being released (`## 2.0.6.PLUS.3` for
tag `v2.0.6.PLUS.3`) as the release notes, and refuses to run without one:
add the section before releasing. Feature details live in the tables of
[`README.md`](README.md).

## 2.0.6.PLUS.4

### Fixed
- **DexNav in Dark Cave after Flash.** Flash only widens the light to level
  1, never back to 0, and the DexNav treated anything above 0 as too dark:
  searches and hidden Pokémon stayed off in a Flash-lit cave. Reported in
  Mikeagan's fork, which fixed the search but not the hidden Pokémon.
  Soulgold never showed it because there Flash lights caves fully.
- **Pokédex: Espeon and Umbreon on Eevee's EVO page.** The page prints at
  most 10 methods and Eevee has 12 in HnS, Espeon and Umbreon last: they
  had an icon but no method. Each evolution's first method now comes before
  any second method for the same one, so the lines that drop are Glaceon's
  Ice Path and Seafoam Islands. Evolutions themselves are unchanged. Day
  and Night read D and N. Also reported in Mikeagan's fork, which fixed it
  by adding Sun and Moon Stone evolutions instead.

## 2.0.6.PLUS.3

### Added
- **PC boxes: SORT.** This box or all boxes, by Pokédex number or by name.
  Johto numbers until the National Dex, national ones after; highest level
  first within a species; Eggs and empty slots last. Sorting all boxes asks
  first.
- **PC boxes: SEARCH.** Letter group, then species, then A jumps to each
  match box after box. Shiny matches get a sparkle.
- Prof. Oak mentions the national sort order when he hands over the
  National Dex.

### Fixed
- A follower re-entering its Poké Ball after surfing could read past its
  animation table (from Soulgold).

### Upstream
- Soulgold evaluated up to `4d4acbe98`. HnS: no new commits.

## 2.0.6.PLUS.2

### Added
- **Fast HMs** (`FAST HMS`, from Soulgold 1.2): no prompt, message or pose
  for field moves.
- **Battle music theme** (`BATTLE MUSIC`, from Soulgold 1.2): HG Johto, HG
  Kanto, FRLG, RSE, DPPt or random for wild, Trainer, Gym and Elite Four
  battles.
- **Night music in Pokémon Centers** (`NIGHT CENTER`, from Soulgold 1.2).
- The title screen shows the version under PRESS START.

### Fixed (from Soulgold 1.2)
- Mirror Armor no longer leaves the attacker as the target of the rest of
  the move.
- Bad Dreams no longer hides later ability pop-ups.
- Revive and half/quarter heals on a Pokémon with 1-3 max HP.
- A Pokémon fainting to hazards no longer runs its switch-in effects.
- Party count after a catch that swaps out a party Pokémon.
- Day-Care Everstone on a foreign regional father.
- SwSh party menu: confirming an incomplete team in battle facilities.

## 2.0.6.plus.1

First release of the fork. Everything listed in the tables of
[`README.md`](README.md) up to this point: the Soulgold ports (speed-ups,
SwSh party menu, bag, dark UI, shiny healthbox, Candy Jar, …), the
features added here (Enhanced DexNav, Trainer catch, Easy catch, …) and the
ones switched on and finished (DexNav, move relearner, type icons).

# pokeemerald-expansion changelogs

Inherited from upstream, for the engine this fork is built on.

### 1.10.x
- **[Version 1.10.2](docs/changelogs/1.10.x/1.10.2.md) - 🧹 Bugfix Release**
- **[Version 1.10.1](docs/changelogs/1.10.x/1.10.1.md) - 🧹 Bugfix Release**
- **[Version 1.10.0](docs/changelogs/1.10.x/1.10.0.md) - ✨ Feature Release**

### 1.9.x
- **[Version 1.9.4](docs/changelogs/1.9.x/1.9.4.md) - 🧹 Bugfix Release**
- **[Version 1.9.3](docs/changelogs/1.9.x/1.9.3.md) - 🧹 Bugfix Release**
- **[Version 1.9.2](docs/changelogs/1.9.x/1.9.2.md) - 🧹 Bugfix Release**
- **[Version 1.9.1](docs/changelogs/1.9.x/1.9.1.md) - 🧹 Bugfix Release**
- **[Version 1.9.0](docs/changelogs/1.9.x/1.9.0.md) - ✨ Feature Release**

### 1.8.x
- **[Version 1.8.6](docs/changelogs/1.8.x/1.8.6.md) - 🧹 Bugfix Release**
- **[Version 1.8.5](docs/changelogs/1.8.x/1.8.5.md) - 🧹 Bugfix Release**
- **[Version 1.8.4](docs/changelogs/1.8.x/1.8.4.md) - 🧹 Bugfix Release**
- **[Version 1.8.3](docs/changelogs/1.8.x/1.8.3.md) - 🧹 Bugfix Release**
- **[Version 1.8.2](docs/changelogs/1.8.x/1.8.2.md) - 🧹 Bugfix Release**
- **[Version 1.8.1](docs/changelogs/1.8.x/1.8.1.md) - 🔥 HOTFIX Release**
- **[Version 1.8.0](docs/changelogs/1.8.x/1.8.0.md) - ✨ Feature Release**

### 1.7.x
- **[Version 1.7.4](docs/changelogs/1.7.x/1.7.4.md) - 🧹 Bugfix Release**
- **[Version 1.7.3](docs/changelogs/1.7.x/1.7.3.md) - 🧹 Bugfix Release**
- **[Version 1.7.2](docs/changelogs/1.7.x/1.7.2.md) - 🧹 Bugfix Release**
- **[Version 1.7.1](docs/changelogs/1.7.x/1.7.1.md) - 🧹 Bugfix Release**
- **[Version 1.7.0](docs/changelogs/1.7.x/1.7.0.md) - ✨ Feature Release**

### 1.6.x
- **[Version 1.6.2](docs/changelogs/1.6.x/1.6.2.md) - 🧹 Bugfix Release**
- **[Version 1.6.1](docs/changelogs/1.6.x/1.6.1.md) - 🔥 HOTFIX Release**
- **[Version 1.6.0](docs/changelogs/1.6.x/1.6.0.md) - ✨ Feature Release**

### 1.5.x
- **[Version 1.5.3](docs/changelogs/1.5.x/1.5.3.md) - 🔥 HOTFIX Release**
- **[Version 1.5.2](docs/changelogs/1.5.x/1.5.2.md) - 🧹 Bugfix Release**
- **[Version 1.5.1](docs/changelogs/1.5.x/1.5.1.md) - 🧹 Bugfix Release**
- **[Version 1.5.0](docs/changelogs/1.5.x/1.5.0.md) - ✨ Feature Release**

### 1.4.x
- **[Version 1.4.3](docs/changelogs/1.4.x/1.4.3.md) - 🧹 Bugfix Release**
- **[Version 1.4.2](docs/changelogs/1.4.x/1.4.2.md) - 🧹 Bugfix Release**
- **[Version 1.4.1](docs/changelogs/1.4.x/1.4.1.md) - 🔥 HOTFIX Release**
- **[Version 1.4.0](docs/changelogs/1.4.x/1.4.0.md) - ✨ Feature Release**

### 1.3.x
- **[Version 1.3.0](docs/changelogs/1.3.x/1.3.0.md) - ✨ Feature Release**

### 1.2.x
- **[Version 1.2.0](docs/changelogs/1.2.x/1.2.0.md) - ✨ Feature Release**

### 1.1.x
- **[Version 1.1.1](docs/changelogs/1.1.x/1.1.1.md) - 🧹 Bugfix Release**
- **[Version 1.1.0](docs/changelogs/1.1.x/1.1.0.md) - ✨ Feature Release**

### 1.0.x
- **[Version 1.0.0](docs/changelogs/1.0.x/1.0.0.md) - ✨ Feature Release**

### Pre-1.0.x:
- **[Version 0.9.0](docs/changelogs/0.9.x/0.9.0.md) - 🦕 Retroactive Version**
