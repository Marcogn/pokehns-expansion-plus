# Piano: quest book e menu di pausa a striscia

**Stato (25/10): tutte e otto le fasi fatte (§5.1-§5.8).** Restano il contenuto del §7 e la release. Si fa dopo Red's Path
([`reds-path.md`](reds-path.md)), una fase per sessione (§5). Le ricerche sono già fatte: i fatti qui sotto sono
verificati nel codice indicato, non vanno rifatte. I confronti e le
opzioni scartate stanno in
[`docs/analysis/quest-book-research.md`](../analysis/quest-book-research.md):
non servono per implementare.

## 1. Cosa si fa

Due schermate nuove, disegnate per stare insieme, tutte e due nello stile
di Pokémon Unbound:

- **Quest book**: per ogni missione, a che punto è e **cosa fare adesso**
  (dove andare, chi cercare). Contiene storia principale, Pokédex,
  leggendari e missioni secondarie (§7).
- **Menu di pausa a striscia**: una fascia di icone in basso, disegnata
  sopra la mappa, al posto della lista classica.

## 2. Decisioni prese

Decisioni definitive dell'utente, prese dopo due anteprime: mappa vera di
New Bark Town, icone di `pollythadon`, font del gioco.

**Menu di pausa**

- Striscia in basso sopra la mappa, **6 icone in una pagina sola**, in
  quest'ordine: Pokédex, DexNav, Squadra, Borsa, **Missioni, Opzioni**.
- **Start** salva, **L** apre la Scheda allenatore, **R** il Pokégear.
- I suggerimenti mostrano **l'immagine del tasto**, non il testo: i glifi
  `{START_BUTTON}`, `{L_BUTTON}`, `{R_BUTTON}` del gioco
  (`graphics/fonts/keypad_icons.png`, `sKeypadIcons` in `src/text.c`).
- Niente pannello con la squadra: la voce Squadra c'è già.
- Dove serve resta la lista classica: Safari, Bug-Catching Contest,
  Piramide, Battle Pike, Union Room.

**Quest book**

- Base: la ricreazione del menu di Unbound di PokemonSanFran (§3.1).
- **Legge lo stato che il gioco ha già**, invece di duplicarlo: flag delle
  medaglie (`FLAG_BADGE01_GET`…`FLAG_BADGE16_GET`), conteggio del Pokédex,
  flag dei leggendari, flag di Red's Path, var del TcloneR. Così costa pochi
  bit e funziona sulle partite vecchie. Solo le missioni inventate hanno
  stato proprio.
- Testi in inglese, tema chiaro e scuro (`IsDarkUiEnabled()`), grafica
  rifatta sulla nostra (CLAUDE.md §2).

## 3. Le basi da portare

Tutte e due vanno portate a mano: nessuna è un branch per expansion 1.15.2.

### 3.1 Quest book: PSF `complex_quests`

`github.com/PokemonSanFran/pokeemerald`, branch `complex_quests`
(RubyRaven6, ultimo commit 2026-10-20), costruito su `unbound-quest-menu`;
`src/quests.c` 2971 righe. È per pret/pokeemerald.
Documentazione: [wiki di PSF](https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu).

Cosa offre:

- stati: bloccata (`????`), sbloccata, attiva, premio da ritirare,
  completata, più "preferita" (Select, va in cima);
- sottomissioni (A sulla missione), con contatore `x/y`;
- filtri per stato con R, ordine alfabetico con Start;
- per ogni voce: mappa, sprite (NPC, oggetto o Pokémon), descrizione
  attiva e descrizione "completata";
- **il pezzo che ci serve**: ogni missione ha un `questVariable`, e
  `desc[]`, `map[]`, `sprite[]` sono indicizzati da
  `VarGet(questVariable)` (`GetQuestDesc`, `GetQuestLocation`). È già il
  "cosa fare adesso" guidato da una var.

Cosa cambiare portandolo:

| Problema in PSF | Cosa fare in Hasep |
| --- | --- |
| Lo stato viene solo da var o dai suoi bit | Tre sorgenti per missione: **var** (TcloneR), **funzione** (medaglie, Pokédex, leggendari, Red's Path: da aggiungere), **bit propri** (missioni inventate). Anche le sottomissioni devono poter leggere una funzione (le 8 sfide di Red's Path, §7) |
| `ListMenu_ProcessInput` (`src/list_menu.c`) è modificata: **tutte** le liste del gioco ricominciano dall'alto dopo l'ultima voce | Lo scorrimento circolare va limitato al quest book |
| Salvataggio in coda a SaveBlock2 (5 bit a missione, 1 a sottomissione), azzerato in `NewGameInitData` | In coda a SaveBlock3, con `STATIC_ASSERT` sull'offset e un passo di migrazione (CLAUDE.md §4, `.claude/rules/save.md`) |
| `desc[VarGet(...)]` su un array di `MAX_QUEST_STATES` = 50 voci, senza controllo: una var oltre 49 legge fuori dall'array | Controllo sul limite |
| Comando di script con l'opcode della tabella di pret | Rinumerare nella tabella di expansion |
| `CreateMonIcon` con un argomento in più (FAQ della documentazione) | Firma di expansion |
| ~570 byte di ROM a missione (50 + 50 puntatori e 50 sprite anche se ne usa 3) | Accettabile (§4) |

