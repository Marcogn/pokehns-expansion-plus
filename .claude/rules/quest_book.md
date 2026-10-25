---
paths:
  - "src/quest_book.c"
  - "include/quest_book.h"
  - "src/quest_log.c"
  - "include/quest_log.h"
  - "src/data/quests.h"
  - "include/constants/quests.h"
  - "graphics/quest_book/**"
---

# Quest book: schermata (fase 2 di `docs/plans/quest-log.md`)

`src/quest_book.c` è il port della schermata di PSF `complex_quests`
(`src/quests.c`). Lo stato lo legge da `quest_log.c`, mai da bit propri.

- **Cosa è cambiato rispetto a PSF**: righe limitate (buffer da 48 byte con
  `StringAppendN`, PSF scriveva in 32 senza controllo); un'unica allocazione
  per le righe (PSF la rifaceva a ogni filtro senza liberare); lista
  ricostruita sul posto, **senza** la dissolvenza a blend fra le liste;
  nessuna modifica a `ListMenu_ProcessInput` (la lista è circolare, Su
  dalla prima voce va su "Close", come **tutte** le liste del gioco: lo fa
  già il `ListMenu` di questo repo, non è la modifica di PSF); la palette 15 si
  carica da `gStandardMenuPalette` (PSF ereditava quella del campo e ne
  caricava 0x60 byte da un file da 32).
- **Comandi**: A su una missione con sottomissioni la apre, B torna (cursore
  ripristinato), R cambia filtro (tutte / non iniziate / attive / premio /
  completate), Start ordina A-Z (le bloccate in fondo, per non rivelare i
  nomi), Select preferita (va in cima, nome verde; il cursore la segue,
  `RebuildListOn`). "NEW" rosso finché il cursore non è passato sulla
  missione **al suo livello attuale**: livelli mai vista / attiva (o
  sbloccata) / premio / completata, 2 bit salvati (`seen` + `seenHigh`,
  `SAVE_VERSION 13`). È NEW solo un livello **più alto** di quello visto;
  quando si scende (TcloneR: premio ritirato, torna attiva) `QuestLog_SyncSeen`,
  all'apertura del libro, abbassa quello salvato senza NEW, così il premio
  dopo torna NEW. Limite: premio ritirato e il successivo già maturato senza
  aprire il libro in mezzo restano allo stesso livello, niente NEW.
- **Testi**: `struct QuestStep` (`desc`, `map`, `sprite`, `spriteType`)
  indicizzata da `QuestLog_GetStep`; `done` (un `QuestStep` con testo, mappa
  e immagine della missione completata: lo step su cui resta una completata
  è spesso lo 0, cioè la prima palestra); `struct QuestSubquestInfo`
  per le sottomissioni (nome, testo, mappa, immagine, etichetta a destra:
  compaiono solo a sottomissione fatta, prima c'è `??????`). `{STR_VAR_2}`
  nei testi vale la mappa dello step; `{STR_VAR_1}` e `{STR_VAR_3}` li riempie
  `bufferText` della missione, se c'è (TcloneR: cloni fatti e cloni del
  prossimo premio, `TClonerGetNextRewardClones`). Una sottomissione con
  `map` NULL non stampa "Location". Una missione senza `steps` si mostra
  vuota. Nomi di missione: **non oltre ~20 caratteri**, la riga ha a fianco
  il glifo A e "NEW" e l'etichetta di stato parte a x=200.
- **Immagini**: `QUEST_SPRITE_OBJECT/ITEM/MON`. Due slot alternati, quello
  vecchio distrutto dopo aver creato il nuovo; **la stessa immagine non si
  ricrea**, altrimenti `DestroySpriteAndFreeResources` libera foglio e palette
  che il nuovo sprite condivide. Gli oggetti si distruggono con quella
  funzione (con `OW_GFX_COMPRESS` il foglio resterebbe in VRAM, uno in più per
  ogni missione sfogliata).
- **Stato in cache**: `RebuildListOn` legge lo stato di ogni missione una
  volta (`ReadQuestStates` → `sBook->states`); ordinamento, righe,
  intestazione, etichette e dettaglio leggono la cache, e `QuestLog_IsNew`
  riceve lo stato invece di ricalcolarlo. Vale perché il libro non cambia
  lo stato delle missioni mentre è aperto: se un giorno lo farà (premio
  ritirato dal libro), va riletto dopo.
- **Entrate**: l'icona Missioni del menu a striscia
  (`.claude/rules/strip_menu.md`; la lista classica non ha più la voce); `questlog QUEST_LOG_OPEN` da script, da far
  seguire da `waitstate`.
- **Missioni di prova**: `-DQUEST_BOOK_TEST_QUESTS=1` non c'è nel Makefile;
  si cambia il `#define` in `include/constants/quests.h` (a 1 aggiunge due
  missioni: var con le tre immagini, funzione con 8 sottomissioni a
  medaglie). **Rimetterlo a 0 prima del commit.**
