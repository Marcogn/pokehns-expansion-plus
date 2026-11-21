# Overworld catch: diario delle fasi

Spostato dal piano [`docs/plans/overworld-catch.md`](../plans/overworld-catch.md)
il 21/11 per alleggerirlo: com'è stata fatta ogni fase, con le misure e le
prove. **Non serve per implementare**: le decisioni, le formule e l'handoff
corrente sono nel piano; trappole e fatti con file:riga nelle regole
`overworld_catch.md`, `overworld_encounters.md`, `arceus.md`, `relearner.md`.

## 3. Fatti verificati (14/11, commit `d6520855`)

**Non c'è nulla di già scritto**: nessun lancio di Ball, nessun proiettile
nell'overworld, nessun `#if IS_HNS` che lo spegne. Non è un port da Soulgold.
Nemmeno fuori: nessun progetto GBA con sorgente pubblico ha il lancio in
overworld (ricerca del 15/11 in
[`docs/analysis/overworld-catch-research.md`](../analysis/overworld-catch-research.md));
HnS-Legends lo ha ma è solo una patch BPS, da provare e non da riusare.

Cosa c'è già e si riusa:

- **Passo lento**: `PlayerWalkSlow()` e `gPlayerAvatar.creeping`
  (`src/field_player_avatar.c:906-945`), oggi solo con `DN_FLAG_SEARCHING` e A
  tenuto. Attenzione al blocco della corsa (`:947`): con AUTORUN acceso
  (`autoRun == 0`) è vero anche senza B e fa `return`, quindi il passo lento
  va provato **prima**, come già fa il ramo del DexNav.
- **Tasti oggi**: `FieldGetPlayerInput` (`src/field_control_avatar.c:111`)
  non legge L; `struct FieldInput` ha bit liberi (`input_field_1_1` ecc.). R
  (`:259-287`, ramo `#if IS_HNS`) alterna Mach/Acro in bici e altrimenti avvia
  la ricerca DexNav sulla specie registrata: il secondo uso va tolto (D1).
  SELECT premuto = `UseRegisteredKeyItemOnField(FALSE)`, tenuto 60 frame =
  `(TRUE)`, il secondo strumento registrato (`registeredItemHold`, SaveBlock3;
  `src/item_menu.c:2451`). **B tenuto** oggi, con AUTORUN acceso (non è il default: §5.2;
  `autoRun == 0`), fa camminare invece di correre; con AUTORUN spento fa
  correre (`src/field_player_avatar.c:947-968`). Il passo lento del DexNav è
  su A tenuto (`:933-945`) e va spostato su B (D1).
- **Percezione attuale**: `CanAwareOWESeePlayer` (`src/wild_encounter_ow.c:2021`).
  Due regole sole: corsa o bici entro `activeDistance` (5) = notato in ogni
  direzione; altrimenti un rettangolo davanti al Pokémon, 4 caselle × 3
  (`viewDistance`, `viewWidth`). `CanOWEReachPlayer` (`:2078`) controlla solo
  il dislivello: gli ostacoli non contano. I sei comportamenti hanno **tutti
  gli stessi numeri** (`src/data/pokemon/wild_encounter_ow_behavior.h`).
  `struct BehaviorOWE` ha 6 bit di padding e campi da 4 bit (massimo 15).
- **"Mi ha notato"** oggi è implicito nello stato della macchina di movimento
  (`sprite->sTypeFuncId` ≥ 7, `SetSavedOWEMovementState`,
  `src/event_object_movement.c:12366`): la fase 2 deve esporlo con una
  funzione, perché serve al ×2 di D2.
- **L'OWE non contiene un Pokémon**: l'oggetto tiene specie, shiny, sesso
  (nel `graphicsId`), livello (`sOverworldEncounterLevel`) e categoria. Il
  Pokémon vero nasce in `StartWildBattleWithOWE` (`:574-624`) con
  `PickWildMonNature`, `CreateMonWithIVs`, `TryApplyPokeblockBonus`. La
  cattura deve passare **dalla stessa costruzione** (funzione comune estratta,
  non una copia).
- **Formula di cattura**: `ComputeCaptureOdds`, `ComputeBallData`,
  `ComputeBallShakeOdds` (`src/battle_script_commands.c:10923-11034`) sono
  `static` e leggono `gBattleMons`, `gBattleTypeFlags`, `gLastUsedItem`. Va
  separata la parte pura (specie, livello, PS, stato, Ball, livello del primo
  Pokémon in squadra) e chiamata da tutti e due i lati. Dentro ci sono già:
  EASY CATCH (`IsGuaranteedCatchEnabled`), malus medaglie, bonus livello
  basso. `B_CRITICAL_CAPTURE` è `FALSE`.
- **Dopo la cattura in lotta** succede: `GiveCapturedMonToPlayer`
  (`src/pokemon.c:5422`), `HandleSetPokedexFlag`, soprannome
  (`Cmd_trygivecaughtmonnick`, con `IsNuzlockeNicknamingActive` e
  `skipNicknamePrompt`), `GAME_STAT_POKEMON_CAPTURES`, Ball scritta nel
  Pokémon, effetti di Friend/Heal Ball. La fase 3 fa l'elenco completo
  leggendo `Cmd_givecaughtmon` e `BattleScript_SuccessBallThrow`.
- **Blocchi di sfida**: `SetNuzlockeChecks` (`src/nuzlocke.c:445`) legge
  `gEnemyParty[0]` e imposta `NuzlockeIsCaptureBlocked`,
  `NuzlockeIsSpeciesClauseActive`, `OneTypeChallengeCaptureBlocked`.
- **Grafica della Ball nell'overworld già in ROM**: `gObjectEventPic_*Ball`
  (`src/data/object_events/object_event_graphics.h:407-458`), una per tipo,
  con palette `OBJ_EVENT_PAL_TAG_BALL_*` (`OW_FOLLOWERS_POKEBALLS`), e le
  azioni `MOVEMENT_ACTION_ENTER_POKEBALL` / `EXIT_POKEBALL` dei follower.
  **Nessun file grafico nuovo è necessario.**
- **Densità**: `OWE_SPAWNS_MAX*` (`include/wild_encounter_ow.h:4-14`),
  `OWE_SPAWN_TIME_REPLACEMENT` 240 (`src/wild_encounter_ow.c:57`).
  `OWE_SPAWNS_MAX` dimensiona gli array degli slot e i `localId` 247-252.
- **Prezzi**: `src/data/items.h`. Le Ball con Ghicocca hanno il prezzo
  commentato (0): non si toccano.
- Lo spawn si ferma con i controlli bloccati (`UpdateOverworldWildEncounter`,
  `:440`). **Con la raffica (D15) i controlli restano liberi**, quindi questa
  protezione non c'è più: spawn e cattura usano entrambi `gEnemyParty[0]`.
  Dall'analisi non si intrecciano, perché ciascuno crea, usa e azzera il
  Pokémon nello stesso frame; la fase 3 lo **prova** (spawn forzato nello
  stesso frame di un impatto, ROM strumentata) invece di fidarsi.

**Banner di cattura (prototipo del 14/11, ROM usa-e-getta, non nel git)**:

- Riquadro e icona riusano quelli della descrizione dell'oggetto ottenuto
  (`ScriptShowItemDescription`, `src/overworld.c:3901`; `field.md`):
  finestra (1,1) 28×3 tile, base 8, palette 15, cornice
  `DrawStdFrameWithCustomTileAndPalette(.., 0x214, 14)`. Il Pokémon è
  l'**icona della squadra** (`LoadMonIconPalette` + `CreateMonIcon` con
  `SpriteCB_MonIcon`, animata) centrata a (26, 20) e `oam.priority` 0: lo
  sprite di lotta 64×64 non entra in un riquadro alto 3 tile. Testo in
  `FONT_SMALL` da x = 40 (a 28, come gli strumenti, l'icona copre le prime
  lettere), riga 2 a y = 12.
- **Il font piccolo non ha i simboli di sesso**: "♂" esce come un "8". Il
  simbolo va stampato a parte in `FONT_NORMAL`, blu
  (`TEXT_COLOR_BLUE`/`LIGHT_BLUE`) o rosso (`RED`/`LIGHT_RED`), alla x
  misurata con `GetStringWidth`; esce 1-2 pixel più basso del testo, da
  allineare. Senza sesso (Magnemite) niente simbolo.
- Provati: Pikachu ♂ in squadra, Eevee ♀ al PC, Magnemite, Crabominable (12
  lettere, ci sta con margine). Chiusura pulita: finestra e icona tolte,
  palette liberata, mappa intatta.
- Nel gioco vero il banner va mostrato **dopo** che il Pokémon è stato dato
  al giocatore (serve sapere se è finito in squadra o al PC, e in che box).
  Con la raffica (D15) i banner vanno in una coda (al massimo 3) e si
  mostrano uno alla volta; il prototipo ne gestiva uno solo e bloccava i
  comandi.

Fatti per la seconda parte (alfa e ricorda mosse):

- **Bit nell'oggetto**: `struct ObjectEvent` ha `padding:2` accanto a `shiny`
  (`include/global.fieldmap.h:289`): un bit diventa `alpha`. Gli altri campi
  che gli OWE riusano sono pieni (`sOverworldEncounterLevel` usa il bit 7 per
  il non-sparire, `src/wild_encounter_ow.c:41-47`). Gli oggetti finiscono in
  SaveBlock1: la fase 12 controlla che un salvataggio vecchio legga 0 lì.
- **Bit nel Pokémon salvato**: bit non usati da nessun file, verificato con
  `grep`: `PokemonSubstruct0` `unused_02:6`, `unused_04:3`, `unused_0A:2`;
  `PokemonSubstruct1` `unused_04:5`, `unused_06:3`; `BoxPokemon` `unused_13`,
  `unused_1E` (`include/pokemon.h:130-170, 255-276`). Meglio uno delle
  sottostrutture: sono cifrate e coperte dal checksum. `CreateMon` azzera
  tutto, quindi un Pokémon vecchio legge 0 = non alfa. **Da controllare in
  fase 13**: che expansion non abbia già preso quel bit nelle versioni dopo la
  1.15.2 (un `/upstream-sync` futuro lo sovrascriverebbe) e che l'app
  compagna di PokeDaisy non lo legga (`hasep_symbols.md`).
- **Aumento da totem già nel motore**: `settotemboost`
  (`asm/macros/event.inc:2252`, `ScriptSetTotemBoost` in
  `src/battle_main.c:6577`) riempie `gQueuedStatBoosts`; in lotta lo applica
  `BS_GetTotemBoost` col messaggio. Gli alfa partono da codice, non da script:
  si scrive in `gQueuedStatBoosts` prima della lotta.
- **Blocco della cattura in lotta**: `GetBallThrowableState`
  (`src/item_use.c:1265`) ha già un elenco di motivi (Nuzlocke, One Type,
  `FLAG_NO_WILD_CATCHING`): si aggiunge `BALL_THROW_UNABLE_ALPHA`.
- **Missione a livelli: il modello c'è già**, TcloneR (`src/tcloner.c:26`,
  `trainer_catch.md`): un contatore in una var (`VAR_TCLONER_COUNT`), un'altra
  var per i premi ritirati (`VAR_TCLONER_REWARDS`), una tabella `sRewards` di
  soglie e premi, lo stato `QUEST_STATE_REWARD` del quest book e una
  sottomissione per premio (**le due liste vanno tenute allineate a mano**,
  `quest_book.md`). Gli alfa servono due var: `vars.h` ne segna diverse come
  `VAR_UNUSED_*`, ma `trainer_catch.md` avverte che alcune vecchie "libere"
  **non sono dimostrabilmente a zero** nei salvataggi: la fase 14 le controlla
  sul `.srm` dell'utente. Il contatore sale a ogni alfa catturato, in lotta e
  col lancio (stessa funzione per i due lati, fase 13).
- `setwildbossbattle` (`src/scrcmd.c:3383`) è una lotta di script con mosse
  scelte, senza aumento di statistiche: non serve agli alfa.
- **Sprite più grande**: il motore scala già sprite di oggetti con matrici
  affini (`src/event_object_movement.c:1531`, riflessi). Le matrici affini
  sono 32 e gli OWE in acqua ne usano una (`ST_OAM_AFFINE_ERASE`). Un oggetto
  scalato oltre i suoi 32×32 viene tagliato se non è in doppia dimensione:
  **prototipo prima di promettere**, in fase 12.
