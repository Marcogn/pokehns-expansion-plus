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
  - "src/trainer_see.c"
  - "src/surfable.c"
  - "include/field_effect_helpers.h"
  - "include/config/overworld.h"
  - "docs/plans/overworld-encounters.md"
  - "docs/plans/overworld-encounters/**"
---

# Pokémon visibili nell'overworld (fasi 1-7 fatte)

Piano, stato, fatti verificati e decisioni in
[`docs/plans/overworld-encounters.md`](../../docs/plans/overworld-encounters.md):
port della PR #8434 di expansion (dal diff del merge `10ae7f9`, che usa
ancora i nomi `OW_` come noi, più le sole correzioni arrivate fino a
`expansion/1.16.4`; **non** da `master`, che dipende da `mass_outbreak.h`)
più un tiro pesato per indole,
sul modello del mod Wilds of Kanto. Le indoli scelte dall'utente sono in
`docs/plans/overworld-encounters/temperaments.json`.

## Stato: fasi 2-7 fatte (21/10); cosa resta nel piano (tabella in cima)

Fase 2: codice della PR più #9925, #9968, #10066, #10753. Fase 3: config di
D3-D5 (6 OWE, ricambio, shiny con scintilla e che non spariscono, tetti
6/3/3 con grotta = `MAP_TYPE_UNDERGROUND` o `MAP_TYPE_INDOOR`).
Tetto Safari 4 (`OWE_SPAWNS_MAX_SAFARI`, richiesto dall'utente dopo la
fase 6; solo sulla terra, controllato con `GetSafariZoneFlag()`): verificato
in Low Mid, mai più di 4 in 50 letture.
Tentativi di spawn: `OWE_SPAWN_TILE_TRIES` **6** caselle per tentativo
(upstream 1, `TrySelectTilesForOWE`), scelto dall'utente; 8 da provare dopo il
suo test. Tempo **di gioco** per arrivare a 6 su Route 31 dal warp, fermo, 3
giri: 4 caselle 17-31 s, 6 caselle 11-29 s, 8 caselle 9-13 s; con 6 e la
correzione sotto, 16 e 19 s. Le attese fra uno spawn e l'altro da sole fanno
~10 s (30 + 30 × attivi frame), quindi oltre 8 non si guadagna.
**Misurare in frame, non con l'orologio**: ogni lettura via GDB ferma il gioco
~0,7 s, e i primi tempi "da orologio" (65 s) erano circa il doppio del vero.
Contatore: `gMain.vblankCounter1` (`gMain` + 0x20).

Comparsa su una casella in uscita: `GetObjectEventIdByPosition` guarda solo
`currentCoords`, quindi un OWE poteva nascere sulla casella che un altro
oggetto stava lasciando e sovrapporsi per il resto del passo. Ora
`IsTileBeingLeftByObject` scarta anche le `previousCoords`, come fa già la
collisione del movimento (`GetObjectObjectCollidesWith`). ROM strumentata con
un contatore: 2 casi evitati in ~4 minuti su Route 31.
Per il resto gli OWE non si compenetrano: ogni passo controlla la casella di
arrivo contro `currentCoords` e `previousCoords` di tutti gli oggetti. Due
eccezioni di upstream: un NPC che cammina addosso a un OWE lo fa sparire
(`DespawnOWEDueToNPCCollision`), e il follower non controlla le collisioni
quando si muove (ma gli altri si fermano davanti a lui).

