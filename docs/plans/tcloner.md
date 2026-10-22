# Piano: TcloneR (cattura dei Pokémon degli allenatori come missione)

## Contesto

Idea dell'utente (22/10/2026, documento allegato in chat; il testo dei
dialoghi è in fondo a questo file, così com'è). In sintesi:

- un ex assistente di Elm, passato al Team Rocket, ha costruito il
  **TcloneR**: clona un Pokémon dell'avversario per lotta, l'avversario non
  se ne accorge e ti sfida di nuovo. Le particelle tachioniche vengono da un
  ciuffo di peli di Celebi;
- oggetto chiave passivo, come la Dowsing Machine, che resta nella borsa e
  **sostituisce l'opzione `TRAINER CATCH`** (tolta dal menu);
- lo scienziato compare in 5 posti (Azalea, Mahogany, Goldenrod, Cerulean,
  laboratorio di Oak) e a ogni incontro dà tutte le ricompense maturate;
  dopo il dialogo sparisce, tranne a Pallet, dove resta;
- ricompense ogni 10 cloni, fino a 100.

## Su cosa si appoggia (c'è già, verificato)

Il motore della cattura esiste ed è stato provato in emulatore
(`.claude/rules/trainer_catch.md`):

- permesso unico in `CanCatchTrainerMon()` (`battle_util.c` ~10835), oggi
  legato a `challengeSettings.trainerCatch`;
- la lotta finisce alla cattura (`B_OUTCOME_CAUGHT`), quindi **un Pokémon per
  lotta è già strutturale**;
- l'allenatore non viene segnato come battuto, lo script dopo la lotta è
  sostituito da `EventScript_TrainerMonCaught` (niente medaglia), e la sua
  vista resta spenta fino al cambio mappa: "ti sfiderà di nuovo se ti vede"
  è già il comportamento di oggi;
- `SetMonOtIdRekeyed` evita il Bad Egg.

Il lavoro nuovo è quindi: oggetto, contatore, ricompense, NPC e dialoghi,
migrazione.

## Valutazione

Funziona bene come struttura: dà un motivo narrativo a una meccanica che oggi
è un interruttore, e i cinque incontri seguono la storia dei Rocket.
Problemi veri, in ordine di peso:

