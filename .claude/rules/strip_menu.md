---
paths:
  - "src/strip_start_menu.c"
  - "include/strip_start_menu.h"
  - "src/start_menu.c"
  - "include/start_menu.h"
  - "graphics/strip_start_menu/**"
---

# Menu di pausa a striscia (fase 5 di `docs/plans/quest-log.md`)

`src/strip_start_menu.c` è il port ridotto di `unbound_start_menu.c` di
`pollythadon` (branch `shattered-emerald`, 4c4df1aa, expansion 1.17.1). **Nel
suo gioco quel file non è agganciato** (Start apre un menu Rotom): l'aggancio è
nostro. **Le verifiche in mGBA (fasi 5, 6, 8), le misure di palette e i casi non
provati stanno nell'appendice di `quest-log.md`.**

## Quando c'è, tasti, ritorni

- `GetStartMenuContext()` (`start_menu.c`) è l'**unico** posto che decide il
  contesto (link, Union Room, Safari, Bug-Catching Contest, Pike, Piramide,
  Multi Partner Room, `DEBUG_OVERWORLD_IN_MENU`, normale); `UseStripStartMenu()`
  mostra la striscia solo nel normale. Un contesto nuovo va aggiunto **solo
  lì**. Si decide in `ShowStartMenu` e `FieldCB_ReturnToFieldStartMenu`.
- Ritorni: `CB2_ReturnToFieldWithOpenMenu` → `FieldCB_ReturnToFieldStartMenu` →
  `StripMenu_ReturnToField` (striscia a schermo nero, poi `FadeInFromBlack`).
- Tasti: sinistra/destra circolari, A apre, B chiude, **Start salva**, **L
  Scheda allenatore** (o Frontier Pass), **R Pokégear** solo con
  `FLAG_SYS_POKENAV_GET`, **Select sposta** l'icona. R+Start apre il debug
  (lo intercetta il campo). Il cursore ricorda la voce (`sStripLastIcon`).
- Ogni icona chiama le stesse `StartMenu_Open*` della lista classica (da
  `Task_StripMenu_FadeAndRun` a schermo nero); solo il libro ha una funzione sua
  (`StripMenu_OpenQuestBook`).
- **Salvataggio**: `StartMenu_SaveFromStrip` fa girare il dialogo classico con
  `sSaveFromStrip`; "No" riapre la striscia (`StripMenu_Reopen`).
- **Mt. Silver**: B chiama `StartMenu_OnClose` (reset di
  `VAR_TEMP_MTSILVER_RESUME_BLIZZARD_SE`).

## Riordino (Select)

Solleva l'icona di `STRIP_ICON_LIFT` px; sinistra/destra scambiano (ai bordi
si ferma), A o Select posano e salvano, B ripristina (`iconsBeforeMove`).
Icone, sprite e gemelli del Flash si permutano insieme (`StripMenu_SwapIcons`);
`StripMenu_PlaceIcons` rifà le posizioni. Salvataggio: `stripMenuOrder`
(`save.md`, `SAVE_VERSION 14`); tutto zero o non-permutazione = ordine di
default (`GetIconOrder`). Le icone non disponibili tengono il posto
(`StripMenu_PlaceMovedIcon`). La finestra dei suggerimenti è larga 28 tile: la
riga col Pokégear fa 188 px (a 24 tile restava 1 px).

## Trappole

- **Palette 14 e 15 sono quelle delle cornici del campo**
  (`STD_WINDOW_PALETTE_NUM`, `DLG_WINDOW_PALETTE_NUM`) e nessuno le ricarica
  dopo: senza, salvataggio e Sì/No avevano la cornice nei colori della fascia.
  Le rimette `StripMenu_DestroyToField` (`LoadMessageBoxAndBorderGfx`), **solo**
  su B e sul salvataggio. **Non in `StripMenu_Destroy`**: a schermo nero
  `LoadPalette` scrive anche il buffer sfumato e le palette tornano a colori
  pieni (un fotogramma di tile colorate aprendo il quest book). Una prova
  d'aspetto va fatta contro una ROM **di prima**.
- **VRAM di BG0**: le 36 tile della fascia stanno all'inizio del char block 2
  (dialoghi da baseBlock 1, finestre della striscia da 48); all'uscita si
  azzerano solo quelle.
- **VRAM degli oggetti**: 6 fogli da 32 tile; se uno non entra
  (`LoadCompressedSpriteSheet` restituisce **`TAG_NONE`**, non 0) la voce resta
  senza icona ma funziona.
- **Flash**: fuori dal cerchio il campo non mostra gli sprite (`WINOUT` in
  `overworld.c`). Ogni icona ha un gemello `ST_OAM_OBJ_WINDOW` (in 1.15.2
  `copyToObjWin` non esiste); all'uscita si ripristinano `DISPCNT_OBJWIN_ON` e
  `WINOUT`.
- Orologio a 12 ore, giorno da `GetDayOfWeek()`, ridisegnato al cambio minuto.

## Grafica

`graphics/strip_start_menu/`: fascia e icone di Pokédex, Squadra, Borsa e
Opzioni sono di `pollythadon`; `dexnav.png` (radar) e `quests.png` (libro,
15x19) sono disegnati qui, 32x32 per stato (metà alta non selezionata, bassa
selezionata), stessa `icons.pal` (contorno bianco = entry 15). Tema scuro:
`bg_dark.pal`, stessi indici di `bg.pal`; cambiano le entry 10 (barra), 11
(linguetta nome), 12/13 (fascia rossa), 14 (bordo), 15 (linguetta orologio); le
1-3 (testo) restano e l'orologio usa `sTextColorLight`. I valori esatti sono
nell'appendice del piano.
