# Piano: descrizione degli oggetti, Frostbite, AI di Red's Path, documentazione per gli agenti

**Stato (26/10): fasi 1 e 2 fatte. Prossima: fase 3.**

## 1. Cosa si fa

- Quando si ottiene un oggetto (a terra, nascosto o regalato) compaiono in
  alto la sua **icona e la sua descrizione**, come in Soulgold.
- **Frostbite** (Leggende Pokémon: Arceus) prende il posto del congelamento:
  il Pokémon non resta bloccato, ma perde 1/8 dei PS a ogni turno (come la
  scottatura, vedi §5.1; il piano diceva 1/16) e le sue mosse speciali fanno
  metà danno.
- I capi di **Red's Path** usano l'AI "Smart Trainer".
- Il testo della missione **TcloneR** smette di parlare di "scienziati" al
  plurale: lo scienziato è uno solo e si sposta.
- La **documentazione per gli agenti** si sfoltisce: meno contesto caricato
  in ogni sessione, nessuna lezione persa.

Il summary "stile Bianco/Nero" resta **fuori** da questo piano (§6).

## 2. Decisioni prese (26/10)

| Tema | Decisione |
| --- | --- |
| Descrizione oggetto | `OW_ITEM_DESCRIPTIONS_ALWAYS`, per **ogni** oggetto ottenuto (a terra, nascosto, regalo di un NPC), come Soulgold. Niente modalità "solo la prima volta" (§4) |
| Frostbite | `B_USE_FROSTBITE TRUE`, come Soulgold |
| AI di Red's Path | **Smart Trainer** per gli 8 capi (`TRAINER_REDS_PATH_*_HNS`), senza Prediction. Restano sotto i capi del postgame (`Smart Trainer / Prediction`) |
| Summary B/N | fuori dal piano finché l'utente non indica una fonte (link o screenshot) |
| Documentazione agenti | sfoltire e spostare: i diari di verifica vanno dalle regole ai piani, le regole tengono trappole e fatti, i `paths:` si restringono, CLAUDE.md scende a circa 7 KB |

## 3. Fatti verificati

### Descrizione dell'oggetto ottenuto

- Il codice c'è già, ed è **spento**: `OW_SHOW_ITEM_DESCRIPTIONS` vale
  `OW_ITEM_DESCRIPTIONS_OFF` in `include/config/overworld.h:21`. Le funzioni
  sono `ScriptShowItemDescription` / `ScriptHideItemDescription`
  (`src/overworld.c` ~3926-3975), chiamate dai comandi `showitemdescription` /
  `hideitemdescription` in `data/scripts/obtain_item.inc`: in
  `EventScript_ObtainedItem` (regali, riga 86), `EventScript_PickUpItem` (a
  terra, 179) e nel percorso degli oggetti nascosti (271). Anche
  `berry_tree.inc` e `apricorn_tree.inc` chiamano `hideitemdescription`.
- Con `FIRST_TIME` spento, `GetSetItemObtained` restituisce sempre FALSE,
  quindi con `ALWAYS` il riquadro compare ogni volta. Nessun campo di
  salvataggio: `itemFlags` esiste solo con `FIRST_TIME`
  (`include/global.h:400`, `src/new_game.c:343`).