- **Ricorda mosse oggi**: `P_ENABLE_MOVE_RELEARNERS` **TRUE**
  (`include/config/summary_screen.h:13`) apre tutto; `P_FLAG_EGG_MOVES` e
  `P_FLAG_TUTOR_MOVES` sono **0**. Il codice li legge già
  (`src/move_relearner.c:1217, 1284`), e anche `data/scripts/move_relearner.inc`.
  In Soulgold (verificato in passato, `relearner.md`): MT spente, uovo dietro
  `FLAG_EGG_MOVES_UNLOCKED` (Egg Move Master, Blackthorn City House 3,
  ¥88.888), tutor dietro `FLAG_TUTOR_MOVES_UNLOCKED` (Tutor Move Master,
  Olivine City House 4, ¥44.444). **Qui** `BlackthornCity_House3_hns` ha già
  il Cancella mosse e un ricorda mosse a Squama Cuore
  (`BlackthornCity_MoveRelearnersHouse_EventScript_MoveRelearner`, riga 107),
  mentre `OlivineCity_House4` non esiste (ci sono House1-3): dove mettere il
  maestro dei tutor si decide in fase 7, guardando le mappe. Il sorgente di SG non è
  in questa sessione: la fase 7 lo aggancia (`add_repo Marcogn/soulgold`).
- **Polarità del ricorda mosse**: i flag nuovi valgono 0 nei salvataggi
  vecchi, quindi chi aggiorna **perde** l'accesso libero a mosse uovo e tutor
  finché non paga. È l'effetto voluto (D10), ma va scritto nel CHANGELOG.

Trappole già note che qui mordono (dalla regola degli OWE):

- VRAM: nel Safari restavano 10 tile su 1024; `OWE_SPRITE_TILE_HEADROOM` ne
  riserva 32. Lo sprite della Ball e gli effetti devono starci, e **uno sprite
  che non si crea non deve lasciare uno script in attesa**.
- Mai `assertf` su stati raggiungibili (schermata blu).
- Un OWE in acqua ha lo sprite vero nascosto e uno da nuoto sopra, più 2
  palette: il risucchio nella Ball va provato a parte.
- `gEnemyParty[0]` azzerato: guardare il Pokémon catturato in squadra (il
  precedente è il Bad Egg di `trainer_catch.md`).

## Formule di Arceus: misure e dati scartati (20/11)

**Effetto misurato** (calcolo; IV 15 EV 0 contro impegno 0, natura neutra,
mossa senza STAB ed efficacia 1, danno massimo, attaccante e difensore della
stessa specie):

| Base, livello, potenza | Classica: colpi per KO | Statistiche Arceus, danno classico | Arceus intero |
| --- | --- | --- | --- |
| 50, Lv 5, 40 | 4 | 12 | 4 |
| 50, Lv 25, 40 | 6 | 9 | 3 |
| 50, Lv 50, 80 | 4 | 4 | 2 |
| 100, Lv 5, 40 | 5 | 23 | 7 |
| 100, Lv 50, 80 | 5 | 6 | 3 |
| 100, Lv 100, 80 | 5 | 5 | 4 |

Le statistiche da sole allungano molto le lotte iniziali; con il danno di Arceus
le lotte iniziali restano simili e quelle di metà gioco si accorciano.

Cattura con la formula di Arceus (PS pieni, comportamento base ×2, rango del
giocatore = medaglie): Sentret Lv 3 con Poké Ball 81%; con 6 medaglie,
Furret Lv 21 32% (alle spalle 37%, Ultra 44%), Pinsir Lv 22 19% (Ultra 31%);
**Pinsir Lv 22 con 0 medaglie: impossibile con la Poké Ball** (a < 0), 7%
con la Ultra. Con la formula di oggi Furret era 12% / 28% e Pinsir 6% / 12%
(fase 6).

**Tassi di Arceus, scartati il 20/11 per il tasso unico** (restano qui come
fonte): `PKHeX.Core/Resources/byte/personal/personal_la` di
[PKHeX](https://github.com/kwsch/PKHeX) (dati estratti dal gioco), voci da
0xB0 byte per numero nazionale, tasso al byte 0x08, presente nel gioco al bit 6
del byte 0x21 (`PersonalInfo8LA.cs`). Letto il 20/11: 226 specie, di cui 48 di
Kanto e 32 di Johto. **Rifatti specie per specie, non scalati**: Onix 45 → 235,
Scyther 45 → 215, Machop 180 → 245, Heracross 45 → 85, Gyarados 45 → 55,
Pikachu 190 → 145.

## 4. Spazio misurato (build pulita di `d6520855`)

| | Usato | Limite | Libero |
| --- | --- | --- | --- |
| ROM | 31 902 628 B (95,08%) | 32 MB, errore CI al 99,5% | 1 651 804 B. Le mega sono fuori per ora (D5); se tornassero (`megas.md`, 1 574 944 B) la ROM andrebbe al 99,77%, già oltre il limite senza questa feature |
| SaveBlock3 | 96 B | 116 | 20 B |
| VRAM sprite | 622 tile, 10 palette (Route 29 di notte, 4 OWE + follower + 3 Ball di tipi diversi in aria; fase 4) | 1024 tile, 16 palette | 402 tile, 6 palette |

Costo previsto della prima parte: **solo codice** (stima 8-15 kB, da
rimisurare a ogni fase), **zero byte di salvataggio** (`gLastThrownBall` è già
in EWRAM, nessuna opzione nuova), nessuna grafica nuova.
Seconda parte: **due flag** nuovi per il ricorda mosse (`save.md`: liberi
`FLAG_*` e engine flag da `+14`), **un bit** in ogni Pokémon salvato (bit già
riservato, struct invariata), un bit nell'oggetto; grafica forse un'icona del
segno alfa per il riassunto. Nessun campo nuovo in SaveBlock3.

## Fasi

### 5.1 Fase 1, com'è stata fatta (16/11)

- **Cosa**: tetti 4/2/2 e Safari 3 (`include/wild_encounter_ow.h`), ricambio a
  tetto pieno 480 frame (`OWE_SPAWN_TIME_REPLACEMENT`), prezzi in
  `src/data/items.h` (Net, Nest, Dive, Dusk, Timer, Repeat, Luxury 100; Heal
  50). I 480 frame non entravano nel contatore: `sOWESpawnCountdown` è ora
  `u16` con segnale `0xFFFF` (`.claude/rules/overworld_encounters.md`).
  `OWE_SPAWNS_MAX` resta 6. README e `CHANGELOG.md` (2.0.6.PLUS.12) aggiornati.
- **Verificato** (banco usa-e-getta: mGBA con `-g`, GDB che ogni ~1 s di gioco
  scarica la tabella oggetti e `vblankCounter1`; teletrasporto di debug per
  ricaricare la mappa; non nel git):
  - Route 29, 70 campioni in 4024 frame (~67 s): ROM nuova **4 generati in
    tutti i campioni**; ROM con i tetti vecchi, stessa misura: da 3 a 6, **più
    di 4 in 64 campioni su 70**.
  - Grotta dell'Unione 1F, 70 campioni in 4029 frame: **massimo 2**. In una
    schermata si vedevano tre Pokémon: due erano decorativi di mappa (vedi
    handoff), non spawn.
  - Prezzi letti **dalla ROM** (`gItemsInfo`, `0x087b32bc`, 0x2C byte a voce,
    `price` primo `u32`): nuova Heal 50, Net/Nest/Dive/Dusk/Timer/Repeat/Luxury
    100, Poké 10, Great 15, Ultra 30, Premier 20, Quick 50; originale Heal
    300, le sei 1000, Luxury 3000.
  - Un salvataggio con 6 OWE, fatto con la ROM vecchia, si carica sulla nuova.
- **Non verificato**: tetti acqua (2) e Safari (3), che passano dalla stessa
  `GetOWESpawnCap`; la grotta con i vecchi tetti; **la schermata di un
  negozio** che il cancello chiedeva: queste Ball non si vendono a Johto e
  Kanto e il teletrasporto di debug ai negozi di Hoenn va in errore anche sulla
  ROM originale (`emulator.md`), quindi i prezzi sono letti dalla ROM.
- **Dopo `/code-review` (16/11)**: corretti quattro punti. (1) Sopra il tetto
  il ricambio uno per uno teneva l'eccesso finché non si cambiava mappa:
  `TryRemoveOWEAboveCap` toglie il più vecchio non shiny ogni 30 frame.
  Prova: lo stesso salvataggio con 6 OWE, caricato senza ricaricare la mappa,
  prima restava a 6 per 641 frame, ora è a 4 dal primo campione per 1514
  frame (ROM finale). (2) Con OWE dell'altra zona (Pokémon di terra quando si inizia il
  Surf) il ricambio è a 30 frame invece di 480 (`IsAnyOWEFromOtherArea`,
  shiny esclusi perché non vengono mai tolti):
  **non provato in emulatore** (serve una rotta con acqua e il Surf). (3)
  Commento di `OWE_SPAWNS_MAX` corretto. (4) `STATIC_ASSERT` dei tetti
  contro `OWE_SPAWNS_MAX`. Lasciati, con il motivo: conteggi ripetuti (non
  per frame, schema di prima), Luxury senza `I_PRICE` (decisione D4),
  CHANGELOG sulle Ball non in vendita (punto aperto qui sotto).
- ROM finale: 95,08% usato.

**Handoff per la fase 2 (Opus 5.5)**
- Non serve il `.srm`. Per misurare posizioni da GDB: `currentCoords` a
  +0x10 (x) e +0x12 (y, `s16`), `previousCoords` a +0x14, `facingDirection`
  a +0x18 (`struct ObjectEvent`, `include/global.fieldmap.h`); il resto della
  ricetta (offset, `vblankCounter1`, tasti con `xdotool`) in `emulator.md` e
  nella regola.
- **Punto aperto per l'utente**: le Ball speciali **non si vendono in nessun
  negozio di Johto/Kanto** (solo Rustboro, Verdanturf e Mossdeep a Hoenn e la
  Casa 2 di Melemele a Alola: mappe che il debug non raggiunge). Il nuovo
  prezzo oggi si vede solo nel valore di vendita. Decidere dove si vendono
  (`sMartExtras`, `src/shop.c:999-1008`, come la Quick Ball ad Azalea) è una
  scelta di contenuto, non di questa fase.
- **Fase 3**: le mappe di HnS hanno **Pokémon decorativi** (oggetti
  `OBJ_EVENT_GFX_MON_BASE+…`, `trainerType` 0): sono sul percorso di un
  lancio ma non sono selvatici. Il bersaglio va scelto con
  `IsOverworldWildEncounter`, e gli altri vanno trattati come ostacoli o
  ignorati (da decidere lì). Occupano anche slot di oggetti e VRAM.
- Da provare quando capita un Surf: il ricambio veloce con OWE dell'altra
  zona (punto 2 della revisione qui sopra).
- Il commento di `include/config/wild_encounter.h:8` cita
  `HasepAreOWEsAllowedHere()`, che non esiste più (gli OWE sono attivi
  ovunque): da togliere alla prima occasione.

### 5.2 Fase 2, com'è stata fatta (16/11)

- **Cosa**: passo lento su B, udito per andatura, cono di vista con
  ostacoli ed erba alta, `IsOWEAwareOfPlayer()`. Fatti e trappole nella
  regola nuova `.claude/rules/overworld_catch.md` (la regola degli OWE era a
  7,3 KB); CHANGELOG (2.0.6.PLUS.12), README, `dexnav.md` aggiornati.
  - `PlayerNotOnBikeMoving`: B tenuto = `PlayerWalkSlow` + `creeping` con
    AUTORUN acceso; **durante una ricerca DexNav B tenuto è il passo lento
    sempre**, a terra e in Surf, anche con AUTORUN spento (correre lì fa
    comunque scappare il bersaglio); A non fa più il creeping. Fuori dalla
    ricerca, AUTORUN spento: B corre come prima (P20). Velocità del Surf
    invariate.
  - `CanAwareOWESeePlayer`: udito (corsa/bici = `activeDistance` 5,
    camminata `OWE_HEARING_WALK` 2, passo lento mai, attraverso gli ostacoli),
    poi cono (1/3/5 di larghezza a distanza 1/2-3/4+, limitato da
    `viewWidth`), linea di vista bloccata da collisioni di mappa e oggetti che
    non sono OWE né follower, erba alta = visto solo dalla casella davanti.
  - Numeri per comportamento: insegue, fugge, sparisce 5×5; osserva 4×5;
    si avvicina 4×3 (curioso, poco guardingo); vaga invariato (non nota mai).
    Erano tutti 4×3. Vincolo `viewDistance <= activeDistance` scritto nel
    file dei dati (motivo nella regola).
- **Fatto nuovo, contro P22**: in una partita nuova **AUTORUN è spento**
  (`src/new_game.c:123`, `autoRun = 1`; 0 = acceso). Verificato anche in
  emulatore: partita nuova, B tenuto correva (7 caselle in ~100 frame),
  senza B camminava (4). Quindi con i default il passo lento non c'è: vedi
  punti aperti.