Bug noto (dalla documentazione): un artefatto di un frame passando da uno
sprite NPC a uno non NPC, mai riprodotto con regolarità.

### 3.2 Menu a striscia: `pollythadon` `unbound_start_menu.c`

`github.com/pollythadon/pokeemerald-expansion`, branch `shattered-emerald`
(commit del 2026-10-13, expansion 1.17.1), `src/unbound_start_menu.c`, 1317
righe. Sta dentro un gioco, non è un feature branch, ed è descritto come
alpha.

- Striscia in basso **sopra la mappa**: icone a `USM_ICON_YPOS` 128, sul
  BG0 del campo, con `FreezeObjectEvents`. Finestre per nome della voce,
  orologio e suggerimenti dei tasti.
- **Le schermate tornano con `CB2_ReturnToFieldWithOpenMenu`, come oggi**:
  per questo i ritorni delle altre schermate non vanno toccati.
- Nell'originale: 6 icone per pagina, più pagine, L cambia pagina, R
  riordina le voci (ordine salvato in SaveBlock3). Da noi servono una
  pagina sola, niente riordino, e L/R ridestinati (§2).
- Base più nuova della nostra (1.17.1 contro 1.15.2): le differenze vanno
  viste al momento del port.

## 4. Spazio misurato

Build `make hns` del 24/10 su `master` 2417f28e. Le dimensioni vengono da
`nm -S pokehns.elf` su `gSaveblock1/2/3`, meno i 128 byte di ASLR
(`SAVEBLOCK_MOVE_RANGE`, `include/load_save.h`); la ROM è misurata fino al
padding `0xFF`.

| | Usato | Limite | Libero |
| --- | --- | --- | --- |
| ROM | 31 862 468 B (94,96%) | 32 MB | ~1,6 MB (le mega, non ancora unite, ne prendono ~1,5) |
| SaveBlock1 | 15 760 B | 15 872 | 112 B |
| SaveBlock2 | 3 892 B | 3 968 | 76 B |
| SaveBlock3 | 60 B | 116 (`SaveBlock3FitsFirstChunk`) | 56 B |

Al quest book servono da 8 a 25 byte in SaveBlock3. I due menu valgono
decine di KB di ROM (stima, non misurata).

## 5. Fasi

Vincoli dell'utente: modelli **non oltre Opus 5.5**, sforzo **medio**,
pochi token al mese. Una fase per sessione, che parte da questo file.
Opus solo dove un errore costa caro, Sonnet per port e grafica, Haiku per
la documentazione.

