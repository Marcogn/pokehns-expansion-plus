---
paths:
  - "include/hasep_symbols.h"
  - "src/hasep_symbols.c"
  - "tools/hasep_symbols.py"
  - "ld_script_modern.ld"
  - "ld_script_test.ld"
  - "src/battle_controller_player.c"
  - "src/event_object_movement.c"
  - "src/field_camera.c"
  - "src/field_move.c"
  - "src/fldeff_flash.c"
  - "src/pokemon.c"
  - "src/region_map.c"
  - "src/script.c"
  - "src/sprite.c"
  - "src/fieldmap.c"
  - "src/heal_location.c"
---

# Tabella dei simboli per le app companion (`HASEPSYM`)

Serve al fork di PokeDaisy (`Marcogn/pokedaisy-hasep-support`). PokeDaisy
riconosce le ROM per SHA-1 e usa indirizzi cablati per *quella* build: ogni
build di Hasep sposta RAM e codice, quindi la ROM dichiara da sé dove stanno le
cose e il fork le legge per nome. Analisi: HnS 2.0.6 ricompilato dal sorgente
coincide con la release in tutta la EWRAM, Hasep no (party +0x58, borsa +0xC...).

- **Formato**: header `struct HasepSymbolsHeader` (magic `HASEPSYM`, formato,
  versione del layout, dimensione voce, puntatore alla fine) subito dopo
  l'header RHH, poi voci da 48 byte `{char name[44]; u32 value}`. Il linker
  script tiene le due sezioni (`.text.header_hasep`, `.text.hasep_symbols`) e
  definisce `__hasep_symbols_end`; senza quelle righe `--gc-sections` le butta.
- **Esportare un simbolo**: `HASEP_EXPORT(sym)` a livello di file. Una
  funzione o tabella `static` si esporta **dal suo file** (così non cambia
  linkage): oggi `battle_controller_player.c`, `event_object_movement.c`,
  `field_camera.c`, `field_move.c`, `fieldmap.c`, `fldeff_flash.c`, `heal_location.c`, `pokemon.c`,
  `region_map.c`, `script.c`, `sprite.c`. Quelli di EXPAND (camera, ordine
  degli sprite, controlli bloccati, palette e grafica degli oggetti) servono
  alla riga nativa `pk_wide` che il fork costruisce dalla tabella. `HASEP_EXPORT_VALUE` per
  espressioni (`gTrainers[DIFFICULTY_NORMAL]`) e costanti.
- **Le funzioni hanno il bit Thumb** (valore dispari), come i puntatori a
  funzione del gioco e come li vuole PokeDaisy.
- **Layout**: il fork riusa gli offset di HnS 2.0.6 (BattlePokemon 0x88,
  SpeciesInfo 0x10C, SaveBlock1 party 0x23C...). Gli `STATIC_ASSERT` in
  `hasep_symbols.c` li fissano: se uno salta, alza
  `HASEP_SYMBOLS_LAYOUT_VERSION` e aggiorna il fork. **Un nome esportato è
  un'API**: non rinominarlo senza aggiornare il fork.
- **Cosa usa il fork, da PLUS.11**: `gEnemyParty` / `gBattlerPartyIndexes` /
  `gBattleStruct` (squadra avversaria, SHINY ODDS con `SHINY_ODDS` = ciò che il
  gioco disegna, non le probabilità dell'opzione sfida), `gPlayerAvatar` (bici e
  surf: in HnS sta 0x30 *prima* di `gObjectEvents`), gli id dei Repellenti e
  `REPEL_LURE_MASK`. Un'app che non trova un nome si comporta come prima.
- **FREE FLIGHT del fork (PokeDaisy 1.6.0), da PLUS.12**: le funzioni di sprite, griglia della mappa e suoni
  (`CreateSprite`, `MapGridGetMetatileBehaviorAt`, `PlaySE`, ...), `CB2_Overworld`, `gPaletteFade`, l'uccello di FLY
  (`gFieldEffectObjectTemplate_Bird`), i follower, `sMapConnectionFlags` (da `fieldmap.c`), `sHealLocations` con
  `HEAL_LOCATION_SIZE` / `HEAL_LOCATION_COUNT` (da `heal_location.c`: la voce è di 8 byte, non 6) e `MAP_GROUP_COUNT`
  (un **indirizzo**, l'array di `data/map_group_count.h`, sotto un nome maiuscolo che lo script stampa come costante)
  con `MAP_GROUPS_COUNT` (lo script lo controlla con `nm` come un indirizzo: `ADDRESSES`). Movimenti, suoni e terreni di atterraggio sono quelli di HnS 2.0.6: gli `STATIC_ASSERT`
  `HasepFlight*` li fissano, con gli offset di ObjectEvent / Sprite / MapHeader / HealLocation che `pk_flight.c` usa, se uno salta il volo del fork va aggiornato. Verificato con il `mgba_dump` del fork:
  decollo da New Bark Town, Percorso 27, atterraggio; il percorso panoramico calcolato dall'app fino a Cherrygrove.
- `gHasepAppScratch[32]` (EWRAM) è la RAM dove l'app scrive argomenti e lo
  script del PC; HnS usava la coda inutilizzata della EWRAM.
- **Verifica**: `tools/hasep_symbols.py pokehns.gba --elf pokehns.elf`
  confronta ogni indirizzo con `nm` (esce 1 su un errore). Verificato anche dal
  vivo con `mgba_dump` del fork: party, SaveBlock, borsa e mapsec letti dalla
  tabella su una nuova partita; PC ovunque, Volo (verso Cherrygrove), Flash
  (Grotta Oscura) ed EXPAND (centro esatto in 59/60 e 203/204 frame) con le
  chiamate dell'app agli indirizzi della tabella.