- **Verificato** (ROM strumentata usa-e-getta, non nel git: tetto 1 OWE, il
  Pokémon resta fermo e non cambia direzione, comportamento forzato, un
  comando in RAM lo sposta a un offset dal giocatore con una direzione e
  registra il frame e le posizioni in cui nota; mGBA + GDB sulla Route 29,
  partita nuova; il ricambio bloccato col bit "non sparire"):
  - **Vista, giocatore fermo** (niente udito), comportamento "osserva" (4×5):
    159 posizioni nelle 4 direzioni (offset avanti 0-5, di lato ±3),
    **159/159 uguali** al modello calcolato sulla griglia delle collisioni
    letta dalla RAM. Notato a 4 di fronte, non a 5; mai alle spalle; il
    cono si allarga come previsto; casi coperti da alberi non notati. NPC in
    mezzo: (4,−1) con l'NPC sulla linea non vede (6 prove, NPC rimasto lì),
    il simmetrico (4,+1) sì.
  - **Vista a 5**, comportamento "fugge" (5×5, scritto da GDB nel
    `movementType`): 189 posizioni, **189/189** col modello, notato a 5 di
    fronte (anche a ±2 di lato) e mai a 6.
  - **Erba alta**: giocatore nell'erba, 161 posizioni: notato **solo** dalle
    3 caselle adiacenti di fronte (la quarta è del follower), 161/161 col
    modello.
  - **Udito**, Pokémon girato dall'altra parte, giocatore che si avvicina in
    linea retta: corsa (DASH acceso) notata **a 5** sia con AUTORUN spento
    (B) sia acceso (senza B); camminata (AUTORUN spento, senza B) **a 2**;
    passo lento (AUTORUN acceso, B) arrivato **adiacente senza essere
    notato**. Passo lento: 3 caselle in ~120 frame, contro 4 della camminata
    e 7 della corsa in ~100.
  - **ROM vera** (quella consegnata, identica byte per byte a quella
    provata): AUTORUN spento, senza B cammina (4 caselle in ~100 frame,
    niente DASH), con B **corre ancora** (7, DASH); AUTORUN acceso, B = passo
    lento (3, `creeping` 1), senza B corsa (DASH); con il flag della ricerca
    DexNav acceso a mano e AUTORUN spento, B = passo lento (2, `creeping` 1)
    e A una camminata (`creeping` 0). A davanti all'NPC della Route 29 apre
    il suo dialogo.
  - **Senza la modifica** (ROM di partenza, stesso salvataggio): AUTORUN
    acceso, B = camminata normale (4 caselle, `creeping` 0). La percezione
    di prima (rettangolo 4×3, niente erba né ostacoli) non è stata rifatta
    in emulatore.
  - **Non verificato**: una ricerca DexNav **vera** con `ENHANCED DEXNAV`
    spento (con l'opzione accesa il bersaglio non scappa mai): provato solo
    che B alza `creeping` col flag della ricerca, che è ciò che la fuga legge
    (`src/dexnav.c:1293`). Accendere il flag a mano fa partire dopo qualche
    passo una lotta (Hoothoot): il DexNav gira senza una ricerca vera, è un
    artefatto della prova. Surf non provato.
  - **AUTORUN acceso di default** (aggiunto dopo la scelta dell'utente):
    senza salvataggio, al menu principale `autoRun` = 0 e OPTIONS mostra
    AUTORUN ON; il salvataggio di prova della Route 29 (fatto con AUTORUN
    spento) caricato sulla stessa ROM legge ancora 1.
- ROM: 31 903 524 B, 95,08% (+896 B sulla fase 1).

- **Dopo `/code-review` (16/11)**: corretti quattro punti. (1) Il salto da
  una sporgenza esce prima dell'azzeramento di `creeping`: dopo un passo
  lento era silenzioso; ora lo spegne. (2) Gli oggetti `invisible` non
  coprono più la vista. (3) `IsPlayerInOWEViewCone` legge la tabella dei
  comportamenti una volta sola (gira ogni frame). (4) Condizione dell'erba
  semplificata (`forward > 1`, equivalente). Lasciati, col motivo: AUTORUN
  ereditato da una New Game sopra un salvataggio (le opzioni del titolo si
  ricopiano di proposito: scritto nel CHANGELOG e nella regola); il ripiego
  `SPECIES_NONE` dell'udito (caso che non capita, un OWE ha sempre una
  specie); `IsOWEAwareOfPlayer` senza chiamanti (è per la fase 3).
  Riprovato con la ROM strumentata ricompilata sul codice corretto: erba
  alta 161/161 (notato solo dalle 3 caselle adiacenti di fronte), terreno
  normale "osserva" 161/161 col modello. Il salto con `creeping` spento
  **non** è provato in emulatore (serve una sporgenza con un OWE vicino).

**Handoff per la fase 3 (Fable 5.2)**
- Serve il `.srm` (cancello della fase 3).
- `IsOWEAwareOfPlayer(owe)` è il "ha notato" del ×2 di D2. Un OWE "vaga"
  (`OWE_IGNORE_PLAYER`) non nota mai: con D2 avrà sempre il ×2.
- **AUTORUN acceso nelle partite nuove** (utente, 16/11, dopo la
  segnalazione qui sopra): `SetDefaultChallengeSettings` mette `autoRun = 0`.
  Gira solo a salvataggio vuoto o corrotto, quindi i salvataggi esistenti
  tengono il loro valore; le opzioni del titolo lo ricopiano già
  (`oak_speech_hns.c`). Verifica in §5.2.
- I muretti/staccionate della Route 29 risultano collisione nella griglia
  (`MapGridGetCollisionAt`), quindi coprono la vista anche se bassi.
- Ricetta di prova (ROM strumentata) riusabile per il bersaglio del lancio:
  §5.2 qui sopra e la regola.
- Il commento di `include/config/wild_encounter.h:8` su
  `HasepAreOWEsAllowedHere()` è ancora da togliere.

### 5.3 Fase 3, com'è stata fatta (17/11)

- **Cosa**: `src/overworld_catch.c` (nuovo), script in
  `data/scripts/overworld_catch.inc`. Fatti e trappole nella regola
  `.claude/rules/overworld_catch.md`, sezione "Lancio"; ricette di prova in
  `docs/claude/emulator.md`. CHANGELOG (2.0.6.PLUS.12), README, `dexnav.md`.
  - Tasti di D1: R lancia (ricordato 16 frame, così a metà passo non si
    perde), L premuto cambia modalità, L tenuto + ←/→ scorre e ferma il
    giocatore; scorciatoia DexNav su R tolta; in bici R resta Mach/Acro.
  - Un lancio è un record con un timer di volo (8 + 8 frame a casella) e poi
    60 frame di "assestamento" al posto delle scosse: la fase 4 ci appende
    gli sprite. Esito all'impatto, Pokémon consegnato subito, banner e
    soprannomi in coda dopo l'ultimo record.
  - Formula di cattura separata dai dati di lotta (`struct CaptureParams`),
    Pokémon costruito da `CreateMonFromOWE` (comune con la lotta), regole di
    sfida da `GetCaptureChallengeChecks`, strumento tenuto da
    `GiveWildMonHeldItem`: quattro refactor di codice esistente.
  - Dopo la cattura, come in lotta: Ball nel Pokémon, amicizia della Friend
    Ball, strumento tenuto, OT, squadra o PC, Pokédex (visto e catturato),
    `GAME_STAT_POKEMON_CAPTURES`, flag Nuzlocke della zona, soprannome. Non
    fatti di proposito: esperienza (P6), pagina del Pokédex (P18), scelta
    "squadra o Box" a squadra piena (P8), TV e catena DexNav.
- **Scelte prese qui** (cambiabili, la 6 rivede il bilanciamento):
  - **Quick Ball**: il suo ×5 vale solo su un Pokémon che non ti ha notato
    (qui non ci sono turni: sarebbe stato ×5 sempre). Timer Ball: mai bonus.
  - **Il tipo "vaga" non reagisce** a una Ball fallita e tiene il ×2.
  - **Pokémon lanciato**: la lotta parte da dove siete, senza
    l'avvicinamento dello script normale (su 5 caselle porterebbe a riva un
    Pokémon che nuota).
  - **Roamer e sciami** (P4): R avvia la lotta e la Ball **non parte** (il
    piano diceva "il lancio fallisce sempre": così non si spreca una Ball).
  - **Acqua**: non ferma il lancio (nessuna casella d'acqua ha il bit di
    collisione); il "dislivello" di P2 è la collisione dei bordi.
  - **`EASY CATCH`** vale anche qui, come in lotta.
- **Verificato** in mGBA + GDB col `.sav` dell'utente (Route 43, squadra da
  6 al 38, 37 Poké Ball; `EASY CATCH` e NICKNAMES OFF accesi, cambiati in
  RAM dove serviva). "ROM di prova" = usa-e-getta, identica salvo gli OWE a
  riposo fermi (non nel git). Bersagli messi sulla linea e specie cambiate
  scrivendo l'oggetto da GDB.
  - **Formula in lotta** (ROM strumentata: le funzioni di prima del
    refactor, da `2dcd0dc6`, girano accanto alle nuove a ogni lancio):
    **15 casi, 15 uguali**. Mewtwo Lv 23: Poké 1/1, Love 4/4 (ramo dei tipi
    di HnS), Quick 1/1 dopo il primo turno, Timer 1/1. Skarmory Lv 23: Quick
    32/32 al primo turno, Dusk 28/28 di notte, Nest 13/13, Level 32/32, Poké
    8/8, Ultra 16/16, Net 8/8, Fast 8/8; con lo stato scritto in RAM: 5 PS su
    70 23/23, 5 PS + paralisi 34/34, 20 PS + sonno con Dusk 175/175.
  - **Master Ball, guardando il Pokémon**: Pikachu ♂ in squadra (slot
    liberato in RAM): Lv 21, Master Ball, checksum valido, OT e ID del
    giocatore, "met at Lv 21, Route 43" nel riassunto, non Bad Egg; Pokédex
    0→1, catture +1, Master Ball −1; soprannome "AA" dalla schermata dei
    nomi, salvato. Venonat ♀ a squadra piena: al PC.
  - **Banner**: squadra e PC, ♂ blu, ♀ rosso, senza sesso (Magnemite, niente
    simbolo), nome da 12 lettere (Crabominable); si chiude con A solo a
    fanfara finita; chiusura pulita. Tema scuro dell'utente: cornice scura.
  - **Raffica**: 2 presi + 1 fallito (il terzo, allertato, mi ha caricato:
    lotta, fuga, poi i due banner in ordine); 3 presi con squadra da 5 (uno
    in squadra, due al PC, tre banner). **Bug trovato qui**: la coda contava
    anche i record già atterrati e rifiutava il terzo lancio.
  - **Nuzlocke, seconda Ball già in volo**: la Ball 1 (volo allungato in
    RAM) atterra dopo che la 2 ha catturato e segnato la zona: rimborso
    (breakpoint su `AddBagItem`: Master Ball, 1), Pokémon ancora sulla mappa.
  - **Rifiuti al lancio**, con messaggio e Ball non usata: zona Nuzlocke
    usata, Box pieni a squadra piena, flag Safari.
  - **Non consumata**: linea vuota, NPC, Pokémon decorativo; lotta partita
    col bersaglio mentre la Ball volava (36→37 dopo la fuga).
  - **Fallito → allertato**: inseguitore di spalle, giocatore fermo, non
    allertato per 3 s; dopo la Ball fallita allertato entro 16 frame, si
    gira e viene. **Bug trovato qui**: il bit di allerta restava acceso su
    un OWE già allertato o "vaga" (ora non si alza).
  - **Spawn nello stesso giro dell'impatto** (conto alla rovescia scritto a
    `timer − 1`, breakpoint d'ordine): 3 prove, ogni volta la specie giusta
    nel Box e il nuovo OWE nato.
  - **Tasti**: L premuto alterna la modalità; L + →/← scorre Ball (ordine
    della tasca, con giro) e Pokémon; L + ↓ per 0,5 s: giocatore fermo; con
    `L=A` L non alterna e R lancia; AUTORUN spento: R lancia; flag della
    bici: R alterna Mach/Acro e non lancia.
  - **Pokémon lanciato**: il secondo della squadra entra per primo da 5
    caselle; dopo la fuga la squadra è nell'ordine di prima.
  - **Acqua** (sintetico: metatile dello stagno scritto sotto il bersaglio,
    Lapras): sprite da nuoto visibile, cattura, oggetto e sprite spariti.
  - **ROM finale** sul salvataggio intatto: cattura con Poké Ball (EASY
    CATCH), banner, chiusura; con EASY CATCH spento un lancio fallito e
    l'allerta. Il resto è sulla ROM di prova.
