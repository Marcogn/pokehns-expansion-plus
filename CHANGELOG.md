# Changelog of `pokehns-expansion-plus`

One section per release, newest first. The release workflow publishes the
section whose heading is the version being released (`## 2.0.6.PLUS.3` for
tag `v2.0.6.PLUS.3`) as the release notes, and refuses to run without one:
add the section before releasing. Feature details live in the tables of
[`README.md`](README.md).

## 2.0.6.PLUS.4

### Added
- **Wild Pokémon visible in the overworld.** They walk in tall grass, on
  water and in caves, up to 6 at a time on land, 4 in the Safari Zone and 3
  on water, in caves and indoors. Walk into one, or let it reach you, to
  battle it; tall grass, surfing and caves no longer start battles at
  random. Fishing, Rock Smash, Headbutt trees, Sweet Scent and the DexNav
  work as before. From pokeemerald-expansion (PR #8434, expansion 1.16),
  with its fixes up to 1.16.4.
- **Each wild Pokémon has a temperament**, rolled when it appears: some
  wander, some watch you, some come to look, some chase you, some run away
  and some vanish when they notice you. The odds depend on the species
  family (a table reviewed by hand, modelled on the Wilds of Kanto mod), so
  two Pidgey can behave differently.
- Shiny ones sparkle and play the shiny sound when they appear, and do not
  disappear while you stay on the map. Roamers can show up among them, and
  a battle with one is a normal roamer battle.
- Always on, saves from earlier versions included. `WILD BATTLES` OFF in the
  challenge settings still turns off all wild battles, visible ones too.
- Bug-Catching Contest: the National Park keeps the classic random
  encounters during the contest, and the gate attendant says so after the
  rules.
- Safari Zone: battles with visible Pokémon are Safari battles, and a
  Pokéblock feeder affects them as it does random encounters (nature,
  perfect IVs, Hidden Ability).

### Fixed
- **The game could freeze when an icon had no room on screen.** If the
  "!", "?", heart or a follower's emote could not get a sprite, the script
  waiting for it (talking to the follower, a Trainer spotting you) waited
  forever. The icon is now skipped. Rare before, likely with many sprites on
  screen; found while testing the visible wild Pokémon.
- **DexNav in Dark Cave after Flash.** Flash only widens the light to level
  1, never back to 0, and the DexNav treated anything above 0 as too dark:
  searches and hidden Pokémon stayed off in a Flash-lit cave. Reported in
  Mikeagan's fork, which fixed the search but not the hidden Pokémon.
  Soulgold never showed it because there Flash lights caves fully.
- **Pokédex: Espeon and Umbreon on Eevee's EVO page.** The page prints at
  most 10 methods and Eevee has 12 in HnS, Espeon and Umbreon last: they
  had an icon but no method. Each evolution's first method now comes before
  any second method for the same one, and Eevee's Johto spots come before
  the Kanto ones, so the lines that drop are Viridian Forest and Seafoam
  Islands. Evolutions themselves are unchanged. Day
  and Night read D and N. Also reported in Mikeagan's fork, which fixed it
  by adding Sun and Moon Stone evolutions instead.
- **Reflections on water could take another sprite's palette.** A Pokémon's
  reflection used the same palette tag as the shiny form of that species,
  and the high-bridge reflection the same as Pidgey's: whichever loaded
  first coloured the other. From expansion (PR #9925); with visible wild
  Pokémon it showed as a follower's reflection in shiny colours.

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
