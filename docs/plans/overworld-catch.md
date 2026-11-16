# Piano: cattura nell'overworld (stile Leggende Pokémon: Arceus)

**Stato (16/11): fase 1 fatta (tetti, ricambio, prezzi); prossima: fase 2 (Opus 5.5).**

Si appoggia agli OWE già in gioco: leggere prima
[`.claude/rules/overworld_encounters.md`](../../.claude/rules/overworld_encounters.md)
e, per i fatti del port, [`overworld-encounters.md`](overworld-encounters.md).

## 1. Cosa si fa

- **Lancio della Ball nell'overworld**: con **R** il giocatore lancia una Ball
  (o il suo Pokémon, che fa partire la lotta) dritta davanti a sé; se sulla linea c'è un Pokémon selvatico visibile (OWE)
  si tenta la cattura senza entrare in lotta, con un'animazione vera (volo,
  Pokémon risucchiato, scosse, esito).
- **Toccare o farsi toccare** resta com'è: lotta classica.
- **Passo lento** (al posto del gattonare): **B tenuto + direzione**, ovunque,
  anche durante la ricerca DexNav (che oggi usa A tenuto).
- **Percezione migliore**: i Pokémon sentono il giocatore in base a quanto
  rumore fa (corsa, camminata, passo lento), lo vedono in un cono, non lo
  vedono attraverso gli ostacoli né quando è nell'erba alta.
- **Meno Pokémon visibili** e ricambio più lento.
- **Ball speciali meno care.** Niente crafting.
- **Pokémon alfa** (seconda parte, fasi 8-11): più grandi sulla mappa, sempre
  aggressivi, più forti all'inizio della lotta come i totem. Se sono troppo
  forti per le medaglie che hai **non si possono catturare**: si possono
  battere, ma conviene fuggire. Catturati restano alfa per sempre, con
  abilità nascosta, una mossa che la specie non ha salendo di livello e un
  segno nel riassunto.
- **Ricorda mosse a pagamento** per le mosse uovo e tutor, come in Soulgold
  (fase 7): così la mossa di un alfa resta speciale.

## 2. Decisioni prese

Risposte dell'utente (14/11):

| # | Decisione |
| --- | --- |
| D1 | **Comandi** (utente, 16/11; sostituisce la prima proposta "L lancia, A lento"): **R** lancia la Ball o il Pokémon scelto; **L premuto e rilasciato** passa da Ball a Pokémon e viceversa; **L tenuto + ←/→** scorre le Ball o i Pokémon disponibili (il giocatore non si muove mentre L è tenuto); **B tenuto** = passo lento. SELECT resta allo strumento registrato (premuto) e al secondo (tenuto 1 s). R in bici alterna ancora Mach e Acro. La scorciatoia R della ricerca DexNav **sparisce**; il DexNav si apre dal menu Start (icona DexNav, `src/strip_start_menu.c:192`). Mock del 16/11 approvati: modalità Ball e modalità Pokémon con L tenuto. |
| D17 | **Lancio del Pokémon**: in modalità Pokémon, R manda il Pokémon scelto contro il selvatico davanti e parte la lotta con **quel Pokémon in campo per primo** (solo per quella lotta). Scelta solo fra Pokémon non esausti e non uova. Nessun bonus di sorpresa nella prima versione. Senza bersaglio non succede niente. |
| D18 | **Pannello** (Ball o Pokémon scelto, quantità o livello, modalità) **in basso a sinistra** (utente, 16/11; nei mock era a destra): compare **mentre L è tenuto e per ~2 s** dopo un cambio o un lancio, poi sparisce; si nasconde quando si apre un messaggio o un menu. Segue il tema scuro senza codice in più se usa la cornice della palette 14 e il testo della 15, come il banner degli strumenti (`dark_ui.md`): da verificare in emulatore, fase 5. |
| D2 | **Probabilità**: formula normale della lotta a PS pieni, **×2 se il Pokémon non ha notato il giocatore**. Se la cattura fallisce il Pokémon si allerta: fugge o attacca secondo l'indole. |
| D3 | **Densità**: tetti **4 / 2 / 2** (erba / acqua / grotta), **Safari 3**; ricambio a tetto pieno da 240 a **480 frame**. |
| D4 | **Prezzi**: Poké/Great/Ultra restano 10/15/30. Net, Nest, Dive, Dusk, Timer, Repeat da 1000 e Luxury da **3000** (non 1000: `I_PRICE >= GEN_8`) a **100**; Heal da 300 a **50**. |