**OWE in acqua: sprite da nuoto** (richiesta dell'utente, 21/10).
`UpdateOWESwimSprites()` (`wild_encounter_ow.c`, a ogni frame in testa a
`UpdateOverworldWildEncounter`): un OWE generato su acqua surfabile la cui
specie ha uno sprite "surfable" di HnS (`src/surfable.c`, 139 specie; 37 delle
41 selvatiche d'acqua, mancano Golbat, Grimer, Muk, Magikarp, che all'utente
vanno bene così) ha lo sprite proprio nascosto con `ST_OAM_AFFINE_ERASE` e uno
sprite da nuoto che lo segue (`SpriteCB_OWESwim`: direzione, dondolio del
Surf, ancora 8 px sopra il fondo dello sprite dell'oggetto). **Non** si usa
`objectEvent->invisible`: `facetogether` (`ObjectEventsTurnToEachOther`) salta
gli oggetti invisibili. Trappole:
- un ricambio riusa slot **e** `localId`: lo sprite da nuoto confronta anche
  il `graphicsId`, altrimenti resta quello della specie vecchia (visto in
  emulatore: Tentacool sopra uno Shellder);
- le palette del Surf hanno lo stesso tag per normale e shiny (va bene per il
  giocatore, non per due selvatici): lo shiny usa tag + `0x200`
  (`OWE_SWIM_SHINY_PAL_TAG_OFFSET`), la palette si libera quando nessuno sprite
  la usa (`FreeOWESwimSpritePaletteIfUnused`; quella normale può essere anche
  del giocatore in Surf);
- **i tag del Surf vanno da `0x3001` a `0x308B`** (enum in
  `surfable_pokemon_templates.h`): si sovrapponevano ai tag grezzi dei
  riflessi di #9925 (`0x3000-0x3010`). `PAL_RAW_REFLECTION_OFFSET` è ora
  `0x3100`, in `field_effect_helpers.h`, con `STATIC_ASSERT` in `surfable.c`;
- palette: l'oggetto nascosto tiene la sua, quindi un OWE in acqua ne usa 2
  (13 su 16 occupate in una prova con 3 OWE e il giocatore in Surf). Se non ce
  n'è una libera lo sprite da nuoto non si crea e il Pokémon resta in piedi;
- lo spawn in acqua conta 64 tile in più per lo sprite da nuoto;
- i Pokémon-oggetto che HnS mette sulle mappe (Tentacool di Route 40, Poliwag
  dei laghetti) non sono OWE e restano com'erano.
Verificato su Route 40 in Surf: specie giuste a ogni campione, lotta da
contatto, sprite ricostruiti dopo la lotta, normale + shiny della stessa
specie insieme (shiny tolto via GDB a uno dei tre) con palette uguali ai file.

Fase 5, poi rivista: **`AreOverworldMonsEnabled()`** (`wild_encounter_ow.c`)
è l'unico interruttore. Falso con WILD BATTLES OFF e durante la Gara,
altrimenti **sempre vero**: la voce `WILD POKéMON` del PC (fase 5) è stata
tolta il 21/10 su richiesta dell'utente (PC di nuovo come prima: 4/5 voci,
case originali, verificato in emulatore con LOG OFF e CHALLENGES). Il bit
che la memorizzava è ora `unusedOverworldMonsBit` (bit 5 del byte a offset
56 di SaveBlock3): **ignorato ma non libero**, nei salvataggi passati per le
build con la voce può valere 1 (verificato: con il bit a 1 gli OWE compaiono
lo stesso). Lo usano lo spawn e `ShouldDisableRandomEncounters()`
(`field_control_avatar.c`): niente incontri a passi su erba, acqua e grotta;
pesca, Spaccaroccia, Testata e Profumino non passano di lì.
`WE_VANILLA_RANDOM` resta TRUE: serve alla Gara, l'unico posto in cui oggi
gli incontri a passi restano. `special GetOverworldMonsOption` dice solo se
WILD BATTLES è ON, per l'avviso della Gara.

**Gara Pigliamosche (fase 6, D6)**: `AreOverworldMonsEnabled()` è falso con
`GetBugContestFlag()` (`FLAG_SYS_BUG_CONTEST_MODE`), quindi niente OWE e
incontri a passi classici nella Gara. L'addetto del cancello
(`Gate_NationalPark_hns/scripts.inc`, le quattro strade che spiegano le
regole) chiama `Gate_NationalPark_EventScript_OverworldMonsNotice`: con
`special GetOverworldMonsOption` (la sola scelta del giocatore, senza la
Gara) dice l'avviso solo se WILD BATTLES è ON. Verificato: avviso visto su due
pagine, nel parco 0 OWE, `StandardWildEncounter` chiamata 4 volte in 12
passi, lotta con Caterpie Lv14 della tabella della Gara. Per arrivare al
punto: debug warp gruppo 22 mappa 7 warp 0, poi su 10 e sinistra 4, l'addetto
è sopra; nel parco l'erba grande è alle righe 27-35 (dall'ingresso: su 1,
destra 4, su 14). Nel debug il numero si cambia anche a passi di 10 con
destra/sinistra.

