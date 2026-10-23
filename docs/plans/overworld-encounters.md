# Piano: Pokémon selvatici visibili nell'overworld (OWE)

Stato (21/10, dopo la fase 7):

| fase | stato | commit |
| --- | --- | --- |
| 1 diff della PR | fatta | — |
| 2 port, tutto spento | fatta | `499d0c82` |
| 3 accensione (Route 29) | fatta | `d2bb0232` |
| 4 tiro pesato per indole | fatta | `8ed39544` |
| 5 interruttore `WILD POKéMON` nel PC | fatta, poi **tolta** su richiesta dell'utente (21/10) | `158fb6cd`, tolta dopo `3d8e9393` |
| 6 casi speciali | fatta (prove nella regola dell'area) | `9ae6029a` (Gara) + commit successivo |
| 7 documentazione | fatta (`CHANGELOG.md` 2.0.6.PLUS.4, `README.md`, CLAUDE.md, report SG) | commit dopo `f15f4f3d` |

Tutto sul ramo `claude/overworld-encounters-fase-2-jomv77`, PR #19 (bozza),
CI verde. Fasi 1-7 chiuse; dopo, su richiesta dell'utente: interruttore del
PC tolto, spawn a 6 caselle per tentativo (8 da provare), niente comparse su
caselle in uscita, OWE in acqua con gli sprite da
nuoto di `src/surfable.c`, tetto 4 nel Safari.
Misure, trappole, prove in emulatore e ricette (GDB su mGBA, breakpoint per
contare gli incontri a passi) in
[`.claude/rules/overworld_encounters.md`](../../.claude/rules/overworld_encounters.md):
leggerla prima di toccare il codice. Se la tabella delle indoli cambia:
`python3 tools/owe_temperaments.py` (il file generato è nel git).

## 1. Cosa vuole l'utente

- I Pokémon selvatici **girano visibili** sull'erba (e in grotta e in acqua,
  vedi decisioni aperte), con lo sprite overworld che il gioco ha già per i
  follower.
- Comportamenti diversi: **alcuni si allontanano, altri ti attaccano, altri
  indifferenti**. Modello preso dal mod
  [Wilds of Kanto / overworld-spawn-mod](https://github.com/YoDrehDenSwagAuf/overworld-spawn-mod)
  (Gen1Recomp, MIT): **tiro pesato per ogni Pokémon che compare**, pesi base
  moltiplicati per un'affinità di specie (`lib/behavior.lua`,
  `DEFAULT_WEIGHTS`, `SPECIES_AFFINITY`, `Behavior.pick`).
- **Incontri a passi nell'erba spenti**: nell'erba alta resta solo il DexNav.
- **Di default ON**, anche nei salvataggi esistenti; per ora si accende e
  spegne dal PC del Centro Pokémon (D2).
- La "fuga" (21/10) è approvata: il Pokémon ti nota e si allontana
  camminando; se resta bloccato sparisce. Non è la fuga dalla lotta.

Dopo questa feature l'utente ha in mente la **meccanica degli alfa** di
Leggende Pokémon: Arceus. Non fa parte di questo piano, ma il port qui sotto
crea già l'oggetto su cui gli alfa andrebbero agganciati (dimensione dello
sprite, livello, comportamento AGGRESSIVO forzato).

## 2. La tabella delle indoli (passo 0, lo fa l'utente)

Pagina: **https://claude.ai/artifact/5wJUsUmXQhwuyvNksf4uBc** ("Indole dei
selvatici"), privata dell'utente. Sorgente e generatori in
[`overworld-encounters/`](overworld-encounters/):

| file | cosa |
| --- | --- |
| `extract_families.py` | da `src/data/wild_encounters.json` (solo mappe `_HNS`) + blocchi `#if P_FAMILY_X` di `gen_N_families.h` → `data.json` |
| `proposals.py` | indole proposta per famiglia + eccezioni per specie + motivo (`P_`, `EXTRA`) |
| `page_head.html`, `page_body.html`, `page_script.js` | la pagina |
| `build_page.py` | `data.json` + parti → `indole_selvatici.html` (identico al pubblicato) |

Per ripubblicare dopo una modifica: `extract_families.py`, `build_page.py`,
poi `Artifact` publish con `url` = quello sopra (va prima letto con
`action: "read"` da una sessione nuova). Le scelte dell'utente **non** stanno
nell'HTML e sopravvivono alla ripubblicazione.

### Modello dei dati salvati (db dell'artifact)

Si legge con `ArtifactData` (`url` sopra):