- **Soulgold la usa** (`OW_ITEM_DESCRIPTIONS_ALWAYS`, `include/config/overworld.h:23`
  di `Marcogn/soulgold` master) e ha **tre differenze** nel blocco, viste con
  un diff contro `src/overworld.c` di SG (26/10):
  1. `ReformatItemDescription` va a capo **per larghezza in pixel**
     (`StringCopy` + `StripLineBreaks` + `BreakStringAutomatic(dest, 196, 2,
     FONT_SMALL, HIDE_SCROLL_PROMPT)`, poi `CountLineBreaks + 1`). La nostra
     versione conta 32 caratteri e poi finisce la parola, senza un limite di
     righe. Le tre funzioni esistono già qui (`include/line_break.h:30-32`);
  2. la palette dell'icona ha un tag suo, `ITEM_PALETTE_TAG (ITEM_TAG |
     BLEND_IMMUNE_FLAG)`, e `DestroyItemIconSprite` libera quel tag. Da noi
     tile e palette usano `ITEM_TAG` (0x2722, lo stesso dell'etichetta dei
     soldi). `BLEND_IMMUNE_FLAG` esiste (`include/palette.h:30`);
  3. `RemoveFieldMugshot()`: **non serve**, in questo repo i mugshot di campo
     non esistono (nessun simbolo).
- Il riquadro usa la cornice standard (`DrawStdFrameWithCustomTileAndPalette(...,
  0x214, 14)`), finestra a (1,1), 28×3 tile; l'icona sta a (26,24). Il tema
  scuro non tocca le finestre di campo (`IsDarkUiEnabled` non è usato in
  `overworld.c` né in `menu.c`), quindi basta un solo tema.
- `handleFlash` (grotta buia o Piramide) accende la finestra OBJ per l'icona:
  è un ramo da provare in una grotta buia.

### Frostbite

- `B_USE_FROSTBITE` è FALSE in `include/config/battle.h:385`; Soulgold lo ha
  TRUE (`include/config/battle.h:371` di SG). L'interruttore decide in
  `src/data/moves_info.h` (8 blocchi `#if`), nelle descrizioni delle MT in
  `src/data/items.h` (12425, 12441), nella Battle Pike (`battle_pike.c:1041`)
  e nell'AI (`battle_ai_util.c:3635`).
- Gli effetti sono già implementati: danno a fine turno in
  `battle_end_turn.c` (~550), attacco speciale dimezzato in `battle_util.c`
  (~7469), cura con gli oggetti in `party_menu.c:4880` e
  `swsh_party_menu.c:7046` (`ITEM_EFFECT_CURE_FREEZE_FROSTBITE`, testo
  `gText_PkmnFrostbiteHealed`).
- **Le icone "FRB" ci sono in tutte le grafiche** (guardate il 26/10):
  `graphics/battle_interface/hns/status.png` (sesta riga, la usa
  `battle_interface.c:2086` con `PAL_STATUS_FRZ`),
  `graphics/interface/status_icons.png` (summary e menu squadra classico) e
  `graphics/party_menu/swsh/status_icons.png` (ottava riga, `AILMENT_FRB` 8).
  Da verificare che si **vedano**, non solo che esistano: palette e indici
  non sono stati controllati.
- Testi di HnS che parlano di congelare: `BattleFrontier_Lounge7_hns`
  (tutor, riga 569 "freeze the foe"), `BattleFrontier_Lounge2_hns:315`,
  `MahoganyTown_Gym_hns:203` (Pryce: "I'll freeze your POKéMON", è una
  battuta: resta). Da rileggere il tutor: se descrive una mossa che ora dà
  Frostbite, il testo va aggiornato.
- Un Pokémon già congelato in un salvataggio vecchio resta congelato finché
  non si scongela: lo stato `STATUS1_FREEZE` esiste ancora, cambia solo
  quello che le mosse infliggono. Nessuna migrazione.

### AI di Red's Path

- Gli 8 capi (`src/data/trainers_hns.party` 16014-16292) hanno `AI: Check
  Bad Move / Try To Faint / Check Viability`, cioè esattamente
  `AI_FLAG_BASIC_TRAINER` (`include/constants/battle_ai.h:46`), come i
  capipalestra di Johto e Kanto della storia (`AI: Basic Trainer`).
- `AI_FLAG_SMART_TRAINER` (riga 47) = Basic + `OMNISCIENT`,
  `SMART_SWITCHING`, `SMART_MON_CHOICES`, `PP_STALL_PREVENTION`,
  `SMART_TERA`, `RANDOMIZE_SWITCHIN`. Il formato `.party` accetta la scritta
  `AI: Smart Trainer` (la usano già `TRAINER_STEVEN_HNS` e altri).
