---
paths:
  - "src/dexnav.c"
  - "include/dexnav.h"
  - "include/config/dexnav.h"
  - "data/scripts/dexnav.inc"
  - "src/field_control_avatar.c"
  - "src/field_player_avatar.c"
  - "graphics/dexnav/**"
  - "src/fldeff_flash.c"
---

# DexNav

- **DexNav**: `src/dexnav.c` (2691 righe), `include/config/dexnav.h`, le 12 grafiche,
  `data/scripts/dexnav.inc` e la voce `MENU_ACTION_DEXNAV` nel menu Start ci sono
  già. `DEXNAV_ENABLED` è `FALSE` e i cinque flag/var sono a 0.

- **DexNav su R**: `TryStartDexNavSearch()` c'era, ma nel ramo `#else` di un
  `#if IS_HNS` in `src/field_control_avatar.c`, dove HnS usa R per scambiare
  mach/acro bike. Nella build `hns` la chiamata non veniva quindi **mai
  compilata** e R a piedi non faceva nulla. Morale: in questo repo un
  `#if IS_HNS / #else` non è un dettaglio di piattaforma, è spesso il posto in
  cui una feature è spenta senza che nessun flag lo dica.
- **Creeping del DexNav**: `gPlayerAvatar.creeping` si alza solo tenendo **A**
  mentre si cammina, e senza di esso il Pokémon fugge appena si entro nei 2
  tile (`CREEPING_PROXIMITY`). In `PlayerNotOnBikeMoving` il controllo stava
  nell'`else if` **dopo** il blocco della corsa, che però fa `return` sempre e
  con AUTORUN attivo (`autoRun == 0`) ha la guardia vera anche senza B: il ramo
  era irraggiungibile e ogni ricerca finiva con "si è mosso troppo in fretta".
  Ora è **prima** del blocco corsa, come già faceva il ramo surf. Soulgold ha
  lo stesso ordine sbagliato, ma lì `ShouldPlayerRun` è uno XOR, quindi tenendo
  B+A si riesce comunque: qui no.

- **DexNav: la divergenza con Soulgold è a DUE SENSI.** Non è un port. SG ha
  cose che qui mancano (search level, area progress, unbind con SELECT, una
  valanga di guardie di robustezza, il level cap, il bug contest) ma **Hasep ha
  cose che SG non ha**: tutta la riga dei Pokémon **nascosti** (`ROW_HIDDEN`,
  `hiddenSpecies`, `CapturedAllHiddenMons`, detector mode), il Pokéblock della
  Safari Zone, il movimento in acqua/grotta (`movementCount`), il confronto per
  numero di Pokédex in `SpeciesInArray` e le API più recenti
  (`GetAbilityBySpecies`, `SafeFreeMonIconPalette`). Un "rendilo come Soulgold"
  alla lettera **cancella feature**. Classificare prima, sempre.
- **Search level senza rompere i salvataggi**: la modalità per-specie di
  upstream infila `dexNavSearchLevels[NUM_SPECIES]` in SaveBlock3 **prima** di
  `challengeSettings` e sposta ogni opzione di ogni salvataggio. SG ha una terza
  modalità, `DEXNAV_SEARCH_LEVELS_REGISTERED_SPECIES`, che tiene il livello
  della sola specie registrata in **una var**: cambiando bersaglio si azzera, ma
  il layout non si muove. È quella in uso. Offset misurati con i flag veri della
  build hns e ora inchiodati in `save.c`: `dexNavChain` a **12**,
  `challengeSettings` a **16**, `sizeof(struct SaveBlock3)` = 52.
- **Meccanica stealth: ora è un'opzione, non una scelta fissa.** SG ha **tolto**
  la fuga per avvicinamento e il timeout dalle ricerche avviate dal giocatore.
  La scelta iniziale dell'utente era tenere quella di Hasep in stile HGSS; alla
  prova sul campo in grotta si è rivelata impraticabile (vedi più sotto), e ora
  convivono dietro un'unica opzione.
- **`ENHANCED DEXNAV`: una sola opzione, default ON.** `DEXNAV SHOW ALL` e
  `DEXNAV SOULGOLD` erano due toggle separati; l'utente le ha volute fuse perché
  non si usano mai separate. Accessore unico `EnhancedDexNav()` in `dexnav.c`,
  campo `basicDexNav` memorizzato invertito (vedi `.claude/rules/save.md`).
  **Attenzione ai crediti, qui è facile sbagliare**: delle due metà **solo la
  ricerca è di Soulgold**. Il "mostra tutti" **non lo è**: verificato leggendo
  `soulgold/src/dexnav.c`, che gatta la lista su `FLAG_GET_SEEN` nelle stesse
  tre funzioni di Hasep. La descrizione dell'opzione nomina quindi SOULGOLD per
  la ricerca e si limita a *constatare* l'elenco completo, senza attribuirlo.