| # | Fase | Modello | Cancello per passare oltre |
| --- | --- | --- | --- |
| 1 | **Fondamenta del quest book**: struct in coda a SaveBlock3, `STATIC_ASSERT`, migrazione (il prossimo `SAVE_VERSION` libero: Red's Path ha preso il 10 e l'11, quindi il 12), API delle tre sorgenti di stato, comando di script | **Opus 5.5** | build verde; un `.sav` di prima carica e il campo nuovo legge zero (emulatore) |
| 2 | **Port della schermata** (§3.1): lista, dettaglio, sottomissioni, filtri, preferite, con le correzioni della tabella. Aperta da una voce provvisoria del menu classico | **Sonnet 5.5** | due missioni di prova navigabili, screenshot guardati |
| 3 | **Grafica del quest book**, tema chiaro e scuro | **Sonnet 5.5** | indici contati sui `.4bpp`, render offline, screenshot in tutti e due i temi (CLAUDE.md §2, `battle_ui.md`) |
| 4 | **Prime missioni**: 16 medaglie, TcloneR, e Red's Path se la sua sessione B è in `master` (§7) | **Sonnet 5.5** | ogni passo verificato con un salvataggio o con il menu debug |
| 5 | **Menu a striscia** (§3.2) con le decisioni del §2 | **Opus 5.5** | ogni voce aperta e chiusa in emulatore, eccezioni comprese |
| 6 | **Grafica della striscia**: fascia, icone nuove per DexNav e Missioni, tema scuro | **Sonnet 5.5** | come la fase 3 |
| 7 | **Documentazione**: `CHANGELOG.md`, `docs/SOULGOLD_PORT_REPORT.md` (l'origine è PSF/pollythadon, non SG), regola nuova in `.claude/rules/` | **Haiku 5.5** | testi riletti contro il codice |
| 8 | **Revisione finale** del diff (`/code-review` medio) prima della release, **più i punti aperti del §5.7**: ognuno va provato in emulatore e, se rotto, corretto | **Opus 5.5** | build verde, ogni punto del §5.7 provato o corretto (o motivato), ROM consegnata zippata |

### 5.1 Fase 1, com'è stata fatta (25/10)

Da qui parte la fase 2: il port della schermata usa questa API, non lo
stato a bit di PSF (`QuestMenu_GetSetQuestState` e `questData`/`subQuests`
in SaveBlock2 vanno sostituiti, non portati).

- **Salvataggio**: `struct QuestLogSaveData`, 24 byte in coda a SaveBlock3
  (offset 60, SaveBlock3 a 84 su 116), `SAVE_VERSION 12`. Capienze e
  layout in `.claude/rules/save.md`.
- **Costanti** (`include/constants/quests.h`): `QUEST_COUNT` (oggi 0),
  `QUEST_OWN_SLOT_COUNT`, `QUEST_OWN_SUBQUEST_COUNT`, `QUEST_SOURCE_*`,
  `QUEST_STATE_*` (bloccata, sbloccata, attiva, premio, completata),
  `QUEST_LOG_*` del comando di script.
- **Tabella**: `src/data/quests.h`, `struct QuestInfo` (`include/quest_log.h`).
  Le sorgenti: `QUEST_SOURCE_VAR` (0 = bloccata, da `doneValue` in su
  completata, in mezzo attiva con passo = valore), `QUEST_SOURCE_FUNC`
  (`getProgress` riempie stato e passo), `QUEST_SOURCE_OWN` (`ownSlot`,
  nibble salvato). Sottomissioni da funzione (`isSubquestDone`) o da bit
  propri (`subquestBitStart`). Testi, mappe e sprite **non ci sono ancora**:
  li aggiunge la fase 2 alla stessa struct, indicizzati da
  `QuestLog_GetStep`, che è già limitato a `numSteps` (il controllo che
  mancava in PSF).
- **API** (`src/quest_log.c`): `QuestLog_GetProgress/GetState/GetStep`,
  `SetState` (solo missioni a stato proprio; un cambio la rimette "nuova"),
  `GetSubquestCount`, `IsSubquestDone`, `SetSubquestDone`,
  `CountSubquestsDone` (la x di x/y), `IsFavorite/SetFavorite`,
  `IsNew/MarkSeen` (salvato come "vista": zero = nuova), `GetName`.
- **Script**: `questlog azione, missione[, arg]`, opcode `0xf7`
  (`SCR_OP_QUESTLOG`), missione e arg possono essere var; le domande
  scrivono `VAR_RESULT`. Manca `QUEST_LOG_OPEN`: lo aggiunge la fase 2.
- **Verificato in emulatore**: un `.sav` riportato a mano a versione 11 con
  spazzatura nei 24 byte (`a5a4a7…`) carica e risalva a versione 12 con i
  24 byte a zero; la stessa prova su una ROM senza il `memset` li lascia
  sporchi. Il `.sav` dell'utente (2.0.6.PLUS.4, versione 8) passa a 12 con
  squadra identica byte per byte, soldi e Candy Jar uguali.

### 5.2 Fase 2, com'è stata fatta (25/10)

Dettagli e trappole in `.claude/rules/quest_book.md`.

- `src/quest_book.c`: schermata portata da PSF, sull'API della fase 1.
  Voce provvisoria `QUESTS` nel menu normale solo se `QUEST_COUNT > 0`;
  `QUEST_LOG_OPEN` per gli script (da far seguire da `waitstate`).
- Testi nella tabella: `steps` (indicizzati da `QuestLog_GetStep`),
  `doneDesc`, `subquestInfo`. Immagini: oggetto, strumento, Pokémon.
- Grafica **di PSF, provvisoria**; la fase 3 la sostituisce.
- Due missioni di prova dietro `QUEST_BOOK_TEST_QUESTS` (spente).
- Verificato in emulatore (vedi la regola); non provati `QUEST_LOG_OPEN` e
  il tema scuro.

### 5.3 Fase 3, com'è stata fatta (25/10)

Decisione dell'utente: **ritinteggiare la grafica di PSF**, non ridisegnarla.
Il tema chiaro resta com'era; dettagli in `.claude/rules/quest_book.md`.

- `graphics/quest_book/menu_dark.pal`: palette scura, stessi indici dei
  `.4bpp` (nessun tile toccato). Testo scuro in `sDarkTextPalette` (pal 15).
- Il riquadro bianco dell'immagine sparisce senza immagine: usa due entry
  usate solo da lui, che prendono il colore del fondo.
- Verificato in emulatore, tutti e due i temi: lista, "Close" (senza
  immagine), sottomissioni. Non provati: Pokémon e oggetto sul riquadro
  scuro, `QUEST_LOG_OPEN`.

### 5.4 Fase 4, com'è stata fatta (25/10)

Quattro missioni in `src/data/quests.h`, tutte a **funzione** (nessun bit
salvato oltre preferita/nuova): `QUEST_JOHTO_BADGES`, `QUEST_KANTO_BADGES`,
`QUEST_TCLONER`, `QUEST_REDS_PATH`. Dettagli in `.claude/rules/quest_book.md`.

- Medaglie: il passo è la **prima palestra mancante** nell'ordine canonico
  (numero di medaglia; Kanto: Janine prima di Sabrina come nel §7). Non è
  un ordine imposto dal gioco. Clair ha due passi: palestra, poi Dragon's
  Den (`FLAG_DEFEATED_BLACKTHORN_GYM`; la medaglia la dà il Den). Blue:
  Cinnabar, poi Viridian. Kanto è bloccata finché manca la medaglia 8.
- TcloneR: bloccata a `VAR_TCLONER_STAGE` 0; passo = tappa; premio da
  ritirare se `TClonerIsRewardDue()`; completata a 12 passi consegnati. I
  testi sono statici: niente conteggio dei cloni.
- Red's Path: bloccata prima di `FLAG_REDS_PATH_INTRO_SEEN`; passi 0-7 =
  prossima sfida, 8-15 = premio in attesa in quella palestra; 8
  sottomissioni.
- Bug del port trovato qui: due immagini oggetto con la stessa palette
  (Falkner e Pryce) si rompevano a vicenda (magenta); ora gli oggetti si
  distruggono uno alla volta.
- **Verificato** in mGBA sul `.sav` dell'utente (6 medaglie, Pryce come
  prossimo; Kanto bloccata) e, con flag/var scritti via GDB: Kanto (Surge,
  Blue a Cinnabar e poi a Viridian), Clair al Den, TcloneR (tappe 2 e 5,
  premio), Red's Path (premio a Pewter, poi Misty, **completata** con le 8
  sottomissioni). **Tema chiaro** guardato: lista, Done/Active, sottomissioni.
