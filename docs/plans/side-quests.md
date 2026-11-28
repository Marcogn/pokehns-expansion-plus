# Piano: missioni secondarie (13 missioni del quest book)

**Stato (28/11): fase 1 fatta; prossima: fase 2 (Opus 5.5).**

Missioni scelte dall'utente il 27/11 da una proposta di 15 (numeri della
proposta tra parentesi, tenuti per riferimento). Il "giardino di Bill"
(`docs/plans/new_quests.md` §1) **non** è in questo piano: chiede una mappa
nuova e resta a parte. Lo sciame (proposta 10) è scartato (§6).

Prima di toccare il codice leggere `.claude/rules/quest_book.md`, la ricetta
`/add-quest` e `docs/plans/quest-log.md` §6.

## 1. Cosa si fa

| Id | Missione (proposta) | Chi / dove | Obiettivo (a step dove indicato) | Premio finale |
| --- | --- | --- | --- | --- |
| A | **Maestro del lancio** (4) | Ranger, Percorso 36 | 3 sottomissioni: 10 catture senza essere notato, 5 lanci a segno da 5 caselle, 3 catture in acqua da terra | **Cromamuleto Cattura** (`ITEM_CATCHING_CHARM`) |
| B | **La scuola ninja** (5) | allieva di Janine, Fucsiapoli | iniziare 5 lotte toccando un selvatico che non ti ha notato | **Focus Sash** |
| C | **Eevee al sole** (3) | signora della Villa, Azzurropoli | dopo Red's Path: mostrarle un Umbreon (`new_quests.md` §3) | Eevee Lv 5 con amicizia vicina alla soglia |
| D | **Gli starter di Oak** (2) | Oak, Biancavilla | 3 step a 3, 5 e 7 sfide vinte di Red's Path (`new_quests.md` §2) | Bulbasaur Lv 5, Squirtle Lv 10, Charmander Lv 5, OT OAK, ID **15196** |
| E | **Le Ball di Kurt** (6) | Kurt, Azalina | a step: una cattura con ognuna delle 7 Ball ghicocca | **Dream Ball ×5** |
| F | **Il Pokédex di Johto** (7) | Elm | a step: 50/100/150/200/250/282 catturati | **Cromamuleto** (`ITEM_SHINY_CHARM`) |
| G | **Collezionista di tipi** (8) | aiutante di Elm | a step: 6/12/18 tipi diversi catturati | **Exp Charm** |
| H | **Rapporto Unown** (9) | laboratorio delle Rovine d'Alfa | a step: 7/14/21/28 forme diverse catturate | **Choice Scarf** |
| I | **La pensione** (11) | pensione, Percorso 34 | 10 uova schiuse | **Ovalcromamuleto** (`ITEM_OVAL_CHARM`) |
| J | **Allenamento Hyper** (12) | NPC nuovo (luogo da scegliere in fase) | portargli un Pokémon al Lv 100; **solo con PLAYER IVs normali** | **Tappo d'Oro**; poi l'NPC fa l'Hyper Training e vende Tappi |
| K | **Rivincite al telefono** (13) | Pokégear / NPC da scegliere | vincere 10 rivincite | **Choice Band** |
| L | **Censimento DexNav** (14) | aiutante di Oak | 18 step, uno per percorso di Johto (29-46): catturare tutte le specie della mappa | Exp Candy per percorso, alla fine **Eviolite** |
| M | **Il custode del Safari** (15) | Zona Safari, Fucsiapoli | una specie rara per ognuna delle 4 aree | **Assault Vest** |

Premi intermedi (step) e Tappi Bottiglia: tabella in §2, D9.

## 2. Decisioni prese

Risposte dell'utente (27/11).

