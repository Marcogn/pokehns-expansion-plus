# Piano: level scaling di allenatori e selvatici

> **Stato (22/10): fatto.** Fasi 0-6 completate; dettagli, agganci e misure in
> `.claude/rules/level_scaling.md`. Fase 0: nel salvataggio dell'utente
> `LEVEL CAP` è **NORMAL** (`tx_Challenges_LevelCap` = 1), non HARD, con 6
> medaglie → tetto 38, e la squadra è tutta Lv38: nessun buco nel tetto.
> Punto 4 (repellente), lasciato aperto qui: confronta il livello **scalato**.
> **Cambiato dopo la consegna (richiesta dell'utente): una sola opzione**
> `LEVEL SCALING` (allenatori, selvatici e lotte di script insieme) invece
> delle due del punto 9 e di D7; usa un solo bit.
> **Cambiato il 23/10 (richiesta dell'utente): i selvatici hanno un tetto.**
> Senza limite verso l'alto i percorsi lasciati indietro seguivano la
> squadra (Percorso 29 con 8 medaglie a Lv52-54). Ora il riferimento non
> supera il level cap e il livello non supera la tabella di più di 15
> (`B_WILD_SCALING_MAX_ABOVE_TABLE`). Allenatori invariati. Decisione ed
> esempi nella sezione "Dopo la consegna" in fondo; verifiche in
> `.claude/rules/level_scaling.md`.

## Contesto

Segnalazione dell'utente (22/10/2026), partita con `LEVEL CAP` HARD, a
Mahogany Town prima del Gyarados rosso: squadra intorno al 40, selvatici
intorno al 28, allenatori intorno al 32. Poco coinvolgente. Proposta
dell'utente: livellare tutto sul Pokémon di livello più basso della squadra.

**Da chiarire con il salvataggio dell'utente, prima di scrivere codice.**
Con HARD e 6 medaglie il tetto è 37 (`sLevelCapTable_Hard[6]`, `src/caps.c`),
con 5 è 33; NORMAL dà 38. Una squadra al 40 sotto HARD non torna: o i livelli
sono arrivati prima che il tetto fosse acceso, o c'è un percorso che non lo
rispetta (esperienza in lotta, caramelle, Day Care e menu SwSh lo rispettano
tutti: `battle_script_commands.c` ~4309, `pokemon.c` ~6176, `daycare.c`,
`swsh_party_menu.c`). Il `.srm` dell'utente lo dice in un minuto: medaglie,
livelli e `tx_Challenges_LevelCap`. Se c'è un buco nel tetto, è un bug a sé
e va corretto prima (o insieme).

## C'è già: Soulgold ha un sistema completo

Il report (`docs/SOULGOLD_PORT_REPORT.md`, riga ~151) mette "level scaling"
fra i contenuti di SG da non portare. È vero per le **regole** (allenatori di
SG, rivali di SG), non per il **motore**, che non dipende da mappe né squadre.
Il motore viene da un branch esterno, citato nel merge di SG:
`f690a85874 Merge branch 'feature/level-scaling' of
https://github.com/fisham-org/pokeemerald-expansion-features`. Storia in SG
(`git log -- src/level_scaling.c`, letta il 22/10 su `Marcogn/soulgold`
`0162333c`): `8d8ee2b41d` (feature), `f7d494d7a1` (adattamento a expansion
1.15, la nostra base), poi `fa819be537`, `30de8892d1`, `22d9e0d26e`,
`773b869c81`, `ce9a01d93a` (i selvatici non scendono sotto il minimo della
tabella), `39f2cc3037`.

File in SG:

| file | righe | cosa |
| --- | --- | --- |
| `src/level_scaling.c` | 581 | livello di riferimento, varianza, evoluzioni |
| `include/level_scaling.h` | 138 | modi, `struct LevelScalingConfig` |
| `include/config/level_scaling.h` | 78 | parametri |
| `src/data/level_scaling_evolution_overrides.h` | 74 | livello minimo delle evoluzioni non a livello (Gengar 30, Scizor 40…) |
| `src/data/level_scaling_rules.h` | 213 | regole per allenatore: **contenuto di SG** |

Come funziona in SG (letto, non dedotto):

- **Riferimento**: `LEVEL_SCALING_PARTY_AVG`, la media della squadra **senza i
  Pokémon 7 o più livelli sotto il più alto** (`GetPlayerPartyAverageLevel`).
  Esistono anche `PARTY_LOWEST`, `PARTY_HIGHEST`, `TO_LEVEL_CAP`.
- **Allenatori** (`CreateNPCTrainerPartyFromTrainer`): asso = media **+1**,
  meno 0..1 deterministico per salvataggio e allenatore (stessa squadra a ogni
  rivincita); gli altri tengono il distacco dall'asso, al massimo 2 livelli.
  **Scala anche verso il basso**: un giocatore sottolivellato trova allenatori
  più facili. Il pavimento al livello scritto c'è solo per una lista di
  allenatori opzionali. Evoluzioni: spente di default.
- **Selvatici** (`CreateWildMon`): media **−8**, meno 0..2, **mai sotto il
  livello della tabella**. Spento nella Gara Pigliamosche.
- Due opzioni ON/OFF (allenatori, selvatici), in SaveBlock2.
- `gTrainerLevelScalingRules` in `level_scaling_rules.h` è **definito e mai
  letto** (`grep` su `src/` e `include/`): c'è dentro una sezione "TEST
  TRAINERS" con Falkner al level cap. Non va portato.

## Valutazione della proposta "sul Pokémon più basso"

Onestamente è il riferimento più fragile dei quattro:

- **si spegne da solo**: un Pokémon appena catturato o un portatore di MN in
  squadra riporta tutto ai livelli di HnS, senza che il giocatore lo voglia;
- **si aggira**: un Lv5 in squadra annulla lo scaling;
- è "giusto" solo con una squadra omogenea, cioè il tuo caso di oggi.

La media senza i passeggeri (quella di SG) risolve entrambi i problemi del
minimo e non punisce chi alleva un asso come farebbe il massimo. **Consiglio
quella.** Il minimo resta disponibile come modo, se lo vuoi provare.

Con la tua squadra di oggi (media ~40): allenatori 40-41, capipalestra idem
(Pryce scritto 37-38 nella terza variante), selvatici 30-32 dove la tabella
dice 28.

## Differenze proposte rispetto a SG (adattamento a HnS)

1. **Pavimento al livello scritto per tutti gli allenatori.** HnS è già
   tarato: i capipalestra hanno tre varianti secondo l'ordine delle medaglie
   (`TRAINER_PRYCE_1_HNS`, `_1_2_`, `_1_3_`, scelte in
   `MahoganyTown_Gym_hns/scripts.inc`). Lo scaling serve a renderlo più
   difficile quando sei avanti, non più facile quando sei indietro. In SG è
   `useAuthoredLevelFloor`, oggi acceso solo per la lista opzionale.
2. **DexNav**: in Hasep la ricerca chiama `CreateWildMon(species, level)`
   (`dexnav.c` ~1421 e ~1531) con un livello che la finestra di ricerca ha già
   mostrato (`sText_MonLevel`). Con l'aggancio in `CreateWildMon` il livello
   in lotta non coinciderebbe con quello mostrato. Scalare in
   `DexNavTryGenerateMonLevel` / `GetEncounterLevelFromMapData` e non due
   volte.
3. **OWE**: il livello si fissa allo spawn leggendo `gEnemyParty[0]` dopo la
   generazione (`wild_encounter_ow.c` ~1484) e la lotta ricrea il Pokémon da
   quel livello (~610, `CreateMonWithIVs`, non `CreateWildMon`). Quindi
   l'aggancio in `CreateWildMon` scala gli OWE una volta sola: va verificato,
   non dato per buono.
4. **Repellente e Sguardo Feroce/Intimidazione**: in `wild_encounter.c` ~658
   `IsWildLevelAllowedByRepel(level)` guarda il livello **della tabella**,
   prima dello scaling. In SG è uguale. Va deciso se il repellente confronta
   il livello scalato (coerente con quello che incontri) o no.
5. **Soldi**: `GetTrainerMoneyToGive` usa il livello **scritto** dell'ultimo
   Pokémon (`battle_script_commands.c` ~6232). Con lo scaling la ricompensa
   resta quella di HnS. Deciso in D6: va scalata.
6. **Mirror Mode** (`battle_main.c` ~667) copia la squadra nemica **dopo** la
   creazione, quindi il giocatore riceve la copia scalata: coerente, da
   verificare in emulatore.
7. **Esclusioni**: strutture, link, e-Reader, Trainer Hill sono già fuori dal
   ramo; da escludere anche il partner in lotta multi, il primo tutorial e i
   rivali del primo scontro (SG azzera varianza e aumento per loro).
   `TRAINER CATCH`: un Pokémon catturato arriva al livello scalato, anche
   oltre il level cap (non guadagna esperienza finché il tetto non sale).
8. **Lotte di script** (`setwildbattle`: Gyarados rosso, Sudowoodo, Snorlax,
   leggendari) **non** passano da `CreateWildMon`: serve un aggancio a parte
   (deciso in D4: scalate sul level cap, leggendari esclusi).
9. **Spazio nel salvataggio**: due bit ON/OFF. In Hasep `ChallengeSettings`
   ha 0 bit liberi; `soundAndFieldOptions` ne ha esattamente **2**
   (`unused:2`, `include/global.h` ~372), azzerati dalla migrazione
   `saveVersion < 8` e mai scritti da allora (da riverificare con `grep`
   prima di usarli). Quindi niente migrazione nuova: polarità 0 = OFF.
   `NewGameInitData` salva e ripristina già tutto il byte
   (`savedSoundAndField`); `oak_speech_hns.c` va controllato
   (`.claude/rules/save.md`).
10. **Cache del riferimento**: in SG si invalida solo all'inizio di una lotta
    con allenatore, quindi per i selvatici può restare quella dell'ultima
    lotta con allenatore. Ricalcolare a ogni generazione costa sei
    `GetMonData`: niente cache, o invalidazione anche lì.

## Decisioni dell'utente (22/10)

| # | domanda | risposta |
| --- | --- | --- |
| D1 | Riferimento | **media senza passeggeri** (quella di SG, spiegata sotto) |
| D2 | Allenatori più facili se sei indietro? | **no: mai sotto il livello scritto** |
| D3 | Scarti | **allenatori +1, selvatici −2** (non −8 come SG) |
| D4 | Lotte di script | **scalate sul level cap; i leggendari no** |
| D5 | Evoluzioni | **sì per gli allenatori, no per i selvatici** |
| D6 | Soldi | **sì, sul livello scalato, più un bonus in percentuale che cresce con il level cap attuale** |
| D7 | Dove | **menu opzioni** |

**"Senza passeggeri"** (D1): la media esclude i Pokémon della squadra che
stanno **7 o più livelli sotto il più alto** (`GetPlayerPartyAverageLevel` in
SG). Squadra 41, 40, 40, 39, 38 + un Abra Lv5 appena catturato: la media
conta solo i primi cinque (≈40), non scende a 34. Il portatore di MN o il
nuovo arrivato non abbassano tutto, e allenare un solo asso non alza tutto.

### Cosa ne segue (specifica)

- **Allenatori** (D2, D3, D5): asso = media **+1**, meno 0..1 deterministico
  (stessa squadra a ogni lotta); gli altri tengono il distacco dall'asso
  (massimo 2); **mai sotto il livello di HnS** per nessun Pokémon, quindi con
  una squadra indietro si gioca HnS com'è. Evoluzioni solo per allenatori e
  solo quelle a livello non ambigue (`EvolveSpeciesForLevel` di SG): Pidgey
  Lv40 → Pidgeot sì; Eevee, Poliwhirl, Slowpoke, evoluzioni a scambio o pietra
  no. Le mosse scritte da HnS restano.
- **Selvatici** (D3, D5): media **−2**, meno 0..2, mai sotto la tabella
  (dal 23/10 anche mai oltre il level cap e mai più di 15 sopra la tabella:
  vedi "Dopo la consegna"). Con
  la squadra di oggi (media ~40): 36-38 dove la tabella dice 28.
  **Avvertenza onesta**: è molto più alto di SG (−8) e di HnS (oggi i
  selvatici stanno circa 9 sotto il tetto HARD). Gli incontri a passi
  diventano quasi lotte con allenatori, e anche la cattura e la fuga
  cambiano. Lo scarto è una costante di `include/config/level_scaling.h`:
  si prova −2 e lo si ritocca con un numero, senza riscrivere nulla.
- **Lotte di script** (D4): `setwildbattle` → `CreateScriptedWildMon`
  (`script_pokemon_util.c`; `scrcmd.c` ~3362), cioè Gyarados rosso, Snorlax,
  Sudowoodo, Electrode e simili. Livello = **level cap attuale**, mai sotto il
  livello scritto. **Esclusi** leggendari, sub-leggendari, mitici e Ultra
  Beast (flag di `SpeciesInfo`: `isRestrictedLegendary`, `isSubLegendary`,
  `isMythical`, `isUltraBeast`) e i roamer. **Esclusi anche i Pokémon degli
  alberi di bacche** (`berry.c` ~2359, Lv14-16): passano dalla stessa
  funzione, ma restano sempre come con lo scaling spento (decisione
  dell'utente); il controllo va fatto apposta, perché "stessa funzione"
  vuol dire che senza un'esclusione esplicita verrebbero scalati.
  **Trappola già vista**: con `LEVEL CAP` OFF, `GetCurrentLevelCap()`
  restituisce `MAX_LEVEL` (`B_LEVEL_CAP_TYPE` è `LEVEL_CAP_NONE`), quindi un
  Gyarados rosso Lv100. Senza tetto acceso si usa la tabella NORMAL per
  medaglie (`sLevelCapTable_Normal`, stessa fonte del tetto). Le lotte di
  script seguono l'opzione dei **selvatici**.
- **Soldi** (D6): `GetTrainerMoneyToGive` legge il livello scritto dell'ultimo
  Pokémon (`battle_script_commands.c` ~6232): va letto quello vero, da
  `gEnemyParty`. Questo da solo fa già crescere i soldi con il livello.

- **Bonus sui soldi** (D6, chiarito dall'utente: "a seconda del livello
  attuale di level cap"): sopra la ricompensa calcolata sul livello vero, un
  **+1% ogni 5 livelli del tetto attuale**, arrotondato per difetto: tetto
  37 → +7%, 56 → +11%, 70 → +14%, 100 → +20%. Il tetto è lo stesso delle
  lotte di script (tabella NORMAL se `LEVEL CAP` è spento, mai il 100 di
  `MAX_LEVEL` prima del tempo). Vale solo con lo scaling degli allenatori
  acceso. Il divisore (5) è una costante in `include/config/level_scaling.h`:
  la proporzione è una proposta, non una richiesta dell'utente.

## Fasi

0. **Salvataggio dell'utente**: leggere medaglie, livelli, opzioni. Se il
   tetto ha un buco, aprire un problema a parte.
1. **Port del motore** da SG (`src/level_scaling.c`, i due header, gli
   override delle evoluzioni), **senza** `level_scaling_rules.h`. Confrontare
   le firme delle funzioni chiamate (CLAUDE.md §6): `CreateNPCTrainerPartyFromTrainer`
   in SG ha un argomento `trainerId` in più, `AssignTrainerMonMoves` al posto
   di `CustomTrainerPartyAssignMoves`, `heldItem[]` multipli. `nm
   --defined-only` sugli `.o` prima di aggiungere simboli.
2. **Allenatori**: aggancio in `battle_main.c`, pavimento (D2), esclusioni,
   evoluzioni (D5).
3. **Selvatici**: `CreateWildMon`, DexNav (punto 2), OWE (punto 3), repellente
   (punto 4), Gara esclusa; lotte di script sul level cap (D4).
4. **Opzioni**: due voci, i 2 bit, `oak_speech_hns.c`.
5. **Verifica in emulatore** (`docs/claude/emulator.md`), con il salvataggio
   dell'utente:
   - **un caso che fallisce senza la modifica**: stesso allenatore di Route 43
     con OFF e con ON, livelli letti da `gEnemyParty` via GDB;
   - un capopalestra (Pryce) e una rivincita: stessa squadra due volte
     (varianza deterministica);
   - selvatici a passi, OWE, DexNav (livello mostrato = livello in lotta),
     pesca; repellente;
   - squadra con un passeggero Lv5: il riferimento non cambia;
   - Gyarados rosso: livello = tetto con LEVEL CAP acceso, tabella NORMAL
     con LEVEL CAP spento (mai 100); un leggendario resta al suo livello;
   - soldi dopo una lotta scalata: calcolati sul livello vero, più il bonus
     del tetto (conto a mano confrontato con quello che arriva);
   - un Pokémon da un albero di bacche: Lv14-16 anche con lo scaling acceso;
   - OFF: livelli identici a prima, byte per byte;
   - **aprire la squadra** dopo una cattura (`TRAINER CATCH`) di un Pokémon
     scalato (`.claude/rules/trainer_catch.md`).
6. `CHANGELOG.md`, `docs/SOULGOLD_PORT_REPORT.md` (il motore passa da "non
   portare" a portato, le regole restano fuori), una regola
   `.claude/rules/level_scaling.md` con `paths:` su `src/level_scaling.c`,
   `src/battle_main.c`, `src/wild_encounter.c`.

## Sessione consigliata

**Una sola sessione**, nuova, sul branch che le assegna l'ambiente.

- Modello: il più capace disponibile (Opus), sforzo alto: il port tocca la
  creazione delle squadre e dei selvatici, che sono percorsi caldi.
- Serve: emulatore (mGBA headless + GDB, guida in `docs/claude/emulator.md`),
  il **`.srm` di Mahogany** dell'utente caricato all'inizio, e un clone di
  sola lettura di Soulgold (`GIT_LFS_SKIP_SMUDGE=1 git clone --depth 1
  https://github.com/Marcogn/soulgold /home/user/marcogn/soulgold`).
- Le decisioni sono tutte in questo file.
- Se il TcloneR (`docs/plans/tcloner.md`) va prima, il bit `trainerCatch`
  liberato lì **non** serve qui: questo piano usa solo `soundAndFieldOptions`.

Prompt da incollare:

> Implementa `docs/plans/level-scaling.md` con le decisioni scritte nel
> piano. Ti allego il mio salvataggio a Mahogany: parti dalla
> fase 0. Consegna la ROM zippata e apri la PR.

## Dopo la consegna: tetto e margine dei selvatici (23/10)

Domanda dell'utente: con lo scaling attivo, un Pidgey del Percorso 1 può
arrivare a Lv80 se la squadra è a 80? Sì: il livello dei selvatici seguiva la
media della squadra senza limite verso l'alto. Il timore specifico, "a Kanto
si riparte da livelli bassi", **non** vale in HnS: le tabelle delle mappe
`*_HNS` di Kanto sono già alte (Percorso 1 42-48, Viridian 45-47, Brock 66,
Blue 67-69); le tabelle Lv2-5 di `wild_encounters.json` sono quelle di
FireRed (`MAP_ROUTE1`), non usate da HnS. Il problema vero era un altro: ogni
percorso lasciato indietro ti segue (Percorso 29 con 6 medaglie a Lv34-36).

Opzioni discusse, con tabelle di esempio per +5, +10 e +15:

1. riferimento limitato al level cap. Con `LEVEL CAP` acceso non scatta
   mai: l'esperienza (`pokemon.c` ~7996) e le caramelle (`party_menu.c`
   ~6034) si fermano già al tetto;
2. margine massimo sopra il tiro della tabella.

**Scelta dell'utente: 1 + 2 con margine +15, solo per i selvatici**;
allenatori come prima (li affronti una volta, quando la loro zona è quella
attuale; le rivincite devono restare al passo). Effetti:

| caso | prima | ora |
| --- | --- | --- |
| Percorso 43 (21-24), squadra 38 | 34-36 | 34-36 |
| Percorso 29 (2-3), squadra 38 | 34-36 | 17-18 |
| Kanto Percorso 1 (42-48), squadra 70 | 66-68 | 57-63 |
| Kanto Percorso 1, squadra 100 (dopo la Lega di Kanto) | 96-98 | 57-63 |

Limite accettato: il margine è assoluto, quindi i percorsi con tabella molto
bassa rispetto al momento in cui si attraversano (Percorso 36 4-6, Percorso
32 5-7) non superano Lv19-22. Un margine in percentuale del tetto è stato
nominato come alternativa, mai calcolato.

