# Changelog of `pokehns-expansion-plus`

One section per release, newest first. The release workflow publishes the
section whose heading is the version being released (`## 2.0.6.PLUS.3` for
tag `v2.0.6.PLUS.3`) as the release notes, and refuses to run without one:
add the section before releasing. Feature details live in the tables of
[`README.md`](README.md).

## 2.0.6.PLUS.8

### Added
- **Item description box** (from Soulgold): picking up, finding or being
  given an item now shows its icon and description in a box at the top of the
  screen, every time, until the "put away in the pocket" message is
  dismissed. Berries picked from a tree show it too. Long descriptions
  wrap by width into two lines.
- **Frostbite** (from Soulgold, from Pokémon Legends: Arceus) replaces
  freezing. A Pokémon hit by an Ice move is no longer locked in place: it
  keeps moving, its special moves do half damage and it loses 1/8 of its HP
  at the end of every turn (the same as a burn in this game). The "FRB"
  icon shows in the battle box, the Summary and both party menus; an Ice
  Heal cures it. A Pokémon already frozen in an old save stays frozen
  until it thaws. The Ice Punch tutor now says "frostbite".

### Changed
- **Red's Path Leaders** now fight with the Smart Trainer AI: they switch a
  Pokémon that is losing the matchup and pick their moves more carefully.
  The Leaders of the post-game keep their stronger AI.
- **The TcloneR quest** no longer speaks of several scientists: there is
  one, and the quest book tells you to find the scientist again.

### Fixed
- **Headbutt with a blank wild Pokémon**: Headbutting a tree could start a
  battle against a nameless Lv0 Pokémon that fainted at once, when a visible
  wild Pokémon appeared nearby during the battle transition. The Headbutt
  script let go of the player before the battle had started, so the
  overworld kept running and the new Pokémon overwrote the one from the
  tree. Headbutt now waits for the battle like Rock Smash does.
- **TcloneR in story scenes**: a Ball no longer works in a battle that is
  part of a scene (the Mahogany hideout alarm grunts, Silver, the Team Rocket
  admins, the Kimono Girls, Red and the like). Catching there cut the scene
  short: the Trainers stayed on the map or walked off into the dark, and the
  story did not move on. The Trainer knocks the Ball away and it is not used
  up. Gym Leaders, the Elite Four and the Champion can still be cloned, and
  so can every Trainer who spots you or that you talk to, Rocket grunts
  included.

## 2.0.6.PLUS.7

### Added
- **The version in the save menu**: the save window now shows the version
  of the hack under the play time, the same one as on the title screen, so
  it can be read off the save screen when reporting a bug.

### Changed
- **New pause menu**: a strip of icons at the bottom of the screen, over the
  map, in the style of Pokémon Unbound: Pokédex, DexNav, Party, Bag, Quests
  and Options, with the name of the selected entry and the day and time.
  **Start** saves, **L** opens the Trainer Card and **R** the Pokégear; B
  closes. **Select** picks up the icon under the cursor: move it with left
  and right, put it down with A or Select (B puts everything back). The
  order is kept with your save, and an icon you do not have yet keeps its
  place for when you get it. The Safari Zone, the Bug-Catching Contest, the Battle Pike and
  Pyramid, the Union Room and link rooms keep the classic list. The bar and
  the Pokédex, Party, Bag and Options icons come from pollythadon's
  Unbound-style start menu; the DexNav and Quests icons are new. It follows
  the Dark UI option.
  Credit: pollythadon's Unbound-style start menu.

### Added
- **Quest book: the first four quests.** The Quests entry of the pause menu
  now lists **Johto Badges** and **Kanto Badges** (one line per Gym, and what
  to do next; Kanto shows up once you are Champion), **The TcloneR** (hidden
  until you meet the first scientist; how many Pokémon you have cloned, how
  many the next prize wants, and one line per prize, "Reward" when one is
  due) and **Red's Path** (hidden until the introduction; one line per
  challenge, "Reward" when a prize waits). A finished quest shows where it
  ended, not its first step. **NEW** marks a quest you have not looked at
  since it appeared, became "Reward" or was completed. Select keeps the
  cursor on the quest it marks as favourite.
  They read the state the game already has, so they work on old saves.
  Credit for the screen: PokemonSanFran's Unbound Quest Menu.
- **Red's Path**, the eight Kanto Gyms replayed with Red's team of the time
  against the Leaders as he met them (FireRed's). Three years after Red saved
  the region from Team Rocket, the Pokémon League lets trainers relive his
  Gym battles in first person with an interactive VR headset.
  - **Where**: the guide at the entrance of each Kanto Gym, once you have
    beaten that Gym's own Leader. Erika's Gym now has a guide too. The first
    time you walk out of a Kanto Gym after beating its Leader, the guide
    calls you over and explains.
  - **In order**, like Red: Brock, Misty, Lt. Surge, Erika, Koga (in
    Fuchsia), Sabrina, Blaine (on the Seafoam Islands), Giovanni (in
    Viridian). A guide whose challenge is not next tells you which one is,
    and whether you still have to beat that Gym's Leader first.
  - **The scene**: you put on the headset, and in the Gym you are Red, in
    front of the Leader as he met them (Koga instead of Janine, Giovanni
    instead of Blue), then the battle. In it you play as Red (his FireRed
    sprite and mugshot), you cannot run, Red's Pokémon gain no experience
    and do not evolve, winning pays no prize money, and losing costs no
    money and does not send you to a Pokémon Center: your own team is back
    either way, and you can try again. The battle texts call you RED.
  - **Prizes**: each Leader's ace (Onix, Starmie, Raichu, Vileplume,
    Weezing, Alakazam, Arcanine, Rhydon) at Lv70 with its level-up moves,
    all IVs 31, a random nature, the Leader as its Original Trainer and a
    Poké Ball. It always goes to your party: with a full party the guide
    keeps it until you come back with room. Shininess is rolled when you
    collect it, at the normal odds: save in front of the guide and you can
    reset for a shiny.