- **Non provato**: TcloneR completata (12 passi); `QUEST_LOG_OPEN` da script;
  una partita vera fino a una missione (solo flag scritti a mano).
- **Per la prossima sessione**
  - Fase 5 (menu a striscia, §3.2): fatta, §5.5.
  - Rifinitura del libro, se serve: a missione **completata** il libro
    mostra lo sprite del passo 0 (Falkner, Brock, lo scienziato), poco
    sensato; basterebbe uno step finale in più, o un campo
    `doneSprite` in `struct QuestInfo`. Le sottomissioni delle missioni
    senza `subquestInfo` (TcloneR) non esistono.
  - Contenuto ancora da fare: Pokédex a scaglioni, leggendari, le cinque
    missioni di raccordo (§7). I passi di Johto non dicono cosa sblocca
    ogni palestra (MN, eventi): solo il cancello di Blackthorn è nei testi,
    letto in `BlackthornCity_hns/scripts.inc`.
  - Il TcloneR non mostra il conteggio dei cloni (testi statici): servirebbe
    un segnaposto numerico in `PrintFooter`.
  - Il `.sav` di prova sta fuori dal repo; per riprovare: `.claude/rules/quest_book.md`,
    sezione "Missioni reali" (offset di flag/var, bit del tema scuro).

### 5.5 Fase 5, com'è stata fatta (25/10)

Dettagli e trappole in `.claude/rules/strip_menu.md`.

- `src/strip_start_menu.c`: port ridotto di `unbound_start_menu.c`, una
  pagina, voci fisse, niente riordino né SaveBlock3. Nel gioco di
  `pollythadon` il file oggi **non è agganciato** (Start apre il menu Rotom):
  l'aggancio in `start_menu.c` è nostro.
- La striscia sostituisce la lista solo nel caso normale; le eccezioni del §2
  (più Multi Partner Room e link) restano classiche. Nessun ritorno di altre
  schermate toccato.
- Voce provvisoria `QUESTS` della lista classica tolta, con
  `gText_MenuQuests`: il libro si apre dall'icona.
- Grafica di `pollythadon`, provvisoria: DexNav e Missioni prendono in
  prestito le icone PokéNav e Save. Niente tema scuro.
- **Verificato in mGBA** sul `.sav` dell'utente: sei voci, L, R, Start
  (annulla e salva), B; Safari e Bug-Catching Contest classici; Grotta Scura
  senza Flash; icone che non entrano in VRAM (ROM strumentata).
- **Non provati**: Piramide, Pike, Union Room, link; una partita nuova.
- **Per la fase 6**: fascia e icone nuove (DexNav, Missioni), tema scuro su
  palette 14 e su quella delle icone. Le finestre della striscia stanno sulle
  entry 10, 11 e 15 di `bg.pal` (sfondo sotto il testo), il testo usa 1, 2 e 3:
  ridisegnando la fascia vanno tenute o spostate insieme.

### 5.6 Fase 6, com'è stata fatta (25/10)

