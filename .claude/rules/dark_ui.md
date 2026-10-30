---
paths:
  - "src/text_window.c"
  - "include/text_window.h"
  - "src/shop.c"
  - "src/region_map.c"
  - "src/field_region_map.c"
  - "src/egg_hatch.c"
  - "src/trainer_card.c"
  - "src/pokemon_summary_screen.c"
  - "src/swsh_party_menu.c"
  - "include/option_menu.h"
---

# Tema scuro fuori dalla lotta

Accessore unico `IsDarkUiEnabled()` (`include/option_menu.h`). La mappa del
tema in lotta è in `battle_ui.md`; il racconto è in
`docs/claude/battle-ui-history.md`.

**Non viene da Soulgold.** In SG 1.2.1 l'opzione copre solo lotta e borsa
(provato compilando SG: con il tema scuro summary, dialoghi, menu di pausa e
squadra SwSh restano chiari; il summary differisce di **0 pixel**). Se qualcuno
dice "in SG è scuro", controlla prima quale opzione e quale schermata.

| Cosa | Dove | Note |
| --- | --- | --- |
| Box dei messaggi, cornici, Sì/No, scelte, salvataggio, menu debug | `ApplyDarkUi*Palette` in `text_window.c`, da `LoadMessageBoxAndBorderGfx`, `LoadSignPostWindowFrameGfx`, `LoadMessageBoxAndFrameGfx` (`menu.c`) | testo 1/2/3 e coppie 4/5, 6/7, 8/9; box 11/12; cartello 11; cornice entry 14 |
| Menu della borsa | `LoadBagMenuTextWindows` (`item_menu.c`) | cornice pal 14, box pal 13, testo pal 15 |
| Menu e messaggi della squadra (entrambe le varianti) | dopo il caricamento di pal 13/14/15 | |
| Squadra SwSh | `ApplyDarkPartyMenuColors` (`swsh_party_menu.c`) | sfondo = solo pal 0; testo pannello destro pal 1 (12/7); riquadri da `palBuffer` (selezionato pal 6, scelto per Switch pal 10) |
| Summary | `LoadDarkSummaryPalettes` (`pokemon_summary_screen.c`, solo `IS_HNS`) | pagine pal 0..3 (5 pannello, 12 pannello colorato, 13 bordo, 14/15 cornice e fondo), pal 4 barra, pal 6/7 testo, pal 8 PP = `gDarkPPTextPalette` |

## Trappole

- **Una box è la cornice di una palette attorno a una finestra stampata con
  un'altra.** Scurire tutti i caricamenti della cornice
  (`LoadUserWindowBorderGfx`) lasciava nella borsa un anello scuro attorno a un
  menu bianco (il suo testo usa `gStandardMenuPalette` su pal 15): i caricatori
  semplici restano neutri, lo scuro lo applica chi carica **entrambe** le metà.
- **Una schermata che non diventa scura e usa la pal 15 senza caricarla deve
  ricaricare quella chiara**: negozio, mappa del Volo (`region_map.c`), mappa
  della città (`field_region_map.c`), Sì/No della schiusa (`egg_hatch.c`); la
  Trainer Card carica le due metà senza scurirle. Idioma già in `pokedex.c` e
  `mail.c`: `LoadPalette(GetOverworldTextboxPalettePtr(), BG_PLTT_ID(15), ...)`.
  Candidati non visti: `daycare.c`, `field_specials.c`.
- Nel summary la entry 8 della pal 6 è **sia** un colore del testo (`{COLOR}{08}`)
  **sia** l'ombra del glifo 7 (♂): resta il valore originale, che va per entrambi.
- Restano chiari, coerenti come prima: PC dei box (pal 15 sua), Pokédex,
  schermata DexNav, opzioni, menu acquisti, Trainer Card, mappa del Volo,
  mappa della città, schiusa.