- `list` su collection **`families`**: un documento per famiglia **solo se
  l'utente l'ha toccata**. Id = nome della famiglia (`P_FAMILY_<id>`, es.
  `MAGIKARP`, `VOLBEAT_ILLUMISE`, `HITMONS`). Campi:
  `profile` (indole della famiglia), `species` (`{ "GYARADOS": "AGGRESSIVO" }`,
  id senza `SPECIES_`; **vuoto = tutte come la famiglia**), `reviewed` (bool),
  `note` (testo libero dell'utente: leggerlo, può contenere richieste).
  **Famiglia senza documento = vale la proposta** di `proposals.py`
  (famiglia + `EXTRA` + override per specie).
  Attenzione: appena l'utente tocca una famiglia, il documento contiene
  l'insieme completo delle eccezioni di quel momento, comprese quelle
  proposte; non va fuso con le proposte.
- `get` su collection **`config`**, doc **`profiles`**: i pesi delle sei
  indoli se l'utente li ha cambiati. Assente = i valori di `PROF` in
  `page_script.js`.

Le sei indoli e i pesi proposti (guarda, gira, si avvicina, insegue, scappa,
sparisce): NEUTRO 30/40/0/15/15/0 (= pesi base, vale anche per le specie
fuori tabella, es. randomizer), TIMIDO 15/25/0/0/60/0, AGGRESSIVO
15/25/0/55/5/0, CURIOSO 20/20/45/10/5/0, PLACIDO 65/25/0/5/5/0, SFUGGENTE
15/15/0/0/30/40. Ricavati dai pesi terra del mod (guarda 30, gira 35,
aggressivo 15, nascosto 20) togliendo "nascosto" (da noi lo fa già il
DexNav con i Pokémon nascosti) e aggiungendo la fuga, che nel mod c'è solo nel
Safari (idle 35, wander 40, flee 25).

Contenuto: **170 famiglie, 307 specie** (rifatto il 21/10 su `master`
`361d0b6d`; rifarlo con lo script se cambiano le tabelle). 322 specie selvatiche
in 165 mappe `_HNS`, di cui 14 solo pesca/Spaccaroccia (escluse). Per file di
famiglia: 149 gen 1, 98 gen 2, 74 gen 3, 1 senza blocco (`SPECIES_CASTFORM`,
alias di `SPECIES_CASTFORM_NORMAL`, Safari Brush: il generatore del codice
deve risolvere gli alias e darle la famiglia `CASTFORM`). Il conteggio per
"gen" è per famiglia: Crobat sta in gen 1 con Zubat, Magnezone con
Magnemite. I selvatici di Hoenn stanno soprattutto in Zona Safari di Johto,
isole e grotte di Alola, Sinjoh, Snowswept Cavern, Route 49-50. Ci sono anche
forme di Hisui (Kleavor, Ursaluna, Wyrdeer, Sneasler, Overqwil, Growlithe e
Arcanine di Hisui): rilevanti per gli alfa.

Le 19 affinità del mod (tutte Kanto, nessuna Johto) sono riprese e marcate
"dal mod". **Le altre proposte sono mie e non verificate su fonti**: motivi
scritti a memoria sul carattere generale dei Pokémon. Quattro contraddicono il
mod (Kangaskhan, Scyther, Tauros, Pinsir: nel mod scappano nel Safari, qui
AGGRESSIVO, segnate "da decidere").

## 2bis. La tabella compilata (21/10)

`temperaments.json` = proposte di `proposals.py` fuse con le scelte salvate
(`export_temperaments.py <dump>/families`, dump fatto con
`ArtifactData list families out_dir=...`). Formato: `behaviours` (ordine dei
sei pesi), `profiles` (pesi per indole: **quelli proposti**, l'utente non li
ha cambiati, `config/profiles` assente), `families` (per famiglia `profile`,
`species` = eccezioni complete, `source` = `user` o `proposal`).

- **23 famiglie cambiate dall'utente**, 147 lasciate sulla proposta. Nessuna
  marcata "rivista" (la casella non è stata usata) e nessuna nota.
- Indoli delle famiglie: NEUTRO 47, PLACIDO 43, AGGRESSIVO 29, CURIOSO 23,
  TIMIDO 19, SFUGGENTE 9. Eccezioni per specie: 49.
- Scelte dell'utente da ricordare: i tre starter di Kanto e Chikorita
  CURIOSO da piccoli (Charizard e Blastoise AGGRESSIVO, Venusaur PLACIDO);
  Mudkip TIMIDO, Treecko PLACIDO; Chansey e Dratini **TIMIDO** invece di
  SFUGGENTE; Yanma SFUGGENTE; Electrike CURIOSO; Tangela TIMIDO (Tangrowth
  PLACIDO); Zubat TIMIDO in una famiglia AGGRESSIVO; Growlithe di Hisui
  CURIOSO; Sneasel di Hisui PLACIDO; Rattata di Alola AGGRESSIVO; Sandslash
  AGGRESSIVO; Alakazam PLACIDO; Pidgeotto CURIOSO; Butterfree, Wurmple,
  Elekid e Magby TIMIDO; Ledian CURIOSO.
- I quattro "da decidere" contro il mod (Kangaskhan, Scyther, Tauros,
  Pinsir) sono rimasti AGGRESSIVO.
- Il bug della pagina corretto il 21/10 (stato sostituito invece che
  aggiornato, commit `2b10f1d7`) può aver perso modifiche fatte prima di quel
  giorno su famiglie riaperte: l'utente è stato avvisato. Se segnala una
  famiglia che non torna, la si corregge a mano in `temperaments.json`.

## 3. Fatti verificati su upstream (non ri-scoprirli)

- La feature è **PR #8434** di rh-hideout/pokeemerald-expansion ("Overworld
  Encounters", Bivurnum + HashtagMarky, crediti a Emerald Rogue di Pokabbie),
  mergiata in `upcoming` il 29/04/2026, **prima release: expansion 1.16.0**
  (assente in 1.15.3). Noi siamo su 1.15.2.
- **Sorgente del port: il diff del merge della PR (`10ae7f9`, §5 fase 1) più le
  correzioni fino a `expansion/1.16.4`; non `master`.**
  `src/wild_encounter_ow.c` è identico fra 1.16.0 e 1.16.4 (2183 righe, 0
  differenze); su `master`/1.17.1 è stato riscritto (1795 righe) e include
  `mass_outbreak.h`, che da noi non esiste. Da 1.16.x niente outbreak.
- File che la toccano in 1.16.0 (grep su `OWE`, `OverworldWildEncounter`,
  `WE_OW`, `wild_encounter_ow`; i conteggi sono righe che corrispondono):
  `src/wild_encounter_ow.c` (241), `src/event_object_movement.c` (63),
  `src/data/object_events/movement_action_func_tables.h` (47),
  `include/wild_encounter_ow.h` (39), `include/event_object_movement.h` (28),
  `src/data/pokemon/wild_encounter_ow_behavior.h` (24),
  `include/config/wild_encounter.h` (21), `include/pokemon.h` (17),
  `src/field_control_avatar.c` (7), `src/field_effect_helpers.c` (6),
  `include/constants/event_object_movement.h` (6), `src/overworld.c` (4),
  `data/field_effect_scripts.s` (3), `include/constants/event_objects.h` (3),
  `include/config/overworld.h` (2, ambient cries), e un aggancio di una riga
  in `dexnav.c`, `battle_setup.c`, `bike.c`, `load_save.c`, `pokemon.c`,
  `sprite.c`, `include/wild_encounter.h`, `data/scripts/wild_encounter.inc`.
  **Non è l'elenco completo del diff**: modifiche senza quelle parole (es. in
  `wild_encounter.c`, `trainer_see.c`) non compaiono. Il devlog dell'autore
  dice 46 file, +3641/−181. Primo passo del port: ricavare il diff vero.