Dettagli in `.claude/rules/strip_menu.md`.

- Icone nuove `dexnav.png` (radar) e `quests.png` (libro con nastro), nella
  palette delle altre; quelle in prestito (PokéNav, Save) tolte.
- Tema scuro: `bg_dark.pal` per fascia e finestre, orologio con testo chiaro.
  Fascia e icone di `pollythadon` restano com'erano.
- Verificato in mGBA sul `.sav` dell'utente (tema scuro) e dopo averlo
  portato a chiaro dalle opzioni: Missioni e DexNav selezionate, in tutti e
  due i temi.
- Approvata dall'utente dopo due giri sulle dimensioni: le icone nuove
  stanno nel riquadro della Poké Ball (19 px, righe 7-25 del foglio).
- **Handoff per la fase 7 (Haiku 5.5, documentazione)**
  - `CHANGELOG.md`, sezione `2.0.6.PLUS.7`: la voce "New pause menu" dice
    ancora "The icons are provisional": va corretta (ora DexNav e Quests
    hanno icone proprie, e c'è il tema scuro). Aggiungere i crediti per le
    icone di `pollythadon` solo dopo averne verificato gli autori nel suo
    repo (§5, Crediti). Leggere ogni frase contro il codice.
  - `docs/SOULGOLD_PORT_REPORT.md`: nessuna riga su quest book e striscia;
    l'origine è PSF/pollythadon, non SG (dirlo).
  - Regole `.claude/rules/quest_book.md` e `strip_menu.md`: già
    aggiornate fino alla fase 6; riletterle contro il codice, e togliere i
    riferimenti a "provvisorio" rimasti.
  - Dopo la 7: fase 8, revisione finale (`/code-review` medio), build
    verde, ROM zippata.
- **Punti aperti**: li controlla e corregge Opus nella fase 8 (elenco sotto,
  tabella di fase 8).

### 5.7 Fase 7 (25/10) e punti aperti per la fase 8 (Opus 5.5)

Fase 7 fatta: `CHANGELOG.md` (voce della striscia corretta, icone e tema
scuro), report di porting (quest book e striscia non sono di SG) e regole
riviste contro il codice. Gli autori delle icone (hedara, vol) **non sono
stati verificati** (nessun credito nel README del repo di pollythadon): la
voce cita solo lui.

Nessuno blocca le fasi precedenti, ma **nessuno è stato provato**: Opus li
verifica uno per uno (emulatore, GDB per flag e var: `.claude/rules/quest_book.md`
e `strip_menu.md`) e corregge ciò che trova rotto.

**Striscia** (`src/strip_start_menu.c`, `src/start_menu.c`)
1. Eccezioni che devono restare alla lista classica: **Piramide**, **Battle
   Pike**, **Union Room**, **link**, Multi Partner Room. Vanno forzate (flag
   o ROM strumentata) e il menu deve aprirsi, selezionare e chiudersi.
2. **Partita nuova** senza Pokédex né Pokémon: devono restare solo Borsa,
   Missioni, Opzioni; nessun crash né cursore su una voce assente.
3. Ritorno da **ogni** schermata della striscia (Pokédex, DexNav, Squadra,
   Borsa, Missioni, Opzioni, Scheda, Pokégear): la striscia si riapre sulla
   stessa voce.

**Quest book** (`src/quest_book.c`, `src/data/quests.h`)
4. `questlog QUEST_LOG_OPEN` **da script** (poi `waitstate`): mai provato.
5. **TcloneR completata** (12 passi consegnati): stato, testo, sprite.
6. A missione **completata** il libro mostra sprite e mappa del passo 0
   (Falkner, Brock, lo scienziato): da correggere con uno step finale o un
   campo `doneSprite` in `struct QuestInfo`.
7. TcloneR: testi statici, **niente conteggio dei cloni** (servirebbe un
   segnaposto numerico in `PrintFooter`); le sottomissioni non esistono.
8. Pokémon e oggetto sul riquadro **scuro** dell'immagine: mai guardati.
9. Una **partita vera** fino a una missione: finora solo flag scritti a mano.

**Revisione del diff** (`/code-review` medio) su tutto il lavoro delle fasi
1-6: save (`SAVE_VERSION 12`, offset, migrazione), nuovo opcode, VRAM e
palette, `#if` bilanciati (CLAUDE.md §1).

### 5.8 Fase 8, com'è stata fatta (25/10)

`/code-review` medio su `4d282bfe..HEAD`: dieci rilievi, sei corretti.

- `questlog QUEST_LOG_OPEN` chiedeva solo `SCREFF_V1`: ora anche
  `SCREFF_HARDWARE`, come `fadescreen` (sfuma e cambia schermata).
- **NEW non tornava** mai sulle missioni a var e a funzione: decisione
  dell'utente, livelli visti a 2 bit, `SAVE_VERSION 13` (6 byte,
  `.claude/rules/save.md`).
