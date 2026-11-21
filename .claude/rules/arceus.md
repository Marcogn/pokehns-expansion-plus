---
paths:
  - "src/pokemon.c"
  - "src/battle_script_commands.c"
  - "src/battle_util.c"
  - "src/overworld_catch.c"
  - "src/option_menu.c"
---

# ARCEUS FORMULAS (D21 del piano `overworld-catch`)

Decisioni, fonti trascritte e prove: [`docs/plans/overworld-catch.md`](../../docs/plans/overworld-catch.md)
(D21, §3 "Fonti per ARCEUS FORMULAS", §5.10, §5.11). Interruttore:
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
- Impegno = base dagli IV (31/26/20 → 3/2/1) + `EV × 7 / 252`, massimo 10
  (utente, 21/11: il 10 vuole IV 31 **e** 252 EV). HOME non converte: tiene i
  livelli d'impegno a parte (PKHeX `GameDataPA8.cs`), nessun precedente.
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

## Danno (`src/battle_util.c`, fase 11)

- **Tre punti, tutti dietro l'opzione**: `CalculateBaseDamage` (formula di
  Bulbapedia, troncata a ogni passo, senza il `+2` classico: un danno 0
  diventa 1 alla fine come prima), `ArceusStabModifier` e
  `ArceusTypeModifier`, applicati in `ApplyModifiersAfterDmgRoll` **e** nel
  calcolo a parte di Divinazione (`DoFutureSightAttackDamageCalcVars`, STAB
  1,5 scritto a mano). L'IA passa da `ApplyModifiersAfterDmgRoll`: stime e
  danno vero coincidono.
- **STAB**: si dimezza il bonus sopra 1 (1,5 → 1,25 di Arceus; Adattabilità
  2 → 1,5; Tera 2,25 → 1,625). Scelta mia: Arceus non ha abilità né Tera.
- **Tipo**: ≥ ×4 → ×2,5, ≤ ×0,25 (non 0) → ×0,4, **solo nel danno**:
  `typeEffectivenessModifier` resta vero per messaggi, Expert Belt, Tinted
  Lens e IA. `UQ_4_12(0.4)` = 1638 (0,3999).
- **Ibrido (fase 11b, utente 21/11)**: il danno base usa le statistiche
  **grezze** (`sArceusRawAtkStat`/`sArceusRawDefStat`, registrate in
  `CalcAttackStat`/`CalcDefenseStat` prima di stadi, abilità, strumenti e
  bonus medaglia), poi `ApplyArceusStatRatio` moltiplica il danno per
  finale/grezzo dell'Attacco e grezzo/finale della Difesa, come AtkMod/DefMod
  di Arceus. Senza, `100 + A + 15 L` somma l'Attacco: ×2 (+2 stadi, Forza
  Pura, Bendascelta) dava solo +15%. Divinazione: l'Attacco viene dalla
  squadra senza modificatori, solo la Difesa ha il rapporto. Le due statiche
  si leggono subito dopo le due chiamate: niente fra mezzo le riscrive.
- **Restano del gioco** (D21, documento chiuso): stadi ±6, brutto colpo
  (**×2**: `B_CRIT_MULTIPLIER` è GEN_3; Arceus 1,5), meteo, scottatura,
  abilità, strumenti, bersagli multipli, Beat Up (Gen 3: usa le basi, non le
  statistiche).
- **Lotte in link** con l'opzione diversa sui due GBA: statistiche e danno
  divergono. Non provato, non gestito.