| # | Decisione |
| --- | --- |
| D1 | **Dialoghi li scrive l'utente in sessione.** All'inizio di ogni fase la sessione elenca i dialoghi che servono, ognuno con: NPC (grafica, mappa, posizione), **situazione completa** (stato della missione, cosa sa il giocatore, cosa succede prima e dopo), condizione che lo fa partire e domande sì/no. L'utente li scrive; la sessione li sistema (a capo `\n`/`\l`/`\p`, larghezza della finestra, `{PLAYER}`, `{STR_VAR_*}`), li mette negli script e mostra il risultato prima del commit. I testi del gioco sono in inglese: se l'utente scrive in italiano la sessione propone la traduzione e la fa approvare. Anche i testi del quest book (nome, step, sottomissioni) seguono questa regola. |
| D2 | **Missioni che usano la cattura OW** (A, sottomissione "non visto"): a OVERWORLD CATCH accesa conta il lancio su un Pokémon che non ti ha notato; **spenta**, conta una cattura in lotta **al primo turno** con Poké, Mega o Ultra Ball (nessun'altra Ball). Le sottomissioni "lancio lungo" e "in acqua da terra" esistono solo a cattura accesa: spenta, il quest book le mostra con "richiede OVERWORLD CATCH". In futuro le lotte normali spariranno: allora l'alternativa si toglie. |
| D3 | **Cattura critica solo col Cromamuleto Cattura.** Oggi `B_CRITICAL_CAPTURE` è `FALSE` (`include/config/battle.h:365`): senza amuleto il gioco resta com'è. Con l'amuleto in borsa la critica è attiva con la formula di expansion (`CriticalCapture`, `src/battle_script_commands.c:11126`, ×2 di `B_CATCHING_CHARM_BOOST` 100) **in lotta e nel lancio OW** (una sola scossa, esito deciso in `ResolveImpact`). |
| D4 | **Unown: step a numero** 7/14/21/28 forme diverse catturate. Gli incontri non si toccano (i puzzle non sbloccano lettere, §3). Le lettere prese stanno in una maschera di 28 bit in SaveBlock3. |
| D5 | **Tappi Bottiglia**: premio di alcuni step **e** in vendita dall'NPC della J dopo la missione. Prezzo da confermare nella fase della J (proposta 20.000). |
| D6 | **Eevee**: Lv 5 con amicizia vicina alla soglia di `FRIENDSHIP_EVO_THRESHOLD`, così basta un livello di giorno per Espeon. Niente Pietra Splendore: in questo repo Espeon si ottiene solo con l'amicizia di giorno (`src/data/pokemon/species_info/gen_1_families.h:18871`). Valore esatto di amicizia da proporre nella fase. |
| D7 | **ID allenatore**: Oak **15196** (era 151096, 15/10/96, la data di Blu in Giappone secondo i siti ufficiali giapponesi; Bulbapedia la indica come data dell'annuncio su CoroCoro), Red **28998** (era 280998, 28/09/98, uscita di Rosso e Blu in Nord America). Il riassunto stampa `(u16)OTID` su 5 cifre (`src/pokemon_summary_screen.c:3985`): i numeri a 6 cifre sarebbero apparsi come 20024 e 18854. Fonti: [Bulbapedia, Red and Blue](https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9mon_Red_and_Blue_Versions), [Bulbapedia, Blue (Japanese)](https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9mon_Blue_Version_(Japanese)). |
| D8 | **Allenamento Hyper solo con PLAYER IVs normali** (`tx_Challenges_MaxPartyIVs == 0`, `include/global.h:316`; 1 = tutti 31, 2 = 30-31, `src/pokemon.c:3277`). L'opzione è `LOCK_FREE` (`src/challenge_menu.c:218`): la condizione si legge **ogni volta** (missione bloccata e NPC che rifiuta se l'opzione è diversa da 0), non una volta sola. |
| D9 | **Censimento DexNav (L): tutti i percorsi di Johto**, 29-46, 18 step in ordine di numero. |
| D10 | Premi finali come in §1 (proposta accettata). **Premi degli step**: proposta da confermare nella fase di ogni missione, con questi vincoli: solo oggetti che oggi non si ottengono (§3) o consumabili (Exp Candy, Ball), almeno un Tappo Bottiglia negli step di E, F, G, H, K, L, M. |

## 3. Fatti verificati (27/11)

**Premi che oggi non si ottengono** (cercati in `data/maps/*_hns/scripts.inc`,
`map.json` e `src/shop.c`): Shiny/Oval/Exp/Catching Charm, Choice Scarf/Band,
Focus Sash, Assault Vest, Eviolite, Rocky Helmet, Weakness Policy,
Heavy-Duty Boots, Exp Candy XL, Tappo Bottiglia e Tappo d'Oro, Dream Ball.
**Si comprano già** (non usarli come premio): Cerotto/Capsula Abilità (Mart di
Violapoli, `src/shop.c:1238`), Mente (negozio di fiori di Fiordoropoli), Ball
ghicocca (Kurt, `sKurtBallShopItems`, `src/shop.c:2600`).

**Effetti degli amuleti**: Shiny Charm `src/pokemon.c:3249` e
`src/scrcmd.c:2697`; Oval Charm `src/daycare.c`; Exp Charm
`src/battle_script_commands.c:12192`; Catching Charm solo dentro
`CriticalCapture` (D3).

**Cattura, due strade**: in lotta `Cmd_handleballthrow` / `Cmd_givecaughtmon`
(`src/battle_script_commands.c`); nel lancio `GiveCaughtMon`
(`src/overworld_catch.c:1134`). Un aggancio comune per le missioni (Ball usata,
specie, forma, "non notato", distanza, acqua) va chiamato da tutte e due.
"Non notato" = `!IsOWEAwareOfPlayer` (già usato per il ×2). La **distanza** del
lancio e il **livello** (acqua/terra, `GetThrowLevel`) vanno letti dal record
del lancio: da verificare nella fase 2 che ci siano già.

**Unown**: Rovine d'Alfa (`MAP_RUINS_OF_ALPH_B1F_HNS`, `_OUTSIDE_HNS` giorno e
notte) hanno `SPECIES_UNOWN` con `RANDOM_UNOWN_LETTER` (`src/wild_encounter.c:599`,
`src/wild_encounter_ow.c:639`): **tutte le 28 lettere, casuali, senza legame con i
puzzle**. Altre lettere fisse nelle stanze di Sinjoh. Il Pokédex le conta come
una specie: la maschera di D4 è l'unico modo di sapere quali lettere sono
state prese. `GetUnownSpeciesId(personality)` (`src/wild_encounter_ow.c:1608`).

