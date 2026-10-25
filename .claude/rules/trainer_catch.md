---
paths:
  - "src/battle_script_commands.c"
  - "src/battle_util.c"
  - "src/battle_setup.c"
  - "src/trainer_see.c"
  - "src/pokemon.c"
  - "data/scripts/trainer_battle.inc"
  - "src/tcloner.c"
  - "include/tcloner.h"
  - "include/constants/tcloner.h"
  - "data/scripts/tcloner.inc"
  - "data/maps/AzaleaTown_hns/**"
  - "data/maps/Mahoganytown_hns/**"
  - "data/maps/RocketHideout_B1F_hns/**"
  - "data/maps/GoldenrodCity_hns/**"
  - "data/maps/CeruleanCity_hns/**"
  - "data/maps/PalletTown_Lab_hns/**"
---

# Cattura dei Pokémon degli allenatori (TcloneR, ex `TRAINER CATCH`)

**Dal 23/10 il permesso è l'oggetto chiave `ITEM_TCLONER` in borsa**, non più
l'opzione `TRAINER CATCH` del challenge menu (tolta; piano e dialoghi in
`docs/plans/tcloner.md`, sessioni A e B fatte). Tutto quello che segue sul motore della cattura vale identico: è
cambiata solo la riga in `CanCatchTrainerMon()`.

## TcloneR: contatore e premi (sessione A)

- **Contatore**: `VAR_TCLONER_COUNT` (0x40E5). `RecordTrainerMonCaught(battler)`
  (`battle_setup.c`) è chiamata da `FinalizeCapture` al momento della cattura,
  perché a lotta finita i dati dei battler non dicono più di chi era il
  Pokémon. In una lotta con **due allenatori** (`BATTLE_TYPE_TWO_OPPONENTS`)
  la squadra del secondo sta in `gEnemyParty[3..5]`: decide l'indice di
  squadra, non il lato. `CB2_EndTrainerBattle` / `CB2_EndRematchBattle` sul
  ramo `B_OUTCOME_CAUGHT` passano l'id a `TClonerRecordCatch` (`tcloner.c`).
- **Un allenatore conta una volta (D1)**: bitfield di 82 byte (83 dal
  `SAVE_VERSION 10`, per gli allenatori 653-660 di Red's Path, che però
  non si possono catturare: `CanCatchTrainerMon` li esclude)
  `pokedex.tclonerTrainerFlags` in SaveBlock2, ricavato dal `filler[0x68]`
  di `struct Pokedex` (104 byte misurati, non 108 come dice il commento di
  `config/save.h`; nessun lettore né scrittore in `src/`). La struct resta
  0x78 byte e gli offset sono inchiodati da `STATIC_ASSERT` in `save.c`.
  **Trappola**: `NewGameInitData` **non** chiama `ClearSav2()`, quindi
  `ResetPokedex()` azzera il bitfield a mano, o una nuova partita sopra un
  salvataggio caricato erediterebbe gli allenatori già contati. Un rematch è
  un altro id allenatore e conta da sé.
- **Premi**: `VAR_TCLONER_REWARDS` (0x40FC) conta i **passi** consegnati, 0-12,
  non gli scaglioni 0-10 del piano: lo scaglione 100 sono tre Pokémon, un
  passo ciascuno, così con i box pieni resta in sospeso solo quello che non
  entra ("I'll keep **this one**"). Tabella `sRewards` in `tcloner.c`.
- **Specials** (`data/specials.inc`): `TClonerGetNextReward` (esito in
  `VAR_RESULT`, costanti `TCLONER_REWARD_*`), `TClonerClaimItemReward` (dopo un
  `giveitem` riuscito), `TClonerGiveRewardMon` (shiny, sempre nel PC con
  `CopyMonToPC`, Pokédex visto/catturato). Lo script comune
  `EventScript_TClonerRewards` (`data/scripts/tcloner.inc`) fa il giro e chiude
  con la riga comune: "not enough" solo se non ha dato niente, "Impressive…"
  a premi finiti, borsa piena con `checkitemspace` prima di `giveitem` (e il
  risultato di `giveitem` ricontrollato: col randomizer l'oggetto può
  cambiare). Lo scienziato della sessione B lo chiama e basta.
