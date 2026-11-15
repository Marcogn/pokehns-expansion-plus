# Ricerca: cattura nell'overworld e alfa in altri progetti (15/11)

Domanda dell'utente: esiste già un hack o un repository con questa
funzionalità, da provare invece di inventarla da zero? Piano:
[`docs/plans/overworld-catch.md`](../plans/overworld-catch.md).

**Risposta breve**: nessun progetto GBA con **sorgente** pubblico ha il lancio
della Ball nell'overworld. Ce n'è uno **chiuso** basato su Heart & Soul, da
provare ma non da riusare. Per gli alfa c'è un'implementazione aperta (Elite
Redux), ma diversa da quella decisa qui.

## Trovati

| Progetto | Cosa ha | Sorgente | Uso per noi |
| --- | --- | --- | --- |
| [HuyMai0509/HnS-Legends](https://github.com/HuyMai0509/HnS-Legends) | Heart & Soul 2.x con "Legends Arceus / Z-A style battles right on the overworld". README: "press **A** next to a wild Pokémon to battle it, or throw a ball at it directly from the item ring". Release `v0.21b` (9/11) e `M8.1b` (12/11/2026), "bugs expected" | **No**: solo README, screenshot e patch BPS nelle release; nessuna licenza indicata | **Da provare** (stessa base, stesse mappe) per vedere com'è un lancio diretto. Il codice non c'è e, senza licenza, non si potrebbe riusare. Non ha alfa né passo furtivo nel README |
| [Elite-Redux/eliteredux-source](https://github.com/Elite-Redux/eliteredux-source) (e il fork `pokemon-hyperion-source`) | Alfa: bit `isAlpha:1` dentro `BoxPokemon` (`include/pokemon.h`, struct propria), `MON_DATA_IS_ALPHA`, creato quando uno script accende `FLAG_ALPHA_CREATION` prima di `CreateBoxMon` (`src/pokemon.c:505`); palette spostata di tinta (`HueShiftMonPalette`, `src/battle_gfx_sfx_util.c:529`); segno `{SUM_ALPHA}` nel riassunto (`src/pokemon_summary_screen.c:3608`). Gli alfa sono **incontri fissi** in 6 mappe e nel negozio, non spawn casuali | Sì, decomp pokeemerald (non expansion); nessuna licenza nel README | **Conferma il disegno** della fase 9 (bit nel Pokémon, accessor `MON_DATA_*`, segno nel riassunto). La struct è diversa dalla nostra: si copia l'idea, non il codice. Il cambio di tinta l'utente l'ha scartato (si confonde con lo shiny) |
| [Shamygo/Sijord PR #16](https://github.com/Shamygo/Sijord/pull/16) | Gioco in TypeScript, non GBA. Lancio in overworld con mira ad arco; cattura ×0,6 in overworld, ×1,5 se il Pokémon non ti ha notato; al fallimento reagisce per indole (fugge, arretra, carica, lotta); Ball mancate restano a terra da raccogliere. Unita il 6/10/2026 | Sì, ma altro linguaggio, nessuna licenza indicata | Solo come **riferimento di bilanciamento**: anche lì il bonus "non notato" esiste; il nostro ×2 (D2) è più generoso del loro rapporto dichiarato 1,5/0,6 = 2,5 rispetto al lancio notato |

## Cercato e non trovato

- **pokeemerald-expansion** (rh-hideout): nessuna issue o PR su lancio in
  overworld, alfa o passo furtivo (ricerca issue/PR del 15/11). Due PR da
  tenere d'occhio perché toccano la stessa area: **#10164 "Wild encounter
  refactor"** (aperta) e **#10783 "on-the-fly decompression of object events
  and mon icons"** (chiusa; potrebbe liberare la VRAM che oggi limita gli OWE,
  `overworld_encounters.md`). Da guardare al prossimo `/upstream-sync`.
- Ricerca di codice su GitHub per nomi plausibili (`ThrowBallOverworld`,
  `OverworldCatch`, `OverworldBallThrow`, `CatchOverworld`, `OverworldThrow`,
  `BallThrowField`, `IsAlphaMon`): zero risultati in progetti Pokémon.
- PokéCommunity: nessun thread decomp con lancio in overworld; solo modifiche
  al lancio **in lotta** e il blocco della cattura (`Cmd_handleballthrow`).
- Hack "Legends Arceus GBA" (FireRed, Darlex Gaming / Eric151): solo pagine
  di download e negozi, nessun sorgente né descrizione verificabile della
  meccanica. Non affidabili come fonte.
- Il blog dell'autore della PR #8434 (hashtagmarky.github.io) non era
  raggiungibile da questa sessione (rete); i suoi repository pubblici non
  contengono codice di gioco.

## Conseguenze per il piano

- Nessuna fase si accorcia: non c'è codice da portare. Il piano resta.
- Prima della fase 3, se l'utente vuole, **provare HnS-Legends** per vedere
  dal vivo un lancio dalla ruota degli strumenti e decidere se il tasto L
  (D1) resta la scelta giusta.
- La fase 9 può guardare Elite Redux per l'accessor e il segno nel
  riassunto (idea, non codice).