- Nessuna opzione del challenge menu cambia l'AI: grep di `challengeSettings`
  in `battle_ai_*.c` trova solo il Sitrus (`battle_ai_switch.c:1849`).
- Il piano di Red's Path (`docs/plans/reds-path.md` §"Base: le squadre di
  FireRed/LeafGreen") scrive "AI FRLG": va aggiornato con la scelta nuova.

### TcloneR

- Lo scienziato è **uno solo** e si sposta in cinque posti, in ordine di
  storia (`data/scripts/tcloner.inc:71-79`, `.claude/rules/trainer_catch.md`
  §"Lo scienziato"). Il quest book dice il contrario in
  `src/data/quests.h:262` (`"Look for more scientists in\nMahogany,
  Goldenrod, Cerulean."`) e `:265` (`"show your\nclones to a TcloneR
  scientist!"`), più il commento a `:276-277` ("whichever scientist").
- Testi proposti, da misurare in emulatore nella finestra del quest book:
  `"Find the scientist again in\nMahogany, Goldenrod, Cerulean."` e
  `"A prize is waiting: show your\nclones to the scientist!"`.

### Documentazione per gli agenti (misure del 26/10)

| File | Byte | Quando si carica |
| --- | --- | --- |
| `CLAUDE.md` | 13 278 | **sempre** |
| `.claude/rules/*.md` (20 file) | ~140 000 | per percorso (`paths:`) |
| `overworld_encounters.md` | 23 932 | `wild_encounter.c`, `field_control_avatar.c`, `event_object_movement.c`, `trainer_see.c`, `config/overworld.h` e altri |
| `trainer_catch.md` | 14 324 | anche `battle_script_commands.c`, `battle_util.c`, `pokemon.c`, sei mappe |
| `reds_path.md` | 13 023 | |
| `save.md` | 12 989 | anche `include/global.h`, `include/constants/flags.h` |
| `dexnav.md` | 11 569 | anche `field_control_avatar.c`, `field_player_avatar.c` |
| `docs/claude/emulator.md` | 5 837 | a mano |
| skill (7) | 2-8 KB l'una | solo quando si chiamano |