- **Da dove arriva il DexNav nella storia.** Non è più agganciato a
  `FLAG_SYS_POKEDEX_GET`: lo consegna **l'assistente di Elm insieme alle prime
  cinque Poké Ball**, nel laboratorio di New Bark
  (`NewBarkTown_Lab_EventScript_TriggerAideGiveBalls`), che è un momento in cui
  la storia si ferma già di suo. Il flag è `FLAG_RECEIVED_FIRST_BALLS`, ed è la
  scelta giusta per tre motivi **verificati, non supposti**:
  1. è alzato una volta sola lì e non viene mai più pulito — i due `clearflag`
     più su nello stesso file girano **prima**, quando Elm ti manda da
     Mr. Pokémon;
  2. è già alzato in qualunque salvataggio oltre New Bark, quindi **nessuna
     migrazione** serve (confermato sul salvataggio 2.0.6 dell'utente: la voce
     DEXNAV è rimasta al suo posto);
  3. arriva **dopo** sia il Pokégear (dalla mamma, prima di uscire di casa,
     `NewBarkTown_PlayersHouse_1F_EventScript_GivePokeGear`) sia il Pokédex
     (da Oak a casa di Mr. Pokémon, `Route30_MrPokemonsHouse_hns`). È la
     precondizione per spostarlo un giorno dentro il Pokégear.
  R sul campo non ha bisogno di un gate proprio: `TryStartDexNavSearch()` esce
  subito se non c'è una specie registrata in `DN_VAR_SPECIES`, e registrarla si
  può solo dalla schermata, che prima del flag non è raggiungibile.
- **Il gate vero della metà "mostra tutti"**: l'opzione va agganciata a
  `DexNavGetSpecies()`, non alle singole schermate. Quella funzione risponde
  `SPECIES_NONE` per una specie non vista, e **R, A e il pannello info passano
  tutti di lì**: agganciare l'opzione solo a `TryDrawIconInSlot` e
  `PrintCurrentSpeciesInfo` disegnava l'icona di un Pokémon che poi non si
  poteva registrare. Un gate a monte batte tre gate a valle.
  Trappola di verifica, ci sono cascato: avevo "confermato" l'opzione
  registrando Caterpie, che in quel salvataggio era **già vista** — funzionava
  anche senza la modifica. Per provare una feature legata al Pokédex serve una
  specie che con l'opzione OFF mostri il **punto interrogativo**: lì era
  Ledyba. Verificato in entrambi i sensi sullo stesso slot (OFF rifiuta, ON
  registra) e la ricerca sul campo parte davvero.
  **Cosa sblocca cosa**: il dettaglio nel pannello è legato a
  `FLAG_GET_CAUGHT`, non a `FLAG_GET_SEEN`. Non catturato = tipi `???` e
  abilità "Capture first!", nome comunque visibile perché serve a sapere cosa
  stai registrando. È una scelta dell'utente e vale **anche con l'opzione
  spenta**: prima una specie vista ma non catturata mostrava i suoi tipi.
  Serviva perché la griglia disegna la stessa icona a colori in entrambi i
  casi, quindi senza questo il pannello non distingueva posseduto da non
  posseduto. Nota di verifica: con `DEXNAV_SEARCH_LEVELS_REGISTERED_SPECIES`
  il search level resta 0 finché non catturi, quindi non aggiunge informazione.
