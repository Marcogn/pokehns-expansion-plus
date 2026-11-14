# Piano: cattura nell'overworld (stile Leggende Pokémon: Arceus)

**Stato (14/11): piano scritto, nessuna fase iniziata.** Prossima: fase 1.

Si appoggia agli OWE già in gioco: leggere prima
[`.claude/rules/overworld_encounters.md`](../../.claude/rules/overworld_encounters.md)
e, per i fatti del port, [`overworld-encounters.md`](overworld-encounters.md).

## 1. Cosa si fa

- **Lancio della Ball nell'overworld**: con **L** il giocatore lancia una Ball
  dritta davanti a sé; se sulla linea c'è un Pokémon selvatico visibile (OWE)
  si tenta la cattura senza entrare in lotta, con un'animazione vera (volo,
  Pokémon risucchiato, scosse, esito).
- **Toccare o farsi toccare** resta com'è: lotta classica.
- **Passo lento** (al posto del gattonare): **A tenuto + direzione**, ovunque e
  non più solo durante una ricerca DexNav.
- **Percezione migliore**: i Pokémon sentono il giocatore in base a quanto
  rumore fa (corsa, camminata, passo lento), lo vedono in un cono, non lo
  vedono attraverso gli ostacoli né quando è nell'erba alta.
- **Meno Pokémon visibili** e ricambio più lento.
- **Ball speciali meno care.** Niente crafting.

## 2. Decisioni prese

Risposte dell'utente (14/11):

| # | Decisione |
| --- | --- |
| D1 | **Comandi**: nessuna preferenza, quindi la proposta: **L lancia**, **A tenuto + direzione = passo lento**. |
| D2 | **Probabilità**: formula normale della lotta a PS pieni, **×2 se il Pokémon non ha notato il giocatore**. Se la cattura fallisce il Pokémon si allerta: fugge o attacca secondo l'indole. |
| D3 | **Densità**: tetti **4 / 2 / 2** (erba / acqua / grotta), **Safari 3**; ricambio a tetto pieno da 240 a **480 frame**. |
| D4 | **Prezzi**: Poké/Great/Ultra restano 10/15/30. Net, Nest, Dive, Dusk, Timer, Repeat, Luxury da 1000 a **100**; Heal da 300 a **50**. |

Scelte di progetto prese qui, **con un default**: l'utente può cambiarle prima
della fase che le realizza.

