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

# Quest book (piano in `docs/plans/quest-log.md`)

`src/quest_book.c` è il port della schermata di PSF `complex_quests`
(`src/quests.c`). Lo stato lo legge da `quest_log.c`, mai da bit propri.
**Le verifiche per fase (25/10), le ricette GDB e i flag usati stanno
nell'appendice del piano.** Ricetta per aggiungere una missione: skill
`add-quest` e `quest-log.md` §6.

## Comandi e stato

- A apre le sottomissioni, B torna (cursore ripristinato), R cambia filtro, Start
  ordina A-Z (le bloccate in fondo, per non rivelare i nomi), Select preferita.
  La lista è circolare (Su dalla prima voce va su "Close", come tutte le liste).
- **"NEW"** rosso finché il cursore non è passato sulla missione **al suo
  livello attuale** (mai vista / attiva / premio / completata, 2 bit: `seen` +
  `seenHigh`, `SAVE_VERSION 13`). `QuestLog_SyncSeen` all'apertura abbassa il
  salvato senza NEW quando il livello scende (TcloneR: premio ritirato). Limite:
  premio ritirato e successivo già maturato senza aprire il libro restano
  allo stesso livello, niente NEW.
- **Stato in cache**: `RebuildListOn` legge ogni missione una volta
  (`ReadQuestStates` → `sBook->states`). Se il libro un giorno cambierà lo
  stato (premio dal libro), va riletto.
- Entrate: icona Missioni della striscia (`strip_menu.md`); `questlog
  QUEST_LOG_OPEN` da script, seguito da `waitstate` (chiede `SCREFF_HARDWARE`).
- **Missioni di prova**: `#define QUEST_BOOK_TEST_QUESTS` in
  `include/constants/quests.h` (non c'è nel Makefile). **Rimetterlo a 0 prima del commit.**

## Testi

- `struct QuestStep` (`desc`, `map`, `sprite`, `spriteType`) da
  `QuestLog_GetStep`; `done` = step della missione completata (spesso lo 0);
  `struct QuestSubquestInfo` (nome, testo, mappa, immagine, etichetta; prima
  di averla fatta si vede `??????`). `{STR_VAR_2}` = mappa dello step;
  `{STR_VAR_1}`/`{STR_VAR_3}` li riempie `bufferText` (TcloneR: cloni fatti e del
  prossimo premio). `map` NULL non stampa "Location".
- **Nomi di missione: non oltre ~20 caratteri** (a fianco il glifo A e
  "NEW"; l'etichetta di stato parte a x=200). **Footer: massimo due righe**
  (~38 caratteri; la terza esce dal riquadro).
- Stato `REWARD`: con `desc` NULL il libro stampa "Your reward is waiting.
  Return to {mappa}!" (Red's Path); con un `desc` stampa quello (TcloneR).
- **Kanto** si sblocca con `FLAG_IS_CHAMPION`, non con l'ottava medaglia.
- **TcloneR**: 12 sottomissioni, una per premio di `sRewards`: **le due liste
  vanno tenute allineate a mano**.

## Immagini e grafica

- `QUEST_SPRITE_OBJECT/ITEM/MON`, due slot alternati, vecchio distrutto dopo il
  nuovo; **la stessa immagine non si ricrea** (`DestroySpriteAndFreeResources`
  libera foglio e palette condivisi). Due immagini oggetto con la stessa
  palette non si creano "nuovo prima, vecchio dopo" (magenta): `ShowPicture`
  distrugge prima il vecchio. Gli oggetti si distruggono con quella funzione
  (con `OW_GFX_COMPRESS` il foglio resterebbe in VRAM).
- Tile e tilemap sono di PSF; cambiano le palette. Indici contati sul `.png`
  (= `.4bpp`): 1 e 6 = riquadro dell'immagine (**solo lì**), 2 carta, 3 righe
  tratteggiate, 4/5 cornice oro, 7/8 margine, 9-12 pannello blu. `menu_dark.pal`
  ridefinisce 1-12. Senza immagine `SetPictureBoxVisible(FALSE)` mette 1 e 6 al
  colore 10 (pannello).
- **Testo scuro**: pal 15 è `sDarkTextPalette`; il testo chiaro sta sulla entry
  10 e **non** sulla 2 (i glifi A e R sono una pillola bianca con la lettera
  sulla 2). Intestazione e lista usano `GetHeaderColors()`.
- La palette 15 si carica da `gStandardMenuPalette` (PSF ereditava quella del
  campo). Righe limitate a un buffer da 48 byte con `StringAppendN`.

## GDB e mGBA

- Offset in SaveBlock1: `flags` 6540, `vars` 6844; **`gSaveBlock1Ptr` cambia a
  ogni ritorno al campo (ASLR)**: rileggilo subito prima di scrivere, o la
  scrittura va nella copia vecchia. Il libro legge lo stato all'apertura:
  chiudilo e riaprilo dopo ogni scrittura. `interrupt` è asincrono: aspetta
  "SIGINT" prima dei comandi.
- **Tema via GDB**: `IsDarkUiEnabled` legge `challengeSettings.darkUi` (bit 2 del
  byte a `gSaveblock3 + 47`); azzerarlo dà il tema chiaro.
- `xdotool search --class mgba` restituisce **due** finestre: i tasti arrivano
  solo all'ultima (`tail -1`).
