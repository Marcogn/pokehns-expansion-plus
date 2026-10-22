---
paths:
  - "src/level_scaling.c"
  - "include/level_scaling.h"
  - "include/config/level_scaling.h"
  - "src/battle_main.c"
  - "src/wild_encounter.c"
  - "src/scrcmd.c"
  - "src/caps.c"
---

# Level scaling (fatto il 22/10, piano in `docs/plans/level-scaling.md`)

Motore da Soulgold (`src/level_scaling.c` di SG, che viene dal branch
`feature/level-scaling` di fisham-org), regole di HnS. Le regole per
allenatore di SG (`level_scaling_rules.h`) **non** sono portate: in SG non le
legge nessuno. Opzioni `TRAINER LEVELS` e `WILD LEVELS` nella pagina BATTLE
del menu opzioni, bit 6 e 7 di `soundAndFieldOptions` (0 = OFF, nessuna
migrazione: erano `unused`, mai scritti). Numeri tutti in
`include/config/level_scaling.h`.

## Dove si aggancia (e perché lì)

| cosa | dove | nota |
| --- | --- | --- |
| allenatori | `CreateNPCTrainerPartyFromTrainerWithId` (`battle_main.c`) | la vecchia firma resta come involucro con `TRAINER_NONE` = niente scaling: debug e `test/` non cambiano |
| soldi | `GetTrainerMoneyToGive` (`battle_script_commands.c`) | livello da `gEnemyParty` (slot 0-2 o 3-5 con due avversari), più `cap / 5` % |
| selvatici da tabella | `ChooseWildMonLevel` (`wild_encounter.c`) | **una** funzione per passi, pesca, Spaccaroccia, Testata, Profumino, Feebas e OWE allo spawn; scala **prima** di repellente e Sguardo Feroce |
| sciami (outbreak) | `SetUpMassOutbreakEncounter` | livello salvato, scalato all'uso |
| DexNav | `DexNavTryGenerateMonLevel` (`dexnav.c`) | il livello mostrato nella finestra è quello della lotta; `CreateWildMon` resta **senza** scaling apposta |
| lotte di script | `ScrCmd_setwildbattle`, `ScrCmd_setwildbattleshiny` (`scrcmd.c`) | **non** in `CreateScriptedWildMon`: la usano anche i parassiti delle bacche, che restano Lv14-16 (oggi irraggiungibili: `OW_BERRY_PESTS` è FALSE) |
| tetto per le lotte di script e il bonus | `GetLevelScalingCap` (`caps.c`) | con `LEVEL CAP` spento usa la tabella NORMAL: `GetCurrentLevelCap()` lì vale `MAX_LEVEL` |

Lasciati fuori di proposito: `setwildbossbattle` (le lotte boss sono già a
60-80 con mosse scritte), roamer, leggendari/mitici/UB, `FLAG_NO_WILD_CATCHING`
(allenamento EV del Dojo di Saffron a **Lv1**, Tentacruel di Route 41 a Lv100:
scalarli rompeva il Dojo), Gara Pigliamosche, Pike e Pyramid, primo rivale
(Cherrygrove), partner in lotta multi (squadra costruita dentro `gPlayerParty`).

**Nessun `Random()` a scaling spento**: `GetWildScaledLevel` esce prima del
tiro, quindi con OFF la sequenza dell'RNG è quella di prima. Lo scarto degli
allenatori non usa l'RNG (seed da `playerTrainerId` e id allenatore, come SG).

## Differenze rispetto a SG, volute

- Pavimento al livello scritto per **tutti** (SG: solo una lista opzionale),
  quindi niente devoluzione: `ValidateSpeciesForLevel` e la tabella degli
  override non servono e non sono portate.
- Evoluzioni: SG evolve se fra le evoluzioni **a livello** c'è un solo
  bersaglio, quindi Slowpoke → Slowbro (Slowking è per scambio). Qui si
  ferma se la specie ha **qualunque** altro bersaglio: Slowpoke, Kirlia,
  Snorunt restano. Lickitung resta (evolve con una mossa).
- Niente cache del riferimento: in SG si invalidava solo all'inizio delle
  lotte con allenatore.
- Uova escluse dalla media (in SG un uovo contava come Lv1... di solito
  scartato comunque dalla regola dei 7 livelli).

## Verifiche in emulatore (salvataggio dell'utente, Route 43, 6 medaglie, NORMAL, squadra tutta Lv38)

Strumenti: debug `Trainers… → Trainer 1 → Try Battle` (usa l'id vero), stati
salvati di mGBA (Shift+F*n* / F*n*), byte `soundAndFieldOptions` scritto via
GDB a `gSaveBlock3Ptr + 0x38` (`0xC1` = FAST HMS + entrambe ON).

- **Brent (112)**: OFF Lickitung 26 / Ivysaur 26; ON Lickitung 38 / **Venusaur** 38.
- **Pryce (410)**: previsto dal seed (scarto 0) asso 39; letto 37/38/37/39
  (scritti 31/32/31/33). OFF identico alla ROM di base in specie, livelli e
  tutte le statistiche; il byte basso della personalità cambia anche fra due
  lotte della base (`TRAINER_MON_RANDOM_GENDER` usa `Random()`).
- **Soldi**: +2439 = 4 × 38 × 15 + 7% (senza scaling 1560).
- **Passeggero**: sesto Pokémon a Lv5 → Brent resta 38; a Lv33 (5 sotto,
  contato) → 37.
- **OWE**: ON 34-36 dove la tabella dice 21-24; OFF 21-24. Lotta con un OWE
  spawnato Lv36 = Lv36 in lotta (nessun doppio scaling).
- **DexNav**: finestra FURRET Lv34, lotta Lv34.
- **Repellente**: lead Lv30 + ON → spawn 34-36; lead Lv30 + OFF → nessuno;
  lead Lv37 + ON → nessuno, e quelli presenti spariscono.
- **Script** (ROM strumentata: script debug 4/7/8 rediretti): Lapras 20 → 38,
  Gyarados rosso 30 → 38 (shiny), con LEVEL CAP spento 38, con HARD 37, OFF
  30; Zapdos 20 resta 20; Steelix del Dojo resta 1.
- Cattura con `TRAINER CATCH` del Venusaur scalato e di un Girafarig OWE:
  squadra e scheda aperte, dati integri.
- Opzioni: menu disegnato, descrizioni nelle due righe, `0xC1` scritto nel
  `.sav` dopo SALVA.

**Non provato in emulatore**: pesca (stesso percorso di `ChooseWildMonLevel`),
Mirror Mode (copia `gEnemyParty` dopo la creazione, quindi riceve la copia
scalata), lotte con due avversari, opzioni scelte dal titolo prima di una
nuova partita (`NewGameInitData` salva e ripristina già tutto il byte).