**Sciami**: in HnS non c'è un sistema di sciami. C'è il motore di Smeraldo
degli "outbreak" (`SetUpMassOutbreakEncounter`, `src/wild_encounter.c:698`),
che nasce solo dagli show della TV (`src/tv.c:1578`); il Gyarados del Lago d'Ira
è un evento di storia.

**Rivincite**: esistono. Circa 40 allenatori in `gRematchTable` sotto
`#if IS_HNS` (`src/battle_setup.c`, da riga ~178), ognuno con id propri per ogni
rivincita (`TRAINER_JOEY_2_HNS`...); script con `trainerbattle_rematch`
(es. `data/maps/Route30_hns/scripts.inc:72`), telefonate in `src/match_call.c`.
Contare le vittorie = contare i flag di allenatore delle colonne 2-5: **da
verificare nella fase** che la vittoria in rivincita alzi il flag dell'id
della rivincita.

**Uova**: `GAME_STAT_HATCHED_EGGS` (`include/constants/game_stat.h:17`),
incrementata in `src/field_control_avatar.c:863`. Conta dall'inizio della
partita: la missione I la legge **rispetto a una base salvata all'accettazione**
(var), se no un salvataggio con molte schiuse la troverebbe già fatta.

**Hyper Training**: solo le macro (`asm/macros/event.inc:2332`, `2339`,
`CanHyperTrain`/`HyperTrain` in `src/script_pokemon_util.c:422`, `453`);
nessuna mappa le usa.