Costo tipico: aprire `field_control_avatar.c` carica `overworld_encounters.md`
e `dexnav.md`, cioè 35 KB, quasi tutti di diari di verifica ("Prove della
fase 3", "GDB su mGBA, come funziona davvero", eccetera). Il grosso delle
regole è **storia**: com'è stato verificato, in quale sessione. È utile, ma
appartiene al piano della feature, che non si carica da solo.

## 4. Spazio misurato (26/10, `make hns` su `master` pulito, 9606adc6)

- **ROM**: usati 31 896 324 byte su 33 554 432 (**95,06 %**), liberi
  1 658 108, misurati come fa `build.yml`. Nessuna fase aggiunge grafica:
  le icone degli oggetti sono già in ROM per la borsa, le icone FRB pure.
  Costo atteso: qualche centinaio di byte di codice e testo.
- **Salvataggio**: `ITEMS_COUNT` 904, quindi `ITEM_FLAGS_COUNT` **113 byte**
  per la modalità `FIRST_TIME`. Spazio libero (`nm -S pokehns.elf`, tolti i
  128 byte di ASLR): SaveBlock3 96/116 (**20**), SaveBlock2 3892/3968
  (**76**), SaveBlock1 15 760/15 872 (**112**). La modalità non entra da
  nessuna parte, neanche in SaveBlock1 (per un byte), ed è il motivo per cui
  Soulgold usa `ALWAYS`. Nessuna fase di questo piano tocca il salvataggio.
- VRAM/palette: il riquadro della descrizione usa una palette sprite
  (`ITEM_TAG`) e la palette BG 14. È da controllare in fase 2 su una mappa
  con molti oggetti e Pokémon-oggetto (rischio noto, CLAUDE.md §6: VRAM
  quasi piena con `OW_GFX_COMPRESS`).

## 5. Fasi

| # | Fase | Modello | Cancello per passare oltre |
| --- | --- | --- | --- |
| 1 | **Frostbite, AI di Red's Path, testo TcloneR.** `B_USE_FROSTBITE TRUE`; testi del tutor rivisti; `AI: Smart Trainer` agli 8 `TRAINER_REDS_PATH_*_HNS`; i due testi e il commento di `quests.h`; `reds-path.md` aggiornato; CHANGELOG | **Sonnet 5.5** | build verde, ROM ≤ 95,1 %. In emulatore: un Pokémon colpito da Ice Beam (forzato via GDB o con una ROM strumentata al 100 %) ha "FRB" visibile nel riquadro di lotta (singola e doppia), nel summary, nel menu squadra **SwSh e classico**; a fine turno perde 1/16 dei PS; si muove; una Ice Heal lo cura con il messaggio giusto. Un capo di Red's Path con un Pokémon in svantaggio lo cambia (screenshot). Il quest book mostra i testi nuovi senza sforare la finestra. Il `.srm` dell'utente **non serve** (nuova partita + debug) |
| 2 | **Descrizione dell'oggetto ottenuto.** `OW_ITEM_DESCRIPTIONS_ALWAYS`; le due correzioni di Soulgold (a capo in pixel, `ITEM_PALETTE_TAG` blend-immune); CHANGELOG, README | **Sonnet 5.5** | build verde. Screenshot in emulatore di: oggetto a terra, oggetto nascosto, regalo di un NPC, MT, bacca da un albero, Ghicocca, una descrizione lunga (va a capo in due righe senza uscire dal riquadro), una grotta buia con e senza Flash (ramo `handleFlash`), una mappa piena di Pokémon-oggetto con OWE attivi (l'icona compare, nessuna schermata blu). Dopo ogni raccolta il riquadro sparisce del tutto (nessuna tile residua: screenshot dopo il messaggio). Il `.srm` dell'utente **serve** per l'ultima prova (una mappa avanzata con molti oggetti e la squadra vera); altrimenti nuova partita + debug |
| 3 | **Documentazione per gli agenti.** Per ogni regola: i diari di verifica ("Prove della fase N", ricette GDB, cronache di sessione) vanno nel piano della feature (`docs/plans/<feature>.md`, sezione "Com'è stata fatta") o in `docs/claude/emulator.md` se sono tecniche dell'emulatore; nella regola restano trappole, invarianti e fatti con file:riga. `paths:` ristretti ai file della feature (niente `src/pokemon.c`, `battle_util.c`, `field_control_avatar.c` come esca generica, se la trappola non riguarda quel file). CLAUDE.md a circa 7 KB: le "Lezioni che valgono ovunque" restano, il resto rimanda alle regole | **Sonnet 5.5** | tabella prima/dopo dei byte di ogni file nel piano; CLAUDE.md ≤ 7,5 KB; nessuna regola sopra 8 KB; **checklist**: ogni titoletto e ogni trappola in grassetto delle regole di oggi si ritrova (regola, piano o emulator.md) con un `grep` documentato nel piano; `doc-sync` e `next-phase` funzionano ancora (le skill citano sezioni che potrebbero spostarsi: grep dei rimandi rotti a `§` e ai nomi dei file). Nessun `.srm` |
| 4 | **Revisione finale.** `/code-review` a medium su tutto il diff del piano; punti aperti provati in emulatore; ROM consegnata zippata | **Opus 5.5** | review senza blocchi aperti; build verde; ROM `pokemonHnS_<descrizione>_<sha>.zip` consegnata in chat. Il `.srm` **serve**: si carica il salvataggio vero, si raccoglie un oggetto e si apre la squadra (Frostbite e descrizione insieme sulla ROM finale) |

### 5.1 Fase 1, com'è stata fatta (26/10)

- **Frostbite**: `B_USE_FROSTBITE TRUE` (`include/config/battle.h:385`). Il
  tutor del Lounge7 diceva "may freeze the foe" per Ice Punch: ora "may give
  frostbite" (`BattleFrontier_Lounge7_hns/scripts.inc`). `Lounge2:315`
  ("FREEZE POKéMON" è la categoria di Articuno) e Pryce restano.
