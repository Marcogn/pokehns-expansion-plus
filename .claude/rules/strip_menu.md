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
`pollythadon` (branch `shattered-emerald`, 4c4df1aa, expansion 1.17.1).
**Nel suo gioco quel file non è più agganciato** (Start apre un menu Rotom,
`rotom_start_menu.c`): l'aggancio qui è nostro, non copiato.

- **Quando c'è**: `UseStripStartMenu()` in `start_menu.c` ricalca i rami di
  `BuildStartMenuActions`: link, Union Room, Safari, Bug-Catching Contest,
  Battle Pike, Piramide, Multi Partner Room e `DEBUG_OVERWORLD_IN_MENU`
  tengono la lista classica. Decide in due punti: `ShowStartMenu` (Start sul
  campo) e `FieldCB_ReturnToFieldStartMenu` (ritorno da una schermata).
- **Ritorni**: nessuno toccato. Le schermate tornano con
  `CB2_ReturnToFieldWithOpenMenu` → `FieldCB_ReturnToFieldStartMenu` →
  `StripMenu_ReturnToField` (striscia disegnata a schermo nero, poi
  `FadeInFromBlack`).
- **Tasti**: frecce sinistra/destra (circolari), A apre, B chiude, **Start
  salva**, **L Scheda allenatore** (o Frontier Pass), **R Pokégear** solo con
  `FLAG_SYS_POKENAV_GET` (senza, R non fa nulla e il suggerimento sparisce).
  R+Start apre ancora il debug: lo intercetta il campo prima del menu.
  Il cursore ricorda la voce (`sStripLastIcon`), come la lista classica.
- **Salvataggio**: la striscia si smonta, poi `StartMenu_SaveFromStrip` fa
  girare il dialogo classico in un task suo con `sSaveFromStrip`: niente
  finestra della lista da togliere in `SaveConfirmSaveCallback`, e "No"
  riapre la striscia (`StripMenu_Reopen`) invece della lista.
- **Mt. Silver**: la chiusura con B chiama `StartMenu_OnClose`, cioè il reset di
  `VAR_TEMP_MTSILVER_RESUME_BLIZZARD_SE` che faceva `HideStartMenu`.
- **VRAM di BG0**: le 36 tile della fascia vanno all'inizio del char block 2
  (la finestra dei dialoghi parte da baseBlock 1, le finestre della striscia
  da 48); all'uscita si azzerano solo quelle 36 tile, non tutto il blocco
  come faceva l'originale. Palette 14 (fascia) e 15 (testo): le finestre
  standard le ricaricano da sole.
- **VRAM degli oggetti**: 6 fogli da 32 tile. Se uno non entra
  (`LoadCompressedSpriteSheet` restituisce **`TAG_NONE`**, non 0) la voce
  resta senza icona ma con etichetta e funziona. Provato con una ROM che fa
  fallire due icone.
- **Flash**: fuori dal cerchio il campo mostra BG0 ma non gli sprite
  (`WINOUT` in `overworld.c`). In 1.15.2 `copyToObjWin` non esiste: ogni icona
  ha un gemello `ST_OAM_OBJ_WINDOW` (stesso trucco di `ShowItemIconSprite`)
  che condivide la matrice affine; all'uscita si ripristinano
  `DISPCNT_OBJWIN_ON` e `WINOUT`.
- **Orologio**: 12 ore come la lista classica, giorno da `GetDayOfWeek()`
  (quello della radio del Pokégear); si ridisegna quando cambia il minuto.
- **Grafica** (`graphics/strip_start_menu/`, fase 6): fascia e icone di
  Pokédex, Squadra, Borsa e Opzioni sono di `pollythadon`; `dexnav.png`
  (radar, 19 px) e `quests.png` (libro, 15x19 con nastro; stessa altezza della Poké Ball, righe 7-25 del foglio) sono disegnati qui, 32x32 per stato, metà
  alta = non selezionata (grigi 11, 12, 14), metà bassa = selezionata, stessa
  palette `icons.pal` (contorno bianco = entry 15). Indici contati sui `.png`.
- **Tema scuro**: `bg_dark.pal`, stessi indici di `bg.pal`; cambiano solo le
  entry usate dai tile della fascia e dalle finestre: 10 barra (40 → 16,16,20),
  11 linguetta del nome (56 → 36,36,44), 12/13 fascia rossa (184,40,56 e
  216,48,64 → 108,24,38 e 140,30,46), 14 bordo (192 → 84,84,96), 15
  linguetta dell'orologio (216 → 56,56,68). Le entry 1-3 (testo) restano.
  L'orologio, scuro su chiaro, in scuro usa `sTextColorLight`. Le icone non
  cambiano palette (contorno bianco leggibile su tutte e due le fasce).
- **Verificato in mGBA (25/10)** sul `.sav` dell'utente (Percorso 43): le sei
  voci, L, R, Start (No torna alla striscia, Sì salva e chiude), B e
  movimento dopo; Safari e Bug-Catching Contest (flag `0x860`/`0x861` via
  GDB) aprono la lista classica; Grotta Scura senza Flash (icone visibili,
  entrata e ritorno). **Fase 6 (25/10)**: icone nuove e tema scuro guardati in mGBA in entrambi i
  temi (DexNav e Missioni selezionate; la striscia chiara è identica a prima).
- **Verificato in fase 8 (25/10)**, forzando le condizioni via GDB invece di
  warpare: Pike e Piramide scrivendo `gMapHeader.mapLayoutId` (offset 18;
  1264 Pike room normal, 1254 Pyramid top), Multi Partner Room e Union Room
  scrivendo `location` (SaveBlock1 + 8: 30/46 con `VAR_FRONTIER_BATTLE_MODE`
  2, 56/60): lista classica in tutti e quattro, e dalla Borsa della Piramide
  si torna alla lista. Tolte le condizioni torna la striscia. Ritorno sulla
  stessa voce da Pokédex, DexNav, Squadra, Borsa, Missioni, Opzioni, Scheda
  (L) e Pokégear (R). Senza Pokédex, Pokémon, DexNav e Pokégear (flag `0x890`,
  `0x891`, `0x892`, `0x962` spenti, `sStripLastIcon` sul Pokédex): tre voci,
  cursore sulla Borsa, R muto e senza suggerimento.
- **Non provati**: il **link** (`IsOverworldLinkActive` è
  `gMain.callback1 == CB1_OverworldLink`, e Start in link passa dallo scambio
  di tasti col partner: con un emulatore solo non si raggiunge; il ramo è
  lo stesso di `BuildStartMenuActions`); una partita nuova vera (provata
  spegnendo i flag, non dall'inizio).
- `UseStripStartMenu` e `BuildStartMenuActions` elencano gli stessi contesti
  in due posti: un contesto speciale nuovo va aggiunto a **tutti e due**.