**Safari**: 4 aree con incontri, `MAP_FUCHSIA_CITY_SAFARI_ZONE_{BEACH,BRUSH,CAVE,MOUNTAIN}_HNS`.
Il lancio OW è spento nel Safari (P5 di `overworld-catch.md`): la M si fa in
lotta con le Safari Ball.

**Percorsi di Johto 29-46**: tutti hanno incontri in
`src/data/wild_encounters.json` (quasi tutti con intestazioni giorno e notte).
"Tutte le specie della mappa" va definito nella fase 8 (quali metodi: erba,
acqua, pesca, Spaccaroccia; tutte le fasce orarie).

**Red's Path** (per D): vittorie = flag di `TRAINER_REDS_PATH_<CAPO>_HNS`;
`IsChallengeWon` in `src/reds_path.c:136`.

**Quest book**: `struct QuestInfo` in `include/quest_log.h`. Fonti: VAR (il valore
della var è lo step), FUNC (funzione sullo stato del gioco, 0 bit), OWN (stato
nel salvataggio). Id in coda, mai in mezzo (`include/constants/quests.h`).

## 4. Spazio misurato

- **ROM**: questo container non ha il toolchain ARM (la build si ferma su
  `arm-none-eabi-gcc: command not found`). Ultima misura documentata:
  **95,13%** (fase 10-11 di `overworld-catch`,
  `docs/claude/overworld-catch-history.md:1289`), circa 1,6 MB sotto
  l'avviso del 98%. Testi e script di 13 missioni stanno nell'ordine delle
  decine di KB: la fase 1 rimisura.
- **Quest book** (`include/constants/quests.h`): 4 missioni su 48, 0 slot
  propri su 16, 0 sottomissioni proprie su 32. Questo piano porta a **17
  missioni**; sottomissioni proprie: E 7 + M 4 = **11**.
- **SaveBlock3**: 100 byte su 116, **16 liberi** (`.claude/rules/save.md`). La
  maschera degli Unown ne usa 4 (passo `SAVE_VERSION 16`, fatto in fase 1): ne restano 12.
- **Var**: servono circa 9 (contatori di A, B, I base, e "premi ritirati" di
  E, F, G, H, L, K). `include/constants/vars_hns.h` ha 44 righe `UNUSED`; sono
  ex var di Smeraldo: **la fase 1 deve dimostrare che sono a zero** nel `.srm`
  dell'utente prima di usarle. Flag: 411 `UNUSED` in `flags_hns.h`, stessa
  verifica.

## 5. Fasi

Una fase per sessione (`/next-phase side-quests`). Ogni fase con missioni
comincia dall'elenco dei dialoghi (D1) e aspetta i testi dell'utente prima
di scrivere gli script. Ogni missione nuova: id in coda, voce in
`src/data/quests.h`, `CHANGELOG.md`, verifica in emulatore con un caso che
fallisce prima della condizione.