- Missione completata con sprite e mappa del passo 0 (punto 6): campo
  `done` in `struct QuestInfo`, al posto di `doneDesc`.
- Kanto attiva con l'ottava medaglia di Johto, prima di poterci andare: ora
  con `FLAG_IS_CHAMPION`. Il testo di Johto completata diceva "Kanto is open
  now!": ora "On to the Pokémon League!".
- TcloneR, premio con "Location: the TcloneR scientist": ora testo proprio
  ("show your clones to a TcloneR scientist") e la mappa dello stadio.
- Select riportava il cursore in cima: ora segue la missione.
- Punto 7, su decisione dell'utente: conteggio dei cloni (`bufferText`,
  `{STR_VAR_1}`/`{STR_VAR_3}`) e 12 sottomissioni, una per premio.
- Gli altri quattro, rimandati in un primo momento, corretti su richiesta
  dell'utente in un secondo giro: un solo `GetStartMenuContext()` per la
  lista classica e la striscia; le icone della striscia aprono le schermate
  con le stesse funzioni della lista (`StartMenu_Open*`); lo stato delle
  missioni letto una volta per ricostruzione della lista; tolti i due
  involucri (`RedsPath_GetNextChallenge` è ora la funzione stessa,
  `GetQuestState` sostituita dalla cache).
- Tolta `CountBadgeFlags`, mai usata (warning).

Punti del §5.7, tutti in mGBA sul `.sav` dell'utente: 1 Pike, Piramide,
Multi Partner Room e Union Room forzati via GDB (lista classica); **link non
provato** (serve un partner); 2 senza Pokédex, Pokémon, DexNav e Pokégear
(flag spenti, non una partita nuova da zero); 3 ritorno da tutte e otto le
schermate; 4 `QUEST_LOG_OPEN` con una ROM strumentata; 5, 7, 8 guardati; 6
corretto; 9 **partita vera**: volo ad Azalea, scienziato, TcloneR sbloccata.
Trovato in più: la lista del libro è circolare come tutte quelle del gioco
(la regola diceva il contrario). Dettagli nelle regole `quest_book.md` e
`strip_menu.md`.