- **1/8, non 1/16.** Il danno di fine turno usa `B_BURN_DAMAGE`
  (`battle_end_turn.c:550`) e qui vale `GEN_3` (`include/config/battle.h:36`):
  scottatura e Frostbite tolgono **1/8**. Misurato in lotta: 110 PS,
  79 → 66 (13 = 110/8). Il piano scriveva 1/16 senza averlo letto. Non
  l'ho cambiato: alzare `B_BURN_DAMAGE` cambierebbe anche la scottatura.
  Se l'utente vuole 1/16, serve un valore a parte solo per Frostbite.
- **Red's Path**: `AI: Smart Trainer` agli otto capi; in
  `src/data/trainers_hns.h` generato tutti e otto hanno
  `AI_FLAG_SMART_TRAINER`. Il testo di `reds-path.md` e la regola
  `reds_path.md` sono aggiornati.
- **TcloneR**: i due testi di `quests.h` ("Find the scientist again in…",
  "…show your clones to the scientist!") e il commento di riga 276.
- **Build**: `make hns` verde, ROM 31 896 452 byte = **95,06 %** (prima
  95,06 %, +128 byte). La sessione non aveva la toolchain: `apt-get update`
  poi `gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi
  mgba-sdl xvfb xdotool imagemagick gdb-multiarch`.
- **Verifica in emulatore** (nuova partita, debug): `Give X → Pokémon
  (Basic)` e `Party → Edit Pokemon → Inflict Status1 → Party-wide →
  Frostbite` (il debug ha già la voce). "FRB" visibile nel menu squadra
  **SwSh** (badge azzurro) e **classico**, nel **Summary** (STATUS FRB), nel
  riquadro di lotta **singola** e **doppia** (entrambi i riquadri).
  In lotta il Pokémon usa le mosse; a fine turno "was hurt by its frostbite!".
  Una Ice Heal (id 46, `Give item XYZ`) usata dalla borsa in lotta doppia
  toglie FRB dal Pokémon scelto e lascia l'altro (screenshot dopo il turno;
  il messaggio di cura non è stato catturato).
- **Quest book**: `VAR_TCLONER_STAGE` (0x40C2) = 1 mostra "Find the scientist
  again in / Mahogany, Goldenrod, Cerulean."; `VAR_TCLONER_COUNT` (0x40E5) = 30
  mostra "A prize is waiting: show your / clones to the scientist!". Entrambi
  stanno nella finestra. Debug: `Flags & Vars → Set Var XYZ`, il valore è
  l'indice decimale (0x4000 = 16384); 0x40C2 = **16578**, non 16450.
- **Non dimostrato**: il cambio di Pokémon di un capo di Red's Path. Brock
  contro Butterfree, Mankey e Charmander non ha mai cambiato (Geodude contro
  tutti restava la scelta migliore): lo scenario va forzato (un capo con un
  Pokémon in netto svantaggio di tipo, es. Misty/Staryu contro un Pokémon
  Elettro o Erba) o va dato per coperto dal codice condiviso
  (`AI_FLAG_SMART_SWITCHING`). Resta aperto per la fase 4.
- Non provato: la metà dei danni speciali (nessuna mossa speciale dei
  Pokémon di prova), Ice Beam vero (lo stato è stato dato dal debug).

**Handoff per la fase 2 (Sonnet 5.5).** `OW_ITEM_DESCRIPTIONS_ALWAYS` e le
due correzioni di Soulgold (§3). Il container di sessione parte senza
toolchain né emulatore: installa i pacchetti sopra prima del primo `make`.
Per arrivare a un'inquadratura utile: nuova partita (A ripetuto; sceglie
"Recommended", poi **R** scorre le pagine e l'ultima chiede conferma), poi
R+Start per il debug; il personaggio resta fermo nella scena della camera,
ma il debug funziona lo stesso (`Give X`, `Warp to map warp`). Il menu a
striscia (Start) apre per ultimo il tasto usato; il riquadro Quest/Party/
Options si raggiunge con Destra. Nelle schermate di debug il cursore si
azzera a ogni riapertura.
### 5.2 Fase 2, com'è stata fatta (26/10)

