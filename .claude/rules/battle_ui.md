---
paths:
  - "src/battle_interface.c"
  - "src/battle_bg.c"
  - "src/battle_gfx_sfx_util.c"
  - "src/battle_controller_player.c"
  - "src/battle_message.c"
  - "include/battle_interface.h"
  - "src/type_icons.c"
  - "include/type_icons.h"
  - "graphics/battle_interface/**"
  - "graphics/types/**"
  - "graphics/text_window/**"
---

# UI di lotta: palette, tema scuro in lotta, icone dei tipi

Tema scuro fuori dalla lotta: `dark_ui.md`. Racconto delle misure: `docs/claude/battle-ui-history.md`.

## Una entry, due significati (esempi della regola del §2 di CLAUDE.md)

Succede di continuo. Esempi reali trovati in questo repo:

- palette del popup abilità: entry 7 è **sia** il pannello del popup **sia** la R
  del tab della ball → risolto spostando i pixel del tab su una entry libera (9);
- entry 5: sul tab MOVE INFO è la didascalia (va chiara), sul tab della ball è la
  lettera R (va scura) → il remap va fatto **per foglio**, non globale;
- barra HP entry 2: binario della barra (scuro) **e** metà bianca della ball di
  cattura (chiara) → remap solo su quell'elemento, con entry di destinazione
  **diversa per skin** (9 su gen4, 1 su gen3).

Prima di scurire/schiarire una entry, controlla **tutti** i disegni che la usano.

## Mappa del tema scuro

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


## Icone dei tipi in lotta

`src/type_icons.c` c'era, con `B_SHOW_TYPES` su `SHOW_TYPES_NEVER`. Una feature
sempre spenta ha dati posizionali di default di expansion (`sTypeIconPositions`
era così). La healthbox ha subpriority **1**, le icone **255**: ciò che si
sovrappone alla box finisce *dietro* e non si vede.

- **Leggi la segnalazione come una misura, non come un sintomo**: «esce e rientra
  dalla hpbox» descriveva l'animazione che funzionava; l'unico bug era
  `tHideIconTimer` mai azzerato.
- Misura: `import` + `convert -sample 240x160!` dà il framebuffer 1:1, `txt:-`
  stampa i pixel. In singola il bordo sinistro della box è a **x 12**, il destro
  a **x 101**; lo sprite è largo 8 e la x è il **centro** (riposo 7).
- **Disposizione**: singola a **sinistra** (`{17, 26}`, la slide toglie 10),
  doppia a **destra** come SG (+66 dall'origine della box). La scaletta di 4 px
  della seconda icona vale solo in doppia (in singola i 12 px non bastano).
- L'arte (`graphics/types/battle_icons*.png`) è identica a SG ed è
  **asimmetrica**. `ShouldFlipTypeIcon` specchia solo in doppia
  (`UseDoubleBattleCoords`): la regola vera è "da che parte della healthbox sta
  l'icona". In singola le direzioni di `GetTypeIconSlideMovement` /
  `GetTypeIconHideMovement` sono speculari a SG, ed è corretto.
- `tHorizontalPosition` (`data[4]`, `type_icons.h`): la slide si aggancia alla x
  di riposo del **singolo sprite**, o l'indent di 4 px viene riassorbito.
  `type_icons.c` differisce da SG solo per la tabella e `tHideIconTimer = 0`.
- **Un diff vuoto fra due estratti vuoti non è una verifica**: l'`awk` con un nome
  inesistente stampava nulla da entrambe le parti. Controlla che l'estratto non
  sia vuoto.
