# Note per Claude — `pokehns-expansion-plus`

Fork personale di `pokemonHnS-expansion` (base: pokeemerald-expansion 1.15.2) in cui
si portano feature da **Soulgold** (`/home/user/soulgold`, se presente nella sessione).

---

## 1. Build e consegna

```sh
make hns -j$(nproc) -O          # NON `make modern`: quello è l'altro repo
zip -9 pokemonHnS_<descrizione>_<sha>.zip pokehns.gba
```

**Consegna sempre la ROM completa da 32 MB compressa.** Niente ROM tagliate: il
padding lo toglie la compressione, non il file. Zippata sta intorno ai 18 MB.

Prima di ogni build, se hai toccato direttive del preprocessore:

```sh
python3 /tmp/ppcheck.py <file...>   # controlla il bilanciamento di #if/#endif
```

Uno `#if` non chiuso si manifesta come `'X' used but never defined` su funzioni
che non c'entrano nulla. È già successo.

---

## 2. Regola numero uno: non copiare i valori di Soulgold

Le palette di Soulgold sono tarate sulla **sua** grafica. Copiare un valore su una
grafica diversa quasi sempre finisce su una entry che lì significa un'altra cosa.
Questa è la lezione che è costata più tempo, ripetutamente.

**Deriva sempre dai file di questo repo**, e verifica *prima* di scrivere codice:

- conta gli indici usati davvero dai `.4bpp` (sono i dati che finiscono in ROM,
  i `.png` hanno palette di authoring che possono differire — succede per
  `hns/healthbox_*.png`);
- renderizza offline tile + tilemap + `.gbapal` veri e **guarda l'immagine**;
- solo allora scegli i valori.

Nella scratchpad ci sono gli script usati: `png.py` (lettura PNG indicizzati),
`render.py` (palette `.gbapal` + writer PNG), `gbapal.py`, `plte.py`.

### Trappola ricorrente: una entry, due significati

Succede di continuo. Esempi reali trovati in questo repo:

- palette del popup abilità: entry 7 è **sia** il pannello del popup **sia** la R
  del tab della ball → risolto spostando i pixel del tab su una entry libera (9);
- entry 5: sul tab MOVE INFO è la didascalia (va chiara), sul tab della ball è la
  lettera R (va scura) → il remap va fatto **per foglio**, non globale;
- barra HP entry 2: binario della barra (scuro) **e** metà bianca della ball di
  cattura (chiara) → remap solo su quell'elemento, con entry di destinazione
  **diversa per skin** (9 su gen4, 1 su gen3).

Prima di scurire/schiarire una entry, controlla **tutti** i disegni che la usano.

---

## 3. Emulatore (verifica vera)

mGBA gira headless e si pilota. Vale l'investimento: è così che sono stati trovati
i bug che l'analisi statica non vedeva.

```sh
apt-get install -y --no-install-recommends mgba-sdl xvfb xdotool imagemagick
Xvfb :99 -screen 0 800x600x24 &
DISPLAY=:99 SDL_AUDIODRIVER=dummy /usr/games/mgba pokehns.gba &   # in background
```

- Tasti: A=`x`, B=`z`, Start=`Return`, Select=`BackSpace`, L=`a`, R=`s`, frecce.
- Input: `xdotool windowfocus --sync $(xdotool search --class mgba|head -1)` e poi
  `xdotool keydown/keyup` (non `--window`: SDL ignora XSendEvent).
- Screenshot: `import -window root out.png`, poi
  `convert out.png -crop 240x160+280+220 +repage -scale 400% zoom.png`.
- L'emulatore è lento sotto Xvfb: servono attese di 2-3 s fra i tasti.
- Menu debug in overworld: tieni **R** e premi **START**
  (`DEBUG_OVERWORLD_MENU` in `include/config/debug.h`, già TRUE).
  Da lì: `Trainers… → Try Battle` entra in lotta in pochi passi.
- Il salvataggio dell'utente è un `.srm`: copialo come `pokehns.sav` accanto alla ROM.

