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
  nessuna modifica a `ListMenu_ProcessInput` (la lista del libro si ferma
  alle estremità, le altre del gioco restano com'erano); la palette 15 si
  carica da `gStandardMenuPalette` (PSF ereditava quella del campo e ne
  caricava 0x60 byte da un file da 32).
- **Comandi**: A su una missione con sottomissioni la apre, B torna (cursore
  ripristinato), R cambia filtro (tutte / non iniziate / attive / premio /
  completate), Start ordina A-Z (le bloccate in fondo, per non rivelare i
  nomi), Select preferita (va in cima, nome verde). "NEW" rosso finché il
  cursore non è passato sulla missione.
- **Testi**: `struct QuestStep` (`desc`, `map`, `sprite`, `spriteType`)
  indicizzata da `QuestLog_GetStep`; `doneDesc`; `struct QuestSubquestInfo`
  per le sottomissioni (nome, testo, mappa, immagine, etichetta a destra:
  compaiono solo a sottomissione fatta, prima c'è `??????`). `{STR_VAR_2}`
  nei testi vale la mappa dello step. Una missione senza `steps` si mostra
  vuota. Nomi di missione: **non oltre ~20 caratteri**, la riga ha a fianco
  il glifo A e "NEW" e l'etichetta di stato parte a x=200.
- **Immagini**: `QUEST_SPRITE_OBJECT/ITEM/MON`. Due slot alternati, quello
  vecchio distrutto dopo aver creato il nuovo; **la stessa immagine non si
  ricrea**, altrimenti `DestroySpriteAndFreeResources` libera foglio e palette
  che il nuovo sprite condivide. Gli oggetti si distruggono con quella
  funzione (con `OW_GFX_COMPRESS` il foglio resterebbe in VRAM, uno in più per
  ogni missione sfogliata).
- **Entrate**: voce provvisoria `QUESTS` nel menu normale, **solo se
  `QUEST_COUNT > 0`** (nelle build di oggi non compare); `questlog
  QUEST_LOG_OPEN` da script, da far seguire da `waitstate`.
- **Missioni di prova**: `-DQUEST_BOOK_TEST_QUESTS=1` non c'è nel Makefile;
  si cambia il `#define` in `include/constants/quests.h` (a 1 aggiunge due
  missioni: var con le tre immagini, funzione con 8 sottomissioni a
  medaglie). **Rimetterlo a 0 prima del commit.**
- **Grafica provvisoria**: `graphics/quest_book/` è quella di PSF
  (`menu.png/.pal/.bin`) com'è, compreso il riquadro bianco dell'immagine
  che resta anche senza immagine. La rifà la fase 3.
- **Verifica fatta (25/10)**: ROM di prova che parte dritta dal libro
  (`CB2_TestBoot` in `main.c`, poi ripristinato): lista, dettaglio con
  oggetto/oggetto/Pokémon, sottomissioni con scorrimento e frecce, B con
  cursore ripristinato, R su tutti e cinque i filtri, Start A-Z, Select;
  e dal menu di avvio di una nuova partita (voce, apertura, B che torna al
  menu). **Non provato**: `QUEST_LOG_OPEN` da script, e la schermata con
  il tema scuro.
- **mGBA**: `xdotool search --class mgba` restituisce **due** finestre; i
  tasti arrivano solo all'ultima (`tail -1`), con la prima sembra tutto
  bloccato.