**Dopo la fase 8** (25/10, PR #41 e #42): corretti anche i quattro rilievi
rimandati; versione della hack nella finestra di salvataggio (issue #39);
cornici delle finestre standard che dopo la striscia restavano nei colori
della fascia (palette 14/15 non restituite al campo, sfuggito perché le
prove confrontavano ROM che avevano già la striscia); **Select** sulla
striscia sposta le icone, ordine in SaveBlock3 (`SAVE_VERSION 14`).
Dettagli in `.claude/rules/strip_menu.md` e `save.md`; il README ha ora le
righe del quest book, della striscia e di Red's Path.

Dopo la fase 1 il quest book (2-4) e il menu (5-6) sono indipendenti. Se i
token stringono, la fase 4 si fa una missione per volta.

**Ordine con Red's Path.** Red's Path (sessioni A1, A2, B, C del suo piano)
viene prima. Due punti di contatto:

- **migrazione**: Red's Path usa `saveVersion < 10` (A1) e `< 11` (B, i
  suoi nove flag); la fase 1 qui usa il **12**. Se un'altra migrazione
  arriva prima in `master`, prende il numero dopo: conta l'ordine in `master`;
- **la voce di Red's Path** nel quest book si aggiunge solo dopo la sua
  sessione B (prima non è raggiungibile in gioco). Le cinque missioni di
  raccordo (§7) vengono dopo tutte e due. Stima a giudizio:
2-3 sessioni il quest book, 1-2 il menu.

**Crediti** da riportare nelle note di release: PSF (Unbound Quest Menu) e
chi elenca la sua documentazione (Skeli, ghoulslash e gli altri);
`pollythadon` per la striscia. Gli autori delle icone (hedara e vol, secondo
la sua descrizione) vanno verificati nel suo repo.

## 6. Aggiungere una missione (dopo le fasi)

Basta Sonnet, o Haiku se la condizione esiste già.

1. Trova la condizione nel gioco (flag, var, medaglia, Pokédex) con `grep`
   negli script `_hns`, e **verifica dove viene impostata**: il nome può
   ingannare. `FLAG_VISITED_KANTO` (`flags_hns.h`) si accende ai primi passi
   sul **Percorso 27** (`Route27_EventScript_FirstSteps`), non ad
   Aranciopoli: per "arriva ad Aranciopoli" serve
   `FLAG_VISITED_VERMILION_CITY`.
2. Se la condizione non c'è (missione inventata), prendi un flag o una var
   liberi da `flags_hns.h`/`vars_hns.h` e impostali nello script nuovo.
3. Aggiungi la voce in tabella, i testi in inglese e l'id in
   `constants/quests.h`. **Gli id vanno in coda, mai in mezzo**, o i bit
   salvati finiscono sulla missione sbagliata.
4. Verifica in emulatore con un caso che fallisce prima della condizione.

**Capienza**: la struct salvata ha posto per 48 missioni (`QUEST_SAVE_MAX_*`);
oltre serve un passo di migrazione. Una missione che legge lo stato del
gioco costa 3 bit (preferita, livello visto a 2 bit), una con stato proprio
7, una sottomissione propria 1. In SaveBlock3 restano 24 byte dopo il
`SAVE_VERSION 13`. Gli
indici di PSF sono `u8`, quindi 255 al massimo. SaveBlock3 si può allargare
fino a 1624 byte togliendo l'assert sul primo chunk: non provato.

## 7. Contenuto (backlog)

Le missioni si valutano una per una con l'utente.

### Storia principale: 16 medaglie

Una missione per regione, un passo per palestra, con la prossima indicata.

| Johto | Mappa | Kanto | Mappa |
| --- | --- | --- | --- |
| Falkner | `VioletCity_Gym_hns` | Brock | `PewterCity_Gym_hns` |
| Bugsy | `AzaleaTown_Gym_hns` | Misty | `CeruleanCity_Gym_hns` |
| Whitney | `GoldenrodCity_Gym_hns` | Surge | `VermilionCity_Gym_hns` |
| Morty | `EcruteakCity_Gym_hns` | Erika | `CeladonCity_Gym_hns` |
| Chuck | `CianwoodGym_hns` | Janine | `FuchsiaCity_Gym_hns` |
| Jasmine | `OlivineCity_Gym_hns` | Sabrina | `SaffronCity_Gym_hns` |
| Pryce | `MahoganyTown_Gym_hns` | Blaine | `SeafoamIslands_Gym_hns` |
| Clair | `BlackthornCity_Gym_hns` | Blue | `ViridianCity_Gym_hns` |

- **Kanto**: ordine libero, tranne Blue, che torna in palestra solo dopo
  averlo incontrato a Cinnabar con 15 medaglie (`CinnabarIsland_hns/scripts.inc`,
  `VAR_NUM_BADGES == 15`). Il passo è l'elenco delle palestre mancanti,
  con Blue per ultimo.
- **Johto**: l'ordine e cosa sblocca la strada (MN, eventi) vanno ricavati
  dagli script nella fase 4. Non ancora verificati.

### Pokédex a scaglioni di 50

| Pokédex | Voci | Scaglioni |
| --- | --- | --- |
| Johto (`JOHTO_DEX_COUNT`, `include/constants/pokedex.h`) | 282 | 50 … 250, 282 |
| Nazionale | 1025 | 50 … 1000, 1025 |

Da decidere: quale Pokédex conta, visti o catturati, e se gli scaglioni
danno un premio.

### Leggendari

Mappe trovate cercando `SPECIES_*` negli script `_hns`. Alcune mappe li
nominano solo nel testo: il luogo della lotta e le condizioni per farli
comparire vanno confermati mappa per mappa.

| Pokémon | Mappa |
| --- | --- |
| Articuno, Zapdos, Moltres | `SeafoamIslands_B1F`, `Route10`, `MtSilver_1F_MoltresRoom` |
| Mewtwo, Mew | `CeruleanCave_B2F`, `FarawayIsland_Interior` |
| Raikou, Entei | `BurnedTower_B1F` (vaganti) |
| Suicune | `BurnedTower_B1F`, poi `CianwoodCity`, `Route42`, `VermilionCity`, `Route25`, `Route14` |
| Lugia, Ho-Oh, Celebi | `WhirlIslands_LugiaChamber`, `TinTower_RoofDay`, `IlexForest` |
| Kyogre, Groudon, Rayquaza | `Route19_Cave`, `SeafoamIslands_SecretCave`, `EmbeddedTower` |
| Latias, Latios | `SouthernIsland_Interior` |
| Deoxys, Jirachi | `BirthIsland_Exterior`, `MtMoon_Outside` |
| Regi (sei) e Arceus | `SinjohRuins_*`, `SinjohRuins_ArceusRoom` |

### Secondarie che esistono già

- **TcloneR** (`docs/plans/tcloner.md`, `.claude/rules/trainer_catch.md`):
  stato in `VAR_TCLONER_STAGE`, `VAR_TCLONER_COUNT`, `VAR_TCLONER_REWARDS`.
- **Red's Path** ([`reds-path.md`](reds-path.md)): una **missione con 8
  sottomissioni**, una per sfida (Brock, Misty, Surge, Erika, Koga, Sabrina,
  Blaine, Giovanni, in quest'ordine). Tutto lo stato c'è già nel suo piano
  (§4 "Stato salvato"), quindi la sorgente è una funzione e non costa bit:
  - sfida vinta = flag di allenatore battuto di
    `TRAINER_REDS_PATH_<CAPO>_HNS` (id 653-660);
  - premio ritirato = `FLAG_REDS_PATH_PRIZE_<CAPO>` (8 flag, sessione B);
  - introduzione vista = `FLAG_REDS_PATH_INTRO_SEEN`.
  La logica "prossima sfida / premio in attesa" esiste già in C:
  `RedsPath_GetAideState` e `GetNextChallenge` (`src/reds_path.c`), da
  riusare invece di riscriverla.

  Passi della missione: bloccata finché l'introduzione non è vista → "next
  challenge: <capo> in <città>" (la prima sfida col flag spento; se il capo
  di HnS di quella palestra non è battuto, `FLAG_DEFEATED_<CITTÀ>_GYM`,
  "beat the Leader there first") → "prize waiting" se una sfida è vinta e
  il premio no (squadra piena) → completata con 8 vittorie e 8 premi.

### Le cinque missioni di raccordo (dopo Red's Path)

Colmano il salto fra la squadra di Red's Path (Charizard, Pikachu-Starter,
Snorlax, Eevee, Poliwrath, Lapras) e quella del Monte Argento
(`TRAINER_RED_HNS`: Pikachu-Starter, Snorlax, Charizard, Venusaur,
Blastoise, Espeon). Agganci che esistono già: casa di Red a Biancavilla
(`PalletTown_RedsHouse_1F_hns`), laboratorio di Oak, casa di Bill sul
Percorso 25, Villa di Azzurropoli. In HnS nessuno script regala gli starter
di Kanto. Ognuna va progettata con l'utente, come Red's Path.