**Misura i pixel invece di fidarti dell'occhio.** Campionare i colori del crop e
risalire alle entry (`#292929` → `RGB(5,5,5)`) ha risolto in un colpo un problema su
cui l'analisi statica girava a vuoto da un'ora.

Con un salvataggio dell'utente già su una rotta con erba la lotta selvatica si
raggiunge camminando avanti e indietro (~15-20 passi). Attenzione: se l'opzione
`WILD BATTLES` è su OFF non succede niente e sembra un bug dell'emulatore.

Due accorgimenti che fanno risparmiare molto tempo:

- avvia `Xvfb` e `mgba` con `setsid nohup ... &`, altrimenti muoiono con la shell
  e ci si ritrova senza display a metà sessione;
- dopo **ogni** passaggio di menu fai uno screenshot e guardalo prima del passo
  successivo. Contare i `Down` alla cieca porta ad aprire le opzioni dal *title
  screen* invece che dal gioco, e te ne accorgi tre schermate dopo.

**ROM strumentata**: quando una feature non si vede e l'analisi statica non
conclude, la via più rapida è una build usa-e-getta con una condizione forzata
(es. `return FALSE;` in cima a `ShouldHideTypeIcon`) o una posizione forzata al
centro schermo. Due build da tre minuti hanno chiuso un problema su cui il
ragionamento girava a vuoto da un'ora. Ricordati di `cp` del file prima.

---

## 4. Salvataggi

`struct ChallengeSettings` in `include/global.h` è **fissata a 32 byte** da
`STATIC_ASSERT(..., ChallengeSettingsLayoutPinned)` in `src/save.c`.

- Aggiungi campi **solo in fondo**, così nessun campo esistente cambia offset.
- Bit liberi nell'ultimo byte: dopo `dexNavSoulgold` ne restano **0**. La struct è
  piena. Verificato aggiungendo un bit finto: `ChallengeSettingsLayoutPinned`
  fallisce. Per una nuova opzione bisognerà ingrandire la struct, e quello sposta
  il layout del salvataggio.
- Dopo ogni aggiunta verifica che compili: l'assert fallisce da sola se sfori.

**Polarità dei bit.** Un salvataggio scritto prima che l'opzione esistesse legge il
bit a **zero**. Quindi se il default deve essere "come prima", memorizza il campo in
modo che zero significhi il comportamento vecchio — anche se il nome viene brutto.
`skipNicknamePrompt` è memorizzato invertito proprio per questo, e la lista di scelte
è ordinata di conseguenza (`sChoices_OnOff` = ON per primo, `sChoices_OffOn` = OFF
per primo). La regola in una riga: **l'etichetta del valore zero deve venire prima**.
`noWildEncounters` segue la stessa logica (0 = incontri attivi = "ON"), e
`dexNavShowAll` pure (0 = nasconde i non visti, quindi `sChoices_OffOn`).

Le opzioni impostate prima di iniziare una partita passano da
`src/oak_speech_hns.c`, che azzera tutta la struct e poi ricopia a mano i campi
del menu opzioni. **Ogni campo nuovo va aggiunto anche a quella lista**, altrimenti
la scelta fatta dal titolo viene persa in silenzio.

---

## 5. Mappa del tema scuro

Per non ri-scoprirla. Accessore unico: `IsDarkUiEnabled()` (`include/option_menu.h`).