- **Config**: in 1.16 le opzioni degli incontri stanno in
  `include/config/wild_encounter.h` con prefisso **`WE_`**
  (`WE_VANILLA_RANDOM`, `WE_FLAG_NO_ENCOUNTER`, `WE_OW_ENCOUNTERS`,
  `WE_OWE_*`). Da noi (1.15.2) le stesse cose sono in `overworld.h` con
  prefisso **`OW_`** (`OW_FLAG_NO_ENCOUNTER` = `FLAG_DISABLE_ENCOUNTERS`). Il
  port deve mappare i nomi, non duplicarli.
- `WE_OW_ENCOUNTERS` richiede `OW_POKEMON_OBJECT_EVENTS` (da noi TRUE).
  Upstream **raccomanda** `OW_GFX_COMPRESS FALSE` per la VRAM: da noi non si
  può (§4).
- Comportamenti (`OWE_IGNORE_PLAYER` = wander, `OWE_CHASE_PLAYER_SLOW`,
  `OWE_FLEE_PLAYER_NORMAL`, `OWE_WATCH_PLAYER_NORMAL`,
  `OWE_APPROACH_PLAYER_SLOW`, `OWE_DESPAWN_ON_NOTICE`), tutti con
  `viewDistance 4`, `viewWidth 3`, `activeDistance 5`. "Si avvicina": viene
  fino a `OWE_APPROACH_DISTANCE` 2 con l'emote «?», indietreggia se lo
  raggiungi a 1 (`MovementType_OverworldWildEncounter_ApproachPlayer_Step11`).
  La fuga sparisce se bloccata per `OWE_FLEE_COLLISION_TIME` 6 × 16 frame
  (`WE_OWE_FLEE_DESPAWN`).
- **Il comportamento è fisso per specie** in upstream: campo
  `overworldEncounterBehavior` in `struct SpeciesInfo`, letto da
  `OWE_GetMovementTypeFromSpecies()` (`include/pokemon.h`, inline) e dalle
  sorelle per velocità/vista. In `gen_1_families.h` il campo **non è mai
  valorizzato**: senza intervento tutti = `OWE_IGNORE_PLAYER`. Il tiro pesato
  è il nostro intervento.
- Lo spawn crea il Pokémon **prima** di scegliere il movimento
  (`SetSpeciesInfoForOWE` → `CreateEnemyPartyOWE`, poi `.movementType =
  OWE_GetMovementTypeFromSpecies(...)`): al momento del tiro specie, livello
  e natura sono noti.