## Fase 6 fatta (21/10): prove e tre correzioni

**Tre difetti trovati in emulatore, tutti nel Safari di Johto** (Low Mid ha
11 Pokémon-oggetto propri: con `OW_GFX_COMPRESS` ognuno tiene tutto il suo
foglio in VRAM, e con 6 OWE restavano **10 tile libere su 1024**):

- **Schermata blu "could not load sprite tiles"**: `CheckCanLoadOWE` usava
  `assertf`. `make hns` non è una build RELEASE, quindi un `assertf` fallito
  mostra al giocatore la schermata di crash (riprendibile). VRAM o palette
  piene, o tabella dei 16 oggetti piena, sono stati del gioco e non errori
  di programma: ora sono `if` semplici e lo spawn salta. Upstream (anche
  `upcoming` di oggi) ha ancora gli `assertf`: loro raccomandano la grafica
  non compressa. **Regola**: un `assertf` su una condizione che il gioco può
  raggiungere, in questa ROM, è una schermata di crash.
- **Gioco bloccato parlando al follower**: lo script del follower aspetta
  `waitfieldeffect FLDEFF_EMOTE`; se `CreateSpriteAtEnd` dell'icona fallisce,
  l'effetto non arriva mai a `FieldEffectStop` e resta attivo per sempre.
  Vale per tutte le icone di `trainer_see.c` («!», «?», cuore, «!!», X,
  emozioni), quindi anche per allenatori e OWE. Ora `IconSpriteNotCreated()`
  toglie l'effetto dalla lista: senza sprite l'icona semplicemente non si
  vede. Provato con due ROM strumentate (sprite dell'emozione che fallisce
  sempre): con la correzione il messaggio del follower compare e il gioco
  va avanti; con il vecchio `return 0` resta su `WaitForFieldEffectFinish`.
- **Riserva di VRAM**: `OWE_SPRITE_TILE_HEADROOM` 32 (`wild_encounter_ow.h`)
  si somma alle tile chieste da `CheckCanLoadOWE_Tiles`, così gli OWE non
  prendono le ultime tile che servono a icone ed effetti di campo. Un foglio
  OWE è 112 tile o più (32x32, 7 fotogrammi). Nella prova lunga dopo la
  correzione: minimo 276 tile libere, quasi sempre 6 OWE.

Prove (mGBA, salvataggio nuovo, RAM via GDB):
- **Safari**: lotta OWE = lotta Safari (Safari Ball, POKéBLOCK, GO NEAR).
  Bonus della mangiatoia (`TryApplyPokeblockBonus`, reso pubblico e chiamato
  in `StartWildBattleWithOWE`): mangiatoia scritta in `sPokeblockFeeders`
  con un Pokéblock ROSSO, due Teddiursa OWE con PS/Attacco/Velocità 31,
  `abilityNum` 2 e natura +Attacco (LONELY, BRAVE); prima della mangiatoia
  un Aipom con IV casuali e `abilityNum` 1. 0 schermate blu in ~12 minuti
  con la ROM corretta (la vecchia ci era andata). Nel Safari le lotte sono
  frequenti: molti AGGRESSIVO (Tauros, Rhyhorn) e 6 OWE attorno.
- **Grotta buia** (Dark Cave, senza Flash): massimo 3 OWE; si vedono solo
  nel cerchio di luce. **Acqua** (Route 40 in Surf): 3 per 10 letture,
  lotta in acqua regolare (Tentacool). **Interno** (Sprout Tower 2F): 3;
  1F non ha tabella: 0.
- **Riflesso/shiny**: Route 30, laghetto in basso (giocatore a (44,49) della
  mappa), follower Caterpie normale riflesso, Force Shiny, Caterpie OWE
  shiny. Tag: follower `0x400A`, riflesso `0x480A`, shiny `0x600A`; palette
  in RAM uguali ai `.gbapal` (normale, normale filtrata, shiny). ROM di
  controllo senza #9925: il riflesso usa `0x600A`, cioè i colori shiny.