- **Non verificato in emulatore**: salita vera in bici (flag scritti in
  RAM) e un OWE d'acqua vero preso in Surf; roamer e sciami (nessuno nel
  salvataggio); rifiuti One Type e Species Clause (stessa funzione della
  zona, altri rami) e Gara/Pike/Piramide/Trainer Hill; soprannome
  obbligatorio della Nuzlocke; lotta doppia selvatica con Pokémon lanciato;
  banner in grotta buia (la seconda icona come finestra OBJ è scritta ma non
  provata); amicizia della Friend Ball e strumento tenuto (i due Pokémon
  guardati non ne avevano); allineamento del simbolo di sesso guardato a 3×,
  non misurato in pixel.
- ROM: 31 908 340 B, 95,09% (+4 816 B sulla fase 2).

- **Dopo `/pr-review` (17/11, PR #60)**: 9 punti dalla revisione generica,
  tutti verificati sul codice. Corretti 6: (1) i banner partivano **prima**
  della gestione del passo e su un frame di passo saltavano contapassi e
  script della casella; (2) Start con una Ball in volo apriva il menu e un
  salvataggio la perdeva (ora Start è ignorato finché vola, < 1 s); (3) R
  premuto durante un messaggio tornava come lancio alla chiusura; (4) L
  tenuto attraverso un menu cambiava modalità al rilascio; (5) la palette
  dell'icona del banner era liberata anche se condivisa (icona della ricerca
  DexNav); (6) due commenti superati. Provati in emulatore (1-4 e il banner).
  Lasciato: il doppio ciclo sulla squadra (stile). **Deciso
  dall'utente (17/11)**: un lancio **fallito** non consuma l'incontro
  Nuzlocke della zona (in lotta qualunque esito lo consuma): col lancio conta
  solo la prima cattura riuscita. Resta così (P3).

**Handoff per la fase 4 (Fable 5.2)**
- **Non c'è ancora niente a schermo** per il lancio né per la scelta: solo
  suoni (`SE_BALL_THROW`, `SE_BALL` presa, `SE_BALL_OPEN` uscita,
  `SE_BALL_BOUNCE_1` rimbalzo, `SE_SWITCH`/`SE_SELECT` per L). Il pannello è
  della fase 5.
- Dove agganciare gli sprite: `struct PendingThrow` in `overworld_catch.c`.
  `OverworldCatch_TryThrow` crea il record, `ResolveImpact` decide (il
  numero di scosse è il ritorno di `RollShakes`, oggi buttato),
  `THROW_SETTLE_FRAMES` è la durata finta delle scosse, `ReturnBall` i casi
  a vuoto. Oggi alla cattura l'OWE è tolto **all'impatto**
  (`RemoveCaughtOWE`): con l'animazione va nascosto nella Ball e tolto alla
  fine, e un fallito va fatto riuscire prima dell'allerta.
- I timer di volo sono `u8` e fermi a controlli bloccati; una lotta che
  parte lascia i record com'erano (al ritorno: bersaglio sparito = Ball
  resa). La fase 4 deve chiudere gli sprite prima della transizione.
- Un lancio senza bersaglio oggi non fa nulla: il "vola 5 caselle e cade"
  di D16 è animazione, fase 4.
- La lotta da Pokémon lanciato parte senza avvicinamento: se la fase 4
  anima l'uscita del Pokémon, lo script è `OverworldCatch_EventScript_Battle`.
- Per le prove: il salvataggio dell'utente ha `EASY CATCH` acceso;
  `emulator.md`, sezione del 17/11.
- **Punti per l'utente** (non bloccano): le scelte elencate sopra (Quick
  Ball, "vaga", roamer, lotta a distanza); le Ball speciali ancora non in
  vendita a Johto/Kanto (handoff della fase 1).

### 5.4 Fase 4, com'è stata fatta (17/11)

- **Modello**: assegnata a Fable 5.2, fatta su Opus 5.5 (cambio deciso
  dall'utente a sessione iniziata).
- **Cosa**: tutto in `src/overworld_catch.c` più quattro agganci
  (`battle_setup.c`, `field_control_avatar.c`, `wild_encounter_ow.c`,
  `GetRunningDirectionAnimNum` esportata). Stati, trappole e misure nella
  regola `.claude/rules/overworld_catch.md`, "Lancio"; metodo di prova in
  `docs/claude/emulator.md`, sezione del 17/11. CHANGELOG (PLUS.12).
  - Il record della fase 3 ora ha stati di animazione (volo, apertura,
    chiusura, caduta, scosse, click o uscita; a vuoto: caduta e Ball a
    terra) e gli sprite sono appesi ai record. Esito e Pokémon consegnato
    restano all'impatto (D15); `THROW_SETTLE_FRAMES` non c'è più.
  - Sprite delle Ball dei follower (nessuna grafica nuova); arco con
    altezza 4 + 2 a casella sopra la retta mano-bersaglio; la Ball insegue
    il bersaglio che si muove.
  - Pokémon congelato e nascosto dentro la Ball dal raggio in poi; alla
    cattura l'oggetto è tolto a fine apertura (non a fine scosse: un
    oggetto nascosto per 3 s attraversava spawn, despawn e collisioni).
  - **D16 completato**: a vuoto la Ball vola 5 caselle e cade, contro un
    muro o un NPC lo colpisce e cade; torna in borsa. Verso il follower non
    parte. Start bloccato finché una Ball è fuori (prima solo in volo).
  - Posa: frame della corsa tenuto 16 frame con un'azione `DELAY_16`.
- **Verificato** in mGBA + GDB, ROM di prova (OWE fermi a comando, OWE messo
  sulla linea, esito forzato, sprite forzato a fallire; non nel git), col
  `.sav` dell'utente (Route 43, giorno, PLUS.11) e una partita nuova (Route
  29 di notte, Union Cave 1F). Foto frame per frame con un breakpoint su
  `OverworldCatch_Update`.
  - **Cattura**: verso sud (Girafarig), est (Sentret, inseguito mentre
    fuggiva), nord (Doduo), in grotta (Geodude ♀ al PC); banner dopo le
    scosse.
  - **Fuga dopo 1 scossa** (Flaaffy: esce, "!", carica, lotta Lv 23) e
    **dopo 3** (Furret, "vaga": esce e resta).
  - **A vuoto**: contro un albero a 2 caselle (cade a 1,5 e torna) e a
    piena gittata verso nord; `RemoveBagItem` e `AddBagItem` una volta
    ciascuno. Ovest: posa e Ball contro l'albero.
  - **Follower davanti**: nessun record creato.
  - **3 Ball di tipi diversi in aria** (Poké, Great, Ultra; Route 29, 4 OWE
    + follower): tile 606 → 622 su 1024, palette libere 8 → 6. Il tetto di 3
    resta.
  - **Sprite che non si crea** (forzato): niente a schermo, l'OWE sparisce
    all'impatto, il record si libera nello stesso frame, banner "sent to
    the PC". Nessuno script in attesa.
  - **Lotta durante le scosse**: Ball fallita a 2 caselle, poi Pokémon
    mandato (L) contro un secondo OWE a 4: la Ball a terra si chiude prima
    della transizione, il Pokémon dentro riappare; dopo la fuga record
    liberi e Pokémon visibile.
  - **Campo ricostruito**: Ball a vuoto lanciata da R + START, poi warp dal
    menu debug: record liberato, Ball resa.
  - Posa letta in RAM: `animNum` 0x17 (corsa est) dai frame 4 a 19, poi la
    faccia torna da sola.
- **Bug trovati qui e corretti**: (1) l'OWE spariva all'impatto invece che
  sul raggio (nascosto a ogni frame dal primo); (2) la posa non si vedeva
  mai: `PlayerFaceDirection` riscrive l'animazione a ogni frame, e
  `ObjectEventIsMovementOverridden` dà occupata l'azione finita del
  giocatore fermo (serve `ClearHeldMovementIfFinished` prima); (3) arco
  troppo alto (28 px a 3 caselle, ora ~20) e Ball che partiva dalla testa.