Scelte di progetto prese qui, **con un default**: l'utente può cambiarle prima
della fase che le realizza.

| # | Scelta | Default | Fase |
| --- | --- | --- | --- |
| P1 | Quale Ball parte | l'ultima lanciata (`gLastThrownBall`) se è in borsa, altrimenti la prima della tasca Ball; **L tenuto + ←/→** la cambia (D1) | 3, 5 |
| P22 | Andature e rumore (fase 2) | AUTORUN acceso (default): direzione sola = **corsa**, sentita a 5 caselle in ogni direzione; B tenuto = **lenta** (`PlayerWalkSlow`, il passo del DexNav), non sentita: conta solo la vista; bici = 5. La **camminata normale** (sentita a 2) esiste solo con AUTORUN spento (P20) | 2 |
| P23 | Forma del cono (fase 2) | oggi è un rettangolo 4×3 uguale per tutti (`CanAwareOWESeePlayer`, `src/wild_encounter_ow.c:2021`). Diventa un cono: larghezza 1 casella a distanza 1, 3 a distanza 2-3, 5 da 4 in poi, fino alla `viewDistance` del comportamento; la vista si ferma contro le caselle che bloccano il passaggio fra il Pokémon e il giocatore | 2 |
| P20 | AUTORUN spento | B tenuto resta la **corsa**, come oggi: senza corsa automatica il passo lento non c'è e la camminata normale si sente a 2 caselle | 2 |
| P21 | BUTTON MODE `L=A` | L vale A (`src/main.c:300`): niente cambio di modalità né scorrimento; R lancia comunque la Ball o il Pokémon già scelto | 3 |
| P2 | Gittata | 5 caselle in linea retta; si ferma al primo ostacolo o dislivello (D16: niente consumo) | 3 |
| P3 | Nuzlocke / One Type / Species Clause | le stesse regole della lotta: dove la cattura sarebbe bloccata la Ball non parte (messaggio); una cattura riuscita segna la zona | 3 |
| P4 | Leggendari erranti e sciami | il lancio fallisce sempre e parte la lotta (i loro dati vivono fuori dall'oggetto) | 3 |
| P5 | Zona Safari, Gara Pigliamosche, Azienda Lotta | lancio **spento** nella prima versione | 3 |
| P6 | Esperienza alla cattura | nessuna (in lotta `B_EXP_CATCH` la dà: qui non c'è chi ha partecipato) | 3 |
| P7 | BUTTON MODE `L=A` | sostituita da P21 (il lancio ora è su R) | — |
| P8 | Squadra piena | va al PC come in lotta; box pieni: la Ball rimbalza. In raffica la squadra si può riempire a metà: il controllo è **all'impatto**, non al lancio, e così Nuzlocke e Species Clause (la prima cattura segna la zona, la seconda Ball rimbalza anche se era già in aria) | 3 |

Il ×2 di D2 è una scelta di bilanciamento **nostra**: non è un valore preso da
Leggende Arceus.

Seconda parte, risposte dell'utente (14/11, dopo la prima stesura):

| # | Decisione |
| --- | --- |
| D5 | **Mega fuori** dal gioco per ora: il limite di ROM non è un vincolo per questo piano. |
| D6 | **DexNav resta**, per ora. Se toglierlo si decide dopo la fase 3. |
| D7 | **Alfa troppo forte**: non catturabile (né in lotta né con il lancio), ma **si può battere e si può fuggire**. È tarato perché fuggire convenga. |
| D8 | **Alfa all'inizio della lotta**: aumento delle statistiche come i totem. |
| D9 | **Alfa catturato**: resta alfa per sempre (bit nel Pokémon salvato), **abilità nascosta garantita**, **una mossa speciale**, **segno nel riassunto**, **follower più grande**, missioni nel quest book. |
| D10 | **Ricorda mosse**: mosse uovo e tutor solo dopo aver pagato (o con eventi), preso da Soulgold. |
| D11 | Alfa e ricorda mosse **nello stesso piano**, in fasi dopo la cattura. |
| D12 | **Fino a 2 alfa per mappa**, e più frequenti di 1 su 50: con i tetti abbassati (D3) sarebbero troppo rari. |
| D13 | **Missione a livelli**: cattura **1 / 5 / 10 / 30** alfa, con premi crescenti. |
| D15 | **Raffica**: fino a **3 Ball in aria**, intervallo minimo ~½ s fra due lanci, il giocatore resta libero di muoversi. Esito **deciso all'impatto** (Pokémon creato e consegnato subito); le scosse sono solo animazione. Banner **in coda**, uno dopo l'altro; soprannomi chiesti **alla fine** della raffica. Se un Pokémon uscito ti carica e parte la lotta, le catture già decise restano. |
| D16 | **Consumo**: la Ball si consuma solo se colpisce un Pokémon; a vuoto (5 caselle e cade) o contro NPC/muro/cartello torna in borsa. Verso il follower non parte. |
| D14 | **Banner di cattura**: il riquadro in alto degli strumenti ottenuti, con l'icona del Pokémon al posto dell'icona dello strumento, "You caught XXX♂!" e sotto "It was added to your party." o "It was sent to the PC.", con una **fanfara breve**. Prototipo approvato in emulatore il 14/11 (§3). |

Default della seconda parte, cambiabili prima della fase:

| # | Scelta | Default | Fase |
| --- | --- | --- | --- |
| P9 | Quanti alfa | **1 su 20** Pokémon generati, **al massimo 2 per mappa** (D12); mai shiny e alfa insieme (lo shiny vince); niente alfa in Safari, Gara, Azienda Lotta, acqua (lo sprite da nuoto non scala). Conto: a tetto pieno nasce un Pokémon ogni 480 frame (8 s, D3), cioè 7,5 al minuto: con 1 su 20 un alfa ogni ~2,7 minuti d'erba, con 1 su 50 ogni ~6,7. La fase 8 lo misura in frame e lo ritocca | 8 |
| P17 | Premi della missione (D13) | 1 alfa: 10 Ultra Ball; 5: **da scegliere** (l'Ability Patch si compra già a Violet, `src/shop.c:1000`: non è un premio); 10: Shiny Charm (funziona, `src/pokemon.c:3249`, e nelle mappe di HnS non si ottiene altrove); 30: Master Ball. Si ritirano da un NPC, come lo scienziato di TcloneR (chi e dove: fase 10) | 10 |
| P10 | Livello | quello tirato dalla tabella (già scalato) **+ 10**, senza il tetto `B_WILD_SCALING_MAX_ABOVE_TABLE` | 8 |
| P11 | "Troppo forte" | livello dell'alfa **sopra** `GetLevelScalingCap()` (il tetto delle medaglie anche con LEVEL CAP spento, `src/caps.c`) | 9 |
| P12 | Comportamento | sempre aggressivo, vista più lunga, non sparisce finché sei sulla mappa (come gli shiny) | 8 |
| P13 | Aumento in lotta | +1 a Attacco, Difesa, Att. Sp., Dif. Sp., Velocità, col messaggio dei totem | 9 |
| P14 | Mossa speciale | una mossa uovo della specie a caso, altrimenti una mossa tutor, altrimenti nessuna; sostituisce l'ultima | 9 |
| P15 | Lancio su un alfa troppo forte | la Ball rimbalza **e non si consuma**, con un messaggio; l'alfa ti nota | 9 |
| P18 | Dopo il banner | soprannome chiesto come in lotta (rispetta `skipNicknamePrompt` e il soprannome obbligatorio della Nuzlocke); **niente** pagina del Pokédex | 3 |
| P19 | Fanfara | `MUS_HG_OBTAIN_EGG` (`FANFARE_HG_OBTAIN_EGG`, 131 frame, ~2 s, "hai ricevuto un Pokémon"); il banner si chiude con A o B solo a fanfara finita | 3 |
| P16 | Ricorda mosse MT | restano **sempre** disponibili (D10 parla solo di uovo e tutor; in SG le MT sono spente) | 7 |

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
  `src/item_menu.c:2451`). **B tenuto** oggi, con AUTORUN acceso (default,
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
  SaveBlock1: la fase 8 controlla che un salvataggio vecchio legga 0 lì.
- **Bit nel Pokémon salvato**: bit non usati da nessun file, verificato con
  `grep`: `PokemonSubstruct0` `unused_02:6`, `unused_04:3`, `unused_0A:2`;
  `PokemonSubstruct1` `unused_04:5`, `unused_06:3`; `BoxPokemon` `unused_13`,
  `unused_1E` (`include/pokemon.h:130-170, 255-276`). Meglio uno delle
  sottostrutture: sono cifrate e coperte dal checksum. `CreateMon` azzera
  tutto, quindi un Pokémon vecchio legge 0 = non alfa. **Da controllare in
  fase 9**: che expansion non abbia già preso quel bit nelle versioni dopo la
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
  **non sono dimostrabilmente a zero** nei salvataggi: la fase 10 le controlla
  sul `.srm` dell'utente. Il contatore sale a ogni alfa catturato, in lotta e
  col lancio (stessa funzione per i due lati, fase 9).
- `setwildbossbattle` (`src/scrcmd.c:3383`) è una lotta di script con mosse
  scelte, senza aumento di statistiche: non serve agli alfa.
- **Sprite più grande**: il motore scala già sprite di oggetti con matrici
  affini (`src/event_object_movement.c:1531`, riflessi). Le matrici affini
  sono 32 e gli OWE in acqua ne usano una (`ST_OAM_AFFINE_ERASE`). Un oggetto
  scalato oltre i suoi 32×32 viene tagliato se non è in doppia dimensione:
  **prototipo prima di promettere**, in fase 8.
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

## 4. Spazio misurato (build pulita di `d6520855`)

| | Usato | Limite | Libero |
| --- | --- | --- | --- |
| ROM | 31 902 628 B (95,08%) | 32 MB, errore CI al 99,5% | 1 651 804 B. Le mega sono fuori per ora (D5); se tornassero (`megas.md`, 1 574 944 B) la ROM andrebbe al 99,77%, già oltre il limite senza questa feature |
| SaveBlock3 | 96 B | 116 | 20 B |
| VRAM sprite | — | 1024 tile | da misurare in fase 4 nel caso peggiore (rotta con 4 OWE + follower) |

Costo previsto della prima parte: **solo codice** (stima 8-15 kB, da
rimisurare a ogni fase), **zero byte di salvataggio** (`gLastThrownBall` è già
in EWRAM, nessuna opzione nuova), nessuna grafica nuova.
Seconda parte: **due flag** nuovi per il ricorda mosse (`save.md`: liberi
`FLAG_*` e engine flag da `+14`), **un bit** in ogni Pokémon salvato (bit già
riservato, struct invariata), un bit nell'oggetto; grafica forse un'icona del
segno alfa per il riassunto. Nessun campo nuovo in SaveBlock3.

## 5. Fasi

Una fase per sessione (`/next-phase overworld-catch`). Modelli scelti fra
Fable, Opus e Sonnet come chiesto dall'utente: Fable solo dove un errore
costa di più del suo prezzo.

| # | Fase | Modello | Cancello per passare oltre |
| --- | --- | --- | --- |
| 1 | **Densità e prezzi** (D3, D4): `OWE_SPAWNS_MAX_LAND/WATER/CAVE/SAFARI`, `OWE_SPAWN_TIME_REPLACEMENT`, prezzi in `items.h`. `OWE_SPAWNS_MAX` resta 6 (dimensiona slot e `localId`) | **Sonnet 5.5** | build verde; in emulatore il conteggio degli OWE su una rotta d'erba non supera 4 in 60 s di gioco (misurato in frame) e 2 in grotta; screenshot di un negozio con i prezzi nuovi. `.srm` non necessario |
| 2 | **Passo lento e percezione**: B tenuto = passo lento con AUTORUN acceso, anche nella ricerca DexNav (al posto di A tenuto), P20 con AUTORUN spento; rumore per andatura (corsa/bici 5, camminata 2, passo lento 0: solo vista); cono di vista con numeri diversi per comportamento; ostacoli che coprono la vista; giocatore nell'erba alta non visto oltre 1 casella; funzione pubblica `IsOWEAwareOfPlayer()` | **Opus 5.5** | build verde; prove in emulatore con posizioni lette via GDB: da dietro a passo lento si arriva adiacenti senza essere notati, camminando si viene notati a 2, correndo a 5, di fronte alla distanza di vista, dietro un ostacolo mai. A premuto davanti a un NPC o un cartello interagisce ancora; nella ricerca DexNav il passo lento con B non fa scappare il bersaglio; con AUTORUN spento B fa ancora correre. `.srm` non necessario |
| 3 | **Nucleo della cattura**, senza animazione (Ball istantanea, messaggio): comandi di D1 (R lancia, L premuto cambia modalità, L tenuto + ←/→ scorre senza muovere il giocatore, scorciatoia DexNav su R tolta, Mach/Acro su R in bici invariato), **lancio del Pokémon** (D17), bersaglio sulla linea (P2), consumo solo a bersaglio colpito (D16), **esito all'impatto e coda per la raffica** (D15: fino a 3 lanci pendenti, intervallo minimo, banner e soprannomi in coda), Pokémon costruito dalla funzione comune con la lotta, formula condivisa con ×2 se non allertato (D2), tutto ciò che la lotta fa dopo una cattura, blocchi P3-P8, reazione al fallimento; **banner di cattura con fanfara** (D14, P18, P19, dal prototipo del §3) | **Fable 5.2** | build verde; `ComputeCaptureOdds` in lotta dà gli stessi numeri di prima su 3 casi (ROM strumentata). In emulatore: ogni tasto di D1 provato a piedi, in bici, con AUTORUN spento e con `L=A`; un Pokémon lanciato scende in campo per primo e la squadra torna nell'ordine di prima; banner guardato per squadra, PC, maschio, femmina, senza sesso e nome lungo; cattura con Master Ball, **guardando il Pokémon in squadra** (specie, livello, shiny, Ball giusti, non Bad Egg), Ball scalata, Pokédex segnato; **raffica di 3** con 2 presi e 1 fuggito, con 3 presi a squadra da 5 (uno in squadra, due al PC), con la seconda Ball su una zona Nuzlocke appena usata (rimbalza, non consumata); lancio a vuoto e contro un NPC (non consumata); spawn forzato nello stesso frame di un impatto; un lancio fallito allerta il Pokémon; zona Nuzlocke già usata, box pieni e Safari rifiutano il lancio; un OWE in acqua o catturato o escluso con il motivo scritto. **Serve il `.srm` dell'utente** (squadra e borsa vere, impostazioni di sfida) |
| 4 | **Animazione**: posa di lancio (derivata dai frame che il giocatore ha già, guardando l'immagine), Ball in volo ad arco con lo sprite del suo tipo, risucchio, caduta, 0-3 scosse da `ComputeBallShakeOdds` (decise all'impatto, D15), cattura o uscita con "!"; **fino a 3 Ball animate insieme**, e una lotta che parte mentre altre Ball tremano le chiude prima della transizione | **Fable 5.2** | build verde; tile e palette libere misurate nel caso peggiore **con 3 Ball di tipi diversi in aria** (se non ci stanno, il limite scende e si scrive), nessuno script in attesa se uno sprite non si crea (provato forzando il fallimento); sequenza di screenshot guardati per cattura, fuga dopo 1 e dopo 3 scosse, nelle 4 direzioni, di giorno e in grotta. `.srm` utile, non necessario |
| 5 | **Rifinitura e documentazione**: pannello di D18 (Ball o Pokémon, quantità o livello, modalità) dai mock del 16/11, rifatto con font e finestre del gioco, suoni, messaggi; `CHANGELOG.md`, `README.md`, regola `.claude/rules/overworld_catch.md` (con `paths:`), indice in CLAUDE.md §7, `docs/SOULGOLD_PORT_REPORT.md` (feature nuova, non un port) | **Sonnet 5.5** | build verde; pannello guardato con 1, 2 e 0 tipi di Ball in borsa e con 1 e 6 Pokémon, in tutti e due i temi; sparisce dopo ~2 s; testi riletti contro il codice; `docs_validate` verde |
| 6 | **Revisione della prima parte**: `/code-review` medio sul diff delle fasi 1-5, ogni punto aperto provato in emulatore, bilanciamento (quante Ball per una cattura a inizio gioco e a metà), ROM consegnata zippata. Qui si decide anche se togliere il DexNav (D6) | **Opus 5.5** | build e CI verdi, percentuale di ROM annotata, punti aperti chiusi o motivati. **Serve il `.srm` dell'utente** |
| 7 | **Ricorda mosse a pagamento** (D10), port da Soulgold: `P_ENABLE_MOVE_RELEARNERS` FALSE, `P_TM_MOVES_RELEARNER` resta TRUE (P16), due flag, due maestri con il loro prezzo (dove: dalle mappe, v. §3), testo nel riassunto quando la voce è chiusa | **Sonnet 5.5** | build verde; nel riassunto le mosse uovo/tutor non compaiono prima e compaiono dopo aver pagato (screenshot); le MT sì in tutti e due i casi; un salvataggio vecchio parte chiuso. **Serve il `.srm`** (Pokémon con mosse uovo vere) |
| 8 | **Alfa sulla mappa**: bit `alpha` nell'oggetto, tiro P9, livello P10, comportamento P12, **prototipo dello sprite più grande** (se non regge: un'aura o un'icona sopra la testa, e lo si scrive) | **Opus 5.5** | build verde; con il debug che forza l'alfa: compare uno per mappa, ti insegue, non sparisce; sprite guardato su erba, in grotta, accanto ad altri OWE; tile e matrici affini misurate; un salvataggio vecchio carica senza alfa fantasma. `.srm` non necessario |
| 9 | **Alfa in lotta e catturato**: aumento P13, blocco della cattura in lotta e col lancio (D7, P11, P15), bit nel Pokémon salvato con `MON_DATA_IS_ALPHA`, abilità nascosta e mossa P14 alla nascita | **Opus 5.5** | build verde; un alfa sopra il tetto respinge la Ball in lotta e nell'overworld (Ball non consumata), si batte e si fugge; uno sotto il tetto si cattura e **in squadra, dopo salvataggio e ricarica e passaggio nel PC, è ancora alfa** con abilità e mossa giuste; un Pokémon del salvataggio vecchio legge non-alfa e il checksum regge. **Serve il `.srm`** |
| 10 | **Alfa visibile e missione**: segno nel riassunto (tutti e due i temi), follower più grande se la fase 8 l'ha reso possibile, **missione a livelli 1/5/10/30** sul modello di TcloneR (contatore, premi P17, NPC che li consegna, sottomissioni nel quest book, `/add-quest`) | **Sonnet 5.5** | screenshot del riassunto chiaro e scuro con indici contati sui `.4bpp`; follower guardato; con il contatore portato a 0, 1, 5, 10, 30 via debug: stato e "NEW" giusti nel libro, ogni premio ritirato una sola volta; le due var nuove a zero nel `.srm` dell'utente prima della modifica. **Serve il `.srm`** |
| 11 | **Revisione finale e documentazione**: `/code-review` medio sul diff delle fasi 7-10, CHANGELOG (con l'avviso del ricorda mosse), regole, report SG (la fase 7 è un port), ROM consegnata | **Opus 5.5** | build e CI verdi, punti aperti chiusi o motivati. **Serve il `.srm`** |

Perché questi modelli: le fasi 1 e 5 sono costanti, dati e testo con un
cancello che prende gli errori. La 2 tocca un invariante nascosto (la macchina
a stati del movimento). La 4 è passata a Fable con la raffica (D15): tre
animazioni concorrenti, la VRAM condivisa con icone ed effetti e una lotta che
può partire nel mezzo. La 3
apre il codice di lotta, crea un Pokémon fuori dalla lotta e incrocia Nuzlocke
e salvataggio: è l'unico punto in cui un errore silenzioso arriva nel
salvataggio dell'utente. Nella seconda parte la 9 tocca il Pokémon salvato, ma
con un bit già riservato e un cancello che salva, ricarica e passa dal PC:
basta Opus. La 7 è un port con sorgente chiaro, la 10 grafica e contenuto.

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
- ROM: 31 902 612 byte usati (95,08%), 1 651 820 liberi.

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
- Un salvataggio con più OWE del tetto li tiene finché non si cambia mappa
  (ricambio uno per uno): innocuo, ma conta se si misura da un `.sav`.
- Il commento di `include/config/wild_encounter.h:8` cita
  `HasepAreOWEsAllowedHere()`, che non esiste più (gli OWE sono attivi
  ovunque): da togliere alla prima occasione.

## 6. Scartato, e perché

- **Crafting delle Ball**: escluso dall'utente, per ora.
- **Gattonare**: non esistono frame del giocatore accovacciato; il passo lento
  c'è già.
- **Strumenti da lancio di Leggende Arceus** (bacche, fango, fumogeni): fuori
  da questo piano.
- **Statistiche, IV e colori come premio dell'alfa** (utente, 14/11): gli IV
  si possono già portare al massimo e gli EV togliere, e un colore diverso si
  confonderebbe con lo shiny.
- **Alfa grande in lotta e nei menu**: gli sprite di lotta sono 64×64 fissi;
  solo la mappa può scalare (fase 8).
- **Mira libera o a parabola regolabile**: il motore si muove a caselle; una
  linea retta nella direzione dello sguardo è leggibile e costa poco.
- **Lancio nel Safari e alla Gara** (P5): hanno Ball contate e regole proprie
  (`gNumSafariBalls`, punteggio della Gara). Si possono aggiungere dopo.
- **Un'opzione per spegnere la feature**: non c'è posto nel salvataggio senza
  allargare una struct, e chi non la vuole non preme R.
- **Tetto `OWE_SPAWNS_MAX` a 4**: sposterebbe i `localId` degli OWE; bastano
  i tetti per zona.