- `VAR_TCLONER_STAGE` (0x40C2): ultima tappa dello scienziato, 0-5 (sotto).
  Scartate 0x40FA/0x40FB, che `vars_hns.h` dà per libere: le usa il
  randomizer (`RANDOMIZER_VAR_SEED_L/H`).
- Icona: l'arte dello Scanner (`gItemIcon_Scanner`) con
  `graphics/items/icon_palettes/tcloner.pal`: corpo blu → grigi Rocket, banda
  gialla → rossa, schermo verde (Celebi) invariato. Valori originali nel
  commento di `src/data/graphics/items.h`.
- **Nuzlocke**: in una lotta con allenatore `SetNuzlockeChecks` non gira e il
  flag della zona si brucia solo con i selvatici (`battle_main.c`, esclude
  `BATTLE_TYPE_TRAINER`): un clone non consuma l'incontro della zona né
  sente la clausola delle specie. Era così con `TRAINER CATCH`, lasciato così.
- **Level cap (D3)**: niente, il clone arriva al suo livello.

### Verificato in emulatore (23/10, ROM strumentata: script di debug 3/4/5 impostano il contatore, 7 chiama i premi, 8 lo mostra)

- Challenge menu: FEATURES finisce con FRONTIER BANS → NEXT.
- Senza TcloneR la Master Ball su Joey (7) è bloccata e **non consumata**
  (21 → 21); con il TcloneR cattura, COUNT 1; seconda cattura da Joey,
  **COUNT resta 1**.
- Doppia Joey (A, già contato) + Ross (2, B): KO al Rattata di Joey, cattura
  del Koffing di Ross → **COUNT 2**, quindi è stato contato B.
- Bitfield nel `.sav`: `bc` = bit 2, 3, 4, 5, 7, cioè esattamente gli
  allenatori clonati.
- 9 → cattura → 10: "Obtained… QUICK BALLs", chiusura senza altre righe;
  di nuovo: "not enough". 20 con la tasca oggetti piena: "Your BAG is full!",
  passo fermo; svuotata: Nugget. 100 con i box pieni: Ultra Ball, Max Repel,
  Rare Candy, Big Nugget poi "Your PC BOXES are full!"; box vuoti: sei
  messaggi "<NOME> was sent to the PC!" e "Impressive…".
- **Box aperto**: tutti e sei shiny, OT del giocatore con il suo ID, Lv5 /
  Lv50. Squadra con i cloni catturati (Rattata, Koffing, Electrode,
  Magnemite, Omastar): nomi e PS normali, nessun Bad Egg.
- Migrazione: `.sav` riportato a versione 8 con il bit di `TRAINER CATCH` a 1,
  bitfield a `0xAA`, var a valori ≠ 0 (checksum ricalcolati); caricato nella
  ROM di produzione e salvato: versione 9, tutto a zero.