| # | Scelta | Default | Fase |
| --- | --- | --- | --- |
| P1 | Quale Ball parte | l'ultima lanciata (`gLastThrownBall`) se è in borsa, altrimenti la prima della tasca Ball; in fase 5 **L tenuto + ←/→** la cambia | 3, 5 |
| P2 | Gittata | 5 caselle in linea retta; si ferma al primo ostacolo o dislivello | 3 |
| P3 | Nuzlocke / One Type / Species Clause | le stesse regole della lotta: dove la cattura sarebbe bloccata la Ball non parte (messaggio); una cattura riuscita segna la zona | 3 |
| P4 | Leggendari erranti e sciami | il lancio fallisce sempre e parte la lotta (i loro dati vivono fuori dall'oggetto) | 3 |
| P5 | Zona Safari, Gara Pigliamosche, Azienda Lotta | lancio **spento** nella prima versione | 3 |
| P6 | Esperienza alla cattura | nessuna (in lotta `B_EXP_CATCH` la dà: qui non c'è chi ha partecipato) | 3 |
| P7 | BUTTON MODE `L=A` | il lancio non è disponibile (L è già A, `src/main.c:300`); nessuna opzione nuova | 3 |
| P8 | Squadra piena | va al PC come in lotta; box pieni: la Ball non parte | 3 |

Il ×2 di D2 è una scelta di bilanciamento **nostra**: non è un valore preso da
Leggende Arceus.

## 3. Fatti verificati (14/11, commit `d6520855`)

**Non c'è nulla di già scritto**: nessun lancio di Ball, nessun proiettile
nell'overworld, nessun `#if IS_HNS` che lo spegne. Non è un port da Soulgold.

Cosa c'è già e si riusa:

- **Passo lento**: `PlayerWalkSlow()` e `gPlayerAvatar.creeping`
  (`src/field_player_avatar.c:906-945`), oggi solo con `DN_FLAG_SEARCHING` e A
  tenuto. Attenzione al blocco della corsa (`:947`): con AUTORUN acceso
  (`autoRun == 0`) è vero anche senza B e fa `return`, quindi il passo lento
  va provato **prima**, come già fa il ramo del DexNav.
- **Tasti liberi**: `FieldGetPlayerInput` (`src/field_control_avatar.c:111`)
  non legge L; `struct FieldInput` ha bit liberi (`input_field_1_1` ecc.). R è
  DexNav / cambio bici, SELECT lo strumento registrato.
- **Percezione attuale**: `CanAwareOWESeePlayer` (`src/wild_encounter_ow.c:2021`).
  Due regole sole: corsa o bici entro `activeDistance` (5) = notato in ogni
  direzione; altrimenti un rettangolo davanti al Pokémon, 4 caselle × 3
  (`viewDistance`, `viewWidth`). `CanOWEReachPlayer` (`:2078`) controlla solo
  il dislivello: gli ostacoli non contano. I sei comportamenti hanno **tutti
  gli stessi numeri** (`src/data/pokemon/wild_encounter_ow_behavior.h`).
  `struct BehaviorOWE` ha 6 bit di padding e campi da 4 bit (massimo 15).
- **"Mi ha notato"** oggi è implicito nello stato della macchina di movimento
  (`sprite->sTypeFuncId` ≥ 7, `SetSavedOWEMovementState`,
  `src/event_object_movement.c:12366`): la fase 2 deve esporlo con una
  funzione, perché serve al ×2 di D2.
- **L'OWE non contiene un Pokémon**: l'oggetto tiene specie, shiny, sesso
  (nel `graphicsId`), livello (`sOverworldEncounterLevel`) e categoria. Il
  Pokémon vero nasce in `StartWildBattleWithOWE` (`:574-624`) con
  `PickWildMonNature`, `CreateMonWithIVs`, `TryApplyPokeblockBonus`. La
  cattura deve passare **dalla stessa costruzione** (funzione comune estratta,
  non una copia).
- **Formula di cattura**: `ComputeCaptureOdds`, `ComputeBallData`,
  `ComputeBallShakeOdds` (`src/battle_script_commands.c:10923-11034`) sono
  `static` e leggono `gBattleMons`, `gBattleTypeFlags`, `gLastUsedItem`. Va
  separata la parte pura (specie, livello, PS, stato, Ball, livello del primo
  Pokémon in squadra) e chiamata da tutti e due i lati. Dentro ci sono già:
  EASY CATCH (`IsGuaranteedCatchEnabled`), malus medaglie, bonus livello
  basso. `B_CRITICAL_CAPTURE` è `FALSE`.
- **Dopo la cattura in lotta** succede: `GiveCapturedMonToPlayer`
  (`src/pokemon.c:5422`), `HandleSetPokedexFlag`, soprannome
  (`Cmd_trygivecaughtmonnick`, con `IsNuzlockeNicknamingActive` e
  `skipNicknamePrompt`), `GAME_STAT_POKEMON_CAPTURES`, Ball scritta nel
  Pokémon, effetti di Friend/Heal Ball. La fase 3 fa l'elenco completo
  leggendo `Cmd_givecaughtmon` e `BattleScript_SuccessBallThrow`.
- **Blocchi di sfida**: `SetNuzlockeChecks` (`src/nuzlocke.c:445`) legge
  `gEnemyParty[0]` e imposta `NuzlockeIsCaptureBlocked`,
  `NuzlockeIsSpeciesClauseActive`, `OneTypeChallengeCaptureBlocked`.
- **Grafica della Ball nell'overworld già in ROM**: `gObjectEventPic_*Ball`
  (`src/data/object_events/object_event_graphics.h:407-458`), una per tipo,
  con palette `OBJ_EVENT_PAL_TAG_BALL_*` (`OW_FOLLOWERS_POKEBALLS`), e le
  azioni `MOVEMENT_ACTION_ENTER_POKEBALL` / `EXIT_POKEBALL` dei follower.
  **Nessun file grafico nuovo è necessario.**
- **Densità**: `OWE_SPAWNS_MAX*` (`include/wild_encounter_ow.h:4-14`),
  `OWE_SPAWN_TIME_REPLACEMENT` 240 (`src/wild_encounter_ow.c:57`).
  `OWE_SPAWNS_MAX` dimensiona gli array degli slot e i `localId` 247-252.
- **Prezzi**: `src/data/items.h`. Le Ball con Ghicocca hanno il prezzo
  commentato (0): non si toccano.
- Lo spawn si ferma con i controlli bloccati (`UpdateOverworldWildEncounter`,
  `:440`): durante il lancio, con `LockPlayerFieldControls`, `gEnemyParty[0]`
  non viene riscritto.

Trappole già note che qui mordono (dalla regola degli OWE):

- VRAM: nel Safari restavano 10 tile su 1024; `OWE_SPRITE_TILE_HEADROOM` ne
  riserva 32. Lo sprite della Ball e gli effetti devono starci, e **uno sprite
  che non si crea non deve lasciare uno script in attesa**.
- Mai `assertf` su stati raggiungibili (schermata blu).
- Un OWE in acqua ha lo sprite vero nascosto e uno da nuoto sopra, più 2
  palette: il risucchio nella Ball va provato a parte.
- `gEnemyParty[0]` azzerato: guardare il Pokémon catturato in squadra (il
  precedente è il Bad Egg di `trainer_catch.md`).

## 4. Spazio misurato (build pulita di `d6520855`)

| | Usato | Limite | Libero |
| --- | --- | --- | --- |
| ROM | 31 902 628 B (95,08%) | 32 MB, errore CI al 99,5% | 1 651 804 B. Le mega (`megas.md`, 1 574 944 B) oggi porterebbero la ROM al **99,77%**, già oltre il limite senza questa feature: è un problema delle mega, non di questo piano, ma ogni kB qui lo aggrava |
| SaveBlock3 | 96 B | 116 | 20 B |
| VRAM sprite | — | 1024 tile | da misurare in fase 4 nel caso peggiore (rotta con 4 OWE + follower) |

Costo previsto: **solo codice** (stima 8-15 kB, da rimisurare a ogni fase),
**zero byte di salvataggio** (`gLastThrownBall` è già in EWRAM, nessuna
opzione nuova), nessuna grafica nuova. Se una fase avesse bisogno di un bit
salvato, si ferma e lo chiede: `soundAndFieldOptions` è pieno (`save.md`).

## 5. Fasi

Una fase per sessione (`/next-phase overworld-catch`). Modelli scelti fra
Fable, Opus e Sonnet come chiesto dall'utente: Fable solo dove un errore
costa di più del suo prezzo.

| # | Fase | Modello | Cancello per passare oltre |
| --- | --- | --- | --- |
| 1 | **Densità e prezzi** (D3, D4): `OWE_SPAWNS_MAX_LAND/WATER/CAVE/SAFARI`, `OWE_SPAWN_TIME_REPLACEMENT`, prezzi in `items.h`. `OWE_SPAWNS_MAX` resta 6 (dimensiona slot e `localId`) | **Sonnet 5.5** | build verde; in emulatore il conteggio degli OWE su una rotta d'erba non supera 4 in 60 s di gioco (misurato in frame) e 2 in grotta; screenshot di un negozio con i prezzi nuovi. `.srm` non necessario |
| 2 | **Passo lento e percezione**: A tenuto = passo lento fuori dal DexNav; rumore per andatura (corsa/bici 5, camminata 2, passo lento 0: solo vista); cono di vista con numeri diversi per comportamento; ostacoli che coprono la vista; giocatore nell'erba alta non visto oltre 1 casella; funzione pubblica `IsOWEAwareOfPlayer()` | **Opus 5.5** | build verde; prove in emulatore con posizioni lette via GDB: da dietro a passo lento si arriva adiacenti senza essere notati, camminando si viene notati a 2, correndo a 5, di fronte alla distanza di vista, dietro un ostacolo mai. A premuto davanti a un NPC o un cartello interagisce ancora; ricerca DexNav invariata. `.srm` non necessario |
| 3 | **Nucleo della cattura**, senza animazione (Ball istantanea, messaggio): tasto L, bersaglio sulla linea (P2), consumo della Ball, Pokémon costruito dalla funzione comune con la lotta, formula condivisa con ×2 se non allertato (D2), tutto ciò che la lotta fa dopo una cattura, blocchi P3-P8, reazione al fallimento | **Fable 5.2** | build verde; `ComputeCaptureOdds` in lotta dà gli stessi numeri di prima su 3 casi (ROM strumentata). In emulatore: cattura con Master Ball, **guardando il Pokémon in squadra** (specie, livello, shiny, Ball giusti, non Bad Egg), Ball scalata, Pokédex segnato; un lancio fallito allerta il Pokémon; zona Nuzlocke già usata, box pieni e Safari rifiutano il lancio; un OWE in acqua o catturato o escluso con il motivo scritto. **Serve il `.srm` dell'utente** (squadra e borsa vere, impostazioni di sfida) |
| 4 | **Animazione**: posa di lancio (derivata dai frame che il giocatore ha già, guardando l'immagine), Ball in volo ad arco con lo sprite del suo tipo, risucchio, caduta, 0-3 scosse da `ComputeBallShakeOdds`, cattura o uscita con "!" | **Opus 5.5** | build verde; tile e palette libere misurate nel caso peggiore, nessuno script in attesa se uno sprite non si crea (provato forzando il fallimento); sequenza di screenshot guardati per cattura, fuga dopo 1 e dopo 3 scosse, nelle 4 direzioni, di giorno e in grotta. `.srm` utile, non necessario |
| 5 | **Rifinitura e documentazione**: scelta della Ball con L tenuto + ←/→ e icona con quantità (P1), suoni, messaggi; `CHANGELOG.md`, `README.md`, regola `.claude/rules/overworld_catch.md` (con `paths:`), indice in CLAUDE.md §7, `docs/SOULGOLD_PORT_REPORT.md` (feature nuova, non un port) | **Sonnet 5.5** | build verde; cambio Ball provato con 1, 2 e 0 tipi in borsa; testi riletti contro il codice; `docs_validate` verde |
| 6 | **Revisione finale**: `/code-review` medio sul diff intero, ogni punto aperto delle fasi provato in emulatore, bilanciamento (quante Ball per una cattura a inizio gioco e a metà), ROM consegnata zippata | **Opus 5.5** | build e CI verdi, percentuale di ROM annotata, punti aperti chiusi o motivati. **Serve il `.srm` dell'utente** |

Perché questi modelli: le fasi 1 e 5 sono costanti, dati e testo con un
cancello che prende gli errori. La 2 e la 4 toccano invarianti nascosti (la
macchina a stati del movimento, la VRAM condivisa con icone ed effetti). La 3
apre il codice di lotta, crea un Pokémon fuori dalla lotta e incrocia Nuzlocke
e salvataggio: è l'unico punto in cui un errore silenzioso arriva nel
salvataggio dell'utente.

## 6. Scartato, e perché

- **Crafting delle Ball**: escluso dall'utente, per ora.
- **Gattonare**: non esistono frame del giocatore accovacciato; il passo lento
  c'è già.
- **Strumenti da lancio di Leggende Arceus** (bacche, fango, fumogeni) e
  **alfa**: fuori da questo piano. Gli alfa restano l'idea successiva
  (`overworld-encounters.md` §1).
- **Mira libera o a parabola regolabile**: il motore si muove a caselle; una
  linea retta nella direzione dello sguardo è leggibile e costa poco.
- **Lancio nel Safari e alla Gara** (P5): hanno Ball contate e regole proprie
  (`gNumSafariBalls`, punteggio della Gara). Si possono aggiungere dopo.
- **Un'opzione per spegnere la feature**: non c'è posto nel salvataggio senza
  allargare una struct, e chi non la vuole non preme L.
- **Tetto `OWE_SPAWNS_MAX` a 4**: sposterebbe i `localId` degli OWE; bastano
  i tetti per zona.
