# Piano: Pokémon selvatici visibili nell'overworld (OWE)

Stato: **bozza, in attesa della tabella delle indoli compilata dall'utente**
(passo 0). Quando la tabella è pronta, questo piano va rifinito (§7) e poi
eseguito da una sessione nuova (prompt di handoff in fondo).

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
- **Di default ON, se si riesce** (vedi decisione aperta D2 sui salvataggi
  esistenti).
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

## 3. Fatti verificati su upstream (non ri-scoprirli)

- La feature è **PR #8434** di rh-hideout/pokeemerald-expansion ("Overworld
  Encounters", Bivurnum + HashtagMarky, crediti a Emerald Rogue di Pokabbie),
  mergiata in `upcoming` il 29/04/2026, **prima release: expansion 1.16.0**
  (assente in 1.15.3). Noi siamo su 1.15.2.
- **Sorgente del port: tag `expansion/1.16.4`, non `master`.**
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

1. **Diff vero della PR.** `git diff expansion/1.15.3 expansion/1.16.4` limitato
   ai file della §3 + quelli che il diff rivela; tenere solo le parti OWE (la
   1.16 porta anche altro). Elenco in questo file prima di scrivere codice.
2. **Port con tutto spento** (`WE_OW_ENCOUNTERS FALSE` o equivalente
   `OW_`): la ROM deve comportarsi come prima. Misurare la ROM.
3. **Accensione su una mappa** (Route 29): spawn, movimento, lotta al
   contatto, cattura (aprire la squadra dopo: lezione del Bad Egg, §7ter),
   VRAM con il follower, ricerca DexNav con OWE presenti.
4. **Tiro pesato**: tabella generata dai dati della pagina
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
5. **Opzione `OVERWORLD MONS`** (§4): ON = OWE accesi e incontri a passi
   spenti su terra (e acqua/grotta secondo D3); OFF = come oggi.
   `WILD BATTLES` OFF spegne entrambi.
6. **Casi speciali**: Gara Pigliamosche, Safari, grotte buie/Flash, acqua,
   roamer, città (`WE_OWE_DESPAWN_ON_ENTER_TOWN`), mappe senza tabella.
7. **Documentazione**: `CHANGELOG.md` (prossima versione), `README.md`,
   CLAUDE.md (nuovo paragrafo con trappole e misure),
   `docs/SOULGOLD_PORT_REPORT.md` solo per dire che non viene da Soulgold.

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
- **D2 default per i salvataggi esistenti**: con il campo invertito un
  salvataggio vecchio legge ON e chi aggiorna si trova gli incontri cambiati.
  Alternativa: ON solo per le partite nuove (migrazione che scrive OFF).
- **D3 acqua e grotte**: solo OWE anche lì, o incontri casuali dove non c'è
  erba?
- **D4 numero di spawn** (`OWE_SPAWNS_MAX` 4) e sostituzione dei vecchi
  (`WE_OWE_SPAWN_REPLACEMENT`, default FALSE upstream).
- **D5 config minori** da proporre all'utente con default: scintilla shiny
  (`WE_OWE_SHINY_SPARKLE`), shiny che non spariscono
  (`WE_OWE_PREVENT_SHINY_DESPAWN`), suono del despawn, despawn entrando in
  città, collisioni con Repel/DexNav attivi.

## 7. Da fare quando la tabella è compilata

1. `ArtifactData list` su `families` (e `get config/profiles`), leggere le
   `note`.
2. Contare: famiglie rivedute, cambiate, eccezioni per specie, distribuzione
   delle indoli; segnalare all'utente le famiglie non rivedute.
3. Esportare in `docs/plans/overworld-encounters/temperaments.json` (proposta
   + scelte fuse: è l'input del generatore della fase 4) e committarlo.
4. Rifinire questo piano: fase 1 con il diff vero, D2–D5 con le risposte,
   stime per fase.

## Prompt di handoff per la sessione nuova

> Leggi `docs/plans/overworld-encounters.md` e `CLAUDE.md`. La tabella delle
> indoli è in `docs/plans/overworld-encounters/temperaments.json` (oppure, se
> manca, leggila dall'artifact https://claude.ai/artifact/5wJUsUmXQhwuyvNksf4uBc
> come descritto al §2). Esegui le fasi del §5 in ordine, partendo dalla 1, e
> fermati a chiedermi le decisioni aperte del §6 che non hanno ancora
> risposta.

Fonti: [PR #8434](https://github.com/rh-hideout/pokeemerald-expansion/pull/8434),
[devlog di HashtagMarky](https://hashtagmarky.com/devlog/overworld-encounters/),
`include/config/wild_encounter.h` e `src/wild_encounter_ow.c` dei tag
`expansion/1.16.0`, `1.16.4`, `1.17.1` (letti direttamente),
[overworld-spawn-mod](https://github.com/YoDrehDenSwagAuf/overworld-spawn-mod)
(`lib/behavior.lua`, `lib/safari_compat.lua`, `lib/config.lua`).