- **Non verificato in emulatore**: OWE in acqua (sprite da nuoto nascosto
  dal flag, scritto non provato), Ball in volo che attraversa una lotta e
  atterra dopo (ricreata all'impatto: scritto, non provato), notte in
  grotta buia con Flash. Il "?" del Pokémon liberato durante la transizione
  di una lotta è voluto (scongelato subito; tenerlo congelato rischiava di
  lasciarlo fermo dopo la lotta).
- ROM: 31 911 700 B, 95,10% (+3 360 B sulla fase 3), dopo le due revisioni.

- **Dopo `/pr-review` (17/11, PR #61)**: 7 punti dalla revisione generica,
  verificati sul codice. Corretti 6: (1) **una seconda Ball in volo verso un
  Pokémon già nella prima lo catturava di nuovo** (l'OWE ora resta fino alla
  chiusura, nella fase 3 spariva all'impatto): diventa un lancio a vuoto; (2)
  nei 12 frame fra impatto e raggio lo stesso Pokémon era ancora un bersaglio
  (anche per il Pokémon lanciato, che avrebbe aperto una lotta con un
  Pokémon già dato); (3) lo spawn poteva ricambiare il Pokémon nella Ball con
  uno della stessa specie, preso per lui: flag "non sparire" finché è dentro;
  (4) `CreateBattleStartTask_Debug` senza l'aggancio; (5) la guardia della
  coda contava solo le Ball in volo, ora vuole anche un record libero; (6)
  un lancio a vuoto senza sprite bloccava Start ~1,3 s per niente. Scartato:
  `GiveCaughtMon` che fallisce dopo un tiro riuscito (non raggiungibile:
  `GetCatchBlock` all'impatto controlla i box pieni prima). Provati (1) e
  (3): due Ball allo stesso Flaaffy, `CreateMonFromOWE` 1 volta,
  `AddBagItem` 1 volta (la seconda resa), nessuna cattura in coda; byte del
  livello del Farfetch'd `0x96` nella Ball e `0x16` dopo l'uscita.

- **Seconda `/pr-review` (17/11, sul commit delle correzioni)**: 9 punti.
  Corretti: (1) dopo una cattura, una Ball ancora in volo verso lo stesso
  slot poteva agganciare un Pokémon della stessa specie appena nato lì (con
  un'Esca il conto alla rovescia dello spawn è 0): diventa un lancio a vuoto
  alla rimozione; (2) la Ball attraversava un Pokémon già colpito ma ancora
  visibile (12 frame prima del raggio): ora lo colpisce come un muro e cade;
  (3) tolta la guardia "record libero" aggiunta prima, che nascondeva il
  messaggio di blocco (il ciclo sui record basta e non perde Ball);
  prototipo doppio; CHANGELOG e `overworld_encounters.md` (il flag "non
  sparire" vale anche per la Ball). Scartati: flag rimasto acceso se
  `GetThrowTarget` smette di riconoscere un oggetto vivo (non succede: NULL
  vuol dire sparito o sostituito); suono e posa di un lancio a vuoto senza
  sprite (stato degenere, il suono conferma il tasto). Provato (2): seconda
  Ball nella finestra, `CreateMonFromOWE` 1, `AddBagItem` 1, 1 cattura.

**Handoff per la fase 5 (Sonnet 5.5)**
- Il pannello di D18 può restare a schermo mentre una Ball è fuori (il
  giocatore è libero); le Ball hanno la priorità OAM del giocatore e
  sottopriorità 1, da tenere presente se il pannello usa sprite.
- `OverworldCatch_IsBallOut()` è il "c'è un lancio in corso" giusto per il
  pannello; `sCatch.monMode` per la modalità.
- R + START del debug ora lancia anche una Ball a vuoto (la combinazione
  preme R prima di Start): innocuo, la Ball torna; se dà fastidio si
  esclude R tenuto con Start.
- Suoni già presenti: `SE_BALL_THROW` lancio, `SE_BALL_TRADE` apertura,
  `SE_BALL_BOUNCE_1` caduta, `SE_BALL` scossa, `SE_RG_BALL_CLICK` presa,
  `SE_BALL_OPEN` uscita. La fase 5 li rivede solo se l'utente li vuole
  diversi.
- Punti per la fase 6 (bilanciamento): la Ball insegue un bersaglio che
  fugge (come nella fase 3, dove bastava che esistesse ancora): più
  generoso di Leggende Arceus.

### 5.5 Fase 5, com'è stata fatta (17/11)

- **Modello**: Sonnet 5.5, come assegnato.
- **Cosa**: `UpdatePanel`/`DrawPanel`/`HidePanel` in `src/overworld_catch.c`
  (regola `.claude/rules/overworld_catch.md`, "Pannello"); CHANGELOG
  (PLUS.12), README (riga nuova in "Written from scratch"), report SG
  (feature di questo fork, non un port). Indice in CLAUDE.md §7: già c'era.
  - **Scelta di progetto**: i mock del 16/11 **non sono nel repo**, quindi il
    pannello è stato disegnato qui: finestra di testo 12x3 a (2,16), riga 1 il
    nome (Ball o soprannome), riga 2 "Ball ×N" o "Lv. N"; "No Balls" e
    "Nobody can fight" quando non c'è niente da scegliere. **Solo testo, senza
    icona**: niente sprite, quindi niente palette OBJ né priorità da
    spartire con le Ball in volo. Se l'utente vuole l'icona della Ball o del
    Pokémon (come nel banner) è un'aggiunta separata.
  - Su con L tenuto o per 120 frame dopo cambio, scorrimento o lancio (anche a
    vuoto); giù con controlli bloccati (script, messaggio, menu Start) e con
    `L=A`. Il rilascio di L senza scorrere cambia modalità (D1), quindi in
    prova serve scorrere prima di rilasciare.
- **Verificato** in mGBA + GDB con una ROM di prova usa-e-getta (un `Cheat()`
  in `OverworldCatch_Update` pilotato da una variabile scritta da GDB: dà
  flag, 6 Pokémon e 3 tipi di Ball, poi toglie Ball o Pokémon; non nel git),
  partita nuova salvata in gioco, Route 29:
  - Ball con 3 tipi (Poké ×12, Great ×5), con 1 tipo (lo scorrimento non
    cambia), con 0 (`Ball` / `No Balls`); Pokémon con 6 (Rattata Lv 25,
    Crabominable Lv 55 da 13 lettere, Pikachu Lv 30), con 1, con tutti a 0 PS
    (`Pokémon` / `Nobody can fight`).
  - **Tema scuro**: cornice e fondo scuri, testo chiaro (guardato a 4×). Vale
    **dopo** il ricaricamento delle pal 14/15: acceso a mappa aperta, il
    pannello restava chiaro finché non è passato un messaggio/menu.
  - Sparisce da solo (visibile a 1 s, assente a 3,5 s), si nasconde aprendo
    il menu Start, ricompare dopo un lancio a vuoto.
  - **Bug trovato qui**: la prima versione ridisegnava con `PIXEL_FILL(0)`
    dopo `SetStandardWindowBorderStyle` (che riempie con 1): il fondo era
    trasparente e il testo aveva uno sfondo bianco a strisce. Ora `PIXEL_FILL(1)`.
- **Non verificato**: il pannello con una Ball in volo (stessa finestra BG0, nessuno
  sprite: non dovrebbe interagire); `docs_validate` in CI (in locale lo script
  non stampa errori); testi riletti contro il codice e non con un giro in grotta
  buia. Suoni non toccati (l'handoff diceva di rivederli solo su richiesta).
- ROM: 31 913 684 B, 95,11% (+1 984 B sulla fase 4).
- **Preesistente, non toccato**: la riga "Overworld speed-up" del README
  chiude con "R throws a Ball at wild Pokémon on the map", fuori posto (non è
  una nota dell'`OW SPEED`).

**Handoff per la fase 6 (Opus 5.5)**
- **Serve il `.srm` dell'utente** (cancello della fase).
- Il pannello non è stato provato con un salvataggio vero, ma non scrive nel
  salvataggio e non ha stato fuori da `sCatch`.
- Per le prove al pannello e al lancio senza passare dai menu: una ROM di prova
  con `Cheat()` come sopra e un'ora di gioco; la partita nuova si salva in gioco
  (Start → Salva) per non rifare l'intro a ogni build.
- Punti già in lista per la 6: bilanciamento (Ball per una cattura a inizio
  e a metà gioco), la Ball che insegue un bersaglio che fugge, la scelta DexNav
  (D6), e il testo "Lv. N" del pannello (FONT_SMALL non ha il glifo `{LV}`: non
  provato, si è usato "Lv. ").

### 5.6 Fase 6, com'è stata fatta (18/11)

- **Modello**: Opus 5.5, come assegnato. Salvataggio dell'utente (PLUS.11,
  Route 43, 6 medaglie, `EASY CATCH` acceso).
- **`/code-review` medio** sul diff `d6520855..HEAD` (fasi 1-5, senza il
  commit HASEPSYM): 10 punti, verificati sul codice e dove serviva in
  emulatore. Corretti 7:
  1. **Pannello sopra la finestra DexNav** (stessa base 8 e stessa riga):
     riprodotto sulla build di prima (L tenuto durante una ricerca:
     "FURRET" sparisce, "Lv. 21" rovinato, poi un buco nella finestra);
     ora il pannello è spento con `DN_FLAG_SEARCHING`. Provato dopo: finestra
     intatta.
  2. **Ordine nel frame**: il pannello si nascondeva a fine CB2, dopo script
     e task. Il caso segnalato (banner coperto) **non si riproduce** (`lock`
     aspetta un frame prima del banner: provato sulla build di prima, banner
     pulito); resta vero per ciò che uno script disegna nel frame del blocco
     (un cartello: per lettura, non visto). Ora `OverworldCatch_UpdatePanel`
     gira prima di `ScriptContext_RunScript`.
  3. **Il lancio ignorava i dislivelli** (contro P2): ora un dislivello ferma
     la Ball e un Pokémon a un'altra altezza è saltato. Provato con un trucco
     (altezza di OWE e giocatore scritte): Ball sopra il Pokémon, resa; Ball
     caduta subito (~0,25 s contro >1,2) con il giocatore "più in alto".
  4. Il bit di allerta forzata (Ball fallita) saltava anche `CanOWEReachPlayer`.
  5. Icona del banner: se la prima non si creava, la seconda (grotta buia)
     restava a schermo; palette ora liberata con `FieldEffectFreePaletteIfUnused`
     (fa anche `ResetPaletteColorMapType`).
  6. `gOverworldThrowLeadSlot` azzerato a ogni frame a controlli liberi
     (difensivo: lo script della lotta tiene i controlli fino alla lotta, il
     caso non è raggiungibile oggi).
  7. Commento della Friend Ball che parlava della Heal Ball.
  Scartati: `TryRemoveOWEAboveCap` "senza animazione" (falso:
  `RemoveObjectEvent` → `OnOverworldWildEncounterDespawn` la fa, e un
  Pokémon nella Ball ha il flag "non sparire"); Start ignorato in silenzio
  con una Ball fuori (voluto, al massimo ~4 s; il salvataggio del menu
  debug resta, è solo debug).
- **Decisioni dell'utente (18/11)**: D6, il DexNav resta. **La Ball non
  insegue più** chi si sposta: va dove il Pokémon era al lancio e lo colpisce
  se all'arrivo è a meno di mezza casella (`IsOWEAtLanding`). Provato: OWE a
  3 caselle che parte di lato al lancio → Ball resa, nessuna cattura; OWE
  fermo a 2 → catturato.
- **Bilanciamento** (formula vera, PS pieni, probabilità per Ball e lanci
  medi; ×2 = non notato). A inizio gioco (Lv 3-7, 0-1 medaglie) quasi sempre
  al primo colpo grazie al bonus dei livelli bassi. A metà (6 medaglie, Lv
  21-22): Furret 12% / 28% (8,1 / 3,6 lanci), Flaaffy 17% / 34%, Girafarig
  8% / 17%, Pinsir 6% / 12% con la Poké Ball; Ultra Ball: Furret 28% / 50%,
  Pinsir 12% / 28%. Le Ball costano 10/15/30: l'utente ha scelto di non
  ritoccare i numeri.
- **Punti aperti delle fasi 4-5**: "Lv. N" del pannello letto bene in
  FONT_SMALL (squadra vera: Vileplume, Quilava, Scizor Lv 38); pannello
  visibile con una Ball a terra; `docs_validate` verde in locale. **Non
  provati**: OWE in acqua, Ball in volo che attraversa una lotta, grotta buia
  con Flash (codice non toccato in questa fase).
- **Verificato** in mGBA + GDB col `.sav` dell'utente, due ROM di prova (build
  di prima e corretta, con un `Cheat()` che mette un OWE fermo davanti, ne
  cambia l'altezza o lo fa spostare al lancio; non nel git).
- ROM consegnata: 31 914 068 B, **95,11%** (+384 B sulla fase 5, dopo la `/pr-review`).

- **`/pr-review` (18/11, PR #65, sul commit `c5b818d2`)**: 9 punti dalla
  revisione generica. Corretti 6: (1) **i dislivelli fermavano ogni lancio
  fra riva e acqua** (acqua 1, riva 3 nelle mappe di HnS, p. es. Lake of Rage
  e Route 43): introdotto in questa fase, ora acqua e terreno sono un livello
  solo (`GetThrowLevel`), anche per l'oggetto saltato; (2) colpito/mancato
  per **casella** (`currentCoords` alla caduta = casella del lancio) invece
  che per pixel: un Pokémon a metà passo al lancio era mancato finendo il
  passo, uno che salta sul posto (gli OWE che si avvicinano) era mancato; (3)
  `GetObjectGroundPos` senza `y2`; (4) il bit dell'allerta forzata si consuma
  al primo controllo (prima aspettava il giocatore e scattava da lontano); (5)
  CHANGELOG: le sporgenze non fermano la Ball. Opzionale, non fatto: palette
  dell'icona del banner senza nessuno sprite creato (tabella degli sprite
  piena: non raggiunto). Scartati con il percorso rifatto: lo slot del Pokémon
  lanciato azzerato troppo presto (lo script tiene i controlli fino alla
  lotta) e la finestra DexNav aperta nello stesso frame del pannello (ogni
  apertura passa da un blocco dei controlli nel CB1).
  Provato in mGBA col `.sav` dell'utente su Route 43, riva in (10,12): Ball
  dalla riva a un Pokémon in acqua → presa (Flaaffy); un OWE che parte di
  lato al lancio → mancato, Ball resa; fermo → preso; in Surf, Pokémon
  mandato (Quilava, scelto con L) contro uno in acqua → lotta con Quilava in
  campo; lotta da contatto subito dopo → Vileplume in campo (primo della
  squadra). Il caso che falliva prima della correzione non è stato rifatto in
  emulatore: lo mostrano i dati della mappa (riva 3, acqua 1) contro il
  vecchio `IsElevationMismatchAt(3, casella 1)` = vero.

- **`/pr-review` della PR #66 (20/11, su `803c87b1`)**: 7 punti. Corretti 2:
  il bit dell'allerta forzata usava `CanOWEReachPlayer` (acqua ≠ riva), quindi
  un Pokémon in acqua colpito dalla riva non notava mai il giocatore; ora usa
  il livello del lancio (`OverworldCatch_AreLevelsApart`) e si consuma solo su
  un altro livello. Regola: rimessi follower, linea vuota e acqua senza
  collisione. Scartati: bersaglio oltre un confine di mappa (l'OWE della mappa
  vecchia viene tolto comunque), ponte ad altezza 3 sull'acqua (sotto un ponte
  si passa solo con caselle 15), controllo del bit fuori da `Step3` (solo con
  `WE_OWE_UNRESTRICT_SIGHT`, FALSE). Lasciati come scelta: il colpo per casella
  nel fotogramma in cui il Pokémon parte, il `y2` del Surf per la mano.
  **Scoperto in emulatore**: un OWE dell'altra zona (in acqua col giocatore a
  terra) viene ricambiato in ~30 frame dopo l'uscita dalla Ball
  (`IsAnyOWEFromOtherArea`, fase 1): i lanci fra riva e acqua toccano solo
  Pokémon rimasti dalla zona di prima. La correzione dell'allerta non si è
  potuta vedere per questo (il Pokémon sparisce prima di guardare); letta sul
  codice. Il caso "presa dalla riva" del 18/11 usava un OWE di terra messo in
  acqua dal trucco.

**Handoff per la fase 7 (Sonnet 5.5)**
- La fase 7 è un port da Soulgold (ricorda mosse a pagamento): non tocca
  `overworld_catch.c`. Leggere `relearner.md` e il §3 per i maestri.
- Il salvataggio dell'utente (PLUS.11) è quello della fase 6: Route 43, 6
  medaglie; serve alla 7 per le mosse uovo vere.
- Ancora non provati in emulatore (da fare se la 9 o la 15 passano di lì):
  Ball in volo durante l'inizio di una lotta,
  grotta buia con Flash, un cartello letto entro 2 s da un lancio.

### 5.7 Fase 7, com'è stata fatta (20/11)

- **Modello**: Sonnet 5.5, come assegnato. Salvataggio dell'utente (PLUS.11).
- **Cosa**: `P_ENABLE_MOVE_RELEARNERS` resta TRUE come in SG (con FALSE il tag "TM START" usciva dallo schermo), egg/tutor chiusi dai due flag in codice, `P_TM_MOVES_RELEARNER` TRUE
  (P16), `P_FLAG_EGG_MOVES`/`P_FLAG_TUTOR_MOVES` agganciati a due flag nuovi
  (`FLAG_EGG_MOVES_UNLOCKED` 0x304, `FLAG_TUTOR_MOVES_UNLOCKED` 0x305, ex
  `FLAG_UNUSED_36/37` di `flags_hns.h`). Due NPC nuovi: Egg Move Master in
  `BlackthornCity_House3_hns` (2,6; ¥88.888) e Tutor Move Master in
  `OlivineCity_House3_hns` (7,4; ¥44.444). Pagano una volta e alzano il flag;
  il resto lo fa il riassunto. Dettagli e trappola della guardia
  `#ifdef POKEMON_HNS` in `relearner.md`.
- **Maestro dei tutor**: la House4 di Olivine di SG qui non esiste; scelta la
  House3 (la stanza col pescatore, stessa mappa di Blackthorn). Da confermare
  con l'utente se preferisce un altro posto.
- **Sorgente di SG**: letto a fine fase (`310bf6a9`, dopo che l'utente ha
  chiesto `add_repo`). Corretto il primo giro: SG tiene
  `P_ENABLE_MOVE_RELEARNERS` TRUE e chiude egg/tutor nel codice. Con FALSE il
  tag "TM START" usciva dallo schermo (`tilemapLeft` 22): visto solo con lo
  stato TM aperto, non con LEVEL. Ora come SG. Prezzi e testi dei maestri
  sono quelli di SG (stessi ¥ e stesso senso); posizioni e sprite no (SG ha
  House4 a Olivine).
- **Verificato** in mGBA col `.sav` dell'utente (Quilava Lv 38, MT in borsa):
  prima del pagamento il ciclo L/R del tag nella pagina mosse è LEVEL↔TM
  (uovo e tutor assenti); soldi insufficienti (¥23.638) = messaggio, nessun
  flag; con i soldi (debug Max Money) il prezzo esatto viene tolto (9.999.999 →
  9.911.111 e → 9.955.555), il secondo colloquio dice "già pagato", e il ciclo
  diventa LEVEL→EGG→TM (uovo: Fury Swipes, Quick Attack, Reversal, Thrash,
  Foresight, Covet) e poi LEVEL→TUTOR→TM. Il `.sav` non è stato modificato
  (cmp). Un salvataggio vecchio parte chiuso: è la prova stessa (flag a 0).
- **Prezzi e script (utente, 20/11, dopo la review)**: script e testi dei due
  maestri ora quelli di SG (dissolvenza, fanfara, istruzioni; `waitfanfare` al
  posto del `waitse` di SG dopo la fanfara). Prezzi metà di SG, 44.444 / 22.222,
  e metà ancora da Campione di Johto (`FLAG_IS_CHAMPION`): 22.222 / 11.111.
  Provato in mGBA col `.sav`: senza titolo chiede ¥44,444 e con ¥23.638 dice "A
  pity."; "No" → "I see."; con `FLAG_IS_CHAMPION` e Max Money chiede ¥22,222 e
  toglie esattamente quello (9.999.999 → 9.977.777), sequenza intera e, riparlando,
  le istruzioni. Il maestro dei tutor è lo stesso modello con altre costanti:
  non riprovato in emulatore dopo questa modifica.
- **Non fatto**: "testo nel riassunto quando la voce è chiusa" (tabella): in
  SG non l'ho potuto vedere e senza fonte non l'ho inventato; il tag mostra
  solo le voci aperte. Da decidere con l'utente. Non provato: il pagamento del
  Tutor con un Pokémon che impara davvero una mossa tutor (solo il ciclo).
- ROM: 31 915 444 B, **95,12%** (+1 376 B sulla fase 6).

**Handoff per la fase 8 (Opus 5.5)**
- `FLAG_UNUSED_38/39` (0x306/7) restano liberi in `flags_hns.h`; la fase 8 non
  ne ha bisogno (bit di `ChallengeSettings`, `save.md`).
- Il passo `saveVersion < 15` della fase 8 può azzerare anche i flag 0x304/0x305
  (`FLAG_EGG/TUTOR_MOVES_UNLOCKED`, ex `FLAG_UNUSED_36/37`): oggi leggono 0 solo
  nel `.sav` provato, e nulla di PLUS.12 è uscito con loro (review della PR #69).
- CHANGELOG: voce nuova sotto PLUS.12 "Changed" per il ricorda mosse.

### 5.8 Fase 8, com'è stata fatta (20/11)

- **Modello**: Opus 5.5, come assegnato. `.sav` dell'utente (quello delle fasi
  6-7).
- **Opzioni** (`src/option_menu.c`) in 4 schede dal documento del riordino:
  GENERAL 11 (con DARK UI, CLOCK TYPE e ALT SHINY), OVERWORLD 10 (con
  OVERWORLD CATCH), BATTLE 11 (BATTLE SCENE e BATTLE STYLE dalla MAIN, ARCEUS
  FORMULAS), SOUND 6. Nomi delle schede in inglese come il resto del gioco
  (GENERAL, OVERWORLD, BATTLE, SOUND al posto di GENERALE, MAPPA, LOTTA,
  SUONO). Testi di OVERWORLD CATCH e ARCEUS FORMULAS scritti per quello che
  faranno dalle fasi 9-11.
- **Sfide** (`src/challenge_menu.c`) in 5 schede: FEATURES dentro MODE (16
  righe: SHINY CHANCE, ITEM DROP, FRONTIER BANS in fondo, sempre attive anche
  con RECOMMENDED, che blocca solo le voci del preset); DIFFICULTY 14 con EASY
  CATCH e LEVEL SCALING (libere a metà partita, come erano in Opzioni).
  `STATIC_ASSERT` sulle righe di ogni scheda nei due menu.
- **Salvataggio 15** (`save.md`, cronaca in `save-history.md`):
  `noOverworldCatch` sul bit 31.7 (invertito), `struct ExtraOptions` a SB3
  +96 con `arceusFormulas` e 7 bit liberi già azzerati; il passo azzera anche
  i flag 0x304/0x305 (handoff della fase 7). Accessori
  `IsOverworldCatchEnabled()` e `AreArceusFormulasEnabled()` in
  `option_menu.h`, ancora letti da nessuno.
- **Titolo**: `oak_speech_hns.c` ricopia anche CLOCK TYPE, ALT SHINY e
  OVERWORLD CATCH (ora voci delle Opzioni); EASY CATCH esce dalla lista e
  LEVEL SCALING viene azzerata: da sfide, ripartono dal default come le altre.
  `NewGameInitData` conserva `extraOptions`. Scelta mia, scritta nel
  CHANGELOG: si cambia in due righe se l'utente preferisce il contrario.
- **Verificato** in mGBA:
  - `.sav` vero: è a **`saveVersion` 8** (PLUS.2-5, non PLUS.11): il bit 31.7
    a 1 era TRAINER CATCH. Caricato e salvato: byte 31 `0x9d` → `0x1d`, byte
    0-30 di `ChallengeSettings`, `soundAndFieldOptions` (`0x01`) ed
    `extraOptions` (0) uguali.
  - **Passo 15 da solo**, il caso che fallisce senza: copia portata a
    versione 14 (checksum ricalcolato) con `extraOptions` `ff ff ff ff` e flag
    0x304/0x305 a 1 → OVERWORLD CATCH ON, ARCEUS FORMULAS OFF a schermo; dopo il
    salvataggio tutto a zero e il resto uguale anche dopo un giro nelle
    Opzioni chiuse con B. Ricaricato: CONTINUE mostra lo slot nuovo (checksum
    buono).
  - Sfide a metà partita (PC dal debug): EASY CATCH OFF → byte 31 bit 0x08,
    LEVEL SCALING ON → `soundAndFieldOptions` bit 0x40; TRAINER EVs e PLAYER
    EVs rifiutano ancora di salire.
  - **New Game** dal titolo con CLOCK TYPE FAKE RTC, ALT SHINY OFF, OVERWORLD
    CATCH OFF, ARCEUS FORMULAS ON (il salvataggio di prima aveva LEVEL SCALING
    ON): Sfide iniziali con EASY CATCH e LEVEL SCALING OFF; salvato in camera:
    bit 31.7 = 1, `extraOptions` = 1, byte 29 = `0x0b` (FAKE RTC sì, ALT SHINY
    no), `soundAndFieldOptions` `0x01`, versione 15; l'orologio finto corre.
  - Ogni scheda guardata (18 screenshot) col tema scuro del `.sav` e, per
    Opzioni e Sfide, col tema chiaro: i due menu non leggono il tema e hanno
    la loro palette. Nessuna scheda oltre 16 righe; le etichette più lunghe
    (OVERWORLD CATCH, ARCEUS FORMULAS) lasciano 8-9 px prima della scelta come
    ENHANCED DEXNAV.
- ROM: 31 916 340 B, **95,12%** (+896 B sulla fase 7).

**Handoff per la fase 9 (Opus 5.5)**
- Leggere OVERWORLD CATCH solo con `IsOverworldCatchEnabled()`; la voce è
  `ITEM_OVERWORLD_CATCH` in `option_menu.c`. Il cambio a mappa aperta passa
  da `Task_Save` delle Opzioni: è lì (o al ritorno sul campo) che vanno tolti
  gli OWE di troppo e chiusi lancio e pannello.
- Il `.sav` dell'utente è a versione 8: caricarlo esegue i passi 9-15 (il 9
  azzera var e flag del TcloneR). Per provare un salvataggio "di oggi" usa
  quello salvato da una build ≥ 15, o porta la versione a mano (`save.md`,
  "Provare una migrazione").
- `ExtraOptions.unused` ha 7 bit azzerati dal passo 15: la prossima opzione
  va lì senza passo nuovo. `ChallengeSettings` è pieno.
- Il menu debug "PC/Bag → Access PC" apre il PC ovunque: è la via rapida per
  le Sfide a metà partita.

### 5.9 Fase 9, com'è stata fatta (20/11)

- **Modello**: Opus 5.5, come assegnato. Senza il `.sav` dell'utente: partita
  nuova, debug (Pokédex, scarpe, Bulbasaur Lv 21) e scritture in RAM.
- **Interruttore** (D20; trappole in `overworld_catch.md`): spenta = gioco di
  prima del piano salvo prezzi e negozi. Tetti 6/3/3 e Safari 4
  (`OWE_SPAWNS_MAX_*_NO_CATCH`), ricambio 240; percezione di prima rimessa
  com'era (`CanAwareOWESeePlayer_NoCatch`: rettangolo 4x3, solo corsa e bici
  sentite); B con AUTORUN acceso torna camminata, ma **nella ricerca DexNav B
  resta il passo lento** (A, il tasto di prima, con AUTORUN acceso non è mai
  andato: scelta mia, da confermare); R = ricerca DexNav; L e pannello spenti.
  Testo della voce corretto: "B runs" valeva solo con AUTORUN spento, ora
  "there is no sneaking".
- **Ball nei Mart** (utente, 20/11, nella lista per medaglie di tutti i Mart,
  anche quelle `_PC`): Heal 50, Nest e Repeat 100 da 0 medaglie; Net, Dive,
  Dusk, Timer, Luxury 100 da 1; le 7 di Kurt **500** (erano 1000) da 2. In
  vendita anche a cattura spenta. Dettagli in `field.md`.
- **Verificato** in mGBA (ROM `ee6eff3a` più il testo della voce):
  - Spawn misurati in frame: Route 29 accesa 4021 frame **massimo 4**; spenta
    a mappa aperta 4022 frame **fino a 6**; riaccesa: **4 al primo campione**
    (684 frame). Route 30, cambiata dal menu Opzioni: spenta fino a 6 in 3069
    frame, riaccesa 4 dal primo campione.
  - Andature a Cherrygrove (caselle in ~92 frame con il tasto tenuto): spenta
    con AUTORUN acceso direzione 7 (corsa), **B 4 (camminata, niente
    creeping)**; accesa B 2 (creeping); AUTORUN spento: B corre in tutti e due
    i casi (8 e 7). Spenta, durante una ricerca DexNav: B 2 caselle, creeping.
  - L tenuto + direzione: spenta il giocatore cammina (3 caselle) e nessun
    pannello; accesa resta fermo con "HEAL BALL Ball ×11".
  - R spenta: su Route 30 parte la ricerca DexNav di Hoothoot (registrato in
    `VAR_DEXNAV_SPECIES`; finestra in basso, OWE tolti, flag di ricerca a 1);
    su un sentiero senza erba il messaggio del DexNav "couldn't be found".
  - Accesa, le prove della fase 6 che non chiedono l'acqua: **mancato** (×11
    → ×10 in volo → ×11), **fermo** (OWE messo a 2 caselle: Hoothoot ♀ preso
    con la Heal Ball, ×10, banner e soprannome), **Pokémon mandato** (lotta
    con Spinarak). **Riva** non rifatta: il codice del lancio non è cambiato
    salvo le guardie in entrata.
  - **Punto aperto della fase 6 chiuso**: una lotta per contatto partita con
    una Ball in volo (OWE spostato sul giocatore dal trucco): dopo la fuga la
    Ball è tornata (×11).
  - Mart di Violet: 0, 1 e 2 medaglie guardate (Ball nell'ordine giusto,
    prezzi 50/100/500, extra di Violet in coda); 11 Heal Ball pagate ₽550.
  - Testi della voce nei due valori guardati: stanno nella finestra.
- **Non verificato**: la percezione spenta in emulatore (è il codice di prima
  del piano, confrontato col diff di `d6520855`); le liste `_PC` (stesso
  codice, lette); il Safari a cattura spenta.
- ROM: 31 916 836 B, **95,12%** (+496 B sulla fase 8).

**Richieste dell'utente dopo la fase 9 (20/11, stessa sessione)**
- Decisioni (utente): **la Ball si consuma solo a cattura riuscita** (non
  più all'impatto: sostituisce D16 per la Ball che colpisce e il Pokémon
  esce; la perdita a lancio fallito arriverà con una missione futura);
  **banner che non fermano il gioco**, pila di 3 in alto, **il nuovo in cima
  che spinge giù gli altri** (utente, dopo una prima versione col nuovo in
  fondo), ~3 s ciascuno, il più vecchio (in basso) esce per primo o se ne
  arriva un quarto (sostituisce D14 per il blocco);
  **niente soprannome** per le catture col lancio salvo la Nuzlocke
  obbligatoria (sostituisce P18); **niente fanfara**, un suono breve
  (`SE_SUCCESS`, sostituisce P19); **una Ball a terra non si attraversa**;
  icona del banner centrata.
- Fatto in `overworld_catch.c` (trappole in `overworld_catch.md`): la Ball
  esce dalla borsa in `GiveCaughtMon`, al lancio si contano quelle in volo;
  `OverworldCatch_IsBallOnGroundAt` in `GetVanillaCollision`
  (`event_object_movement.c`); banner come finestre BG0 da 92 (fuori dalla
  finestra della ricerca DexNav, 8-91), tolti dal popup del nome mappa che
  condivide i tile e scorre BG0; script dei risultati ridotto ai soprannomi.
- **Verificato** in mGBA (partita nuova, Route 30 di notte, 11 Poké Ball dal
  debug): tre lanci in raffica su Pokémon messi davanti da RAM: ×11 durante
  il volo, ×10 e ×9 alle due catture, il terzo non preso e non scalato; due
  banner impilati (Hoothoot, Spinarak), il primo sparito da solo e il
  secondo salito in cima, poi sparito; il giocatore e i Pokémon si muovono
  intanto. Icona misurata: 5 px sopra e 4 sotto nel primo banner, 7 e 6 nel
  secondo (prima 9 e 0, `BANNER_ICON_Y` 20 → 16). Ball a terra: un record
  `DROPPED` scritto da RAM su (50,15) a Cherrygrove tiene il giocatore su
  (49,15) per 230 frame con Destra tenuta; finito il record, passa.
- **A foglietti** (utente, 21/11: "uno sopra l'altro come dei post-it, non
  una colonna"): tutti nello stesso posto in alto, il nuovo sopra, dei
  vecchi spunta solo il bordo inferiore (8 px più in basso e una casella più
  stretti per lato ciascuno), escono dal fondo ~3 s dopo il loro arrivo.
  Provato (Route 29 nord, tre lanci, ×11 → ×8): Rattata♂, poi Geodude sopra
  col bordo di Rattata sotto, poi Rattata♀ sopra col bordo di Geodude
  mentre Rattata♂ era già uscito. Tre foglietti insieme non visti: le
  catture distavano ~1,2 s; stesso ciclo di `DrawPileEdges`. Trappola: con
  `ScheduleBgCopyTilemapToVram` i bordi non comparivano, serve
  `CopyBgTilemapBufferToVram`.
- **Foglietti con la carta e follower** (utente, 21/11): dei foglietti sotto
  si vedeva solo il bordo (2 px di carta su 5 di cornice: "trasparenti");
  ora ognuno spunta di due righe, una di carta (lati 3/5 e riempimento 4
  della cornice, colore 14 che segue il tema) e il bordo. Tutte le 20
  cornici del gioco hanno il centro pieno (letto dai `.png`). Provato: tre
  foglietti insieme, ognuno con la sua striscia bianca. **Follower sulla
  linea** (sostituisce "verso il follower non parte" di D16): saltato dal
  bersaglio e rimandato nella Ball con la stessa animazione di quando va
  nascosto (`SendFollowerIntoPokeball`); provato: Bulbasaur davanti, R, rientra
  e la Ball parte; al primo passo esce e torna a seguire. Non provato col
  tema scuro.
- **Lotta durante una cattura e bordo bianco col tema scuro** (utente,
  21/11, col suo `.sav`: 6 medaglie, tema scuro, cornice n. 1): (1) una lotta
  che partiva con una Ball a terra scartava il foglietto della cattura; ora
  i foglietti si nascondono (lotta, messaggio, menu, dissolvenza) e tornano
  con il tempo pieno. Provato: Ball su un Girafarig, un altro Girafarig mi
  carica e parte la lotta; fuggito, compare "You caught GIRAFARIG! It was
  sent to the PC." e sparisce da solo. (2) Il "bordo bianco" non era il
  tema: dopo il Continue la palette 14 tiene i colori del titolo (entry 2
  bianca, letta in RAM) finché un messaggio o un menu non carica la
  cornice; pannello e foglietti ora la caricano da sé (non durante una
  ricerca DexNav, che usa la 14). Provato: pannello subito dopo il
  caricamento, uguale al menu Start. Una prima correzione sul tema scuro
  globale (entry chiare della cornice scurite) è stata tolta: non era la causa.
- **`/pr-review` della PR #71 (21/11, su `25fdeeb3`)**: 5 punti dalla
  revisione generica, tutti veri. Corretti: (1) il foglietto saltato quando
  la lotta parte durante le scosse (`FinishThrow` escludeva
  `battleStarting`); provato con un record `SHAKING` scritto in RAM e una
  lotta per contatto: al ritorno "You caught FLAAFFY! It was sent to the
  PC."; (2) una Ball in volo durante una lotta poteva essere usata in lotta
  e poi catturare gratis: ora all'impatto, senza la Ball in borsa, è un
  lancio a vuoto (letto sul codice); (3) a cattura spenta, R con foglietti a
  schermo cancellava la finestra della ricerca DexNav in alto: la ricerca
  toglie i foglietti prima di disegnare (letto); (4) la casella bloccata
  dalla Ball tiene la mappa del lancio; (5) commento della lista più lunga
  dei Mart (39).
- (Versione precedente, colonna col nuovo in cima) Geodude, Geodude,
  Rattata presi (×11 → ×8); Rattata compare in cima e spinge giù il Geodude
  rimasto, che poi sparisce dal basso mentre Rattata resta fermo.
- **Non provati**: la Nuzlocke col soprannome obbligatorio, il popup del
  nome mappa sopra i banner, banner durante una ricerca DexNav, una Ball
  vera (non scritta da RAM) a vuoto calpestata.

**Handoff per la fase 10 (Opus 5.5)**
- `AreArceusFormulasEnabled()` esiste già (`option_menu.h`), letto da
  nessuno; il bit è `ExtraOptions.arceusFormulas` (SB3 +96).
- La cattura di Arceus va in `ComputeOverworldCaptureOdds` solo a cattura
  accesa (spenta non si lancia nulla: niente da gestire).
- Banco di prova: `/tmp` non sopravvive al container, la ricetta sì
  (`emulator.md`, "Un client RAM"). Partita nuova + debug basta per le
  statistiche; il `.srm` serve al cancello (la tabella lo chiede).
- Da chiedere all'utente: il passo lento con B nella ricerca DexNav a cattura
  spenta (tenuto) e il Mart di Cherrygrove, che fino al rifornimento della
  storia non vende Ball.

### 5.10 Fase 10, com'è stata fatta (21/11)

- **Modello**: Opus 5.5, come assegnato. Col `.sav` dell'utente (PLUS.11, 6
  medaglie, Route 43, squadra al Lv 38 con IV 31).
- **Statistiche** (`CalculateMonStats`, dettagli e trappole in
  `.claude/rules/arceus.md`): formula di PKHeX, termini base in `float` a 32
  bit come il gioco (con interi esatti escono +1 in 19 PS e 89 altre
  statistiche su 25 500 coppie base × livello),
  bonus d'impegno intero. **Impegno da IV ed EV**: base dagli IV (≥ 20 → 1,
  26 → 2, 31 → 3) + EV, max 10; prima `EV × 10 / 252`, poi `× 7` (utente,
  sotto).
  `RecalculatePartyStats()` al cambio (menu Opzioni, anche dal titolo) e al
  caricamento, PS in proporzione arrotondati al più vicino.
- **Cattura** (`ComputeArceusCaptureOddsFromParams`): formula di Bulbapedia
  con le scelte del 20/11 (tasso 135, leggendari col loro, rango da medaglie
  di Johto + Campione, Ball del documento chiuso). **Heavy Ball**: il bonus
  fisso del peso va sul tasso, come oggi in lotta (scelta mia). Colpo alle
  spalle = il Pokémon guarda dove andava la Ball (`throw->direction`).
- **Verificato** in mGBA con una ROM strumentata (banco di prova in
  `OverworldCatch_Update`, tolto prima del commit) e un riferimento Python
  indipendente (PKHeX in `float32`):
  - **3 Pokémon Lv 5, 25, 50** (Bulbasaur Adamant, Sentret Timid, Onix
    Hardy, IV ed EV misti, IV 19/20/21/25/26/30/31 tutti presenti): 36
    statistiche su 36 uguali al riferimento, accesa e spenta.
  - **Squadra vera**: accesa dal menu Opzioni, 36 su 36 uguali; ferito 50/104
    → 70/145 → di nuovo 50/104, avvelenato resta avvelenato, esausto resta a
    0, pieni restano pieni. Spenta, le statistiche tornano identiche a
    quelle del `.sav`.
  - **Cattura, 6 casi** (6 medaglie, EASY CATCH spento in RAM: nel `.sav` è
    acceso): Furret Lv 21 Poké Ball 116; Pinsir Lv 22 Ultra, allertato,
    alle spalle 140; Pinsir Lv 95 Net Ball (rango −3) 63; Suicune Lv 40 Ultra
    (tasso 3) 47; Furret Lv 100 allertato 18; Master Ball garantita. Tutti
    uguali al calcolo a mano fatto prima. **Lancio vero**: Furret Lv 21 messo
    a 2 caselle, girato come il giocatore, Poké Ball: 127 (calcolo 127,5).
  - **Caricamento**: spenta, la squadra dopo `LoadGameSave` è identica alla
    copia in SaveBlock1; salvato con l'opzione accesa e riavviato
    l'emulatore, la squadra torna identica (statistiche, PS, stato).
- **Non verificato**: una lotta con l'opzione accesa (fase 11, col danno);
  il riassunto a schermo; un Pokémon ritirato dal PC (stesso
  `CalculateMonStats` di sempre via `BoxMonToMon`).
- ROM: 31 919 588 B, **95,13%** (+2 752 B sulla fase 9, tutto compreso).

- **`/pr-review` della PR #72 (21/11, su `c0de0281`)**: 4 punti dalla
  revisione generica, 3 veri e corretti. (1) I Pokémon nel PC e nella
  Pensione tengono solo i PS persi (il deposito non cura): spenta l'opzione,
  al ritiro `max - persi` andava in underflow (65 000 PS). Ora al cambio
  vengono riscalati come la squadra, e `BoxMonToMon` li limita comunque.
  Trovato rileggendo: anche un esausto della squadra teneva i PS persi
  vecchi. Provato con un banco di prova: Pidgey nel box 40/70 → 19/33 →
  40/70, l'esausto della squadra 140 → 101 PS persi su 101, tutto il PC
  riscalato in 1 frame. (2) Il rango contava anche le 8 medaglie di Kanto
  (`NUM_BADGES` 16 in HnS): ora solo Johto. (3) Al caricamento con uno slot
  rotto (`SAVE_STATUS_ERROR`, lo slot buono si carica) la squadra non veniva
  ricalcolata. (4) Colpo alle spalle anche su un Pokémon allertato che
  fugge: scelta di regola, chiesta all'utente.
- Già nel codice di prima (non toccato): `ComputeCaptureOddsFromParams`
  conta anch'esso 16 medaglie e poi le limita a 8 (`NUM_BADGES_CAPPED`) per
  il malus delle medaglie mancanti: senza effetto, perché in HnS le medaglie
  di Kanto arrivano dopo le 8 di Johto.

- **Bug grafici segnalati dall'utente (21/11, con screenshot)**, riprodotti
  in mGBA e corretti:
  - **Menu Start e menu debug con dei buchi** dove c'erano il pannello (in
    basso a sinistra) e il foglietto (in alto): i due menu si aprono nel CB1
    e disegnano subito, la pulizia di pannello e foglietti nel CB2 veniva
    dopo e li bucava. Ora `OverworldCatch_ClearScreenForMenu()` li toglie
    prima. Provato: Start con il foglietto di Raticate a schermo (menu
    intero, il foglietto torna alla chiusura); R+START durante le scosse con
    il pannello su (menu debug intero; prima, stessa sequenza, bucato).
  - **"Pietra" blu impassabile**: non era la Ball. *Access PC* del debug
    accende e spegne lo schermo del PC sulla casella che il giocatore
    guarda, e HnS aveva tolto il controllo "davanti a un PC": guardando a
    sud è la casella del giocatore (rossa durante il PC, blu e solida dopo).
    Ora la metatile cambia solo se lì c'è un PC. Provato: dal debug su Route
    43 nessun quadrato e la casella si attraversa; il PC vero del Centro di
    Violet va ancora da 0x062 a 0x063 e torna 0x062.
  - Visto di passaggio: R+START (il debug) lancia anche una Ball, perché R
    è il tasto del lancio. Lasciato così, scritto nella regola.