1. **Il contatore si "farma".** Oggi la squadra si rigenera e lo stesso
   Pokémon si cattura all'infinito (scelta dell'utente, va bene senza premi).
   Con i premi, 100 cloni sono 100 lotte con lo stesso Youngster all'uscita
   di una casa: tre shiny Lv50 in mezz'ora. **Proposta: conta un allenatore
   una volta sola** (catturare di nuovo resta possibile, non conta). Servono
   652 bit (gli id HnS vanno da 1 a 652, `TRAINERS_COUNT_HNS` 653), cioè 82
   byte. In SaveBlock3 restano 56 byte (60 usati su 116). Candidato: il
   `filler[0x68]` di `struct Pokedex` (104 byte, "Previously Dex Flags, feel
   free to remove", `FREE_EXTRA_SEEN_FLAGS_SAVEBLOCK2 FALSE`): **da
   verificare** che nessuno lo legga o scriva e che una migrazione lo azzeri
   (nota: il commento in `include/config/save.h` dice 108 byte, la struct
   0x68 = 104: controllare quale è vero).
2. **Vecchi e nuovi salvataggi non servono: servono i flag della storia.** I
   dialoghi "vecchi" e "nuovi" del documento sono in realtà "dopo l'evento" e
   "durante l'evento": a Goldenrod "Sapevo non sarebbe durata" è dopo la
   liberazione della Torre Radio, "Stai dando parecchie noie ai piani alti" è
   durante; ad Azalea "C'è stato un gran baccano" è dopo il Pozzo Slowpoke.
   Scegliendo posto e dialogo dai flag della storia
   (`FLAG_HIDE_AZALEA_TOWN_WELL_ROCKET`, `FLAG_HIDE_MAHOGANY_ROCKETS`,
   `FLAG_HIDE_GOLDENROD_ROCKETS`… da confermare uno per uno) si coprono
   insieme un salvataggio nuovo, uno vecchio e uno nuovo che ha saltato un
   incontro, senza un flag "salvataggio vecchio".
3. **Chi riceve l'oggetto.** Nel documento solo Azalea lo dà. Un salvataggio
   già a Mahogany (il tuo) dovrebbe tornare ad Azalea prima di parlare con
   lo scienziato di Mahogany, che non saprebbe cosa dire. Proposta: **il
   primo incontro, qualunque sia, dà il TcloneR** se non c'è nella borsa.
4. **Level cap.** Un clone di un capopalestra arriva al suo livello, anche
   sopra il tetto. Con il tetto acceso non guadagna esperienza, ma si usa
   subito, e il tetto serve proprio a evitarlo. Il problema c'è già con
   `TRAINER CATCH`; diventa più visibile ora che è una missione. Opzioni:
   lasciare, o portare il clone al tetto (livello, esperienza e statistiche
   ricalcolate). Vedi D3.
5. **Togliere `TRAINER CATCH` toglie la scelta.** Chi non vuole la meccanica
   non lancia ball agli allenatori: va bene. Il bit `trainerCatch` diventa
   libero **solo** dopo una migrazione che lo azzera (`.claude/rules/save.md`,
   stessa storia di `unusedDexNavBit`).
6. **Mancano due dialoghi**: "non hai ancora raggiunto il prossimo premio"
   (lo scienziato se ne va lo stesso?) e cosa succede con la **borsa piena** o
   i **box pieni** (proposta: il premio resta da ritirare e lo dice).
7. **Lingua**: il gioco è in inglese (testi di mappa, menu). I dialoghi vanno
   tradotti; la bozza italiana resta la fonte. "Clonali tutti!" → "Gotta
   clone 'em all!".

## Ricompense

Nomi inglesi da `include/constants/items.h` (da confermare in fase 1):

| cloni | premio |
| --- | --- |
| 10 | 20 Quick Ball (Velox Ball) |
| 20 | 10 Nugget |
| 30 | 20 Ultra Ball |
| 40 | 15 Max Repel |
| 50 | 10 Rare Candy |
| 60 | 10 Big Nugget |
| 70 | Totodile shiny Lv5 |
| 80 | Chikorita shiny Lv5 |
| 90 | Cyndaquil shiny Lv5 |
| 100 | Weezing, Arbok e Meowth shiny Lv50 |

- Un messaggio per premio, in fila ("Obtained 20 QUICK BALLS!", "TOTODILE was
  sent to the PC!"). Con `giveitem` i messaggi arrivano da soli.
- I Pokémon vanno **sempre nei box**, anche con la squadra non piena.
  `givemon` (`ScrCmd_createmon`) mette nel primo posto libero della squadra:
  serve una variante che scriva nel PC, o uno `special`. Lo shiny c'è già
  (`shinyMode` di `givemon`).
- Tutti ritirati: "Impressive. I have nothing left to give you…", in
  qualunque posto.

## Stato nel salvataggio

- `VAR_TCLONER_COUNT` (cloni contati) e `VAR_TCLONER_REWARDS` (scaglioni già
  ritirati, 0-10): due var libere di HnS (`include/constants/vars_hns.h`, 43
  `UNUSED`).
- Un flag per incontro fatto (5) e uno per "è nel laboratorio di Oak": flag
  liberi di HnS (`include/constants/flags_hns.h`, `FLAG_UNUSED_*` da 0x2E0).
  **Non** usare flag con byte basso `0xFF` vicino ai massi
  (`.claude/rules/overworld_encounters.md`, `trainer_type`).
- Bitfield degli allenatori clonati (punto 1).
- `SAVE_VERSION` 9: azzera `trainerCatch`, il bitfield e le var; `STATIC_ASSERT`
  sugli offset; il passo va scritto **dopo** quello del level scaling se
  quello arriva prima (oggi non ne ha bisogno).

## Decisioni per l'utente (prima della sessione)

| # | domanda | consiglio |
| --- | --- | --- |
| D1 | Un allenatore conta una volta sola? | sì |
| D2 | Posto e dialogo dai flag della storia invece che da "salvataggio vecchio/nuovo"? | sì |
| D3 | Clone sopra il level cap: lasciarlo o portarlo al tetto? | portarlo al tetto, solo con il tetto acceso |
| D4 | Il primo incontro, qualunque sia, dà il TcloneR? | sì |
| D5 | Dialoghi in inglese, tradotti dalla bozza? Li rivedi tu prima della build finale | sì |
| D6 | Quando appare lo scienziato ad Azalea in un salvataggio nuovo: già all'arrivo, o dopo il Pozzo? Il dialogo "nuovo" non nomina il baccano, quindi all'arrivo | all'arrivo |
| D7 | Icona dell'oggetto: ricolorare un'icona esistente (es. Dowsing Machine) o arte tua? | ricolorare, con palette derivata dai file (CLAUDE.md §2) |
| D8 | Nel laboratorio di Oak "cammina": in uno spazio piccolo un NPC che vaga può bloccare il passaggio. Raggio limitato o fermo? | raggio 1 |

## Fasi

### Sessione A: motore (C)

1. `ITEM_TCLONER`: tasca oggetti chiave, nessun uso (descrizione e "non si
   può usare ora"), icona (D7), nome `TCLONER` nella lista.
2. `CanCatchTrainerMon()`: `CheckBagHasItem(ITEM_TCLONER, 1)` al posto del
   bit; tolta la voce dal challenge menu (`ITEM_FEATURES_TRAINER_CATCH`,
   descrizioni, copia in/da `ChallengeSettings`); bit rinominato e azzerato.
3. Contatore in `CB2_EndTrainerBattle` e `CB2_EndRematchBattle` sul ramo
   `B_OUTCOME_CAUGHT`, con il bitfield (D1). In doppia con due allenatori:
   contare quello a cui apparteneva il Pokémon (lato del battler catturato →
   `opponentA`/`opponentB`), non sempre `opponentA`.
4. `special` per le ricompense: dà gli scaglioni maturati uno alla volta,
   ferma e lascia in sospeso con borsa o box pieni; Pokémon diretti al PC.
5. Level cap (D3), Nuzlocke: controllare cosa fa oggi `TRAINER CATCH` con le
   regole Nuzlocke (`nuzlocke.h`) e mantenerlo.
6. Migrazione `SAVE_VERSION` 9, `CHANGELOG.md`, aggiornare
   `.claude/rules/trainer_catch.md`.
7. Verifica: debug per dare l'oggetto, var del contatore portata a 9/19/99
   via GDB, una cattura → scaglione maturato; stesso allenatore due volte →
   conta una volta; senza oggetto la ball è bloccata come prima; **aprire la
   squadra e il box** dopo cattura e premio (shiny, OT, livello).

### Sessione B: mondo (mappe, script, testi)

1. Cinque oggetti evento (`OBJ_EVENT_GFX_SCIENTIST_M_HNS` o simile) nelle
   mappe `AzaleaTown_hns`, `RocketHideout_B1F_hns` / `MahoganyTown_Shop_hns` /
   `Mahoganytown_hns`, `GoldenrodCity_hns`, `CeruleanCity_hns`,
   `PalletTown_Lab_hns`, ciascuno con il suo flag e la scelta del posto dai
   flag della storia (D2).
2. Script comune: dà l'oggetto se manca (D4), chiama lo `special`, dice i
   messaggi, nasconde l'NPC (tranne Pallet), sblocca il laboratorio dopo
   Cerulean.
3. Testi in inglese (D5), a riga corta: controllare in emulatore che stiano
   nelle due righe del box.
4. Verifica: percorso completo con warp di debug in tutti e cinque i posti,
   sia "durante" sia "dopo" ogni evento (flag forzati via GDB); salvataggio
   dell'utente a Mahogany (caso vecchio reale); screenshot di ogni dialogo.

## Sessione consigliata

**Due sessioni in fila**, la B dopo il merge della A (la B usa lo `special` e
le var della A). Il level scaling è indipendente e può andare prima o dopo.

- Modello: il più capace disponibile (Opus). A: sforzo alto (salvataggio,
  lotta, migrazione). B: sforzo medio, ma tanta verifica in emulatore.
- Serve: emulatore (`docs/claude/emulator.md`), il `.srm` dell'utente per il
  caso "salvataggio vecchio", le risposte a D1-D8. Soulgold non serve: non è
  un port.

Prompt da incollare (A):

> Implementa la sessione A di `docs/plans/tcloner.md`. Decisioni: D1=…, D3=…,
> D4=…, D7=…. Consegna la ROM zippata e apri la PR.

Prompt da incollare (B):

> Implementa la sessione B di `docs/plans/tcloner.md` (la A è in `master`).
> Decisioni: D2=…, D5=…, D6=…, D8=…. Ti allego il mio salvataggio.

## Bozza originale dell'utente (fonte dei dialoghi)

Riportata com'era, refusi compresi; la traduzione si fa in sessione B.

```text
Azalea Town — vicino al pozzo, fermo.
  Vecchi salvataggi: Psst.. Ehi tu! Ti manda Elm? Io sono un ex aiutante di
  Elm. C'è stato un gran baccano qua eh? Tieni. (TcloneR) È il TcloneR,
  usalo. Puoi rubarEHM CLONARE un Pokémon dell'avversario per lotta. L'altro
  non se ne renderà neanche conto e ti sfiderà di nuovo se ti vede. Il
  Pokémon sarà come se l'avessi catturato tu. È complicato da spiegare, te lo
  dirò se ci rivedremo ma nessuno si fa male... Forse.. Ti aspetta una bella
  ricompensa se ne rubi ehm.. cloni tanti. Ora devo andare a cercare gli
  altri, ho sentito che vicino a Mahogany Town c'è un bel lago.
  Nuovi salvataggi: come sopra senza "C'è stato un gran baccano qua eh?";
  chiude con "Ora devo andare."

Mahogany Town — appena entri nell'hideout sotto il negozio, fermo
(vecchi salvataggi: fuori dal negozio, stesso dialogo).
  Guarda chi c'è. Come sta andando? Quanti ne hai presi? (premi) Ce ne sono
  ancora tanti di premi per te. Come? Chi sono? Ero un aiutante di Elm. Ora
  sto col Team Rocket. Qui ho meno vincoli e più soldi. Non mi piace molto,
  ma posso fare quello che voglio. Ora devo andare ma penso ci rivedremo
  presto all'ombra di una torre. Clonali tutti!

Goldenrod — appena fuori dalla Torre Radio, fermo.
  Vecchi: Sapevo non sarebbe durata. Ora devo trovare un nuovo lavoro, mi sa
  che andrò a Kanto. E tu? Quanti ne hai clonati? (premi) Gli altri
  allenatori non se ne rendono conto perché usiamo delle particelle
  tachioniche che fanno... Mamma mia, che espressione vuota... Vabbè, diciamo
  che c'entra Celebi. Ok? Ora devo fuggire anche io. A presto!
  Nuovi: Sapevo che saresti arrivato tu. Stai dando parecchie noie ai piani
  alti, non credo molleranno facilmente. Comunque, quanti ne hai clonati?
  (premi) [stesso finale, "Ora devo andare. A presto!"]

Cerulean City — fuori dal Centro Pokémon, cammina. Resta finché non avviene
il dialogo, poi sparisce e va nel laboratorio di Oak.
  Vecchi: Ce ne hai messo di tempo a trovarmi. / Nuovi: Ce ne hai messo di
  tempo ad arrivare qua. Poi: Io? Sto ancora cercando lavoro, forse tornerò
  da Oak. Tu? (premi) Celebi? Diciamo che abbiamo avuto uno spiacevole
  scontro con Celebi, ha perso alcuni peli e da lì abbiamo estratto le
  particelle. Praticamente è come un viaggio nel tempo, per capirci, ma
  limitatissimo. Chissà cosa avremmo potuto fare catturandolo... Ora devo
  andare, sembra che ci sia ancora qualcuno che c'è l'ha col Team Rocket.
  Clonali tutti!

Pallet Town — laboratorio di Oak, cammina. Resta per sempre.
  Sono venuto a trovare un vecchio amico e mi ha chiesto di restare, che
  fortuna eh? Meglio così, la vita sul filo del rasoio non fa per me. Il
  nostro accordo vale ancora però. Tu? (premi) Perché ti ho chiesto di fare
  questo? Ero convinto che se le loro ricerche fossero progredite, anche il
  Team Rocket avrebbe avuto più possibilità di guadagno, ma ormai è inutile.
  Penso rimarrò qua per un po'. Passa a salutare quando sei qua, magari ho
  qualcosa per te. Catturali tutti!

Premi finiti, ovunque: "Impressionante. Non ho più niente da darti, sei
stato veramente in gamba. Bravo!"
```