| Cosa | Dove | Note |
| --- | --- | --- |
| Box messaggio, prompt azione | BG pal 0, entry 15 (fondo), 6 (ombra), 7/9–14 (bordo) | `ApplyDarkBattleUiPalettes()` in `battle_bg.c` |
| Box comandi, mosse, sì/no | BG pal 1 entry 14 (interno cornice) + BG pal 5 entry 8 | tutte e 20 le cornici in `graphics/text_window/` hanno l'interno sulla entry 14, bianca |
| Testo dei box comandi | BG pal 5: fg 14, ombra 13, fondo 8 | entry 8/9/10 di `text.gbapal` sono nere e inutilizzate |
| Cursore ▶ | BG pal 13 (`BATTLE_COMMAND_PAL_NUM`) | i tile 1 e 2 del textbox hanno lo sfondo sulla entry 1, che deve restare bianca per il testo |
| Healthbox / barra HP / shiny | palette sprite in `battle_gfx_sfx_util.c` | 4 palette per skin: normale, shiny, scura, shiny scura |
| Popup abilità + tab R e MOVE INFO | `sDarkAbilityPopUpPalette*` in `battle_interface.c` | palette condivisa fra tre disegni: attenzione |
| Simboli di efficacia accanto ai PP | stringhe in `MoveSelectionDisplayMoveEffectiveness` (`battle_controller_player.c`) | hanno il colore incorporato: solo l'highlight va da 14 a 8 |

Tutti i punti che ricaricano `gBattleTextboxPalette` su BG pal 0 chiamano subito dopo
`LoadBattleMenuWindowGfx()`, quindi gli override vivono solo lì dentro.

**Differenza voluta rispetto a Soulgold:** SG tiene chiara la healthbox shiny anche
in tema scuro e per farlo riscrive i pixel di ogni sprite della barra HP. Qui vanno
scuri tutti i box insieme, così la palette della barra è uno scambio secco e nessuno
sprite ha stato per-sprite che possa restare sbagliato dopo un cambio.

**Attenzione alle stringhe con i colori incorporati.** `gText_BattleYesNoChoice` e
`gText_BattleSwitchWhich` avevano `{BACKGROUND}`/`{TEXT_COLORS}` dentro il testo, che
scavalcano il colore scelto da `BattlePutTextOnWindow`. In Soulgold quelle stringhe
sono testo puro. Se una finestra resta chiara pur essendo nella lista di
`IsDarkBattleCommandWindow`, guarda prima la stringa.

---

## 6. Prima di portare qualcosa: controlla se c'è già

Vale più di qualunque stima. Cose date per mancanti che invece c'erano, spente:

- **DexNav**: `src/dexnav.c` (2691 righe), `include/config/dexnav.h`, le 12 grafiche,
  `data/scripts/dexnav.inc` e la voce `MENU_ACTION_DEXNAV` nel menu Start ci sono
  già. `DEXNAV_ENABLED` è `FALSE` e i cinque flag/var sono a 0.
- **Ricorda mosse dal riassunto**: è una feature di expansion 1.15.2
  (`P_ENABLE_MOVE_RELEARNERS`), non di Soulgold. Hasep ne aveva però solo metà:
  `ShowRelearnPrompt()` disegnava il prompt e **nessuno gestiva START**.
  Come lo configura Soulgold, verificato: `P_TM_MOVES_RELEARNER` **FALSE** (MT
  mai disponibili), mosse uovo dietro `FLAG_EGG_MOVES_UNLOCKED` (Egg Move Master,
  Blackthorn City House 3, ¥88.888) e tutor dietro `FLAG_TUTOR_MOVES_UNLOCKED`
  (Tutor Move Master, Olivine City House 4, ¥44.444). È per questo che lì un
  Pokémon di livello basso non vede mosse fuori scala: qui quei due cancelli non
  ci sono per scelta.
  Soulgold apre il relearner **solo dalla pagina Battle Moves** e forza
  `showContestInfo` a FALSE.
  Attenzione prima di gridare al bug sulla categoria LEVEL: `P_LVL_UP_LEARNSETS`
  qui è **GEN_7**, e con "modern moves" attivo il gioco usa quel learnset, non
  il gen 3 di `gLevelUpLearnsets_Gen3`. Le due tabelle non coincidono: Cyndaquil
  impara SMOKESCREEN a 6 in gen 1-6/8-9 ma EMBER a **8** in gen 7. Un Cyndaquil
  Lv6 che conosce solo Tackle e Leer quindi non ha davvero niente da ricordare,
  e `HasRelearnerLevelUpMoves` risponde giusto. Verificato con una build
  strumentata che stampava cache, specie, numero di voci del learnset e livello
  (`0101 0 155 18 1 6`): 18 voci = gen 7.