- **DexNav in grotta: la colpa è di HnS, non di Soulgold.** Errore mio da non
  ripetere: ho detto all'utente che il codice era «identico in Soulgold» senza
  averlo aperto, e queste stesse note dicevano il contrario. Verificato:
  in `soulgold/src/dexnav.c` **non esiste** né `movementCount` né il blocco
  "Caves and water the pokemon moves around". Il bersaglio che si sposta è
  roba di HnS.
  E la scelta della casella in SG non ha alcun tiro di dado: è
  `weight = !MapGridGetCollisionAt(topX, topY);` in tutti i rami, grotta, erba e
  acqua, per cui la ricerca **riesce sempre** se esiste una casella valida. SG
  tratta anche `MAP_TYPE_INDOOR` come grotta (Sprout Tower).
  HnS invece pesa con `Random() % scale`, dove in grotta
  `scale = 440 - dist/2 - 2*(tileX + tileY)`. Due difetti: `scale` è `u8`, quindi
  tutto oltre 255 si tronca (e a 256 esatti torna la divisione per zero che il
  `max(1, ...)` non copre, perché il clamp agisce prima dell'assegnazione); e il
  termine sulle **coordinate assolute** lega la probabilità a dove ti trovi sulla
  mappa invece che alla distanza, che è già un termine a parte.
  Misurato sul salvataggio dell'utente a Burned Tower B1F, stesso punto:
  **0 ricerche avviate su 7** con il comportamento HnS, **5 su 5** con quello di
  SG. Oggi dentro `ENHANCED DEXNAV`, accesa di default.
  **Le fughe sono la seconda metà, ed è la più grossa.** SG mette *ogni*
  uscita per fallimento dietro `hiddenSearch`, con tanto di commento: una
  ricerca avviata dal giocatore «stays active until completed, canceled, or
  left behind». HnS le applica anche alle tue: fuori raggio (`LostSignal`),
  non stai strisciando entro 2 caselle, corri o sei in bici entro 4, e un
  timer di 15 s. In grotta quelle quattro ti mangiano vivo, perché il
  bersaglio nasce lontano e strisciare è metà velocità.
  **`gPlayerAvatar.creeping`: in SG ha ZERO lettori.** Lo scrivono in quattro
  punti (`field_player_avatar.c`, `dexnav.c`) e non lo legge nessuno. In HnS
  l'unico lettore è il controllo di fuga. Quindi appena si allinea a SG il
  creeping diventa un flag morto: tenere A rallenta e basta. È così anche in
  Soulgold, non è una svista del port.
  Con l'opzione accesa, verificato end-to-end sul salvataggio dell'utente:
  ricerca avviata, avvicinamento **di corsa** senza tenere A, bersaglio fermo,
  "You encountered a wild MAGMAR!". Senza opzione, stesso punto, non ci si
  arriva.
- **DexNav e Flash: il livello 1 è "illuminato".** `GetFlashLevel()` vale
  0 fuori dalle grotte buie, 7 al buio e **1** dopo Flash
  (`EventScript_UseFlash` e `SetDefaultFlashLevel`): mai 0. Il controllo di
  upstream `> 0` lasciava quindi il DexNav "troppo buio" a Dark Cave anche
  con Flash. Ora `IsTooDarkToSearch()` (`> 1`) in `InitDexNavSearch` **e**
  in `TryFindHiddenPokemon`: il fork di Mikeagan, da cui arriva la
  segnalazione, aveva corretto solo il primo. In Soulgold il bug non si vede
  perché lì `SetDefaultFlashLevel` mette 0 se hai la MN in borsa.
  Verificato in mGBA, nuova partita + Cheat start + warp di debug (Dark Cave
  = gruppo 24, mappa 0), flag e var scritti via GDB: senza Flash "too dark",
  con Flash (flag alzato su Route 31, poi dentro) livello 1, ricerca avviata
  e ZUBAT incontrato. Offset misurati compilando `offsetof` con la toolchain:
  `flashLevel` 0x34, `flags` 0x198C, `vars` 0x1ABC — i commenti in
  `global.h` (0x30, 0x1270…) sono vecchi, non fidarti. `FLAG_SYS_USE_FLASH` si
  azzera uscendo all'aperto: va alzato fuori e poi si entra.

## Finestre e testo sulla schermata DexNav

- **Finestre e testo, due trappole misurate sulla schermata DexNav**:
  1. `FONT_SMALL` mette l'inchiostro **3 righe sotto** la y che gli passi. Per
     centrare una scritta in una barra alta 10px bisogna chiedere `y_barra - 3`.
  2. Due finestre che **condividono anche una sola riga di tile** si corrompono
     a vicenda: mettere un contatore a `tilemapTop 1` mentre `WINDOW_REGISTERED`
     occupa le righe 0-1 sporcava entrambe. Se la riga serve e la finestra
     esistente c'è già, **allarga quella** e stampa dentro (è così che il
     contatore dell'acqua vive dentro `WINDOW_REGISTERED`, alta 3 righe).
  Le barre delle intestazioni sono **arte del BG** (`gui_tilemap.bin`), non
  finestre: una finestra riempita con `PIXEL_FILL(TEXT_COLOR_TRANSPARENT)` ci
  scrive sopra senza cancellarle. Estensioni misurate: barra acqua y13-22,
  terra y54-63, nascosti y120-129. I simboli "catturati tutti" sono sprite 8px
  **centrati** su x139, x152 e x114, quindi occupano x135-142, x148-155 e
  x110-117: compaiono solo a riga completata, cioè proprio quando il contatore
  è al massimo della larghezza, e lì si sovrapporrebbero. I contatori finiscono
  quindi a x130, x143 e x103.
