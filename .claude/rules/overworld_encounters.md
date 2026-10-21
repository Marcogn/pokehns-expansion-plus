---
paths:
  - "src/wild_encounter.c"
  - "src/wild_encounter_ow.c"
  - "include/wild_encounter_ow.h"
  - "include/config/wild_encounter.h"
  - "src/data/pokemon/wild_encounter_ow_behavior.h"
  - "src/data/pokemon/owe_temperaments.h"
  - "tools/owe_temperaments.py"
  - "src/event_object_movement.c"
  - "src/field_control_avatar.c"
  - "src/field_effect_helpers.c"
  - "include/config/overworld.h"
  - "docs/plans/overworld-encounters.md"
  - "docs/plans/overworld-encounters/**"
---

# Pokémon visibili nell'overworld: in corso

Piano, stato, fatti verificati e decisioni in
[`docs/plans/overworld-encounters.md`](../../docs/plans/overworld-encounters.md):
port della PR #8434 di expansion (dal diff del merge `10ae7f9`, che usa
ancora i nomi `OW_` come noi, più le sole correzioni arrivate fino a
`expansion/1.16.4`; **non** da `master`, che dipende da `mass_outbreak.h`)
più un tiro pesato per indole,
sul modello del mod Wilds of Kanto. Le indoli scelte dall'utente sono in
`docs/plans/overworld-encounters/temperaments.json`.

## Stato: fase 5 fatta (21/10), OWE accesi ovunque, interruttore nel PC

Fase 2: codice della PR più #9925, #9968, #10066, #10753. Fase 3: config di
D3-D5 (6 OWE, ricambio, shiny con scintilla e che non spariscono, tetti
6/3/3 con grotta = `MAP_TYPE_UNDERGROUND` o `MAP_TYPE_INDOOR`).