- **Grafica (fase 3)**: tile e tilemap sono quelli di PSF; cambiano solo le
  palette. Indici contati sul `.png` (= `.4bpp`): 1 e 6 = riquadro
  dell'immagine (**solo lì**), 2 carta, 3 righe tratteggiate, 4/5 cornice
  oro, 7/8 margine, 9-12 pannello blu. `menu_dark.pal` ridefinisce 1-12.
  Senza immagine `SetPictureBoxVisible(FALSE)` mette 1 e 6 al colore 10
  (pannello): il riquadro sparisce senza toccare la tilemap.
- **Testo scuro**: pal 15 è `sDarkTextPalette`. Il testo chiaro sta sulla
  entry 10 e **non** sulla 2, perché i glifi A e R sono una pillola bianca
  con la lettera sulla entry 2: con la 2 chiara la lettera non si legge
  (provato). Intestazione e lista usano `GetHeaderColors()`.
- **Verifica fatta (25/10)**: ROM di prova che parte dritta dal libro
  (`CB2_TestBoot` in `main.c`, poi ripristinato): lista, dettaglio con
  oggetto/oggetto/Pokémon, sottomissioni con scorrimento e frecce, B con
  cursore ripristinato, R su tutti e cinque i filtri, Start A-Z, Select;
  e dal menu di avvio di una nuova partita (voce, apertura, B che torna al
  menu). `QUEST_LOG_OPEN` da script provato in fase 8 (ROM strumentata:
  apre, B chiude, lo script riprende dopo `waitstate`); chiede
  `SCREFF_HARDWARE` come `fadescreen`, perché sfuma lo schermo. Pokémon e
  oggetto sul riquadro scuro guardati in fase 8.
- **mGBA**: `xdotool search --class mgba` restituisce **due** finestre; i
  tasti arrivano solo all'ultima (`tail -1`), con la prima sembra tutto
  bloccato.

## Missioni reali (fase 4)

- `src/data/quests.h`: quattro missioni a funzione. Per aggiungerne una
  copia il modello: funzione di progresso, `steps`, `subquestInfo`.
- **Footer: massimo due righe** di testo (provato: la terza esce dal
  riquadro), circa 38 caratteri per riga.
- **Stato `REWARD`**: con `desc` NULL il libro stampa il suo testo fisso
  "Your reward is waiting. Return to {mappa}!" (Red's Path); con un `desc`
  stampa quello (TcloneR: il premio lo paga lo scienziato che incontri,
  non un posto).
- **Palette condivise**: due immagini oggetto con la stessa palette non si
  possono creare "nuovo prima, vecchio dopo": il vecchio libera la palette
  del nuovo (magenta). `ShowPicture` distrugge prima l'oggetto vecchio.
- Verifica via GDB senza ROM strumentata: offset in SaveBlock1 `flags` 6540,
  `vars` 6844; `gSaveBlock1Ptr` va letto (ASLR). Il libro legge lo stato
  quando si apre, quindi chiudilo e riaprilo dopo ogni scrittura.
- **Tema via GDB**: `IsDarkUiEnabled` legge `gSaveblock3.challengeSettings.darkUi`,
  bit 2 del byte a `gSaveblock3 + 47` (0x02009253 nella build del 25/10;
  ricontrolla con `objdump -d --disassemble=IsDarkUiEnabled`). Azzerarlo
  porta il tema chiaro senza passare dal menu opzioni.
- **Completate**: mostrano `done` (Johto: Lance, Indigo Plateau; Kanto:
  Blue, Viridian; TcloneR: scienziato, Pallet; Red's Path: Red).
- **Kanto** si sblocca con `FLAG_IS_CHAMPION` (Hall of Fame di HnS), non con
  l'ottava medaglia: a Kanto si arriva con il biglietto della S.S. Aqua, che
  Elm dà dopo la Lega.
- **TcloneR**: 12 sottomissioni, una per premio di `sRewards` (`tcloner.c`),
  fatte se `VAR_TCLONER_REWARDS` le ha superate. **Le due liste vanno tenute
  allineate a mano.**
- **Verifica fase 4**: tema chiaro, Red's Path completata e Blue a Viridian
  guardati (25/10). Flag usati: badge 2176-2191, Red's Path intro 961,
  allenatori `0x500 + 653..660`, premi 962-969.
- **Verifica fase 8** (tema scuro, `.sav` dell'utente): TcloneR attiva con
  conteggio, premio (NEW che torna), ritirato (niente NEW), completata con le
  12 righe; Johto completata con Kanto ancora bloccata a 8 medaglie, Kanto
  attiva con `FLAG_IS_CHAMPION` (`0x89F`); Select su una missione fuori
  schermo. **Partita vera**: volo ad Azalea, dialogo con lo scienziato a
  (54,10), TcloneR da `??????` ad attiva con NEW. Var: `VAR_TCLONER_STAGE`
  `0x40C2`, `_COUNT` `0x40E5`, `_REWARDS` `0x40FC`.
- **GDB**: `gSaveBlock1Ptr` cambia a ogni ritorno al campo (ASLR): rileggilo
  subito prima di scrivere, o la scrittura va nella copia vecchia e sembra
  ignorata. `interrupt` è asincrono: aspetta "SIGINT" nell'output prima dei
  comandi.