- **Icone dei tipi in lotta**: `src/type_icons.c` c'è, `B_SHOW_TYPES` era
  `SHOW_TYPES_NEVER`.
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
- **Toggle del follower nel menu squadra**: esisteva solo in `party_menu.c`
  (`MENU_PKMN_FOLLOWER`, `CursorCb_PkmnFollower`, `followerEnable`). In
  `swsh_party_menu.c` non c'era **niente**: svista, non scelta. Portato. Il
  codice di disegno delle voci è identico fra i due file, quindi il port è una
  copia di cinque pezzi: enum (prima di `MENU_FIELD_MOVES`, che indicizza le
  mosse campo come `MENU_FIELD_MOVES + j`), voce in `sCursorOptions`, colore
  nel loop di disegno, append in `SetPartyMonFieldSelectionActions`, e copie
  `static` di `GetFirstLiveMonIndex` e `CursorCb_PkmnFollower` (static, quindi
  nessuna collisione di link con la variante classica).
- **Learnset e relearner, da dove pesca**: `GetSpeciesLevelUpLearnset()` sceglie
  fra `gLevelUpLearnsets_Gen3` e `gSpeciesInfo[].levelUpLearnset` (gen 7) in base
  a `tx_Mode_Modern_Moves`, che si imposta **solo a inizio partita** — il
  challenge menu è raggiungibile unicamente da `oak_speech_hns.c`. Creazione e
  relearner passano entrambi da lì, senza bypass: dentro una stessa partita non
  possono divergere. Verificato in emulatore: un Pidgey Lv7 creato ora nasce con
  TACKLE + PECK + SAND ATTACK, cioè il gen 7.
  Da ricordare: **le mosse di livello 1 arrivano solo alla creazione**.
  `MonTryLearningNewMoveAtLevel` cerca solo entry con `level ==` il livello
  appena guadagnato, quindi un livello 1 non è mai raggiungibile salendo. E
  `GiveBoxMonInitialMoveset` tiene le **ultime quattro** mosse disponibili al
  livello di cattura, scartando le prime: per un selvatico di livello alto è
  normale che il relearner offra mosse che non ha mai avuto.
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
  convivono: `DEXNAV SOULGOLD` spenta = comportamento Hasep, accesa = SG.
- **DEXNAV SHOW ALL, il gate vero**: l'opzione va agganciata a
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
  averlo aperto, e la mia stessa §6 diceva il contrario. Verificato:
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
  SG. Dietro l'opzione `DEXNAV SOULGOLD`, spenta di default.
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
- **Mente**: tutte e 21 già in vendita al negozio di fiori di Goldenrod, dietro
  medaglia 3 e dietro il toggle `MODE_MINTS` del challenge menu.
- **`swsh_party_menu.c`** si è portato dietro roba di Soulgold mai agganciata
  (`Task_ShinGenome` era già lì). Controlla con
  `nm --defined-only build/hns/src/swsh_party_menu.o` prima di scrivere un doppione:
  un simbolo senza prefisso di variante è una collisione di link che aspetta.

Corollario sulle coordinate: quando una feature è sempre stata spenta, i suoi
dati posizionali sono i default di expansion, non valori tarati su questo repo.
`sTypeIconPositions` era così.

La healthbox è creata con subpriority **1**, le icone dei tipi con **255**:
tutto ciò che si sovrappone alla box viene disegnato *dietro* e non si vede mai.

**Errore mio da non ripetere.** Dalla segnalazione «compare per un secondo e poi
sparisce, esce e rientra dalla hpbox» avevo concluso che la posizione di partenza
(`{20, 26}`, il default di expansion) finisse dentro la box, e avevo spostato le
icone a **destra** della healthbox. Sbagliato: quel default mette l'icona a
**sinistra** della box, dove c'è spazio libero, e «esce e rientra dalla hpbox»
descriveva l'animazione che funzionava. L'unico vero bug era
`tHideIconTimer` che non si azzerava mai. La frase dell'utente conteneva già la
risposta: **leggere la segnalazione come una misura, non come un sintomo**.