Non provato: rematch (`CB2_EndRematchBattle`, stesso codice), nuova partita
sopra un salvataggio caricato (l'azzeramento in `ResetPokedex`).

## Lo scienziato (sessione B)

- Sei oggetti evento `OBJ_EVENT_GFX_SCIENTIST_M_HNS`, uno per mappa, ognuno
  col suo flag `FLAG_HIDE_TCLONER_*` (`FLAG_UNUSED_EXTENDED_80..85`: i
  `FLAG_UNUSED_3x` liberi erano solo 4): Azalea (54,10) accanto al pozzo,
  covo B1F (50,4) all'ingresso, Mahogany (17,12) fuori dal negozio, Goldenrod
  (14,23) accanto alla porta della Torre Radio, Cerulean (27,31) fuori dal
  Centro (vaga, raggio 1), laboratorio di Oak (10,14) (raggio 1, D8). Tutti
  gli script stanno in `data/scripts/tcloner.inc`; le mappe chiamano solo
  `TCloner_EventScript_UpdateNpcs` dal loro `ON_TRANSITION` (Goldenrod, B1F,
  Mahogany e il laboratorio non ne avevano uno: aggiunto).
- **Visibilità ricalcolata a ogni ingresso**, quindi nessuna migrazione:
  tappa *s* visibile se `VAR_TCLONER_STAGE < s`, più la condizione della
  mappa. Covo se `VAR_MAHOGANY_TOWN_STATE < 14`, negozio se `>= 14` (14 =
  covo liberato, `RocketHideout_B2F`); Goldenrod da
  `VAR_GOLDENROD_CITY_STATE >= 6` (presa della Torre); Pallet da tappa `>= 4`,
  per sempre. Parlarci porta la tappa a *s* e `removeobject VAR_LAST_TALKED`
  (alza il flag); Pallet non se ne va.
- Flag di storia per i dialoghi: Pozzo fatto = `FLAG_HIDE_AZALEA_TOWN_ROCKETS`
  (alzato in `SlowpokeWell_B1F`); Torre occupata = `VAR_GOLDENROD_CITY_STATE`
  6-9, liberata da 10 (Archer battuto, `RadioTower_5F`).
- Primo incontro = tappa 0: presentazione (piena con `VAR_0x8005` TRUE ad
  Azalea e Mahogany, corta a Goldenrod e Cerulean), consegna del TcloneR
  (saltata se è già in borsa, cioè solo da debug), niente premi.
  Le varianti di "Sequenza rotta" si scelgono dalla tappa al momento del
  dialogo (`goto_if_ne VAR_TCLONER_STAGE, 1` ecc.).
- Il font non ha la lineetta lunga: "stea— ahem" → "stea… ahem".

### Verificato in emulatore (23/10, ROM strumentata: script di debug 7/8 = scenario successivo/stesso, che imposta tappa e stato della storia e teletrasporta davanti allo scienziato; per Cerulean e Pallet una build con lo scienziato fermo, perché vagando non si lascia pilotare)

Sedici scenari, ogni pagina fotografata: Azalea prima e dopo il Pozzo
(consegna del TcloneR, "Quite a commotion", rimando al lago); covo con
premio; **primo incontro a Mahogany**; Goldenrod durante e dopo, normale,
**primo incontro durante e dopo**, con Mahogany saltata durante e dopo (riga
di recupero); Cerulean normale, primo incontro, tappa 1 e tappa 2; Pallet
prima visita (resta) e visite successive. Tutte le righe stanno nel box.
Visibilità: a tappa 5 Azalea e Cerulean vuoti; a tappa 0 con la Torre non
ancora presa Goldenrod vuota; con il covo attivo nessuno fuori dal negozio.
Vista in emulatore anche la vera build con Cerulean che vaga.

**Sul salvataggio vero dell'utente** (2.0.6.PLUS.4, Lago d'Ira, 6 medaglie,
Pozzo fatto, `VAR_MAHOGANY_TOWN_STATE` 1, Goldenrod 5), ROM di produzione,
su una copia:
- migrazione: versione 8 → 9, storia intatta, il bit di `TRAINER CATCH`
  **era a 1** (byte 47 di SaveBlock3 `0x9d` → `0x1d`): l'utente aveva
  l'opzione accesa e la migrazione l'ha spenta come previsto;
- Mahogany: nessuno fuori dal negozio (covo non liberato); Azalea: lo
  scienziato c'è, variante "dopo il Pozzo", TcloneR consegnato, tappa 1;
- **lotta vera** con un allenatore di mappa, parlandoci (Brent, Route 43,
  `trainerbattle_single`, mai battuto): Ivysaur preso con una Ultra Ball,
  poi via GDB `VAR_TCLONER_COUNT` 1, tappa 1, bit 112 alzato nei flag del
  TcloneR, flag "battuto" di Brent (0x570) ancora a 0. Scheda aperta: OT e
  ID del giocatore, incontrato a Lv26 su ROUTE 43, nessun Bad Egg.

Non provato: il percorso naturale della storia oltre questo punto, la tasca
strumenti chiave piena al primo incontro (riga `TCloner_Text_NoRoom`).