| # | Fase | Modello | Cancello per passare oltre |
| --- | --- | --- | --- |
| 1 | **Fondamenta**: aggancio comune di cattura (lotta e lancio, §3) con Ball, forma, "non notato", primo turno; maschera Unown in SaveBlock3 con passo 16; var e flag scelti e dimostrati a zero; ROM rimisurata | **Opus 5.5** | build verde; il `.srm` dell'utente carica, maschera e var nuove leggono 0, checksum regge; con una ROM strumentata l'aggancio scatta una volta per cattura in lotta e una per lancio, mai per un lancio a vuoto. **Serve il `.srm`** |
| 2 | **A Maestro del lancio + cattura critica col Cromamuleto** (D2, D3) | **Opus 5.5** | build verde; senza amuleto mai critica (lotta e OW, 50 prove via debug); con l'amuleto la critica compare, in OW con una scossa; le tre sottomissioni si contano solo nei casi giusti (lancio notato non conta, Quick Ball al primo turno a cattura spenta non conta); premio ritirato una volta. **Serve il `.srm`** |
| 3 | **B ninja, C Eevee, J Hyper Training** (D6, D8, D5) | **Sonnet 5.5** | build verde; B: lotta da selvatico che ti ha notato non conta; C: Eevee con l'amicizia scelta, Espeon di giorno dopo un livello; J: con PLAYER IVs ≠ 0 bloccata e NPC che rifiuta, con 0 un Lv 100 riceve il Tappo d'Oro e l'Hyper Training cambia l'IV nel riassunto; Tappi in vendita. Screenshot del quest book |
| 4 | **D Gli starter di Oak** (D7) | **Sonnet 5.5** | build verde; con 2, 3, 5, 7 sfide vinte via debug lo step giusto e un solo starter per step; riassunto con OT OAK e ID 15196; squadra piena gestita (PC o attesa, da decidere con l'utente) |
| 5 | **F Pokédex di Johto, G Collezionista di tipi** | **Sonnet 5.5** | build verde; contatori portati a soglia e oltre via debug: stato, "NEW" e premi giusti, nessun premio doppio; G conta i tipi delle specie catturate (forme regionali comprese: da decidere in fase) |
| 6 | **E Le Ball di Kurt, H Rapporto Unown** | **Sonnet 5.5** | build verde; E: Ball giusta contata in lotta e nel lancio, Ball ghicocca comprata non basta senza cattura; H: due Unown della stessa lettera contano uno; step 7/14/21/28 |
| 7 | **I pensione, K rivincite, M Safari** | **Sonnet 5.5** | build verde; I conta dall'accettazione; K: verificato che la rivincita alza il flag della colonna giusta, 10 vittorie; M: una specie per area, Safari Ball |
| 8 | **L Censimento DexNav** (D9) | **Sonnet 5.5** | build verde; lista delle specie per percorso generata dai dati degli incontri (non scritta a mano) e confrontata con il DexNav in emulatore su 3 percorsi; step che avanza solo a percorso completo |
| 9 | **Documentazione**: CHANGELOG, `quest_book.md` (missioni e id), `README`, `SOULGOLD_PORT_REPORT.md` se serve, CLAUDE.md §7 solo se nasce una regola nuova | **Haiku 5.5** | ogni affermazione della regola controllata sul codice; `/doc-sync` pulito |
| 10 | **Revisione finale**: `/code-review` medio sul diff delle fasi 1-9, punti aperti provati in emulatore, ROM consegnata | **Opus 5.5** | build e CI verdi, ROM sotto il 98%; tutte le 13 missioni aperte nel quest book in tutti e due i temi. **Serve il `.srm`** |

### 5.1 Fase 1, com'è stata fatta (28/11)

- **Aggancio comune**: `SideQuests_OnCatch(const struct SideQuestCatch *)` in
  `src/side_quests.c`, chiamato da `Cmd_givecaughtmon` (dopo
  `GiveCapturedMonToPlayer`, se non `MON_CANT_GIVE`) e da `GiveCaughtMon` del
  lancio. Campi: specie, personality, Ball, `overworld`, `unnoticed`,
  `firstTurn`, `trainerMon`, `thrownFromLand`, `targetOnWater`, `distance`.
  Dettagli e trappole: `quest_book.md`, sezione "Missioni secondarie".
- **Lettere Unown**: `unownLettersCaught` a SB3 +100, `SAVE_VERSION 16`. Il
  passo azzera e **riempie la maschera con gli Unown che il giocatore ha in
  squadra e nel PC** (decisione di questa fase: chi è già stato alle Rovine
  non riparte da zero; chi ha liberato i suoi sì). Oggi la riempie solo
  l'aggancio; la H la legge con `SideQuests_CountUnownLetters`.
- **Var**: `VAR_SQ_THROW_MASTER` (A), `_NINJA` (B), `_OAK_STARTERS` (D),
  `_KURT_BALLS` (E), `_JOHTO_DEX` (F), `_TYPE_COLLECTOR` (G), `_UNOWN_REPORT`
  (H), `_DAY_CARE_BASE` (I), `_REMATCHES` (K), `_DEXNAV_CENSUS` (L): 0x40D8-0x40DA
  e 0x40DE-0x40E4. C, J, M non ne hanno: stato negli slot propri del quest
  book (16 liberi) o nei flag. **Flag**: 16 da 0x498 (`SIDE_QUEST_FLAGS_START`),
  senza nome. Tutti a zero nel `.sav` dell'utente e azzerati dal passo 16.
- **ROM**: 95,14% (95,13% prima; +800 byte).
- **Verificato** (ROM strumentata: contatore e ultimo record in EWRAM, e un
  trucco per una lotta con un Unown; dettagli in
  `docs/claude/save-history.md`): `.sav` dell'utente a versione 8 caricato,
  maschera `0x400000` (la W, calcolata offline dal PC), var e flag a 0;
  cattura in lotta al turno 1 → contatore 1, `firstTurn` 1, lettera I; al
  turno 2 (dopo un cambio) → 2, `firstTurn` 0, lettera A; lancio a vuoto →
  resta 2; lancio su un Girafarig a 2 caselle → 3, `overworld`, `unnoticed`,
  `thrownFromLand`, `distance` 2. Salvato, checksum buoni, ricaricato.
  Passo 16 da solo su una copia sporcata: tutto azzerato e maschera
  ricostruita.
- **Non provato**: cattura del TcloneR (`trainerMon`) e `targetOnWater` a 1.

**Handoff per la fase 2 (Opus 5.5)**

- I contatori di A vanno in `VAR_SQ_THROW_MASTER` (16 bit: 10 + 5 + 3 entrano
  in 4 + 3 + 2 bit; il resto per lo stato o il premio). Lo stato della
  missione: slot proprio del quest book (`QUEST_OWN_SLOT_COUNT` è 0, si
  aggiunge in coda) o la var.
- "Non notato" in lotta (D2, OVERWORLD CATCH spenta) è `firstTurn` più la
  Ball (Poké, Mega, Ultra): **non** c'è un bit "non notato" per le lotte.
- `targetOnWater` legge `owe->currentMetatileBehavior` all'impatto
  (`MetatileBehavior_IsSurfableWaterOrUnderwater`); `distance` è quella di
  `FindThrowTarget`, la casella del Pokémon al lancio, non dove cade. Provare
  in emulatore il lancio da riva a un Pokémon in acqua.
- La critica col Cromamuleto (D3): `CriticalCapture` è `static` in
  `battle_script_commands.c` e legge `B_CRITICAL_CAPTURE` (oggi FALSE): per
  il lancio OW va esposta o riscritta, e `ResolveImpact` deve mostrare una
  scossa sola.
- Il client RAM senza GDB è quello di `docs/claude/emulator.md` (fase 9 di
  overworld-catch); per leggere un `.sav` offline (slot attivo, checksum,
  squadra e box decifrati) gli offset giusti sono in
  `docs/claude/save-history.md`, sezione `SAVE_VERSION 16`.
- Indirizzi della ROM pulita di questa fase: `gSaveBlock1Ptr` 0x030041C8,
  `gSaveblock3` 0x02009244 (cambiano a ogni build: rileggerli con `nm`).

## 6. Scartato, e perché

- **Sciame (proposta 10)**: in HnS non ci sono sciami (§3); andrebbero
  costruiti da zero. Si rivede se arrivano.
- **Giardino di Bill** (`new_quests.md` §1): non scelto per questo piano
  (mappa nuova, lotte di livello 80). Resta in `new_quests.md`.
- **Pietra Splendore per Espeon**: non fa evolvere Eevee in questo repo (D6).
- **ID a 6 cifre**: il riassunto ne mostra 5 (D7); un'altra base numerica
  non aiuta, perché la cifra stampata è sempre il decimale dei 16 bit bassi.
- **Lettere Unown legate ai puzzle**: più lavoro sugli incontri (lotta e OW),
  per un risultato che lo step a numero dà già (D4).
- **Critica per tutti**: cambierebbe il gioco a chi non fa la missione (D3).