## 2.0.6.PLUS.6

### Changed
- **Level scaling: routes you left behind no longer follow your team.** A wild
  Pokémon now ends up at most 15 levels above what the area would give it
  without scaling, and never above the current level cap. Where you are
  playing nothing changes (Route 43 with a Lv38 team: still 34-36); going
  back is different: Route 29 never goes above Lv18 (table 2-3), and
  Kanto's Route 1 tops out at Lv57-63 even with a Lv100 team (it was 96-98).
  Trainers are unchanged. The option's description says so.
- **The TcloneR replaces the `TRAINER CATCH` option.** Catching a Trainer's
  Pokémon is no longer a switch in the challenge menu: it works while the
  TcloneR, a new key item, is in your Bag. How it works is unchanged: one
  Pokémon per battle, the battle ends with the catch, the Trainer is not
  beaten and challenges you again. Saves that had the option on start with
  it off, like everyone else, until they get the item.
- **Clones count towards rewards.** Every Trainer counts once: catching from
  the same Trainer again still works, it just scores nothing (a rematch with a
  new team counts as a new Trainer). In a battle against two Trainers the
  clone counts for the one it belonged to.
- **Rewards every 10 clones, up to 100**: 20 Quick Balls, 10 Nuggets, 20 Ultra
  Balls, 15 Max Repels, 10 Rare Candies, 10 Big Nuggets, then a shiny
  Totodile, Chikorita and Cyndaquil at Lv5 and, at 100, a shiny Weezing,
  Arbok and Meowth at Lv50. The Pokémon always go to the PC. With a full Bag
  or full Boxes the reward waits for the next time.
- **Elm's former assistant hands out the TcloneR and the rewards.** You meet
  him in five places, following Team Rocket's story: Azalea Town (by the
  Slowpoke Well), the Mahogany hideout entrance (or outside the shop once it
  is cleared), outside the Radio Tower once Team Rocket takes it, outside the
  Cerulean Pokémon Center, and finally Prof. Oak's lab, where he stays for
  good. Each meeting pays out every reward you have earned so far.
- Meetings only go forward: meet him further along and the earlier ones are
  gone. Whichever meeting comes first gives you the TcloneR, and his lines
  adjust so nothing is said twice or answers something never said, so a save
  already past Azalea is not stuck.

### Fixed
- **Overworld Pokémon in towns no longer vanish at every step.** In towns
  and cities with wild Pokémon (Lake of Rage, Violet, Cherrygrove, Ecruteak,
  Olivine, Cianwood, Blackthorn, New Bark and more) every visible Pokémon
  disappeared as soon as you moved one tile, so it looked like they fled from
  far away and could not be reached. They now stay; the Pokémon of the route
  you came from still disappear when you walk into town.

## 2.0.6.PLUS.5

### Added
- **Level scaling for Trainers and wild Pokémon**, one option in the BATTLE
  page of the options menu (`LEVEL SCALING`), OFF by default: with it off
  the game plays exactly as before, saves from earlier versions included.
- Both follow the average level of your team, leaving out any Pokémon 7 or
  more levels below your strongest, so a fresh catch or an HM carrier does
  not pull everything down.
- **Trainers**: the strongest Pokémon of a team comes one level above your
  average (or level with it, fixed per Trainer, so a rematch brings the same
  levels); the others keep their distance from it, at most 2 levels. **No
  Pokémon is ever below the level HnS gives it**, Gym Leaders included: if
  you are behind, you play HnS as it is. A Trainer's Pokémon evolves when the
  level allows it and the evolution has only one possible result (an Ivysaur
  at 38 is a Venusaur; Slowpoke, Eevee, Kirlia and trade or stone
  evolutions stay as written). Their moves stay as written. The first rival
  battle and the partner in multi battles are not scaled.
- **Prize money** from a scaled Trainer is worked out on the level you
  actually fought, plus 1% for every 5 levels of the current level cap
  (+7% at cap 38, +14% at 70).
- **Wild Pokémon** come 2 to 4 levels below your average, never below the
  level of the area: random encounters, visible ones, fishing, Rock Smash,
  Headbutt and the DexNav (the level the search shows is the one you
  battle). Repel and Keen Eye compare the level you would actually meet.
  The Bug-Catching Contest is left as it is.
- **Scripted battles** (the red Gyarados, Snorlax, Sudowoodo, Lapras, the
  Rocket Electrode…) come at the current level cap,
  never below their own level. With `LEVEL CAP` off in the challenge
  settings the cap is the NORMAL one for your badges. Legendary, mythical
  and Ultra Beast Pokémon, roamers, the Fighting Dojo training battles and
  the Route 41 Tentacruel keep their level.
- The engine comes from Soulgold, with HnS rules instead of Soulgold's.

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
- On water they swim, half under the surface, with the same sprites HnS uses
  for the Pokémon you surf on (37 of the 41 wild water species; Golbat,
  Grimer, Muk and Magikarp keep their own sprite).
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
  Pokémon it showed as a follower's reflection in shiny colours. The fix's
  own tags for untagged palettes overlapped HnS's surf mount palettes and
  were moved.

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