- Massimo **4** OWE generati (`OWE_SPAWNS_MAX`).
- **DexNav già integrato**: niente spawn con `DN_FLAG_SEARCHING`
  (`wild_encounter_ow.c`), e `dexnav.c` chiama
  `DespawnAllOverworldWildEncounters(OWE_GENERATED, 0)` prima della
  dissolvenza. Il nostro DexNav usa lo stesso `DN_FLAG_SEARCHING`
  (`FLAG_DEXNAV_SEARCHING`), ma è divergente da upstream (CLAUDE.md §6):
  verificare il punto esatto dove agganciare il despawn.
- Incontri a passi: in 1.16 `ShouldDisableRandomEncounters()` in
  `field_control_avatar.c` restituisce `!WE_VANILLA_RANDOM` (con eccezioni
  per Pike/Pyramid). Da noi `CheckStandardWildEncounter` controlla solo
  `OW_FLAG_NO_ENCOUNTER`. `WE_VANILLA_RANDOM` è di compilazione: per
  un'opzione serve un controllo a runtime.
- Repel/Lure, follower (collisione, rientro nella ball), roamer, Safari,
  shiny con scintilla, bici che spaventa: gestiti da upstream.

## 4. Fatti verificati su Hasep

- **Spazio in ROM**: CLAUDE.md §7bis dava 94,77% (31 799 860 byte) prima
  delle ultime PR: **rimisurare** sul primo build. I 1389
  `graphics/pokemon/**/overworld*.png` fanno **4,6 MB** decompressi in 4bpp
  (misurato da larghezza×altezza dei PNG): con ~1,7 MB liberi spegnere
  `OW_GFX_COMPRESS` non è un'opzione. La VRAM con compressione accesa, 4 OWE
  + follower + NPC va misurata in emulatore; ripiego: `OWE_SPAWNS_MAX` 3.
- **Opzione WILD BATTLES** (`noWildEncounters` in `challengeSettings`) passa
  da `AreWildEncountersDisabledByOption()` (`src/wild_encounter.c`, chiamata
  in quattro punti). Upstream guarda solo il flag: gli OWE devono controllare
  anche questa, altrimenti con WILD BATTLES OFF i Pokémon visibili ti fanno
  ancora lottare.
- **Dove mettere l'opzione nuova**: `struct SoundAndFieldOptions`
  (`include/global.h`) ha **3 bit liberi** (`unused:3`); engine flag liberi
  da `ENGINE_FLAGS_START + 14` (+13 è `FLAG_FAST_HMS`); `SAVE_VERSION` è 8.
  Regole di CLAUDE.md §4: polarità (default ON → campo memorizzato invertito,
  es. `classicWildEncounters` o `noOverworldMons`), `NewGameInitData`
  salva/ripristina `soundAndFieldOptions` dopo `ClearSav3()`, flag specchio
  scritto da `Task_Save` e `NewGameInitData` se gli script devono leggerlo,
  migrazione `saveVersion < 9` se serve.
- HnS ha script propri da tenere d'occhio: Gara Pigliamosche
  (`MAP_NATIONAL_PARK_BUG_CONTEST_HNS`), Zona Safari di Johto
  (`MAP_FUCHSIA_CITY_SAFARI_ZONE_*_HNS`), alberi con Testata
  (`MetatileBehavior_IsHeadbuttTree`, `field_control_avatar.c`: è un incontro
  per interazione, non deve cambiare), incontri per fascia oraria
  (`GetTimeOfDayForEncounters`, compatibili: OWE pesca dalle stesse tabelle),
  `TRAINER CATCH` (§7ter, non tocca i selvatici).
- `#if IS_HNS / #else` in `field_control_avatar.c` è già stato il posto in cui
  una feature era spenta in silenzio (DexNav su R, CLAUDE.md §6): controllare
  ogni aggancio del port in entrambi i rami.

## 5. Piano a fasi (bozza)

Ogni fase finisce con build `make hns` verde e, dove detto, una prova in mGBA
(CLAUDE.md §3).

