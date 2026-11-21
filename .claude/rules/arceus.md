---
paths:
  - "src/pokemon.c"
  - "src/battle_script_commands.c"
  - "src/battle_util.c"
  - "src/item.c"
  - "src/battle_ai_items.c"
  - "src/overworld_catch.c"
  - "src/option_menu.c"
---

# ARCEUS FORMULAS (D21 del piano `overworld-catch`)

Decisioni e formule: [`docs/plans/overworld-catch.md`](../../docs/plans/overworld-catch.md)
(D21, §3); prove nel diario [`docs/claude/overworld-catch-history.md`](../../docs/claude/overworld-catch-history.md). Interruttore:
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

## Danno (`src/battle_util.c`, fasi 11 e 11b)

Formula e scelte nel piano §3. Il calcolo, con l'opzione accesa:

1. **Base** (`CalculateArceusBaseDamage`): `((100 + A + 15 L) × P / (D + 50))
   / 5`, senza il `+2` classico (un danno 0 diventa 1 alla fine, come prima).
   **A e D sono grezzi**: `sArceusRawAtkStat` e `sArceusRawDefStat`, scritti
   da `CalcAttackStat`/`CalcDefenseStat` dopo la scelta della statistica
   (Foul Play, Body Press, Psyshock, Wonder Room) e prima di stadi, abilità,
   strumenti, bonus medaglia ed Esplosione.
2. **Rapporti**, nello stesso calcolo: il numeratore × finale/grezzo
   dell'Attacco × grezzo/finale della Difesa **prima** delle due divisioni,
   in `u64`. Ogni modificatore pesa come nella classica (+2 stadi = ×2), come
   AtkMod/DefMod di Arceus; anche sui colpi deboli (applicati dopo il
   troncamento, base 0 restava 0 con qualunque stadio). Senza modificatori i
   rapporti si annullano e il risultato è identico al troncare a ogni passo.
   **Perché**: Arceus somma A; con gli stadi sulla statistica l'Attacco ×2
   dava solo +15% (Lv 50, A = D = 150).
3. **Selvatici di inizio gioco** (sezione sotto), poi bersagli multipli,
   meteo, brutto colpo, tiro casuale: del gioco.
4. **STAB** (`ArceusStabModifier`): si dimezza il bonus sopra 1 → 1,25;
   Adattabilità 1,5 (scelta dell'utente); Tera 2,25 → 1,625.
5. **Tipo** (`ArceusTypeModifier`): ≥ ×4 → ×2,5, ≤ ×0,25 (non 0) → ×0,4
   (`UQ_4_12(0.4)` = 1638), **solo nel danno**: `typeEffectivenessModifier`
   resta vero per messaggi, Expert Belt, Tinted Lens e IA.

Trappole e fatti:

- Le due statiche si leggono subito dopo le due chiamate in
  `DoMoveDamageCalcVars`: nulla fra mezzo le riscrive. Se si aggiunge codice
  lì, non chiamare altre funzioni di danno prima della base.
- **Divinazione** (`DoFutureSightAttackDamageCalcVars`) ha un calcolo suo:
  Attacco dalla squadra, già senza modificatori (grezzo = finale), quindi
  solo il rapporto della Difesa; anche lì la riduzione dei selvatici; STAB
  1,5 scritto a mano passato da `ArceusStabModifier`.
- **L'IA** usa `DoMoveDamageCalcVars` (senza tiro) e
  `ApplyModifiersAfterDmgRoll`: le sue stime seguono la formula.
- Brutto colpo **×2** del gioco (`B_CRIT_MULTIPLIER` GEN_3; Arceus 1,5):
  tenuto di proposito. Beat Up Gen 3 usa le basi: invariato.
- Banco di prova: struct in EWRAM scritta in `DoMoveDamageCalcVars` quando
  `ctx->randomFactor` (A e D grezzi e finali, base, tiro, STAB e tipo
  grezzi, danno). Prove del 21/11 nel diario (`overworld-catch-history.md`).
- **Lotte in link** con l'opzione diversa sui due GBA: statistiche e danno
  divergono. Non gestito.

## Inizio gioco: cure e selvatici (fase 11c, utente 21/11)

- **Pozioni come in Arceus** (Serebii, Bulbapedia): Pozione 60, Superpozione
  100, Iperpozione 150 (Pozione Max già piena). `GetArceusPotionHealAmount`
  (`src/item.c`) sostituisce l'importo fisso nei **tre** punti che lo
  calcolano: `ExecuteTableBasedItemEffect` (`pokemon.c`, menu e lotta del
  giocatore), `BS_ItemRestoreHP` (`battle_script_commands.c`) e la stima
  dell'IA (`battle_ai_items.c`, `GetHPHealAmount` in `u32`: in `u8`
  tagliava `maxHP - hp` della Pozione Max a 256). Le descrizioni in borsa cambiano in
  `GetItemDescription` (stringhe `static const`: un `COMPOUND_STRING`
  restituito da una funzione è un indirizzo locale, `-Werror`). Bevande e
  bacche restano: Arceus non le ha.
- **Selvatici ridotti** (`GetArceusEarlyWildDamageModifier`, dopo i
  rapporti): ×0,6 / 0,75 / 0,9 con 0 / 1 / 2 medaglie di Johto, pieno dalla
  terza; solo lotte senza `BATTLE_TYPE_TRAINER`, attaccante dell'avversario
  **e bersaglio del giocatore** (non il compagno in doppio, non sé stesso
  in confusione). Allenatori e giocatore mai. Medaglie: `FLAG_BADGE01..08_GET`
  (in HnS `NUM_BADGES` è 16).
