---
paths:
  - "src/dexnav.c"
  - "include/dexnav.h"
  - "include/config/dexnav.h"
  - "data/scripts/dexnav.inc"
  - "graphics/dexnav/**"
  - "src/fldeff_flash.c"
---

# DexNav

Racconto delle misure e delle verifiche: `docs/claude/dexnav-history.md`.
Lì stanno anche i dettagli di "da dove arriva nella storia" e della prova di
Burned Tower. Il DexNav di questo repo era già nel codice, **spento**
(`DEXNAV_ENABLED FALSE`); `src/dexnav.c`, `include/config/dexnav.h`, le 12
grafiche, `data/scripts/dexnav.inc` e `MENU_ACTION_DEXNAV` c'erano.

## Divergenza con Soulgold: va nei DUE sensi

Non è un port. SG ha cose che qui mancano (search level, area progress, unbind
con SELECT, guardie di robustezza, level cap, bug contest), ma **Hasep ha cose
che SG non ha**: la riga dei Pokémon **nascosti** (`ROW_HIDDEN`, `hiddenSpecies`,
`CapturedAllHiddenMons`, detector mode), il Pokéblock della Safari Zone, il
movimento in acqua/grotta (`movementCount`), il confronto per numero di Pokédex
in `SpeciesInArray`, `GetAbilityBySpecies`, `SafeFreeMonIconPalette`. Un
"rendilo come Soulgold" alla lettera **cancella feature**: classifica prima.
**Non dire "è identico in SG" senza averlo aperto**: per la grotta l'avevo detto
e non era vero (`movementCount` non esiste in SG).

## Opzioni e storia

- **`ENHANCED DEXNAV`**: una sola opzione, default ON, accessore
  `EnhancedDexNav()`, campo `basicDexNav` invertito (`save.md`). **Crediti**:
  solo la *ricerca* è di Soulgold; il "mostra tutti" no (SG gatta la lista su
  `FLAG_GET_SEEN` come noi).
- **Search level** in una var, per la sola specie registrata
  (`DEXNAV_SEARCH_LEVELS_REGISTERED_SPECIES`): la modalità per-specie di
  upstream metterebbe `dexNavSearchLevels[NUM_SPECIES]` prima di
  `challengeSettings` e sposterebbe ogni opzione. Offset inchiodati in
  `save.c`: `dexNavChain` 12, `challengeSettings` 16, `SaveBlock3` 52 (allora).
- **Consegna**: l'assistente di Elm con le prime cinque Poké Ball
  (`NewBarkTown_Lab_EventScript_TriggerAideGiveBalls`), flag
  `FLAG_RECEIVED_FIRST_BALLS`: alzato una volta, già alzato in ogni salvataggio
  oltre New Bark (nessuna migrazione), dopo Pokégear e Pokédex. R sul campo non
  ha un gate: `TryStartDexNavSearch()` esce se `DN_VAR_SPECIES` è vuota.
- **Il gate del "mostra tutti" va su `DexNavGetSpecies()`**, non sulle
  schermate: R, A e il pannello info passano tutti di lì (`SPECIES_NONE` per
  una specie non vista). Il dettaglio del pannello è legato a `FLAG_GET_CAUGHT`
  (tipi `???`, "Capture first!") anche con l'opzione spenta.
- **Stealth**: SG ha tolto fuga per avvicinamento e timeout dalle ricerche
  del giocatore; qui convivono dietro `ENHANCED DEXNAV`. SG mette *ogni* uscita
  per fallimento dietro `hiddenSearch`; HnS le applicava anche alle tue
  (`LostSignal`, creeping entro 2 caselle, corsa entro 4, timer 15 s) e in
  grotta ti mangiavano vivo.
- **Scelta della casella in grotta**: SG usa `weight = !MapGridGetCollisionAt`
  (riesce sempre; `MAP_TYPE_INDOOR` come grotta). HnS pesava con
  `Random() % scale`, `scale` è `u8` (oltre 255 si tronca, a 256 divisione per
  zero) e dipende dalle coordinate assolute: **0 ricerche su 7** contro 5 su 5.

## Trappole

- **`#if IS_HNS / #else`**: `TryStartDexNavSearch()` era nel ramo `#else` di
  `field_control_avatar.c` (dove HnS usa R per mach/acro bike): mai compilata,
  R non faceva nulla.
- **Creeping**: `gPlayerAvatar.creeping` si alza solo tenendo **A**; in
  `PlayerNotOnBikeMoving` il controllo era dopo il blocco corsa, che fa
  `return` sempre con AUTORUN: ramo irraggiungibile. Ora è **prima**. In SG il
  campo ha zero lettori.
- **Flash**: `GetFlashLevel()` vale 0 fuori, 7 al buio, **1** dopo Flash (mai
  0). `IsTooDarkToSearch()` (`> 1`) in `InitDexNavSearch` **e** in
  `TryFindHiddenPokemon`. Offset veri (da `offsetof`): `flashLevel` 0x34, `flags`
  0x198C, `vars` 0x1ABC; i commenti di `global.h` sono vecchi.
  `FLAG_SYS_USE_FLASH` si azzera uscendo all'aperto.
- **Verifica con un caso che fallisce senza la modifica**: Caterpie era già
  visto e "confermava" un gate che non c'era; serve una specie col `?` con
  l'opzione OFF (Ledyba).
- **Finestre**: `FONT_SMALL` mette l'inchiostro **3 righe sotto** la y data
  (barra di 10 px → `y_barra - 3`). Due finestre che **condividono anche una
  sola riga di tile** si corrompono: allarga quella esistente
  (`WINDOW_REGISTERED`, 3 righe, ospita il contatore dell'acqua). Le barre sono
  **arte del BG** (`gui_tilemap.bin`): una finestra con `PIXEL_FILL
  (TEXT_COLOR_TRANSPARENT)` ci scrive sopra. Barre: acqua y13-22, terra y54-63,
  nascosti y120-129; simboli "catturati tutti" centrati su x139, x152, x114,
  quindi i contatori finiscono a x130, x143, x103.
