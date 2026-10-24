# Piano (bozza): registro delle missioni

**Stato (24/10): raccolta di idee e di fatti, niente di deciso né
pianificato.** Si progetta più avanti, dopo Red's Path.

## L'idea

Un **registro delle missioni** consultabile dal giocatore: per ogni missione,
a che punto è e **cosa fare adesso** (dove andare, chi cercare), passo per
passo. Dentro ci vanno:

1. **la storia principale**: le 16 medaglie (8 di Johto, 8 di Kanto), con
   l'indicazione della prossima palestra;
2. il **Pokédex**, a scaglioni di 50;
3. le **catture dei leggendari**;
4. le **missioni secondarie**: quelle che esistono già (TcloneR, Red's Path)
   e quelle da inventare.

### Un principio da tenere

Quasi tutto lo stato esiste già nel salvataggio: flag delle medaglie
(`FLAG_BADGE01_GET`…`FLAG_BADGE16_GET`) e dei capi battuti
(`FLAG_DEFEATED_*_GYM`, `include/constants/flags_hns.h`), conteggio del
Pokédex, flag degli eventi leggendari, flag e var di Red's Path e del
TcloneR. Il registro dovrebbe **leggere** questi dati per decidere passo e
testo di ogni missione, non duplicarli: così costa poco nel salvataggio e
funziona anche su partite vecchie. Le missioni nuove (inventate) avranno
invece bisogno di flag propri.

### Da progettare (nulla verificato nel codice)

- Dove si apre: oggetto chiave, voce del menu di pausa, pagina del Pokégear.
- Come si presenta: categorie, missioni attive/completate, una riga "cosa
  fare adesso" per missione.
- Se c'è una missione "seguita" in evidenza.
- Se esiste qualcosa di riusabile: nella config di expansion
  (`include/config/`) non c'è un registro missioni.

## Confronto con Soulgold (24/10)

**Soulgold non ha un registro delle missioni.** Verificato sul mirror e
sull'upstream `Eemeliri/soulgold` (`master` = `4d4acbe98`, lo stesso punto
già valutato in `state.json`; nessun branch remoto con un quest log). Le
"quest" di SG sono solo script e var senza interfaccia
(`VAR_BAOBA_QUEST_STATE`, `FLAG_STARTED_VICTINI_QUEST`,
`VAR_POSTGAME_RIVAL_LEGENDARYQ` per Cresselia): sono contenuto, e le
missioni si valutano una per una, non si portano in blocco.

La cosa più vicina in SG sono i **Trofei** (achievement): un elenco di
obiettivi con stato e progresso, che è metà di un registro. Non dice "cosa
fare adesso". Il §5 del port report lo classifica come contenuto di SG;
qui interessa solo il **motore**.

| Pezzo di SG | File | Riusabile per il registro? |
| --- | --- | --- |
| Tabella dichiarativa: id, nome, descrizione, categoria, contatore + soglia, oppure un **predicato** `bool32 (*)(void)` | `src/achievements.c`, `include/achievements.h` | Sì, come forma. I predicati leggono lo stato che c'è già (`FlagGet(FLAG_BADGE0x_GET)`, `GetSetPokedexFlag(..., FLAG_GET_CAUGHT)`, var di script): è il principio scritto sopra. Al registro serve un passo in più: una funzione che restituisce il **passo corrente** (indice di testo), non solo vero/falso. |
| Salvataggio: 48 byte in SaveBlock1 (6 contatori `u32`, 16 byte di bit sbloccati, coda di 6 popup, magic `0xACE1` che inizializza i salvataggi vecchi) | `include/global.h`, `include/constants/achievements.h` | No così com'è. Un registro che legge lo stato esistente non ha bisogno di contatori né di bit "completato": al massimo una missione seguita e i bit "già letto/nuovo". In Hasep un campo nuovo va in coda a SaveBlock3 con migrazione (CLAUDE.md §4), non con un magic. |
| Controllo: `Achievement_CheckAll()` chiamato da cattura, evoluzione, oggetti, strutture lotta; `Achievement_OnTrainerDefeated()` | sparsi in 10+ file | Non serve per il registro: lo stato si calcola all'apertura. Servirebbe solo per avvisare "missione aggiornata". |
| Popup in overworld con coda salvata, agganciato a `OverworldBasic()` | `src/achievements_popup.c` | Possibile, da valutare a parte (usa `comfy_anim`, che in Hasep c'è: `include/comfy_anim.h`). |
| Schermata: lista di 6 righe, icona per categoria (Poké/Mega/Ultra/Master Ball), stato a destra (`Done`, `x/y`, `Locked`), descrizione in basso | `src/achievements_menu.c` | Come riferimento di layout. Prende lo sfondo da `graphics/pokegear/tiles.*` di SG: in Hasep va derivato dalla grafica nostra (CLAUDE.md §2). |
| Ingresso: app `POKEGEAR_APP_TROPHIES` del Pokégear di SG | `src/pokegear.c` | No: Hasep usa il Pokénav di HnS (§5 del port report). |

### Dove aprirlo in Hasep (fatti, nessuna scelta)

- **Pokénav di HnS**: il menu più completo (`POKENAV_MENU_TYPE_UNLOCK_MC_RIBBONS_RADIO`,
  `src/pokenav_menu_handler.c`) usa già 6 voci su `MAX_POKENAV_MENUITEMS` = 6.
  Una settima voce vuol dire alzare il massimo e una grafica di pulsante
  nuova: costo non misurato.
- **Menu di pausa**: c'è già una voce aggiunta da HnS (`MENU_ACTION_DEXNAV`,
  `src/start_menu.c`); una voce in più è la strada più corta.