- **Roamer**: debug Script 5 (`InitKantoRoamers`, Latias Lv60),
  `sRoamerLocation[0]` portato sulla mappa corrente via GDB (dopo il warp:
  cambiare mappa lo sposta). Latias compare come OWE, la lotta ha
  `BATTLE_TYPE_ROAMER`, dopo la fuga il roamer va su un'altra mappa.

Ricette e trappole della sessione:
- **`gdb` su un file con `sed -i` perde l'output**: `sed -i` crea un file
  nuovo e `gdb` continua a scrivere su quello cancellato. Leggere da
  `/proc/<pid di gdb>/fd/1`.
- **I breakpoint con `commands ... continue &` perdono colpi**: alcuni non
  scattano mai pur passando di lì (`StartWildBattleWithOWE`). Non usarli
  come prova di "non chiamato": guardare i dati in RAM (o un watchpoint).
- **Decodificare un Pokémon in RAM**: `struct Pokemon` 100 byte, sicuri a
  +32, 4 sottostrutture da 12 byte cifrate con `personality ^ otId`, ordine
  `sSubstructOffsets[tipo][personality % 24]`; IV nel secondo u32 della
  sottostruttura 3, `abilityNum` bit 29-30 del terzo.
- **Force Shiny resta nel salvataggio** (è un flag) e gli shiny che non
  spariscono occupano i posti: per una prova senza shiny spegnerlo e
  cambiare mappa.
- Uscire dal Safari con il warp del debug lascia acceso
  `FLAG_SYS_SAFARI_MODE`: le lotte restano Safari.

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
  agli IV del Pokéblock (`TryApplyPokeblockBonus`) è collegato dalla fase 6.
- `UpdateOverworldWildEncounter` (ogni frame, da `OverworldBasic`) chiama
  `CheckCurrentWildMonHeaderForOWE` **prima** di guardare
  `WE_OW_ENCOUNTERS`: costo per frame anche a feature spenta. Non toccato
  (upstream), ma è il primo posto dove guardare se il frame rate cala.
- **Despawn "entrando in città" (23/10): in HnS spariva tutto a ogni passo.**
  `TryDespawnOWEsCrossingMapConnection` gira a **ogni** passo della camera
  (`field_camera.c`), non solo al cambio mappa, e upstream toglie tutti gli
  OWE se la mappa corrente è `MAP_TYPE_CITY`/`TOWN`: upstream le città non
  hanno selvatici. In HnS 15 ne hanno, a terra o in acqua (Lago d'Ira è
  `CITY`, poi Violet, Cherrygrove, Ecruteak, Olivine, Cianwood, Blackthorn,
  New Bark, Monte Argento esterno, Sinjoh, Celadon, Cerulean, Vermilion,
  Fuchsia, Viridian): gli OWE nascevano col giocatore fermo e sparivano
  (spruzzo d'acqua) al primo passo. L'utente lo ha segnalato come "i
  Magikarp scappano da lontano". Ora si tolgono solo gli OWE con
  `mapNum`/`mapGroup` diversi da quelli del giocatore (lo spawn li crea sulla
  mappa corrente, `SpawnSpecialObjectEvent`). Verificato col salvataggio
  dell'utente: prima, 5 passi in Surf = tutti spariti; dopo, 12 passi e
  restano, lotta col Magikarp raggiunta; dal Percorso 43 al Lago i 5 OWE del
  percorso spariscono ancora al passaggio. In acqua resta il ricambio: con
  tetto 3 e `OWE_SPAWN_TIME_REPLACEMENT` 240 frame ognuno vive ~12 s.
- `ShouldDisableRandomEncounters()` (`field_control_avatar.c`) usa
  `WE_VANILLA_RANDOM`, di compilazione: per la Gara serve il controllo a
  runtime che c'è (`AreOverworldMonsEnabled`).

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
  della mappa (Wooper, Pineco) no. (Dal 23/10 spariscono solo quelli di
  Route 29: vedi il despawn in città fra le trappole.)
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