- `OW_SHOW_ITEM_DESCRIPTIONS = OW_ITEM_DESCRIPTIONS_ALWAYS`
  (`include/config/overworld.h:21`). In `src/overworld.c`: a capo in pixel
  (`ReformatItemDescription` ora sono 5 righe) e `ITEM_PALETTE_TAG`. Il
  `RemoveFieldMugshot()` di Soulgold non serve (§3). Tratti e trappole nella
  regola `field.md`.
- **Build**: verde, ROM 31 897 108 byte = **95,06 %** (+656 rispetto alla fase 1).
- **Verifica in emulatore** (ROM strumentata con `debug.inc` Script 7 =
  `giveitem` di TM13, Pamtre Berry, Potion; ripristinato prima del commit),
  nuova partita:
  - regalo (`EventScript_ObtainedItem`): TM13 con la descrizione su due righe,
    Pamtre Berry (59 caratteri, la più lunga) su due righe dentro il riquadro,
    Potion su una riga (y=4);
  - oggetto a terra (`finditem`, Script 2): riquadro con "found one POTION";
  - **nessuna tile residua**: confronto pixel delle 32 righe in alto prima e
    dopo una raccolta = **0 pixel diversi**; con il riquadro aperto 7376
    (controllo che il confronto vede la differenza);
  - Dark Cave senza Flash (`handleFlash`): icona visibile, riquadro che sparisce,
    nessun crash;
  - Safari di Johto Low Mid (11 Pokémon-oggetto, OWE attivi): icona e riquadro
    ci sono, nessuna schermata blu (un OWE aggressivo ha iniziato una lotta
    mentre premevo A, normale).
- **Col `.srm` dell'utente** (`v2.0.6.plus.4`, 6 medaglie, Pokédex 120,
  Vileplume): il salvataggio vecchio si carica con la ROM nuova; stessa ROM
  strumentata nel Safari Low Mid con gli OWE attivi: TM13 e Potion mostrano
  il riquadro, che sparisce a ogni raccolta, nessuna schermata blu.
- **Alberi di bacche: in HnS il riquadro non c'era (26/10).** HnS compila
  il ramo `#if IS_HNS` di `berry_tree.inc` (raccoglie e ripianta subito,
  `BerryTree_EventScript_Hns_PickBerry`), che **non** chiamava
  `showberrydescription`/`hideitemdescription`: le chiamate citate al §3
  sono nel ramo vanilla `#else`. Aggiunte nello script HnS, fra la raccolta e
  il ripianto (`ObjectEventInteractionPlantBerryTree` richiama
  `GetBerryTreeData` e ricarica `VAR_0x8006`); l'id dell'oggetto lo scrive
  `ObjectEventInteractionPickBerryTree` in `VAR_0x8006`, perché lì
  `GetBerryTreeData` aveva messo il **numero** di bacche. Nel ramo vanilla, e
  in Soulgold (`berry_tree.inc`, `ScriptShowItemDescription` di SG letti il
  26/10), `showberrydescription` legge proprio quel numero come id: non visto
  in gioco, non toccato.
- **Albero vero, salvataggio dell'utente**: Route 30, warp di debug 0/12/0,
  albero (23,38): "Mark picked the 5 Oran Berries." con icona e "A hold item
  that restores 10 HP in battle.", il riquadro sparisce col messaggio,
  l'albero rinasce germogliato.
- **Oggetto nascosto vero, salvataggio dell'utente**: flag dei nascosti letti
  in RAM (`SaveBlock1 + 0x198C`, controllati sulle medaglie: 1-6 accese, 7-8
  spente), scelto il Potion di New Bark (7,7), flag 637 spento; posizione letta
  a ogni passo (`SaveBlock1 + 4`, dopo `saveVersion`): da (6,7) verso destra,
  "Mark found one POTION!" con il riquadro, poi "put away", riquadro sparito.
  Righe in alto prima/dopo: 41 pixel diversi, tutti in x 130-142 / y 0-4,
  uno sprite che si muove sul bordo; il riquadro starebbe in x 8-232 / y 8-39.