- **Confermato dall'utente (21/11)**: il colpo alle spalle vale anche su un
  Pokémon allertato che fugge (conta solo la direzione); la Heavy Ball somma
  il bonus del peso al tasso. **EV → impegno: `EV × 7 / 252`** (al posto di
  ×10): il massimo vuole IV 31 e 252 EV, così IV ed EV contano tutti e due
  (con 510 EV se ne massimizzano comunque solo due). Nessun hack trovato che
  l'abbia già fatto; HOME non converte, tiene i livelli d'impegno a parte
  (PKHeX `PKM/HOME/GameDataPA8.cs`). Provato con un banco di prova (tolto):
  i 3 Pokémon sintetici 36 su 36 uguali al riferimento (Attacco del
  Bulbasaur IV 20 + 252 EV 110 → 82, PS del Sentret IV 15 + 252 EV
  137 → 111); la squadra dell'utente (IV 31) non cambia: 3 + 7 = 10 come prima.

**Handoff per la fase 11 (Opus 5.5)**
- Le statistiche di Arceus a inizio gioco sono **molto** più alte (Bulbasaur
  Lv 5 con 252 EV in Attacco: 110 contro 15): finché non c'è il danno di
  Arceus le prime lotte con l'opzione accesa sono lunghissime. Anche i
  Pokémon degli allenatori e i selvatici usano le statistiche nuove
  (`CreateMon` → `CalculateMonStats`).
