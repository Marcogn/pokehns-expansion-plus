---
paths:
  - "src/pokemon.c"
  - "src/battle_script_commands.c"
  - "src/overworld_catch.c"
  - "src/option_menu.c"
---

# ARCEUS FORMULAS (D21 del piano `overworld-catch`)

Decisioni, fonti trascritte e prove: [`docs/plans/overworld-catch.md`](../../docs/plans/overworld-catch.md)
(D21, §3 "Fonti per ARCEUS FORMULAS", §5.10). Interruttore:
`AreArceusFormulasEnabled()` (`ExtraOptions.arceusFormulas`, SB3 +0x60 bit 0,
0 = spenta).

## Statistiche (`CalculateMonStats`, `src/pokemon.c`)

- **Riferimento: PKHeX** (`PA8.cs` `GetStatHp`/`GetStat`/`GetGanbaruStat`,
  `IGanbaru.cs`), dati del gioco, non Bulbapedia. **IV ≥ 20 → impegno 1**
  (Pokébip ed Eternia scrivono 21: sbagliato).
- I due termini base sono **`float` a 32 bit di proposito**: il gioco tronca
  dei float, e la matematica intera esatta esce più alta di 1 in 19 PS e 89
  altre statistiche su 25 500 coppie (basi 1-255 × livelli 1-100). Il bonus d'impegno è intero
  (`Sqrt(base << 24)`, 1/4096) e coincide in tutti i casi. Le funzioni
  `float` vengono da libgcc (soft-float).
- Natura sul termine base, **prima** del bonus (`ModifyStatByNature`, stessa
  troncatura `×110/100` di PKHeX).
- Impegno = base dagli IV (31/26/20 → 3/2/1) + `EV × 10 / 252`, massimo 10.
  `BST EQUALIZER` sostituisce la base anche qui; Hyper Training = IV 31.
- **Al cambio** (`Task_Save` del menu Opzioni, solo se il valore cambia) e
  **al caricamento** (`LoadGameSave`, slot normale, `OK` o `ERROR`: anche
  `ERROR` carica lo slot buono) `RecalculatePartyStats()`: PS in proporzione
  (`ScaleHPToNewMax`, al più vicino: troncando 50/104 → 69/145 → 49/104),
  ferito resta ferito, esausto resta 0, stato intatto. **`SetMonData(HP)`
  anche sull'esausto**: è lui che riallinea i PS persi della metà
  `BoxPokemon`, che un deposito porta nel PC. Al caricamento è un no-op se
  le statistiche sono già coerenti.
- **PC e Pensione tengono solo i PS persi** (il deposito non cura,
  `OW_PC_HEAL`): al cambio `ConvertStoredMonsHPForStatFormulaSwitch()` li
  riscala tutti (14 box + 2 Pensione, 1 frame), se no al ritiro con la
  formula più piccola `max - persi` andava in underflow (65 000 PS).
  `BoxMonToMon` ora li limita comunque al massimo.
- Anche il menu Opzioni del **titolo** ricalcola: la squadra è già caricata.

## Cattura (`ComputeArceusCaptureOddsFromParams`, `battle_script_commands.c`)

- **Solo col lancio nell'overworld** (`ComputeOverworldCaptureOdds`); in lotta
  resta `ComputeCaptureOddsFromParams`.
- `a` in millesimi, s64 per il termine della Ball (la Nest arriva a 49152/16384
  dopo ×3/4). Risultato 0-255, nello stesso `RollShakes`: P ≈ a/255 come in
  Arceus. Sotto 0 = 0 (impossibile).
- Tasso 135, salvo `isRestrictedLegendary`/`isSubLegendary`/`isMythical`
  (tasso della specie). `flatBonus` della Heavy Ball sul tasso.
- Ball: Poké/Great/Ultra 75/150/225 %; Beast, Safari, Sport e **qualunque
  Ball su un'Ultracreatura** come in `ComputeBallData`; le altre ×3/4.
  Master ed EASY CATCH = `CAPTURE_GUARANTEED`.
- Rango giocatore = `FLAG_BADGE01..08_GET` + `FLAG_IS_CHAMPION`. **In HnS
  `NUM_BADGES` è 16** (Kanto subito dopo, `flags_hns.h`): il ciclo va fino a
  `NUM_BADGES_CAPPED`. Il campione di Kanto è `FLAG_IS_KANTO_CHAMPION`. Rango Pokémon `(L-1)/10+1`, max 10
  (+4 alfa: fase 13).
- Comportamento 2 se `!IsOWEAwareOfPlayer`, 1 se allertato (sostituisce il
  ×2). **Colpo alle spalle** = `owe->facingDirection == throw->direction`
  all'impatto (direzione del giocatore salvata al lancio). Quick Ball: come
  prima, solo se non allertato.
