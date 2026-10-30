---
paths:
  - "src/level_scaling.c"
  - "include/level_scaling.h"
  - "include/config/level_scaling.h"
  - "src/caps.c"
---

# Level scaling (piano in `docs/plans/level-scaling.md`)

Motore da Soulgold (`level_scaling.c`, da `feature/level-scaling` di
fisham-org), regole di HnS; le regole per allenatore di SG
(`level_scaling_rules.h`) **non** sono portate (in SG non le legge nessuno).
**Verifiche in emulatore, tabelle di prova e scelte dell'utente stanno
nell'appendice del piano.** Un'unica opzione `LEVEL SCALING` (pagina BATTLE),
bit 6 di `soundAndFieldOptions` (`levelScaling`, 0 = OFF); il bit 7 resta
`unusedLevelScalingBit`. `IsWildLevelScalingEnabled` legge lo stesso bit ma
esclude Gara, Pike e Pyramid. Numeri in `include/config/level_scaling.h`.

## Dove si aggancia

| cosa | dove | nota |
| --- | --- | --- |
| allenatori | `CreateNPCTrainerPartyFromTrainerWithId` (`battle_main.c`) | la vecchia firma è un involucro con `TRAINER_NONE` = niente scaling: debug e `test/` non cambiano |
| soldi | `GetTrainerMoneyToGive` (`battle_script_commands.c`) | livello da `gEnemyParty` (slot 0-2, o 3-5 con due avversari), più `cap / 5` % |
| selvatici da tabella | `ChooseWildMonLevel` (`wild_encounter.c`) | **una** funzione per passi, pesca, Spaccaroccia, Testata, Profumino, Feebas e OWE; scala **prima** di repellente e Sguardo Feroce |
| sciami | `SetUpMassOutbreakEncounter` | livello salvato, scalato all'uso |
| DexNav | `DexNavTryGenerateMonLevel` | `CreateWildMon` resta **senza** scaling apposta |
| lotte di script | `ScrCmd_setwildbattle`, `ScrCmd_setwildbattleshiny` (`scrcmd.c`) | **non** in `CreateScriptedWildMon`: la usano i parassiti delle bacche (Lv14-16) |
| tetto per script e bonus | `GetLevelScalingCap` (`caps.c`) | con `LEVEL CAP` spento usa la tabella NORMAL (`GetCurrentLevelCap()` vale `MAX_LEVEL`) |

Esclusi di proposito: `setwildbossbattle`, roamer, leggendari/mitici/UB,
`FLAG_NO_WILD_CATCHING` (Dojo di Saffron a **Lv1**, Tentacruel Lv100: scalarli
rompe il Dojo), Gara, Pike, Pyramid, primo rivale, partner in lotta multi.

## Invarianti e trappole

- **Nessun `Random()` a scaling spento**: `GetWildScaledLevel` esce prima del
  tiro. Lo scarto degli allenatori non usa l'RNG (seed da `playerTrainerId`).
- **Tetto e margine dei selvatici** (solo selvatici): riferimento limitato a
  `GetLevelScalingCap()` e risultato mai oltre il tiro della tabella +
  `B_WILD_SCALING_MAX_ABOVE_TABLE` (15, scelta dell'utente). Il margine è
  assoluto: percorsi a tabella bassa (Percorso 36, 32) non superano Lv19-22.
  Vale anche per DexNav e sciami; le lotte di script non passano di qui.
- La descrizione dell'opzione su ON deve stare in 200 px utili (finestra di 26
  tile, testo da x = 8): misurata con `gFontNormalLatinGlyphWidths` e
  `charmap.txt` (172 e 190 px).
- **Banco di prova**: `CreateMon` non calcola le statistiche (PS 0/0, squadra
  KO alla prima lotta): serve `CreateMonWithIVs`.
- Differenze volute rispetto a SG: pavimento al livello scritto per **tutti**
  (niente devoluzione, niente `ValidateSpeciesForLevel`); evoluzione solo se
  la specie non ha **altri** bersagli (Slowpoke, Kirlia, Snorunt restano);
  niente cache del riferimento; le uova non entrano nella media
  (`IsPartySlotCounted`).
- Non provato: pesca, Mirror Mode, lotte in doppio e con due avversari, primo
  rivale e partner in multi, opzione scelta dal titolo prima di una nuova partita.