| # | Missione | Cosa racconta | Dipende da | Note per il quest book |
| --- | --- | --- | --- | --- |
| 1 | **Il PC di Bill**: i Pokémon del viaggio di Red (le panchine di Red's Path), da vedere o da affrontare | dove finisce la squadra del viaggio | motore di lotta di Red's Path (sessione A) se c'è una lotta | flag propri (missione inventata) |
| 2 | **Gli starter di Oak**: Red torna da Oak per Bulbasaur e Squirtle; il giocatore porta a Red un oggetto di Oak fino al Monte Argento | Venusaur e Blastoise | progressione verso il Monte Argento: va controllata | var a passi: bersaglio naturale di `complex_quests` |
| 3 | **Eevee al sole**: la signora della Villa riceve la foto di un Espeon, o un indizio in Johto | Espeon | — | flag propri |
| 4 | **La lettera alla madre**: tre anni di viaggio in poche righe | tutto, solo testo | — | 1 flag |
| 5 | **Red contro Blue**: nona lotta, Red con la squadra di Giovanni contro Blue campione (`TRAINER_CHAMPION_FIRST_*` di FRLG) | — | motore di Red's Path; **un id allenatore in più** | vedi sotto |

Suggerimento del piano di Red's Path: 1 e 4 come base, 2, 3 e 5 come
aggiunte.

**Id allenatore.** Dopo la sessione A1 di Red's Path i flag del TcloneR
coprono gli id fino a 663 (`TCLONER_TRAINER_FLAGS_BYTES` 83) e
`TRAINERS_COUNT_HNS` è 661: restano **3 id** prima di dover allargare di
nuovo. La missione 5 ne usa uno; se la 1 ha lotte con allenatori nuovi,
contale. Oltre, si allarga ancora: nel vecchio filler dei flag del Pokédex
restano 21 byte (`reds-path.md` §4).

## 8. Scartato, e perché

Una riga per voce; i dettagli sono nell'analisi.

- **Trofei di Soulgold** (`achievements*.c`): SG non ha un quest book, e i
  trofei sono un elenco di obiettivi, non dicono "cosa fare adesso". Il
  loro modello a funzioni ha suggerito la sorgente di stato "funzione"
  (§3.1). Le quest di SG sono contenuto suo.
- **Menu missioni di ghoulslash** (`quest-menu`): fermo al 2023, superato da
  quello di PSF, che parte da lì.
- **Menu a schermo intero di Team Aqua's Hideout** (`full_start_menu`): 6
  caselle fisse, e i ritorni di 13 file da riscrivere. Scartato a favore
  della striscia.
- **Voce nel Pokénav**: il menu più completo usa già 6 voci su
  `MAX_POKENAV_MENUITEMS` = 6.
- Migliorie del wiki di pret valutate per l'occasione:
  - **Improved Audio Mixer**: c'è già. `src/m4a_1.s:88` è l'HQ-Mixer rev
    4.0 di ipatix.
  - **BetterBag**: superata dall'opzione `I_COMBINE_BAG_POCKETS` di
    expansion (oggi `TRUE`, 6 tasche). Separare Strumenti lotta e Tesori
    richiede una migrazione della `struct Bag` e la grafica per 8 tasche.
  - **free_saveblock**: superata dai flag `FREE_*` di
    `include/config/save.h`. Lo spazio oggi non serve, e ogni flag in più
    sposta i campi e rompe i salvataggi vecchi senza migrazione.