Fase 5: **`AreOverworldMonsEnabled()`** (`wild_encounter_ow.c`) è l'unico
interruttore. Falso con WILD BATTLES OFF; altrimenti legge
`soundAndFieldOptions.noOverworldMons` (bit 5 del byte a offset 56 di
SaveBlock3, **invertito**: 0 = ON). I 3 bit liberi valevano 0 in ogni
salvataggio a `SAVE_VERSION` 8 (la migrazione `< 8` azzera il byte, le
partite nuove nascono a 0): nessuna migrazione, default ON anche per i
salvataggi vecchi (D2). Lo usano lo spawn e `ShouldDisableRandomEncounters()`
(`field_control_avatar.c`): con ON niente incontri a passi su erba, acqua e
grotta; pesca, Spaccaroccia, Testata e Profumino non passano di lì.
`WE_VANILLA_RANDOM` resta TRUE (vale solo con l'opzione OFF).
Per togliere l'interruttore in futuro: togliere la voce del PC e far
restituire TRUE ad `AreOverworldMonsEnabled()` dopo il controllo su WILD
BATTLES.

PC: voce `WILD POKéMON` (`gText_WildPokemon`) dopo CHALLENGES nelle quattro
liste di `data/scripts/pc.inc` (case rinumerati) e in `CreatePCMultichoice`
(`script_menu.c`, finestra 10/12 righe); sì/no con
`gText_ShowWildPokemonOverworld` (`data/text/pc.inc`, testi approvati
dall'utente) e `special SetOverworldMonsFromYesNo` (in fondo a
`data/specials.inc`). Il menu opzioni scrive `soundAndFieldOptions` campo per
campo, quindi non tocca il bit.

**Prima del rilascio manca la fase 6** (Gara Pigliamosche, Safari, grotte
buie, acqua, roamer, città): oggi gli OWE compaiono su ogni mappa con una
tabella di erba o acqua.

### Prove della fase 5 (mGBA, breakpoint GDB su `StandardWildEncounter`)

`StandardWildEncounter` è chiamata solo se `ShouldDisableRandomEncounters()`
dice di no, quindi il conteggio dei passaggi misura gli incontri a passi.
- Salvataggio scritto dalla ROM della fase 2 (bit mai toccato): OWE presenti
  su Route 29, 40 passi nell'erba = **0** chiamate; la lotta avvenuta era da
  contatto.
- PC di Cherrygrove: voce e domanda leggibili (il testo sta nelle due righe).
  NO → byte `0x20`; Route 30: nessun OWE, **39** chiamate e lotta a passi.
- YES → byte `0x00`; Route 30: 5 OWE (Caterpie, Ledyba, Pidgey).

## Fase 4 fatta (21/10): tiro pesato per indole

- `tools/owe_temperaments.py` legge `temperaments.json` e i blocchi
  `#if P_FAMILY_X` e scrive `src/data/pokemon/owe_temperaments.h`
  (`sOWETemperamentWeights`, `sOWESpeciesTemperament[NUM_SPECIES]`). **Si
  rilancia a mano** dopo ogni modifica della tabella; il file generato è nel
  git, la build non lo rigenera. Ereditarietà per famiglia; un'eccezione per
  specie vale anche per le sue `_MEGA`, `_MEGA_X/Y`, `_GMAX` (Mega Charizard
  = Charizard), le forme regionali no (vanno nominate nella tabella); fuori
  tabella = NEUTRO (= 0). 426 specie non NEUTRO.
- Corrispondenza indole → comportamento upstream: guarda `WATCH`, gira
  `IGNORE` (vaga), si avvicina `APPROACH`, insegue `CHASE`, scappa `FLEE`,
  sparisce `DESPAWN`. Parametri di vista e velocità quelli di upstream.
- `OWE_RollMovementType()` sceglie allo spawn. Il comportamento non sta più
  nella specie: il campo `overworldEncounterBehavior` di `SpeciesInfo` è
  tolto (upstream non lo valorizzava mai) e i getter `OWE_Get*FromSpecies`
  sono diventati `OWE_Get*(oggetto)`, che lo ricavano dal `movementType`
  (uno per comportamento; upstream stesso lo usa così). ROM 94,93%, **più
  piccola** della fase 3: il campo costava un byte per specie.
- Verifica: il testo vero di `OWE_RollMovementType` estratto dal sorgente e
  compilato sull'host con la tabella generata e i dati di comportamento
  veri, 600 000 tiri per specie: percentuali uguali ai pesi entro ±0,1% per
  tutte e sei le indoli, Pidgeot (eredità), Mega Charizard X (eccezione con
  forma), Mew (fuori tabella). In emulatore (RAM via GDB, 80 letture su
  Route 29): Pidgey e Sentret TIMIDO mai `CHASE`/`APPROACH` (peso 0), i
  comportamenti cambiano fra individui della stessa specie. Le percentuali
  in gioco non si misurano bene così: 20 spawn in 5 minuti, e chi scappa e
  sparisce fra due letture sfugge al conteggio.

## Adattamenti alla nostra base (1.15.2)

- `enum Species` non esiste da noi: nel codice portato è `u32`.
- `gParties[B_TRAINER_1]` → `gEnemyParty`.
- Le sei costanti `MOVEMENT_TYPE_*_OWE` partono da `0x54`, non `0x53`
  come upstream: `0x53` da noi è `MOVEMENT_TYPE_TOWER_BEAM` di HnS.
- `ComputePlayerShinyOdds()` (nuova, estratta da `CreateBoxMon`) usa
  `GetShinyOdds()` come il codice che sostituisce, non `SHINY_ODDS`: senza,
  l'opzione SHINY CHANCE del challenge menu sparirebbe in silenzio.
- `WE_OWE_PREVENT_SPECIAL_MOVEMENT_DESPAWN` aggiunta a mano (viene da #9968).

## Trappole trovate nel port

- **Gli hunk entrati "con fuzz" vanno riletti.** `patch -F3` ha messo
  `CleanIncompatibleGenderSpecies` *dentro* lo `switch` di `IsValidGender`
  (`pokemon.c`) e l'`.include` di `wild_encounter.inc` sotto il commento
  `@ FRLG scripts` invece che dopo `apricorn_tree.inc`. Compilavano entrambi.
- **`TryAndDespawnOldestGeneratedOWE_ToFreeObject` con OWE spenti** restituiva
  `FALSE`, che per `GetAvailableObjectEventId` vuol dire "slot libero": a
  tabella dei 16 oggetti piena uno spawn avrebbe usato un indice non
  inizializzato. Upstream l'ha corretto in 1.16.4 dentro un refactor di
  `GetAvailableObjectEventId` che non abbiamo preso; qui `return TRUE`.
  Il refactor 1.16.4 di quella funzione **non** va preso così com'è: cambia
  la firma e l'ordine di ricerca degli slot per tutti gli oggetti.
- **`DespawnOWEOnBattleStart`** gira a ogni inizio lotta (`battle_setup.c`)
  e indicizzava `gObjectEvents` con il risultato "non trovato"
  (`OBJECT_EVENTS_COUNT`): lettura, e potenziale `ClearObjectEvent`, dei
  `.sbss` di `field_region_map.o`/`field_specials.o` che seguono l'array.
  Guardia aggiunta. Lo stesso schema c'era nei loop sugli slot OWE
  (`GetOldestActiveOWESlot`, `SortOWEAges`): uno slot vuoto leggeva
  `gObjectEvents[16]`, e `SortOWEAges` ci **scriveva** l'età; lo stesso
  nello spawn (`ShouldDespawnGeneratedForNewOWE` su uno slot libero, che
  poteva finire in `RemoveObjectEvent`). Corretto in fase 3 con
  `GetOWEBySpawnSlot()`, che restituisce NULL per uno slot vuoto. Restano
  come upstream `SetOverworldObjectSpecies` (comando di script) e il percorso
  degli OWE manuali, che Hasep non usa.
- **Tetti per area e sostituzione**: a tetto pieno il ricambio è uno per
  uno, quindi entrando in acqua con 6 OWE di terra il totale non scende
  subito a 3: scende man mano che quelli di terra escono di vista.
- `GetAvailableObjectEventId`: con la tabella piena e un `localId` di OWE
  generato, cade su `*objectEventId = 16` e restituisce `FALSE`. Upstream
  non ci arriva perché controlla prima `CheckCanLoadOWE`; da tenere presente
  se si cambia lo spawn.
- **`trainer_type` dei massi di HnS**: i massi di Forza che cadono nelle buche
  (Ice Path B1F) mettono un flag (`FLAG_HIDE_ICEPATH_B2F_BOULDER1..4`,
  `0x2F7-0x2FA`) nel campo `trainer_type`. `ObjectEvent.trainerType` è `u8`,
  e upstream riconosce un OWE da `trainerType == 0xFF`
  (`TRAINER_TYPE_OW_WILD_ENCOUNTER`). Oggi i byte bassi sono `F7-FA`: un
  flag nuovo `0x?FF` usato così diventerebbe un Pokémon selvatico.
- Gli OWE generati usano i `localId` da `LOCALID_OW_ENCOUNTER_END` (252) in
  giù, `OWE_SPAWNS_MAX` di numero: con 6 (D4) diventano 247-252, ancora
  sotto `OBJ_EVENT_ID_NPC_FOLLOWER` (253) e sopra Berry Blender (236-240).
  Le mappe HnS arrivano a 64 oggetti (Bellchime Trail).
- **DexNav**: l'aggancio upstream (`DespawnAllOverworldWildEncounters` in
  `Task_DexNavExitAndSearch`) copre solo la ricerca dalla schermata. Da noi
  la ricerca parte anche da **R** (`TryStartDexNavSearch` →
  `InitDexNavSearch`): in fase 3 il despawn è anche in `InitDexNavSearch`.
  La ricerca passiva dei nascosti (`FlagSet(DN_FLAG_SEARCHING)` in
  `dexnav.c`, ~riga 2960) resta come upstream: niente despawn, ma con il
  flag acceso non ne nascono di nuovi.
- **Il menu debug fa sparire gli OWE**: si apre con R tenuto + START, e R
  avvia la ricerca DexNav sulla specie registrata. Non è un bug; ma per una
  prova sugli OWE non si può passare dal debug con una specie registrata.
- La lotta con un OWE ricrea il Pokémon da specie, livello, sesso e shiny
  salvati nell'oggetto: upstream usava `NATURE_RANDOM`, cioè niente
  Sincronismo e niente Pokéblock del Safari. Ora usa `PickWildMonNature`
  (resa non statica in `wild_encounter.c`) come `CreateWildMon`. Il bonus
  agli IV del Pokéblock (`TryApplyPokeblockBonus`) resta perso: da vedere
  con il Safari in fase 6.
- `UpdateOverworldWildEncounter` (ogni frame, da `OverworldBasic`) chiama
  `CheckCurrentWildMonHeaderForOWE` **prima** di guardare
  `WE_OW_ENCOUNTERS`: costo per frame anche a feature spenta. Non toccato
  (upstream), ma è il primo posto dove guardare se il frame rate cala.
- `ShouldDisableRandomEncounters()` (`field_control_avatar.c`) usa
  `WE_VANILLA_RANDOM`, di compilazione: l'interruttore della fase 5 ha
  bisogno di un controllo a runtime lì.

## Riflessi (#9925), l'unica modifica sempre attiva voluta

Offset del tag del riflesso `0x2000 → 0x0800`, raw `0x4000 → 0x3000`,
controlli `== 0xFF`. Corregge anche una seconda collisione che il piano non
diceva: il vecchio `HIGH_BRIDGE_PAL_TAG 0x4010` era il tag della palette di
Pidgey in overworld (`16 + OBJ_EVENT_MON`).

Verifica (mGBA, ora finta con `faketime`, stesso salvataggio caricato nelle
due ROM): Violet City, isolotto del ponte (26,25), giocatore + follower
Bulbasaur + NPC sopra il laghetto. Riquadro dei riflessi di giocatore e
follower: **0 pixel diversi** fra base e nuova (39 colori nel riquadro).
Il caso shiny + normale della stessa specie resta da provare: serve acqua,
quindi in fase 6 (gli OWE generati non hanno riflesso, `SetUpReflection`;
la collisione possibile è fra il riflesso del follower e uno shiny OWE).

## Prove della fase 3 in emulatore (21/10)

Dopo le prove una terza guardia sugli slot (spawn): ricompilata e riprovata
in breve (spawn, lotta al contatto, fuga, nuovi spawn). Per separare le lotte
da OWE da quelle a passi, una ROM strumentata con `WE_VANILLA_RANDOM FALSE`.
Letture della RAM con `gdb-multiarch` (vedi sotto).

- Spawn su Route 29: fino a 6 OWE insieme (localId 247-252), più follower e
  NPC; picco palette sprite **10 su 16** (6 OWE di 3 specie, alcuni shiny,
  follower, 2 NPC). Nessun difetto grafico visto con `OW_GFX_COMPRESS` acceso.
- Lotta premendo A e lotta al contatto (il Pokémon che cammina addosso al
  giocatore, con il «!»); cattura di un Pidgey sicuramente da OWE; squadra e
  scheda aperte: dati integri (OT, livello, sesso uguale allo sprite, luogo).
- DexNav da R con 5 OWE in campo: spariscono, nessuno spawn durante la
  ricerca, lotta DexNav regolare, OWE di nuovo presenti dopo.
- Shiny (Force Shiny): scintilla allo spawn, palette shiny (0 pixel del rosa
  della palette normale sul corpo di Hoppip, 47 sul normale), shiny anche in
  lotta, catturato con la stella. D5: 6 shiny, Force Shiny spento, giocatore
  a 17-21 caselle e ritorno: stessi 6 localId, nessuno spawn normale.
- Uscendo verso New Bark Town (città) spariscono tutti; i Pokémon-oggetto
  della mappa (Wooper, Pineco) no.
- **Non provato** (Route 29 non ha acqua né è una grotta): tetti 3/3 per
  acqua e grotta, collisione palette riflesso/shiny. Vanno in fase 6.

### GDB su mGBA, come funziona davvero

Lo stub di `mgba -g` (porta 2345) accetta **una sola connessione** per
sessione e non supporta il detach (`E07`): un `gdb -batch` per ogni
lettura non va, la seconda connessione resta appesa. Funziona un solo
`gdb-multiarch` sempre aperto, alimentato da una fifo (`tail -f fifo |
gdb-multiarch`), con `continue &` dopo il collegamento e `interrupt` / `dump
binary memory` / `continue &` per ogni lettura. `pokehns.elf` non ha
informazioni di debug: gli indirizzi si prendono con `nm`, gli offset dei
campi compilando un `offsetof` con i flag del Makefile (`-mthumb -mabi=apcs-gnu
-march=armv4t -iquote include -DPOKEMON_HNS`). `gSaveBlock1Ptr` punta dentro
`gSaveblock1` con uno spostamento casuale: si legge il puntatore.
Mai `pkill -f` con una parola che compare nel comando stesso: uccide la shell.

## Prove della fase 2 in emulatore

Nuova partita, Bulbasaur dal debug, warp su Route 29: incontro a passi
nell'erba alta (Sentret Lv2), fuga, ritorno in mappa con follower; i
Pokémon-oggetto già sulle mappe (Route 29, isolotto di Violet) si vedono e
riflettono come prima; salvataggio e caricamento.

Mappa ASCII dei tile riflettenti di un layout (per trovare un punto di
prova): HnS ha **640** metatile primari (`include/fieldmap.h`), non 512;
la collisione sono i bit 10-11 di ogni u16 di `map.bin`.