- **Ghicocche**: nessuna mappa HnS ha alberi di Ghicocche (grep dei
  `map.json`), quindi non c'è un caso da provare. Flash usato: stesso ramo
  di "senza Flash", non provato a parte.
- **Anomalia del "wild !" Lv0, spiegata e corretta (26/10)**: non era un OWE
  né questa fase, era la **Testata**. Lo script di HnS rilasciava il
  giocatore prima della lotta (`goto` incondizionato prima del `waitstate`,
  dall'import iniziale); durante la transizione uno spawn di OWE riscriveva e
  azzerava `gEnemyParty`. Provato con due ROM strumentate gemelle (dettagli e
  regola in `field.md`): con lo script vecchio la lotta parte con specie 0 e
  89/89 fotogrammi sbloccati, con quello nuovo 0/89 e la specie della Testata.
  Corretto nello stesso branch, commit a parte.
- **Quando camminare in emulatore**: la prima pressione di una freccia gira
  soltanto, la seconda muove; `k.sh Left 3` fa due passi. Warp di debug a un
  numero di mappa: il campo "Map" riparte da 0 dopo il gruppo.

**Handoff per la fase 3 (Sonnet 5.5).** Documentazione per gli agenti, nessun
codice. Ricorda che la regola `field.md` ora ha `src/overworld.c` tra i `paths:`
(la fase 3 li restringe): tienila stretta. Il piano (§5 fase 3) chiede la
checklist dei titoletti prima/dopo; fai `grep -c '^#\|\*\*'` delle regole
oggi, prima di spostare niente.

Le fasi 1, 2 e 3 sono indipendenti: l'ordine è dal più economico da
verificare al meno, ma si possono scambiare.

## 6. Scartato, e perché

- **Descrizione solo la prima volta** (`OW_ITEM_DESCRIPTIONS_FIRST_TIME`):
  113 byte di bitfield, più di quanto resta in qualsiasi blocco del
  salvataggio (§4). Si potrebbe fare un bitfield ridotto (solo gli oggetti
  ottenibili in HnS), ma costa una tabella di corrispondenza e una
  migrazione per un dettaglio che Soulgold non ha.
- **Summary "stile Bianco/Nero / Unbound"**: non identificato. Unbound gira
  su CFRU (FireRed in binario, non decomp), quindi il suo summary non si
  porta: andrebbe rifatto. Fonti pubbliche trovate il 26/10: solo il summary
  DP/Pt di Jaizu (pokeemerald vanilla, ramo `citrusbolt/pokeemerald`
  `summary_screen`, 2022) e nessuno per expansion. Nei rami di
  `DizzyEggg/pokeemerald` non c'è un summary (elenco dei rami letto con
  `git ls-remote`). Il nostro summary è quello di expansion 1.15.2 con la
  grafica di HnS (`graphics/summary_screen/hns/`). Si riapre con un piano a
  parte quando l'utente indica la fonte.
- **Ricorda mosse dal summary**: non è una feature da fare. Le opzioni di
  `include/config/summary_screen.h` (`P_SUMMARY_SCREEN_MOVE_RELEARNER`,
  `P_ENABLE_MOVE_RELEARNERS`…) sono **quelle già accese e collegate**
  (`.claude/rules/relearner.md`). Expansion `master` ha poi aggiunto un suo
  gestore di START (`HandleMoveRelearnerInput`) e nuove opzioni sulla
  pagina statistiche (`P_SUMMARY_SCREEN_IV_EV_INFO` e simili), che 1.15.2
  non ha; da noi c'è `P_SUMMARY_SCREEN_IV_EV_VALUES TRUE` (IV come numeri).
- **AI Smart + Prediction per Red's Path**: è quella dei capi del postgame;
  l'utente la vuole "un po' elevata", non al massimo.
