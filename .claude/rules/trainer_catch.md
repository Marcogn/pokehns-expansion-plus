---
paths:
  - "src/battle_script_commands.c"
  - "src/battle_util.c"
  - "src/battle_setup.c"
  - "src/trainer_see.c"
  - "src/pokemon.c"
  - "data/scripts/trainer_battle.inc"
---

# Cattura dei Pokémon degli allenatori (`TRAINER CATCH`)

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