- Gli EV degli allenatori scalati (`TrainerScalingEVs`, `caps.c:169`)
  alzano l'impegno: da guardare nella tabella dei colpi per KO.
- Banco di prova: la ricetta è in §5.10 sopra e in `emulator.md`; il client
  RAM (`ramd.py`) va riscritto a ogni container (non nel git).
- Da chiedere all'utente: la mappatura EV → impegno e la Heavy Ball (sopra);
  dalla fase 9, il passo lento con B nella ricerca DexNav a cattura spenta e
  il Mart di Cherrygrove.

### 5.11 Fase 11, com'è stata fatta (21/11)

- **Modello**: Opus 5.5, come assegnato. Col `.sav` dell'utente (PLUS.11, 6
  medaglie, Route 43, squadra al Lv 38).
- **Danno** (`src/battle_util.c`, trappole in `.claude/rules/arceus.md`):
  formula base di Arceus in `CalculateBaseDamage`, STAB 1,25 e tipo 2,5 /
  0,4 come deciso il 20/11 (documento chiuso: il resto resta del gioco).
  Scelte mie, da confermare: **Adattabilità e Tera** col bonus sopra 1
  dimezzato (2 → 1,5); **brutto colpo ×2** del gioco (`B_CRIT_MULTIPLIER`
  GEN_3; Arceus 1,5), il documento non ne parlava; tipo ritoccato **solo nel
  danno**, non nei messaggi né nell'IA.