## Motore della cattura (da `TRAINER CATCH`, invariato)

Non c'era. `MIRROR THIEF` sembra la stessa cosa e **non lo è**: con Mirror Mode
combatti con una copia della squadra avversaria, e "thief" te la lascia dopo.

- Il blocco è in `Cmd_handleballthrow` (`BALL_TRAINER_BLOCK`); il permesso sta in
  `CanCatchTrainerMon()` (`battle_util.c`), che esclude strutture, link, partner,
  primo tutorial e **Mirror Mode** (a fine lotta ripristina la squadra dal backup
  e butterebbe via la cattura). Anche `item_use.c` lo consulta: la regola Gen4+
  che non consuma la ball contro un allenatore vale solo quando la ball è bloccata.
- **La trappola vera è lo script dopo la lotta.** Dopo un `trainerbattle` lo
  script di mappa riprende come se avessi vinto: per un capopalestra
  (`trainerbattle_no_intro`) quella ripresa è la **medaglia**. `B_OUTCOME_CAUGHT`
  in `CB2_EndTrainerBattle` non alza il flag dell'allenatore e accende
  `sTrainerMonCaught`; poi `BattleSetup_GetScriptAddrAfterBattle` e
  `BattleSetup_GetTrainerPostBattleScript` (le due strade, `gotopostbattlescript`
  e `gotobeatenscript`) restituiscono `EventScript_TrainerMonCaught`, che fa solo
  `releaseall; end`. Il flag si azzera all'inizio di ogni lotta con allenatore.
- Verificato in emulatore: opzione accesa **da PC a partita iniziata**, Master
  Ball su Pidgey di Bird Keeper Rod e di Falkner, Pokédex/esperienza/nickname
  normali, ball consumata (5 → 4), **nessuna medaglia**, Falkner sfida di nuovo;
  battuto onestamente, la medaglia e la MT arrivano come prima.
- **Vista sospesa fino al cambio mappa** (scelta dell'utente): senza, un
  allenatore che ti vede ti risfidava **subito**, perché il suo flag non è
  alzato. Ora `sSightSuspendedTrainer` (`battle_setup.c`) spegne la sua vista in
  `CheckTrainer` (`trainer_see.c`) finché non cambi mappa
  (`ClearTempFieldEventData` → `ClearTrainerSightSuspension`); parlandoci la
  lotta parte lo stesso, quindi decidi tu quando rifarla. Verificato: dopo la
  cattura Rod non ti vede, parlandoci ti sfida.
  La sua squadra si rigenera a ogni lotta e lo stesso Pokémon si può catturare
  più volte: lasciato così per scelta dell'utente.
- **Bug trovato in emulatore, e il più insidioso: il Pokémon catturato diventava
  un Bad Egg.** `GiveCapturedMonToPlayer` imposta l'OT ID con `SetMonData`, che
  per `MON_DATA_OT_ID` scrive **in chiaro**, senza ricifrare. Ma l'OT ID è metà
  della chiave dei substruct (`EncryptBoxMon`: `personality ^ otId`). Un
  selvatico nasce già con l'OT del giocatore, quindi la chiave non cambia e nel
  vanilla non succede mai niente; un Pokémon di allenatore ha l'ID
  dell'allenatore, la chiave cambia e alla lettura successiva il checksum non
  torna: "Egg" nel menu squadra, e l'assert `invalid item: 1004` alla lotta
  dopo era spazzatura letta da dati non decifrabili, non un vero oggetto.
  Correzione: `SetMonOtIdRekeyed` (`pokemon.c`) decifra con la chiave vecchia,
  scrive l'ID e ricifra; conserva anche la shininess, che expansion salva
  relativa all'OT ID (`shinyModifier`). Il checksum è calcolato sui dati in
  chiaro, quindi non cambia.
  **Lezione di verifica**: nel primo giro di test avevo controllato Pokédex,
  ball consumata e medaglia, ma **non avevo aperto la squadra** dopo la
  cattura. Il Pokémon catturato è il prodotto della feature: va guardato.
