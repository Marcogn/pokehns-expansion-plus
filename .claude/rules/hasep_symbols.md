---
paths:
  - "include/hasep_symbols.h"
  - "src/hasep_symbols.c"
  - "tools/hasep_symbols.py"
  - "ld_script_modern.ld"
  - "ld_script_test.ld"
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
  linkage): oggi `battle_controller_player.c`, `field_move.c`,
  `fldeff_flash.c`, `pokemon.c`, `region_map.c`. `HASEP_EXPORT_VALUE` per
  espressioni (`gTrainers[DIFFICULTY_NORMAL]`) e costanti.
- **Le funzioni hanno il bit Thumb** (valore dispari), come i puntatori a
  funzione del gioco e come li vuole PokeDaisy.
- **Layout**: il fork riusa gli offset di HnS 2.0.6 (BattlePokemon 0x88,
  SpeciesInfo 0x10C, SaveBlock1 party 0x23C...). Gli `STATIC_ASSERT` in
  `hasep_symbols.c` li fissano: se uno salta, alza
  `HASEP_SYMBOLS_LAYOUT_VERSION` e aggiorna il fork. **Un nome esportato è
  un'API**: non rinominarlo senza aggiornare il fork.
- `gHasepAppScratch[32]` (EWRAM) è la RAM dove l'app scrive argomenti e lo
  script del PC; HnS usava la coda inutilizzata della EWRAM.
- **Verifica**: `tools/hasep_symbols.py pokehns.gba --elf pokehns.elf`
  confronta ogni indirizzo con `nm` (esce 1 su un errore). Verificato anche dal
  vivo con `mgba_dump` del fork: party, SaveBlock, borsa e mapsec letti dalla
  tabella su una nuova partita.