- **Oggetto chiave**: come la Mappa città.

## Menu missioni esistenti per i decomp (letti nel codice, 24/10)

Tutti e due sono per **pret/pokeemerald**, non per expansion: si portano a
mano (lo dice la stessa documentazione di PSF).

### ghoulslash, `quest-menu`

`github.com/ghoulslash/pokeemerald`, branch `quest-menu`, ultimo commit
2023-05-11; 24 file, +2103 righe, di cui `src/quests.c` 1499
([thread](https://www.pokecommunity.com/threads/pokefirered-pokeemerald-quest-menu.434462/)).

- 30 missioni con nome, descrizione **fissa**, persona da cercare, mappa,
  premio (icona dell'oggetto). Stati: sbloccata, completata, una sola
  "attiva".
- Salvataggio: due bitfield da 4 byte e un byte in coda a SaveBlock2.
- Schermata: è il PC degli oggetti di FireRed (cornici `fr_std*`).
- Voce `QUESTS` nel menu di pausa dietro `FLAG_SYS_QUEST_MENU_GET`; comando
  di script `questmenu` (opcode `0xe3` della tabella di pret).
- Superato dal successivo: non vale la pena partire da qui.

### PokemonSanFran, "Unbound Quest Menu"

`github.com/PokemonSanFran/pokeemerald`: branch `unbound-quest-menu`
(allineato a pret il 2026-08-02) e `complex_quests` (RubyRaven6, ultimo
commit 2026-10-20), +3977 righe, `src/quests.c` 2971. Documentazione:
[wiki di PSF](https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu),
elencata fra i feature branch del
[wiki di pret](https://github.com/pret/pokeemerald/wiki/Feature-Branches).
Parte da ghoulslash e imita il menu di Pokémon Unbound.

- **Stati**: bloccata (`????`), sbloccata, attiva, premio da ritirare,
  completata; più "preferita" (Select, va in cima). 5 bit per missione,
  1 bit per sottomissione: 25 byte in SaveBlock2 per 30 + 30.
- **Sottomissioni** (A su una missione): solo bloccata/completata, con
  contatore `x/y`. Esempio della doc: i sette simboli d'oro del Frontier.
- **Filtri** con R (per stato), **ordine alfabetico** con Start.
- Ogni voce: mappa, sprite (NPC, oggetto **o** Pokémon), descrizione
  attiva e descrizione "completata".
- **`complex_quests` è la parte che ci interessa**: ogni missione ha un
  `questVariable` e array `desc[]`, `map[]`, `sprite[]` indicizzati da
  `VarGet(questVariable)` (`GetQuestDesc`, `GetQuestLocation`). È il "cosa
  fare adesso" guidato da una var che già esiste: va bene così com'è per
  Red's Path e per il TcloneR (`VAR_TCLONER_STAGE`). Per medaglie, Pokédex
  e leggendari lo stato non è una var ma una funzione: servirebbe un
  callback al posto della var.

Cose da sapere prima di portarlo:

- **Cambia tutte le liste del gioco**: `ListMenu_ProcessInput`
  (`src/list_menu.c`) viene modificata perché ogni lista ricominci dall'alto
  dopo l'ultima voce. La doc lo segnala; in Hasep toccherebbe anche borsa,
  PC e menu nostri. Va tolto o messo dietro un parametro.
- **Salvataggio in SaveBlock2**, azzerato a mano in `NewGameInitData`
  (`QuestMenu_ResetMenuSaveData`). Da noi un campo nuovo va in coda a
  SaveBlock3 con migrazione (CLAUDE.md §4). Se si legge lo stato esistente,
  i 5 bit per missione si riducono a "preferita" e "nuovo".
- **Nessun controllo di limite**: `desc[VarGet(...)]` su un array di
  `MAX_QUEST_STATES` = 50 voci, ricade sull'indice 0 solo se la voce è
  `NULL`. Una var oltre 49 legge fuori dall'array.
- **ROM**: ogni missione porta 50 + 50 puntatori e 50 sprite anche se ne
  usa 3. Circa 570 byte a missione, ~17 KB per 30: poco, ma la ROM è al
  99,46% con le mega (`.claude/rules/megas.md`).
- Comando di script con opcode nella tabella di pret: in expansion la
  tabella è diversa, va rinumerato. `CreateMonIcon` in expansion ha un
  argomento in meno (FAQ della doc).
- Bug noto della doc: un artefatto per un frame passando da uno sprite NPC
  a uno non NPC, mai riprodotto con regolarità.
- Sfondo: versione modificata di quello FRLG di Skeli; ci sono varianti
  (stile Pokédex Plus, scuro). Va accreditato l'autore. Da noi va comunque
  rifatto sulla nostra grafica e sul tema scuro (CLAUDE.md §2 e §5).

## Il menu di pausa

Hasep e Soulgold hanno tutti e due il menu di pausa classico, una lista
(`BuildNormalStartMenu`, `src/start_menu.c`); HnS aggiunge solo la voce
DexNav. Soulgold non lo ha rifatto: le sue app stanno nel Pokégear.

Rifacimenti esistenti:

- **Full Screen Start Menu** (Archie e Mudskip, Team Aqua's Hideout):
  branch `full_start_menu` di `TeamAquasHideout/pokeemerald`, ultimo
  commit 2024-10-14. Il
  [wiki di pret](https://github.com/pret/pokeemerald/wiki/Full-Screen-Start-Menu-by-Archie-and-Mudskip)
  lo dice compatibile con expansion; con il Pokédex HGSS (Hasep usa
  `pokedex_plus_hgss.c`) va cambiato il ritorno in `Task_ClosePokedex`. Usa
  gli stessi flag del menu classico (voce grigia se manca), salva senza la
  conferma di sovrascrittura. Non aperto nel codice.
- `pollythadon/pokeemerald-expansion`, branch `shattered-emerald`
  (expansion 1.17.1, commit del 2026-10-13): contiene
  `src/unbound_start_menu.c` e `src/rotom_start_menu.c`. Una ricerca lo
  descrive come "alpha"; non aperto, base più nuova della nostra (1.15.2).

Il legame con il registro: in un menu a schermo intero le voci hanno posti
fissi, quindi il registro va previsto nel disegno. Il costo vero è un
altro: ogni schermata aperta dal menu deve tornarci (`CB2_ReturnTo...`),
quindi vanno ritoccati tutti i ritorni, compresi quelli nostri (party menu
SwSh, DexNav, borsa, Pokédex).

## Fattibilità e costi: quest book di Unbound + menu a schermo intero (24/10)

Richiesta dell'utente: il quest book di Unbound (la ricreazione di PSF) e un
menu di pausa a schermo intero, disegnati insieme. **Fattibile.** Non c'è
niente di bloccante; il costo è lavoro di port e di grafica, non spazio.

### Spazio misurato (build `make hns` del 24/10, `master` 2417f28e)

| | Usato | Limite | Libero |
| --- | --- | --- | --- |
| ROM | 31 862 468 B (94,96%) | 32 MB | ~1,6 MB (le mega, non unite, ne prendono ~1,5) |
| SaveBlock1 | 15 760 B | 15 872 (4 settori) | 112 B |
| SaveBlock2 | 3 892 B | 3 968 (1 settore) | 76 B |
| SaveBlock3 | 60 B | 116 (`SaveBlock3FitsFirstChunk`) | 56 B |

Misure: `nm -S pokehns.elf` su `gSaveblock1/2` meno i 128 byte di ASLR
(`SAVEBLOCK_MOVE_RANGE`, `include/load_save.h`); ROM fino al padding `0xFF`.

Al quest book servono pochi byte: con lo stato letto dal gioco bastano i bit
"preferita" e "nuova" (8 byte per 30 missioni); con il modello completo di
PSF, 25 byte per 30 missioni + 30 sottomissioni. Ci stanno tutti e due in
SaveBlock3, in coda, con migrazione. Il codice e la grafica dei due menu
valgono decine di KB di ROM (stima, non misurata).

### Quest book: cosa si fa

Base: `complex_quests` di PSF, portata a mano su expansion 1.15.2.

1. **Motore**: `quests.c`, tabella delle missioni, schermata, comando di
   script rinumerato nella tabella di expansion, `CreateMonIcon` con un
   argomento in meno.
2. **Tre sorgenti di stato per missione**, in modo che il registro legga il
   gioco invece di duplicarlo: una var (già in PSF: Red's Path, TcloneR), una
   funzione (da aggiungere: medaglie, Pokédex, leggendari), oppure i bit di
   PSF per le missioni nuove scritte apposta.
3. **Togliere l'effetto su tutte le liste** (`ListMenu_ProcessInput`): la
   lista che ricomincia dall'alto resta solo nel quest book.
4. **Limite sull'indice** di `desc[]`/`map[]`/`sprite[]` (oggi nessuno).
5. **Grafica**: sfondo fatto sulla nostra grafica, tema chiaro e scuro
   (`IsDarkUiEnabled()`), testi in inglese.
6. **Contenuto iniziale**: storia principale (16 medaglie), Red's Path,
   TcloneR. Le altre missioni una per una, come deciso.

### Menu a schermo intero: cosa si fa

Le basi esistenti sono due, nessuna da prendere così com'è:

- **Team Aqua's Hideout** (`ui_startmenu_full.c`, 1577 righe, compatibile
  con expansion per il wiki di pret): 6 icone fisse (Pokédex, Squadra,
  Borsa, Scheda, Mappa/Pokénav, Opzioni) più icone della squadra con barre
  PS, ora, nome della mappa. **Salva non è un'icona**: sta su Start
  (`Task_StartMenuFullMain`, poi conferma con A, annulla con B); Select
  cambia l'orologio 12/24 h se `FLAG_CLOCK_MODE` è configurato. L e R non
  sono usati. Dai suoi ritorni, la Zona Safari, la Piramide,
  la Battle Pike e la Union Room restano sul menu classico.
- **`pollythadon/pokeemerald-expansion`** (`unbound_start_menu.c`, 1317
  righe, expansion 1.17.1): barra di icone in stile Unbound, 6 per pagina
  con **più pagine**, voci riordinabili dal giocatore (ordine salvato in
  SaveBlock3). È dentro un gioco (`shattered-emerald`), non un feature
  branch; descritto come alpha. **Non è a schermo intero**: è una striscia
  in basso disegnata sopra la mappa (icone a `USM_ICON_YPOS` 128, BG0 del
  campo, `FreezeObjectEvents`), come il menu di Unbound. Le schermate
  tornano con `CB2_ReturnToFieldWithOpenMenu` come oggi. Tasti: frecce
  (a destra dell'ultima icona passa alla pagina dopo), L pagina, R sposta
  voce, B esce; finestre per nome, orologio e suggerimenti.

A Hasep servono 8 icone più Salva su Start, come fa Team Aqua: Pokédex,
DexNav, Squadra, Borsa, Pokégear, Scheda, **Missioni**, Opzioni (più Debug
nelle build di debug e i ritiri di Safari e Bug-Catching Contest). Le 6
caselle fisse di Team Aqua non bastano: o si
ridisegna la griglia, o si usano le pagine dello stile Unbound. Per un menu
"compagno" del quest book di Unbound il secondo è il più vicino.

Il costo vero sono i **ritorni**: `CB2_ReturnToFieldWithOpenMenu` compare in
13 file (`start_menu.c` 5, `naming_screen.c` 4, `swsh_party_menu.c`,
`party_menu.c` e `pokenav.c` 2, `pokedex_plus_hgss.c`, `dexnav.c`,
`item_menu.c`, `region_map.c` e altri 1). Ognuno va portato al nuovo menu,
con le eccezioni dove deve restare la lista classica (Safari, Bug-Catching
Contest, Piramide, …), e verificato in emulatore uno per uno.

### Due strade, scelta dell'utente (24/10)

| | A. Schermo intero (base Team Aqua) | B. Striscia in basso (base stile Unbound) |
| --- | --- | --- |
| Aspetto | schermata propria, griglia di icone | sopra la mappa, 6 icone visibili per pagina |
| Ritorni da riscrivere | 13 file (vedi sopra) | nessuno: restano quelli di oggi |
| Voci | griglia da ridisegnare per 8 | 8 su due pagine, o 6 + scorciatoie |
| Costo stimato | 3-4 sessioni | 1-2 sessioni |

Preferenze dell'utente, valide per tutte e due: **L apre il Pokégear, R le
Opzioni, Salva su Start; niente griglia della squadra** (la voce Squadra
c'è già, mostrarla accanto è doppio). Con L e R come scorciatoie, sulla
striscia il cambio pagina resta alle frecce e il riordino delle voci si
perde o va su un altro tasto. Restano 6 icone (Pokédex, DexNav, Squadra,
Borsa, Scheda, Missioni): con Pokégear e Opzioni sui dorsali stanno in una
pagina sola.

### Decisione (24/10): strada B, la striscia

Decisioni definitive dell'utente, dopo due anteprime
(`New Bark Town` vera, icone di `pollythadon`, font del gioco):

- **Strada B**: striscia in basso sopra la mappa, stile Unbound.
- **Tasti come immagine del tasto**, non come testo: i glifi
  `{START_BUTTON}`, `{L_BUTTON}`, `{R_BUTTON}` del gioco
  (`graphics/fonts/keypad_icons.png`, `sKeypadIcons` in `src/text.c`).
- **Start** salva, **L** apre la **Scheda allenatore**.
- **Missioni e Opzioni sono le ultime due icone.** Ordine dell'anteprima:
  Pokédex, DexNav, Squadra, Borsa, Missioni, Opzioni.
- **R apre il Pokégear** (confermato dall'utente).
- Con 6 icone la striscia ha una pagina sola: il cambio pagina e il riordino
  delle voci di `pollythadon` non servono.

## Fasi di lavoro (24/10)

Vincoli dell'utente: modelli **non oltre Opus 5.5**, sforzo **medio**,
pochi token al mese. Quindi: una fase per sessione, Opus solo dove un
errore costa caro (salvataggio, integrazione col menu, revisione finale),
Sonnet per il port e la grafica, Haiku per testi e documentazione. Ogni
sessione parte da questo file e dalla regola indicata, senza rifare le
ricerche: i fatti sono già qui sopra.

| # | Fase | Modello | Esce | Cancello per passare oltre |
| --- | --- | --- | --- | --- |
| 1 | **Fondamenta del quest book**: struct in coda a SaveBlock3 + `STATIC_ASSERT` + passo di migrazione (`saveVersion < 10`), API delle tre sorgenti di stato (var, funzione, bit), comando di script nella tabella di expansion | **Opus 5.5** | build verde, salvataggio vecchio che carica | un `.sav` di prima apre e il campo legge zero (emulatore) |
| 2 | **Port della schermata** di `complex_quests`: lista, dettaglio, sottomissioni, filtri R/Start, preferite; senza wrap globale delle liste; limite sull'indice; `CreateMonIcon` di expansion | **Sonnet 5.5** | schermata aperta da una voce provvisoria del menu classico | due missioni di prova navigabili, screenshot guardati |
| 3 | **Grafica del quest book**: sfondo sulla nostra grafica, tema chiaro e scuro | **Sonnet 5.5** | `.png`/`.gbapal` nostri | indici contati sui `.4bpp`, render offline e screenshot in tutti e due i temi (CLAUDE.md §2, `battle_ui.md`) |
| 4 | **Prime missioni**: 16 medaglie (ordine di Johto ricavato dagli script), Red's Path, TcloneR | **Sonnet 5.5** | voci in tabella con funzioni/var | ogni passo verificato con un salvataggio o con il menu debug |
| 5 | **Striscia del menu di pausa**: port di `unbound_start_menu.c` da 1.17.1 a 1.15.2, una pagina sola, niente riordino, Start salva, L Scheda, R Pokégear, glifi dei tasti, lista classica dove serve (Safari, Bug-Catching Contest, Piramide, Pike, Union Room) | **Opus 5.5** | menu nuovo al posto del vecchio | ogni voce aperta e chiusa in emulatore, eccezioni comprese |
| 6 | **Grafica della striscia**: fascia e icone sulla nostra grafica, icone nuove per DexNav e Missioni, tema scuro | **Sonnet 5.5** | grafica nostra al posto di quella di `pollythadon` | come fase 3 |
| 7 | **Chiusura**: `CHANGELOG.md`, `docs/SOULGOLD_PORT_REPORT.md` (non è da SG: annotare l'origine PSF/pollythadon e i crediti), regola nuova in `.claude/rules/` per le due aree | **Haiku 5.5** | documenti | testi riletti contro il codice |
| 8 | **Revisione finale** del diff completo (`/code-review` medio) prima della release | **Opus 5.5** | eventuali correzioni | build verde, ROM consegnata zippata |

Le fasi 1-4 (quest book) e 5-6 (menu) sono indipendenti dopo la 1: il
quest book si prova da solo con una voce provvisoria, e il menu lo ospita
quando arriva. Se i token stringono, la 4 si può fare a più riprese, una
missione per volta.

Crediti da riportare nel gioco o nelle note di release: PSF (Unbound
Quest Menu) e chi cita la sua documentazione (Skeli, ghoulslash e gli
altri); `pollythadon` per la striscia (e hedara, vol per icone e sprite,
secondo la sua descrizione: da verificare nel suo repo).

### Dopo le fasi: aggiungere una missione

Ricetta per il modello che la aggiunge (Sonnet o Haiku bastano, se la
condizione esiste già):

1. trova la condizione già nel gioco (flag, var, medaglia, Pokédex) con
   `grep` negli script `_hns`, e **verifica dove viene impostata**: il nome
   può ingannare. Esempio: `FLAG_VISITED_KANTO` (`flags_hns.h`) si accende
   ai primi passi sul **Percorso 27** (`Route27_EventScript_FirstSteps`),
   non ad Aranciopoli: per "arriva ad Aranciopoli" serve
   `FLAG_VISITED_VERMILION_CITY`;
2. se la condizione non c'è (missione inventata), un flag o una var liberi
   da `flags_hns.h`/`vars_hns.h`, impostati dallo script nuovo;
3. voce in tabella + testi (in inglese) + id in `constants/quests.h`;
   **gli id si aggiungono in coda, mai in mezzo**, o i bit salvati finiscono
   sulla missione sbagliata;
4. verifica in emulatore con un caso che fallisce prima della condizione.

Capienza (stima sui numeri misurati sopra): una missione che legge lo stato
del gioco costa 2 bit di salvataggio (preferita, nuova), una con stato
proprio 5, una sottomissione 1. I 56 byte liberi di SaveBlock3 sono 448 bit:
circa 200 missioni del primo tipo o 90 del secondo, meno quello che servirà
ad altro. Gli indici di PSF sono `u8`, quindi 255 al massimo. In ROM ~570
byte a missione. Oltre, SaveBlock3 si può allargare fino a 1624 byte
togliendo l'assert sul primo chunk: non provato.

### Ordine consigliato

1. Disegno comune dei due schermi (bozza su carta o mockup), così il menu
   prevede il posto delle Missioni e i due condividono stile e palette.
2. **Quest book** con una voce nel menu classico: si prova da solo.
3. **Menu a schermo intero**, che poi lo ospita.

Stima: 2-3 sessioni il quest book (motore + grafica + prime missioni), 3-4
il menu (schermata + ritorni + verifica in emulatore). Stima a giudizio,
non misurata.

## Altre migliorie viste nel wiki di pret (24/10)

Lette nel codice di Hasep: **due su tre sono già superate da expansion**.

- **Improved Audio Mixer** (Kurausukun, `newmix`): **già presente.**
  `src/m4a_1.s:88` è l'HQ-Mixer rev 4.0 di ipatix; `src/m4a.c` ha
  `SoundMainRAM_Buffer[0xB40]` e `hq_buffer_ptr[0x130]` (18 157 Hz), con
  `PCM_DMA_BUF_SIZE` 2128 e 15 canali DirectSound; le scritture DMA atomiche
  sono in `include/gba/macro.h`. Non c'è niente da portare.
- **BetterBag** (AsparagusEduardo): **superata.** Expansion ha le tasche
  come opzione: `I_COMBINE_BAG_POCKETS` (`include/config/item.h`) è `TRUE`
  in Hasep, cioè 6 tasche (Strumenti, Medicine, Poké Ball, MT/MN, Bacche,
  Strumenti chiave). Con `FALSE` si aggiungono **Strumenti lotta** e
  **Tesori**; i posti totali restano uguali (236 + 92 contro
  200 + 36 + 60 + 32), ma le tasche nuove stanno in mezzo alla
  `struct Bag`, quindi serve una migrazione che ridistribuisca gli oggetti di
  un salvataggio vecchio. Va rifatta anche la grafica della borsa per 8
  tasche. Soulgold ha 8 tasche, ma diverse: Strumenti lotta e
  **Megapietre**, niente Tesori.
- **free_saveblock** (ghoulslash): **superata.** Expansion ha
  `include/config/save.h` con i flag `FREE_*`; Hasep ne usa già quattro
  (seen flag in SaveBlock1, buffer Mystery Event, Bacca Enigma, Dono
  Segreto). Oggi lo spazio non serve: il quest book ci sta (vedi sopra).
  Ogni altro `FREE_*` toglie un campo, e se non è in coda alla struct sposta
  tutto quello che segue: rompe i salvataggi vecchi senza una migrazione.
  `FREE_MATCH_CALL` va lasciato, perché il Pokégear di HnS usa le chiamate.

## 1. Storia principale: le 16 medaglie

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
  `VAR_NUM_BADGES == 15`; verificato per Red's Path). Il passo "prossima
  palestra" in Kanto è quindi un elenco di quelle mancanti, con Blue per
  ultimo.
- **Johto**: l'ordine e i blocchi di storia (cosa serve per arrivare a ogni
  palestra) vanno ricavati dagli script di HnS in fase di progettazione:
  non ancora verificati.
- Il testo di ogni passo dice dove andare e, se serve, cosa sblocca la
  strada (una MN, un evento).

## 2. Pokédex a scaglioni di 50

| Pokédex | Voci | Scaglioni |
| --- | --- | --- |
| Johto (`JOHTO_DEX_COUNT`, `include/constants/pokedex.h`) | 282 | 50, 100, 150, 200, 250, 282 |
| Nazionale (tutte le generazioni attive, fino a Pecharunt) | 1025 | 50, 100, … 1000, 1025 |

Da decidere: quale Pokédex conta (Johto, nazionale, o entrambi in due
missioni), visti o catturati, e se ogni scaglione ha un premio (come gli
scaglioni del TcloneR).

## 3. Leggendari

Dove compaiono negli script di HnS (ricerca di `SPECIES_*` nelle mappe
`_hns`; alcune mappe li citano solo nel testo, come la casa di Bill o il
teatro di Amarantopoli: il luogo della lotta va confermato mappa per mappa).

| Pokémon | Mappa |
| --- | --- |
| Articuno | `SeafoamIslands_B1F` |
| Zapdos | `Route10` |
| Moltres | `MtSilver_1F_MoltresRoom` |
| Mewtwo | `CeruleanCave_B2F` |
| Mew | `FarawayIsland_Interior` |
| Raikou, Entei | `BurnedTower_B1F` (vaganti) |
| Suicune | `BurnedTower_B1F`, poi `CianwoodCity`, `Route42`, `VermilionCity`, `Route25`, `Route14` (inseguimento) |
| Lugia | `WhirlIslands_LugiaChamber` |
| Ho-Oh | `TinTower_RoofDay` |
| Celebi | `IlexForest` |
| Kyogre | `Route19_Cave` |
| Groudon | `SeafoamIslands_SecretCave` |
| Rayquaza | `EmbeddedTower` |
| Latias, Latios | `SouthernIsland_Interior` |
| Deoxys | `BirthIsland_Exterior` |
| Jirachi | `MtMoon_Outside` |
| Regirock, Regice, Registeel, Regieleki, Regidrago, Regigigas | `SinjohRuins_*` |
| Arceus | `SinjohRuins_ArceusRoom` |

Ogni leggendario è una missione (o un passo di una missione per gruppo:
uccelli di Kanto, cani, Regi), con le condizioni per farlo comparire. Le
condizioni vanno lette negli script di ciascuna mappa.

## 4. Missioni secondarie

### Esistenti

- **TcloneR** (`docs/plans/tcloner.md`, `.claude/rules/trainer_catch.md`):
  lo scienziato in cinque posti, gli scaglioni di cloni e i premi. Le tappe
  e i contatori ci sono già (`VAR_TCLONER_STAGE`, `VAR_TCLONER_COUNT`,
  `VAR_TCLONER_REWARDS`).
- **Red's Path** ([`reds-path.md`](reds-path.md)): otto sfide in ordine.
  Passi del registro: "batti un capo di Kanto" (prima dell'introduzione) →
  "prossima sfida: <capo> a <città>" (con "batti prima il capo di HnS" se
  serve) → "premio da ritirare" (squadra piena) → completata. Lo stato sono
  i flag degli otto allenatori di Red's Path e dei premi, già previsti nel
  piano.

### Da Red's Path al Monte Argento

Nate dal piano di Red's Path (24/10). Red's Path finisce con Charizard,
Pikachu-Starter, Snorlax, Eevee, Poliwrath, Lapras; al Monte Argento HnS gli
dà Pikachu-Starter, Snorlax, Charizard, Venusaur, Blastoise, Espeon
(`TRAINER_RED_HNS` in `src/data/trainers_hns.party`). Queste missioni colmano
il salto: da dove vengono **Venusaur e Blastoise** (in FireRed, partendo con
Charmander, non si ottengono), quando **Eevee diventa Espeon** (evoluzione di
Gen 2), dove finiscono **Poliwrath, Lapras, Butterfree** e la panchina, e
come si arriva a Red.

Agganci già in HnS: la **casa di Red** a Biancavilla
(`PalletTown_RedsHouse_1F_hns`: la madre dice che Red è via da tanto e non ha
sue notizie), il **laboratorio di Oak** (`PalletTown_Lab_hns`), la casa di
**Bill** sul Percorso 25, la **Villa di Azzurropoli** (dove Red ha avuto
Eevee). In HnS **nessuno script regala gli starter di Kanto** (solo il menu
debug, `data/scripts/debug.inc`).

1. **Il PC di Bill.** Bill custodisce i Pokémon del viaggio di Red
   (Butterfree, Mankey, Diglett/Dugtrio, Gloom, Poliwrath, Lapras: le
   panchine di Red's Path). Il giocatore li vede o combatte una lotta
   speciale con loro (motore di Red's Path). Racconta: dove finisce la
   squadra del viaggio.
2. **Gli starter di Oak.** Red è tornato da Oak dopo la Lega a chiedere i
   due starter rimasti. In *Pokémon Origins* Blue sceglie Squirtle, quindi
   Bulbasaur sarebbe rimasto al laboratorio; per Blastoise serve
   un'invenzione. Il giocatore porta a Red un oggetto di Oak fino al Centro
   Pokémon del Monte Argento. Racconta: Venusaur e Blastoise. Attenzione:
   tocca Oak e l'accesso al Monte Argento (progressione).
3. **Eevee al sole.** La signora della Villa di Azzurropoli riceve la foto
   di un Espeon e chiede notizie; oppure un indizio in Johto. Racconta:
   Espeon.
4. **La lettera alla madre.** Red racconta in poche righe i tre anni dopo il
   viaggio e i nuovi compagni. Racconta tutto, solo testo.
5. **Red contro Blue.** Una nona lotta dopo Red's Path: Red contro Blue
   campione, con la squadra di Giovanni. Le squadre FRLG del campione ci sono
   già (`TRAINER_CHAMPION_FIRST_*` in `src/data/trainers_frlg.party`; per
   Red con Charmander, Blue ha `..._SQUIRTLE`).

Suggerimento iniziale: 1 + 4 come base, 2, 3 e 5 come aggiunte.

### Da inventare

(da aggiungere insieme all'utente)