1. **Diff vero della PR — fatto il 21/10.** Il merge è il commit
   **`10ae7f9`** di `upcoming` ("Overworld Encounters (#8434)", padre
   `39842afc`). Per riottenerlo senza clonare tutto expansion:

   ```sh
   git init up && cd up
   git fetch --filter=blob:none --depth=1 https://github.com/rh-hideout/pokeemerald-expansion.git 10ae7f9f54a806497ac42e744968a5c9e2378785 39842afcb63be3fbf31e6d99971d1f65d5f2add1
   git diff 39842afcb63be3fbf31e6d99971d1f65d5f2add1 10ae7f9f54a806497ac42e744968a5c9e2378785 > pr8434.diff
   ```

   **46 file, +3641/−181**, come dice il devlog. Prova a secco sul nostro
   albero (`patch -p1 --dry-run -F3`, file per file): **falliscono solo 7
   hunk in 6 file**, il resto entra con offset/fuzz.

   | file | hunk | falliti |
   | --- | --- | --- |
   | `src/event_object_movement.c` | 29 | 2 |
   | `include/event_object_movement.h` | 4 | 1 |
   | `include/constants/event_object_movement.h` | 1 | 1 |
   | `include/wild_encounter.h` | 3 | 1 |
   | `src/battle_pyramid.c` | 4 | 1 |
   | `src/pokemon.c` | 5 | 1 |
   | `graphics/field_effects/pics/shiny_sparkle.png` | binario | va copiato |

   Entrano con fuzz/offset (da rileggere hunk per hunk, perché sono file che
   HnS ha modificato): `src/wild_encounter.c` (14 hunk),
   `include/pokemon.h`, `src/sprite.c`, `src/field_effect_helpers.c`,
   `include/config/overworld.h`, `include/sprite.h`, `data/event_scripts.s`.
   Puliti: gli altri 33, fra cui `src/wild_encounter_ow.c` (nuovo, 2178
   righe), `src/field_control_avatar.c`, `src/dexnav.c`,
   `src/follower_npc.c`, `src/overworld.c`, `asm/macros/event.inc`, e la
   guida `docs/tutorials/how_to_overworld_wild_encounters.md` (134 righe, da
   leggere prima di tutto).

   **Base del port: il diff della PR, non il tag 1.16.4.** Il merge usa
   ancora i nomi `OW_` (`OW_FLAG_NO_ENCOUNTER`), come la nostra base; fra
   `10ae7f9` e `expansion/1.16.4` i file OWE cambiano poco
   (`wild_encounter_ow.c` +30/−25, `include/config/wild_encounter.h`
   +36/−25, `wild_encounter_ow.h` +2/−1) e sono in gran parte la
   rinomina `OW_` → `WE_` e `gParties[B_TRAINER_1]` →
   `gParties[B_TRAINER_OPPONENT_A]`. Dopo il port si guarda quel diff e si
   prendono **solo le correzioni vere** (es. `GetOverworlWildEncounterScript`
   che usa lo script dell'oggetto solo per gli OWE `OWE_MANUAL`).

   **API più nuove della nostra base**, da adattare a mano dove il
   compilatore le segnala: `gParties[B_TRAINER_1][0]` (da noi
   `gEnemyParty[0]`), `gWildFeebas` (struct nuova: da noi Feebas passa da
   `CheckFeebas`/`sFeebasRngValue` in `wild_encounter.c`), `sqrt`. Un
   confronto grezzo dei simboli chiamati dal codice aggiunto contro il nostro
   albero ne trova 28 mancanti, ma quasi tutti sono definiti dalla PR stessa
   attraverso macro (`movement_type_def`): l'elenco vero lo dà il primo
   build.
2. **Port con tutto spento — fatto il 21/10.** `WE_OW_ENCOUNTERS FALSE`;
   ROM +22 416 byte (94,94%). Due bug della PR sui percorsi che girano anche
   a feature spenta corretti, adattamenti e prove nella regola dell'area.
3. **Fatto il 21/10** (prove nella regola; acqua e grotta rimandate alla
   fase 6 perché Route 29 non le ha).
   **Accensione su una mappa** (Route 29), con le config del §6 D5 e
   `OWE_SPAWNS_MAX` 6 (D4): spawn, movimento, lotta al
   contatto, cattura (aprire la squadra dopo: lezione del Bad Egg, §7ter),
   VRAM con il follower, ricerca DexNav con OWE presenti.
4. **Fatto il 21/10.** **Tiro pesato**: tabella generata da `temperaments.json`
   (`tools/owe_temperaments.py` → `src/data/pokemon/owe_temperaments.h`,
   array per specie di indole + pesi delle indoli) e sostituzione di
   `OWE_GetMovementTypeFromSpecies()` con il tiro al momento dello spawn.
   **Ereditarietà per famiglia** (decisa dall'utente il 21/10): ogni membro
   di una famiglia della tabella prende l'indole della famiglia anche se non
   è selvatico (Pidgeot = indole di Pidgey, Raichu = di Pikachu), salvo
   un'eccezione per specie. La tabella elenca solo i membri selvatici: i
   blocchi `#if P_FAMILY_X` danno al generatore anche gli altri. Alias
   (`SPECIES_CASTFORM`) e forme risolti allo stesso modo. Specie di
   famiglie assenti dalla tabella = NEUTRO.
5. **Fatto il 21/10**, voce `WILD POKéMON` (testi confermati dall'utente).
   **Interruttore `OVERWORLD MONS` dal PC del Centro Pokémon** (D2): ON =
   OWE accesi e incontri a passi spenti su terra, acqua e grotta (D3); OFF =
   come oggi. `WILD BATTLES` OFF spegne entrambi. **Default ON anche per i
   salvataggi esistenti**, quindi il campo in `soundAndFieldOptions` è
   memorizzato invertito (es. `noOverworldMons`, 0 = ON: un salvataggio
   vecchio legge 0 e si ritrova la feature accesa, come vuole l'utente).
   Niente migrazione per il default; `SAVE_VERSION` resta 8 se non serve
   altro. Il campo va anche salvato e ripristinato in `NewGameInitData`
   (CLAUDE.md §4).
   **Dove**: il menu del PC sta in `data/scripts/pc.inc`
   (`EventScript_AccessPC_BeforeLeague` / `_AfterLeague` e le varianti
   `Fake`, che hanno già la voce `EventScript_ChallengeViewer` →
   `Script_OpenChallengeMenu`). **Confermato dall'utente: una voce propria
   del PC** con un sì/no, non dentro il challenge menu (che ha la regola
   "dopo l'inizio si può solo rendere più facile" e vive in
   `ChallengeSettings`, 0 bit liberi). La voce va aggiunta in tutte e
   quattro le liste (`AccessPC`/`AccessFakePC` × prima/dopo la Lega), con i
   `case` rinumerati, e nel multichoice che le disegna.
   **Perché dal PC**: si cambia stando in un edificio, quindi la mappa
   all'aperto si ricarica uscendo e non serve gestire OWE nati con
   un'impostazione e una mappa già caricata con l'altra (l'utente lo dice
   esplicitamente: "così sistemiamo anche il caricamento delle mappe").
   **In prospettiva** l'utente pensa di tenere solo gli OW e togliere
   l'interruttore: scrivere il codice in modo che spegnerlo sia togliere la
   voce del PC e fissare il valore.
6. **Casi speciali** — decisioni prese dall'utente il 21/10 (D6-D8, §6).
   - **Gara Pigliamosche: fatta.** Durante la Gara (`FLAG_SYS_BUG_CONTEST_MODE`)
     `AreOverworldMonsEnabled()` è falso: niente OWE, incontri a passi
     classici con la tabella della Gara. L'addetto del cancello
     (`Gate_NationalPark_hns`), dopo le regole e solo se l'opzione è ON,
     dice "During the contest, POKéMON won't show themselves easily. They'll
     only appear in tall grass!". Verificato in mGBA: avviso visto, nel
     parco 0 OWE, `StandardWildEncounter` chiamata, lotta (Caterpie Lv14).
   - **Zona Safari: fatta.** Deciso ON (D7). Lotta con un OWE = lotta Safari
     (Safari Ball, POKéBLOCK, GO NEAR). Il bonus della mangiatoia
     (`TryApplyPokeblockBonus`, ora pubblico) è collegato in
     `StartWildBattleWithOWE` come la natura: con un Pokéblock ROSSO due
     Teddiursa OWE di fila con PS/Attacco/Velocità 31, abilità nascosta e
     natura +Attacco.
   - **Grotte buie: fatta** (D8). Dark Cave senza Flash: gli OWE si vedono
     solo nel cerchio di luce, come gli NPC.
   - **Acqua e grotte, tetti 3/3: fatti.** Route 40 in Surf, Dark Cave,
     Sprout Tower 2F: mai più di 3. Lotta in acqua regolare.
   - **Palette riflesso/shiny: fatta.** Follower Caterpie riflesso + Caterpie
     OWE shiny: tag distinti e colori giusti; con il fix #9925 tolto (ROM di
     controllo) il riflesso prende la palette shiny.
   - **Roamer: fatta.** Latias (script debug 5) portato sulla mappa via GDB:
     compare come OWE, la lotta è da roamer (`BATTLE_TYPE_ROAMER`), dopo la
     fuga il roamer cambia mappa.
   - **Città e mappe senza tabella: fatte** (New Bark in fase 3, Sprout
     Tower 1F: 0 OWE).
   - **Trovati nelle prove, corretti**: schermata blu di `assertf` quando la
     VRAM degli sprite è piena (la nostra ROM non è RELEASE); blocco del gioco
     quando un'icona («!», «?», emozione del follower) non trova spazio; riserva
     di 32 tile VRAM per gli OWE. Dettagli nella regola dell'area.
   - **Da decidere con l'utente**: aspetto degli OWE in acqua (oggi sprite
     intero, come sulla terra).
7. **Documentazione**: `CHANGELOG.md` (prossima versione: oggi c'è solo
   il fix delle palette dei riflessi), `README.md`, CLAUDE.md (una riga
   nell'indice esiste già; la regola dell'area passa da "in corso" a
   definitiva), `docs/SOULGOLD_PORT_REPORT.md` solo per dire che non viene
   da Soulgold. Nel CHANGELOG dire anche: interruttore `WILD POKéMON` nel PC,
   default ON anche sui salvataggi vecchi, avviso della Gara.

Dopo, **richiesto dall'utente**: togliere l'interruttore dal PC ("non mi
piace lì dov'è, voglio proprio toglierlo"). Come: togliere la voce
`WILD POKéMON` (`data/scripts/pc.inc`, `CreatePCMultichoice`, i due script
`EventScript_*WildPokemonSwitch`, la special) e far restituire TRUE ad
`AreOverworldMonsEnabled()` dopo i controlli su WILD BATTLES e Gara. Il bit
`noOverworldMons` resta nel salvataggio, ignorato (non riusarlo: nei
salvataggi può valere 1). L'avviso della Gara allora va detto sempre.

Fuori da questo piano, da fare in futuro:

- **Randomizer completo.** Con il randomizer può comparire qualsiasi specie.
  Le famiglie senza alcun membro selvatico in Hasep (leggendari, Mew, fossili
  mai selvatici, famiglie di gen 4+) oggi ricadono su NEUTRO. Per dare loro
  un'indole propria basta estendere `extract_families.py` a tutte le
  famiglie (opzione "includi anche le non selvatiche") e farle passare dalla
  stessa pagina; il generatore della fase 4 non cambia, perché legge già per
  famiglia. Lo si fa quando l'utente lo chiede.

Fuori da questo piano anche: lancio della ball dall'overworld (c'è nel mod, non in
upstream), alfa.

## 6. Decisioni aperte

- **D1 indoli**: la tabella (passo 0). Deciso: i membri non selvatici
  ereditano l'indole della famiglia (fase 4).
- **D2 — deciso il 21/10**: feature **accesa** anche nei salvataggi
  esistenti. Per ora si accende e si spegne **dal PC del Centro Pokémon**
  (fase 5). Il 21/10, vista la voce, l'utente ha detto che non gli piace lì
  e che **vuole toglierla** in futuro (§5, dopo il punto 7).
- **D6 — deciso il 21/10, fatto**: Gara Pigliamosche senza OWE, incontri a
  passi classici; avviso dell'addetto con il testo del punto 6 (l'utente ha
  chiesto "un NPC esistente con dialogo modificato o il tizio che fa iniziare
  la gara", proponendo il senso del testo).
- **D7 — deciso il 21/10**: Zona Safari di Johto con OWE, da provare.
- **D8 — deciso il 21/10**: grotte buie senza Flash con OWE, come gli NPC.
- **D3 — deciso il 21/10**: OWE **anche in acqua e in grotta**, ma **meno
  numerosi** che sull'erba ("tanto c'è il DexNav"). Upstream ha un solo tetto
  (`OWE_SPAWNS_MAX`, che dimensiona anche gli array di ordinamento per età):
  si tiene quello come massimo e si aggiunge un tetto per area al momento
  dello spawn (il controllo `GetNumberOfActiveOWEs(OWE_GENERATED) <
  OWE_SPAWNS_MAX` in `wild_encounter_ow.c`). **Confermato dall'utente:
  erba 6, acqua 3, grotta 3**, con grotta = `MAP_TYPE_UNDERGROUND` **e
  `MAP_TYPE_INDOOR`** (Sprout Tower e simili; confermato dall'utente il 21/10).
- **D4 — deciso il 21/10**: **6** OWE, "se il sistema regge". Limiti veri da
  misurare in fase 3: gli object event sono **16 in tutto**
  (`OBJECT_EVENTS_COUNT`, `include/constants/global.h`) e li condividono
  giocatore, follower, NPC e i Pokémon che, dice l'utente, sono già messi come
  oggetti sulle mappe vicino ai percorsi; le palette sprite sono 16 e ogni specie diversa ne vuole una.
  Upstream non fa spawnare (o fa sparire il più vecchio) quando un limite è
  pieno (`CheckCanLoadOWE`, `TryAndDespawnOldestGeneratedOWE_ToFreeObject`),
  quindi 6 è un tetto, non una garanzia: va provato su una mappa piena di
  NPC. Ripiego: 4. **Sostituzione dei vecchi: sì** (confermato dall'utente
  il 21/10): `WE_OWE_SPAWN_REPLACEMENT TRUE` (default upstream FALSE). A
  tetto pieno il più vecchio sparisce dopo poco e al suo posto ne nasce uno
  nuovo; gli shiny ne sono esclusi grazie a `WE_OWE_PREVENT_SHINY_DESPAWN`
  (D5). Da verificare in fase 3 che il ricambio con i tetti per area (D3)
  conti per area e non sul totale.
- **D5 — deciso il 21/10**: scintilla e suono shiny **sì**
  (`WE_OWE_SHINY_SPARKLE TRUE`); shiny che **non spariscono**
  (`WE_OWE_PREVENT_SHINY_DESPAWN TRUE`: non spariscono fuori schermo né per
  sostituzione finché resti **sulla stessa mappa**; cambiando mappa sì, va
  detto all'utente); **sprite shiny** sì: upstream usa già la palette shiny
  dell'overworld (`overworldShinyPalette`). Il difetto noto della PR
  ("palette sbagliata per gli shiny in acqua") è **analizzato, vedi sotto**,
  e la correzione va portata in fase 2; suono quando un
  OWE sparisce **no** per ora (`WE_OWE_DESPAWN_SOUND FALSE`). Restano ai
  default upstream: despawn entrando in città (`TRUE`, ma dal 23/10 solo
  per gli OWE di un'altra mappa: vedi la regola), collisioni con
  Repel/DexNav attivi (`WE_OWE_NO_REPEL_DEXNAV_COLLISION FALSE`).

### Palette shiny sbagliata vicino all'acqua: causa e correzione (21/10)

La PR la elenca fra i problemi noti senza spiegarla. Causa, letta nel
codice: **il tag della palette shiny di una specie coincide con il tag del
riflesso della stessa specie non shiny.**

- Tag della palette di un Pokémon in overworld: `species + OBJ_EVENT_MON`
  (`1 << 14`), più `OBJ_EVENT_MON_SHINY` (`1 << 13`, cioè `0x2000`) se
  shiny (`include/constants/event_objects.h`, righe 611-614; lo stesso
  calcolo in `CheckCanLoadOWE_Palette` di upstream).
- Tag della palette del riflesso sull'acqua: `tag + PAL_TAG_REFLECTION_OFFSET`,
  che da noi vale **`0x2000`** (`src/field_effect_helpers.c`, riga 21).
- Quindi riflesso di X normale = palette di X shiny. Se il riflesso è
  caricato prima, lo shiny che compare trova il tag già presente e usa la
  palette **filtrata del riflesso** (`ApplyPondFilter`): colori sfasati. Se
  lo shiny c'è già, il riflesso del normale mostra i colori shiny. Con gli
  OWE succede spesso, perché vicino all'acqua compaiono più individui della
  stessa specie con riflessi attivi. Oggi in Hasep non si vede quasi mai:
  servirebbero un follower e uno shiny della stessa specie insieme.
- **Correzione upstream: PR #9925 di expansion** (commit `44b6958d`,
  07/05/2026, "Prevent reflection tag palette collisions"), solo
  `src/field_effect_helpers.c`: offset del riflesso a `0x0800`, offset raw a
  `0x3000`, `STATIC_ASSERT(NUM_SPECIES <= PAL_TAG_REFLECTION_OFFSET, ...)`, e
  i due controlli `paletteNum <= 16` / `>= 16` sostituiti con `== 0xFF` (il
  primo era rovesciato: caricava la palette filtrata quando il tag c'era
  **già**). **`git apply --check` passa pulito sul nostro albero.** Con i
  nuovi offset i tag dei Pokémon stanno in `0x4000-0x7FFF` e il riflesso
  accende il bit `0x0800`, che resta libero finché `NUM_SPECIES <= 0x800`:
  l'assert lo controlla in build. Fuori dai Pokémon, nessun tag nel nostro
  codice cade in `0x3000-0x30FF` (gli `0x3000` in `item_menu_icons.c`,
  `dodrio_berry_picking.c`, `rayquaza_scene.c` sono dimensioni di sprite
  sheet, non tag).
- Verifica in fase 3, in emulatore: forzare uno shiny (`FLAG_FORCE_SHINY` o
  il debug) su una specie d'acqua mentre c'è un individuo normale della
  stessa specie con il riflesso, e misurare i colori dello sprite contro la
  palette shiny vera, **prima e dopo** il fix (CLAUDE.md §6: un caso che
  fallisce senza la modifica).

### Correzioni upstream arrivate dopo la PR (da prendere in fase 2)

Dal log di `upcoming` dopo `10ae7f9`, filtrato su OWE, palette e riflessi:

| PR | commit | data | cosa | in 1.16.4? |
| --- | --- | --- | --- | --- |
| #9925 | `44b6958d` | 07/05 | palette dei riflessi (sopra) | sì (verificato su `field_effect_helpers.c` del tag) |
| #9968 | `17868675` | 12/05 | gli OWE "che non spariscono" non spariscono neanche per il loro movimento (`CanRemoveObjectForOWEMovement`) | sì |
| #10066 | `da607f24` | 27/05 | OWE generati che leggevano dati spazzatura (lo script dell'oggetto solo per gli `OWE_MANUAL`) | sì |
| #10753 | `7644501f` | 05/09 | suono di fuga a ogni passo nelle città | **no**, va presa a parte |

Con `WE_OWE_DESPAWN_SOUND FALSE` (D5) #10753 cambia poco, ma va presa
comunque, così il difetto non torna se un giorno il suono si accende.

## Prompt di handoff per la sessione nuova

> Leggi `docs/plans/overworld-encounters.md`, `CLAUDE.md` e
> `.claude/rules/overworld_encounters.md`. Le fasi 1-7 sono fatte e
> anche le richieste successive (PC, spawn, acqua). Lavora sul ramo della PR
> #19 (`claude/overworld-encounters-fase-2-jomv77`) se è ancora aperta,
> altrimenti chiedimi.

Fonti: [PR #8434](https://github.com/rh-hideout/pokeemerald-expansion/pull/8434),
[devlog di HashtagMarky](https://hashtagmarky.com/devlog/overworld-encounters/),
`include/config/wild_encounter.h` e `src/wild_encounter_ow.c` dei tag
`expansion/1.16.0`, `1.16.4`, `1.17.1` e il diff del merge `10ae7f9` (letti
direttamente),
[overworld-spawn-mod](https://github.com/YoDrehDenSwagAuf/overworld-spawn-mod)
(`lib/behavior.lua`, `lib/safari_compat.lua`, `lib/config.lua`).