**Come si misura davvero.** `import` + `convert -sample 240x160!` dà il
framebuffer 1:1, e `convert ... txt:-` ne stampa i pixel: da lì i bordi si leggono
senza interpretare. In singola il bordo **sinistro** della box è una linea
verticale a **x 12** su tutte le righe, quello destro sta a **x 101** sulla riga
del nome. Restano quindi 12 px liberi a sinistra; lo sprite è largo 8 e la x
memorizzata è il **centro**, quindi riposo 7 = icona su x 3..10.

**Disposizione attuale:** singola a **sinistra** della box (`{17, 26}`, la slide
toglie 10), doppia a **destra** come Soulgold (offset +66 dall'origine della box
su entrambi i lati, identico a SG). La scaletta di 4 px della seconda icona vale
solo dove le icone stanno a destra, quindi è attiva solo in doppia: in singola i
12 px non bastano e 4 px in un verso o nell'altro tagliano il bordo schermo o
nascondono mezzo glifo dietro la box.

**Tre differenze di comportamento (non di coordinate) trovate rispetto a SG,
tutte allineate a SG:**

- l'arte in `graphics/types/battle_icons*.png` è **identica** a quella di SG ed è
  **asimmetrica**. `ShouldFlipTypeIcon` di expansion sceglieva il lato giocatore
  in singola e quello avversario in doppia — non possono essere giusti entrambi:
  qui le icone avversarie uscivano specchiate in singola e non in doppia. SG
  specchia sempre sul lato avversario;
- SG indenta di **4 px** la seconda icona di un doppio tipo sul lato avversario
  (`SetTypeIconXY`), la scaletta in stile HGSS. Expansion le impila a filo. Qui
  la scaletta è attiva solo in doppia, per lo spazio (vedi sopra);
- le direzioni di `GetTypeIconSlideMovement` e `GetTypeIconHideMovement` in
  singola sono speculari a quelle di SG, ed è corretto così: qui l'icona sta
  dall'altro lato della box, quindi esce verso sinistra e si ritrae verso destra,
  rientrando sotto la healthbox. Il segno del riposo è `x - 10`, per questo la
  entry vale 17 e non 7. Il ramo doppie è identico a SG.

Per farlo funzionare serve anche `tHorizontalPosition` (`data[4]`) in
`include/type_icons.h`: la slide deve agganciarsi alla x di riposo **del singolo
sprite**, non alla entry di tabella condivisa, altrimenti l'indent di 4 px viene
riassorbito. Oggi `src/type_icons.c` differisce da quello di SG solo per la
tabella delle coordinate e per il `sprite->tHideIconTimer = 0;`.

Trappola di metodo, costata un'ora: avevo "verificato" che la geometria dei
sotto-sprite fosse identica fra i due repo con
`diff <(awk "/nome/,/^};/" a.c) <(awk "/nome/,/^};/" b.c)`. Il nome non esisteva
in **nessuno** dei due file, quindi awk non stampava nulla da entrambe le parti e
il diff tornava vuoto: l'ho letto come "identici". **Un diff vuoto fra due estratti
vuoti non è una verifica.** Controlla sempre che l'estratto non sia vuoto.

---

## 7. Altre cose da sapere

- **Menu squadra SwSh**: due varianti compilate insieme. `tools/gen_party_menu_variant.py`
  rigenera `include/party_menu_variant.h` e `src/party_menu_dispatch.c`. Lo script è
  idempotente (toglie i prefissi prima di riapplicarli) e verifica che ogni
  sostituzione compaia esattamente una volta.
- **Negozi**: tutti i Poké Mart normali usano `pokemart 0`, cioè un inventario unico
  scalato sui medaglieri (`sShopInventories` in `src/shop.c`). Per vendere qualcosa
  in una sola città si **appendono** extra a quella lista (`sMartExtras`), non si dà
  una lista fissa alla città. Nessun tetto al numero di oggetti: `MAX_ITEMS_SHOWN 8`
  è solo quante righe si vedono.