- **Verificato** in mGBA con una ROM strumentata (in
  `DoMoveDamageCalcVars` una struct in EWRAM con potenza, Att., Dif.,
  livello, danno base, tiro, STAB e tipo grezzi, danno finale; tolta prima
  del commit) e un riferimento Python scritto da Bulbapedia, non dal C:
  - **3 casi, opzione accesa**: Mega Drain di Vileplume Lv 38 (potenza 48,
    Att. Sp. 261) su Skarmory Acciaio/Volante (Dif. Sp. 92), doppia
    resistenza: base 62, tiro 89 → 55, STAB → 69, ×0,4 → **28**, PS 126 →
    98. Stesso colpo con Skarmory reso Roccia/Terra in RAM: tiro 94 → 58 →
    72 → ×2,5 → **180**. Dazzling Gleam (80, niente STAB) su Furret: base
    122, tiro 93 → **113**, PS 146 → 33. Tutti uguali al riferimento.
  - **Opzione spenta** (bit scritto in RAM a metà lotta): Fury Swipes di
    Furret 4 e Mega Drain 78, uguali alla formula classica: la strada di
    prima non è cambiata.
  - **Allenatore scalato** (POKéMANIAC BRENT, Route 43) con LEVEL SCALING,
    BST EQUALIZER 100 e le sfide IV/EV degli allenatori del `.sav`, opzione
    accesa: avversari a Lv 38, statistiche come previsto (EV su PS,
    Velocità e la statistica più alta di ogni coppia); Chip Away 70 → **57**
    e Extrasensory ×2 → **124**, uguali al riferimento; lotta vinta fino in
    fondo, nessun blocco né schermata blu.
- **Colpi per KO** (calcolo col riferimento verificato sopra; IV 15, EV 0,
  natura neutra, statistiche di base del repo; danno minimo-massimo):

| Fase | Attaccante → difensore | Mossa | Classica: PS, danno, colpi | Arceus: PS, danno, colpi |
| --- | --- | --- | --- | --- |
| inizio | Cyndaquil Lv 6 → Pidgey Lv 4 | Ember 40 STAB | 17, 7-10, **2-3** | 47, 25-30, **2** |
| inizio | Pidgey Lv 4 → Cyndaquil Lv 6 | Tackle 40 | 21, 2-3, **7-11** | 49, 15-18, **3-4** |
| inizio | Chikorita Lv 8 → Rattata Lv 6 | Tackle 40 | 20, 5-7, **3-4** | 39, 22-26, **2** |
| inizio | Rattata Lv 6 → Chikorita Lv 8 | Tackle 40 STAB | 26, 4-6, **5-7** | 59, 19-22, **3-4** |
| inizio | Totodile Lv 10 → Pidgeotto Lv 9 | Water Gun 40 STAB | 31, 7-9, **4-5** | 81, 25-30, **3-4** |
| inizio | Pidgeotto Lv 9 → Totodile Lv 10 | Gust 40 STAB | 31, 6-7, **5-6** | 69, 25-30, **3** |
| metà | Quilava Lv 22 → Miltank Lv 20 | Flame Wheel 60 STAB | 71, 12-15, **5-6** | 142, 40-47, **4** |
| metà | Miltank Lv 20 → Quilava Lv 22 | Stomp 65 STAB | 60, 21-25, **3** | 101, 56-67, **2** |
| metà | Croconaw Lv 28 → Haunter Lv 27 | Bite 60 ×2 | 65, 44-54, **2** | 95, 114-136, **1** |
| metà | Haunter Lv 27 → Croconaw Lv 28 | Shadow Ball 80 STAB | 78, 40-48, **2** | 122, 85-100, **2** |
| metà | Bayleef Lv 30 → Graveler Lv 28 | Razor Leaf 55 STAB ×4 | 73, 52-64, **2** | 109, 100-117, **1-2** |
| metà | Graveler Lv 28 → Bayleef Lv 30 | Rock Slide 75 STAB | 80, 28-34, **3** | 120, 67-80, **2** |
| metà | Typhlosion Lv 38 → Skarmory Lv 36 | Flamethrower 90 STAB ×2 | 98, 122-146, **1** | 138, 212-252, **1** |
| metà | Skarmory Lv 36 → Typhlosion Lv 38 | Steel Wing 70 STAB ×0,5 | 112, 15-18, **7-8** | 160, 35-41, **4-5** |
| metà | Croconaw Lv 30 → Onix Lv 30 | Water Gun 40 STAB ×4 | 65, 72-88, **1** | 87, 117-140, **1** |

  Lettura: con Arceus intero le lotte si accorciano **da tutte e due le
  parti**, e all'inizio soprattutto quelle subite (un selvatico Lv 4 batte
  un iniziale Lv 6 in 3-4 colpi invece di 7-11): la Difesa pesa poco
  (`D + 50`) e il livello molto (`15 × L`). Le resistenze valgono meno (Steel
  Wing su Typhlosion da 7-8 a 4-5 colpi). **Da decidere con l'utente** se
  ritoccare; la tabella del §3 (stessa specie, stesso livello) diceva lo
  stesso.
- ROM: 31 920 212 B, **95,13%** (+624 B sulla fase 10).
- **Non verificato**: Divinazione, Tera e Adattabilità in lotta (stesso
  codice, solo letto); lotte in doppio e in link.

**Handoff per la fase 12 (Opus 5.5)**
- La fase 12 non tocca le formule: gli alfa (+10 livelli, P10) con
  l'opzione accesa picchiano forte (il livello pesa `15 × L` nel danno), da
  tenere a mente quando si tara P13 nella fase 13.
- Da chiedere all'utente, se non risposto: il bilanciamento della tabella
  sopra, il brutto colpo ×2 e l'Adattabilità ×1,5 (sopra); dalla fase 9 il
  passo lento con B nella ricerca DexNav a cattura spenta e il Mart di
  Cherrygrove.
- Strumentazione del danno: la ricetta è sopra (struct in EWRAM scritta da
  `DoMoveDamageCalcVars` quando `ctx->randomFactor`), il riferimento
  Python va riscritto a ogni container (non nel git).

### 5.11b Fase 11b, stadi e modificatori sul danno finale (21/11)

- **Perché**: la formula di Arceus **somma** l'Attacco (`100 + A + 15 L`),
  mentre stadi, abilità e strumenti del gioco agiscono sulla statistica. Caso
  Lv 50, Att. = Dif. = 150, potenza 80: Attacco ×2 (+2 stadi, Forza Pura,
  Bendascelta) dava **+15%** (classica +95%), Difesa ×2 −44% (classica
  −49%). In Arceus gli stadi moltiplicano il danno finale (AtkMod, DefMod).
  Trovato rifacendo i conti dopo la fase 11; deciso dall'utente il 21/11.
- **Fatto**: danno base con le statistiche **grezze**, poi il danno ×
  finale/grezzo dell'Attacco × grezzo/finale della Difesa
  (`ApplyArceusStatRatio`, `src/battle_util.c`; regola `arceus.md`).
- **Verificato** in mGBA (ROM strumentata con anche le statistiche grezze,
  tolta prima del commit), `.sav` dell'utente:
  - Dazzling Gleam di Vileplume con **Att. Sp. +2** su Flaaffy: grezzo 261,
    finale 522, base 114 → ×2 = 228, tiro 92 → **209** (senza 134).
  - Stesso colpo su Girafarig con **Dif. Sp. +2** (84 → 168): base 111 →
    55, tiro 97 → **53**, PS 129 → 76.
  - Stomp di Girafarig su Vileplume (Difesa 129, 142 col bonus medaglia
    ×1,1): base 38 → 34 → tiro 90 → 30 → STAB **37**, PS 170 → 133.
  - Opzione spenta: Stomp 11 → 10 → **15**, classica.
- La tabella dei colpi per KO della §5.11 non cambia (senza modificatori
  grezzo = finale). ROM 95,13% (+224 B). **Adattabilità ×1,5** confermata
  dall'utente.