- **Nickname**: la domanda dei regali sta nei singoli script di mappa. È dietro la
  macro `asknickname` (`asm/macros/event.inc`) che, con l'opzione spenta, non mostra
  niente e lascia `VAR_RESULT` a NO. I due prompt del *valutatore di nickname* usano
  un testo proprio e non vanno toccati.
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
- **VBlank**: aprire una schermata senza installare il proprio `SetVBlankCallback`
  eredita quello del chiamante. È così che il Pokédex aperto dal menu SwSh scorreva
  in diagonale.
- **Aprire una schermata full-callback dall'interno di una lotta**: il
  *meccanismo* funziona — `OpenPokedexInfoScreen(species, returnCallback)` prende
  il main callback, e come `returnCallback` si passa `ReshowBattleScreenAfterMenu`
  (`include/reshow_battle_screen.h`), la stessa strada di borsa e menu squadra,
  che ricostruisce la lotta e finisce su `BattleMainCB2`, che è esattamente ciò
  che lo script di cattura in `battle_script_commands.c` aspetta.
  **Ma per il Pokédex la memoria non basta, ed è una misura, non un'opinione.**
  Censimento dell'heap fatto in emulatore con la pagina "nuova voce" aperta
  dentro una lotta (`HeapHead()` percorso a mano, stampato nella finestra):
  **23 440 byte liberi**, in un unico blocco, su 116 480 totali. La lotta da sola
  ne tiene **~66 000**. La pagina della cattura ne tiene altri 26 924
  (20 608 di buffer finestre da `sNewEntryInfoScreen_WindowTemplates`, 4 096 di
  BG 2/3, 2 220 di `PokedexView`): restituendoli tutti si arriva a 48 144.
  La schermata info ne chiede **46 464 prima di disegnare qualsiasi cosa**
  (31 616 di finestre da `sInfoScreen_WindowTemplates` — di cui 7 168 di
  `WIN_CRY_WAVE` e 2 560 di `WIN_VU_METER`, usate solo dalla schermata del
  verso — più 8 192 di BG 0-3 e 6 656 per `tileset_menu1.4bpp`), poi lo sprite
  del Pokémon, poi la sotto-schermata su cui si naviga. Muore sui 6 656 con
  `out of memory`, e la frammentazione peggiora il conto: i 4 buffer BG occupano
  la testa del buco grande, `WIN_INFO` (20 480) è costretto in coda, e alla
  decompressione resta il blocco più grande sotto i 6 656.
  **Morale: un `out of memory` con un numero preciso va tradotto in un file.**
  6 656 è esattamente `graphics/pokedex/hgss/tileset_menu1.4bpp`; `ls -l` sui
  `.4bpp` dice in dieci secondi quale schermata sta fallendo.
  Anche Soulgold, sulla pagina della cattura, esce e basta: A e B fanno la stessa
  cosa. Non era un port, era una mia aggiunta, ed è stata tolta.
  Se un giorno la si rivuole, le due strade sono (a) un set di finestre ridotto
  quando si apre da una lotta, che recupera i ~9,7 KB della schermata del verso
  ma la disabilita, oppure (b) aprire la voce **dopo** la lotta, quando l'heap è
  libero.
  Nota di metodo: **verifica in emulatore le feature che tocchi, non solo quelle
  che rompi.** Questa era stata consegnata senza una cattura vera di prova.

  **Secondo tentativo, misurato: due muri distinti, uno superato e uno no.**
  Patch di lavoro in `dex-navigation-wip.patch` (non su branch: l'uscita è rotta).
  1. *Memoria della schermata info* — **superato.**
     `sInfoScreen_WindowTemplates` alloca `WIN_CRY_WAVE` (7 168) e
     `WIN_VU_METER` (2 560) che solo la schermata del verso usa: 9 728 byte
     sprecati su ogni pagina. Con un secondo array identico ma con quelle due
     finestre a 1×1 — indici invariati, quindi nessun codice legge fuori
     tabella — la schermata info si apre dentro una lotta e **INFO, STATS, EVO
     e FORMS funzionano davvero**: verificato in emulatore su un Pidgey
     catturato (statistiche base, lista mosse con su/giù, toggle con A, catena
     evolutiva, "no alternate forms").
  2. *Schermata AREA* — **non superabile.** Decomprime l'intero tileset della
     mappa regionale: `out of memory trying to allocate 16384 bytes`, in un
     blocco solo. Non esiste margine del genere sopra una lotta, con nessun
     riordino. In modalità ridotta destra da INFO salta quindi su STATS.
  3. *Uscita verso la lotta* — **rotta, causa ignota.** `B` dalla schermata
     info (e da FORMS) **resetta il gioco**, cioè si arriva alla schermata
     GAME FREAK: sintomo tipico di un salto a callback NULL. Il giro è quello
     documentato sopra (`Task_ExitInfoScreen` distrugge il task →
     `Task_WaitForExitInfoScreenFromSummary` fa `SetMainCallback2`), e lo
     stesso giro **funziona dal menu squadra**. Ipotesi scartate leggendo il
     codice: `ResetTasks()` non è sul percorso; `monSpriteIds` è inizializzato
     a 0xFFFF quindi `ClearMonSprites` non tocca sprite validi; il comando di
     cattura in `battle_script_commands.c` aspetta correttamente tutte e tre
     le condizioni (fade, `BattleMainCB2`, task morto). Da provare la prossima
     volta: **strumentare, non dedurre** — una build che congela invece di
     saltare quando `sExternalReturnCallback` è NULL separa in un colpo
     "callback a zero" da "reshow che crasha".

---

## 7bis. Megaevoluzioni: misurate, non stimate

`P_MEGA_EVOLUTIONS` (`include/config/species_enabled.h`) è **FALSE** qui e
**TRUE** in Soulgold. Messo a TRUE la build `hns` **compila**, ma:

| | ROM | % di 32 MB |
| --- | --- | --- |
| com'è oggi | 31 799 860 | 94,77% |
| con le mega | 33 374 804 | **99,46%** |

Costa **1 574 944 byte** e lascia **179 628 byte liberi**, cioè lo 0,54%. Parte
di quel costo è `P_MODIFIED_MEGA_CRIES`, che nell'expansion è definito uguale a
`P_MEGA_EVOLUTIONS` e da solo dichiara ~3% di ROM: si può spegnere a mano per
recuperare spazio, e va provato prima di dire che non ci stanno.

**Compilare non vuol dire funzionare**: questa misura dice solo che la ROM sta
nei 32 MB. Non è stata provata in emulatore, e comunque la feature non è finita
finché non ci sono Pietraiuto/Megapietre ottenibili da qualche parte e il
giocatore ha modo di megaevolvere. Quello è lavoro di script e negozi, non un
flag.

Per il seguito: dopo le mega qualunque altra aggiunta grossa non entra più.

---

## 8. Rotture preesistenti (non tue)

Verificate su `master` pulito, **prima** di qualunque modifica:

- `make all`, `make firered`, `make leafgreen` falliscono su `src/mom_savings.c`;
- `make check` fallisce in entrambe le varianti; `test/save.c` fissa dimensioni stale;
- la CI **non costruisce mai** il target `hns`, quindi oggi non protegge da nulla.

Se qualcuno chiede di sistemare la CI, il lavoro utile è restringerla a `hns`.

---

## 9. Stile

- Commenti e messaggi di commit **in inglese**, conversazione con l'utente in italiano.
- Commenti che spiegano **perché**, non cosa: soprattutto quando un valore è stato
  derivato, di' da dove. Nelle palette è d'obbligo tenere il valore originale a
  fianco (`// was RGB(...)`).
- Commit descrittivi: cosa è rotto, perché, come si è verificato.
- Branch di sviluppo: quello assegnato dalla sessione. Non pushare altrove senza permesso.
