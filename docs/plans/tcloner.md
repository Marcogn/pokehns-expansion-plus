# Piano: TcloneR (cattura dei Pokémon degli allenatori come missione)

**Stato (23/10): sessioni A e B fatte, nella stessa PR (#22).** Quello che è
venuto diverso dal piano è annotato sotto "Sessione A" e "Sessione B"; il
riferimento per il codice è `.claude/rules/trainer_catch.md`.

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

Scritta prima delle risposte dell'utente; le decisioni prese stanno in
"Decisioni dell'utente" e valgono su quello che dice questa sezione.

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

- `VAR_TCLONER_COUNT` (cloni contati), `VAR_TCLONER_REWARDS` (scaglioni già
  ritirati, 0-10; **fatto: passi 0-12**, vedi "Sessione A") e `VAR_TCLONER_STAGE` (ultimo incontro fatto, 0-5, vedi
  "Ordine degli incontri"): tre var libere di HnS
  (`include/constants/vars_hns.h`, 43 `UNUSED`). La tappa in una var invece
  che in cinque flag rende "gli incontri vanno solo avanti" un confronto.
- Un flag per oggetto evento (nascondere/mostrare): flag liberi di HnS
  (`include/constants/flags_hns.h`, `FLAG_UNUSED_*` da 0x2E0). **Fatto con
  `FLAG_UNUSED_EXTENDED_80..85`**: i `FLAG_UNUSED_3x` liberi erano solo 4 e
  gli oggetti sono sei (il covo e il negozio di Mahogany sono due). **Non** usare
  flag con byte basso `0xFF` vicino ai massi
  (`.claude/rules/overworld_encounters.md`, `trainer_type`).
- Bitfield degli allenatori clonati (D1), **solo se costa poco** (sotto).
- `SAVE_VERSION` 9: azzera `trainerCatch`, il bitfield e le var; `STATIC_ASSERT`
  sugli offset; il passo va scritto **dopo** quello del level scaling se
  quello arriva prima (oggi non ne ha bisogno).

**D1, regola di costo** (decisione dell'utente: sì se non costa troppo, il
farming non è un problema perché i premi sono neutri). Si fa **solo** se il
`filler` di `struct Pokedex` risulta davvero libero: nessun lettore o
scrittore (`grep` su `src/`), dimensione confermata (104 o 108 byte), e una
migrazione che lo azzera. Allora costa 82 byte già esistenti e una funzione.
Se serve spostare il layout di un SaveBlock o trovare spazio altrove, **non
si fa**: il contatore conta ogni cattura e lo si scrive nella PR.

## Decisioni dell'utente (22/10)

| # | domanda | risposta |
| --- | --- | --- |
| D1 | Un allenatore conta una volta sola? | sì, **se non costa troppo** (regola sopra) |
| D2 | Posto e dialogo dai flag della storia? | sì |
| D3 | Clone sopra il level cap? | **lasciarlo così** (arriva al suo livello) |
| D4 | Il primo incontro, qualunque sia, dà il TcloneR? | sì, **mantenendo l'ordine degli incontri** (sotto) |
| D5 | Dialoghi in inglese | sì: bozza tradotta e rivista qui sotto, da confermare |
| D6 | Scienziato ad Azalea in un salvataggio nuovo | all'arrivo |
| D7 | Icona | ricolorare un'icona esistente, palette derivata dai file (CLAUDE.md §2) |
| D8 | Movimento nel laboratorio di Oak | raggio 1 |

## Ordine degli incontri (D4)

L'utente vuole la progressione Azalea → Mahogany → Goldenrod → Cerulean →
Pallet, e le rivelazioni seguono quell'ordine (chi è, il Team Rocket, le
particelle, Celebi, il perché). La regola che la tiene anche quando il primo
incontro non è Azalea:

1. **Gli incontri vanno solo avanti.** Lo scienziato della tappa *s* è visibile
   se `VAR_TCLONER_STAGE < s` (e le condizioni della sua mappa: Pallet solo con
   la tappa 4 fatta). Parlarci porta la tappa a *s*: **tutte le tappe
   precedenti spariscono**, perché "è già andato avanti". Non si torna mai
   indietro, quindi nessun dialogo può arrivare fuori ordine.
2. **Il primo incontro, dovunque sia, apre con il blocco "presentazione"**
   (chi è, dà il TcloneR, come funziona) **al posto** del saluto della tappa,
   e salta "quanti ne hai presi?" e i premi: il contatore parte con l'oggetto,
   quindi vale per forza 0.
3. **Le rivelazioni si leggono dalla tappa.** Tre cose che un blocco può
   dare per già dette: "sto con il Team Rocket" (detta a ogni incontro dalla
   tappa 2 in poi, quindi già detta se `VAR_TCLONER_STAGE >= 2`), "c'entra
   Celebi" (detta a Goldenrod: `>= 3`), e la promessa "te lo spiego se ci
   rivediamo" della presentazione (ha senso solo se la spiegazione viene
   dopo, cioè se il primo incontro è ad Azalea o a Mahogany). Quando la
   condizione non è vera si usa la variante scritta in "Sequenza rotta",
   sotto: nessuna frase si ripete e nessuna risponde a qualcosa mai detto.
4. Pallet resta per sempre; dalla seconda visita in poi usa il dialogo
   breve.

Caso reale, il salvataggio dell'utente (a Mahogany, prima del covo): lo
scienziato di Azalea c'è ancora. Se l'utente torna ad Azalea, prende
l'oggetto lì e segue l'ordine completo; se entra prima nel covo, lo prende a
Mahogany (presentazione + resto della tappa 2) e quello di Azalea sparisce.

**Premi in sospeso**: con borsa o box pieni il premio resta da ritirare, lo
scienziato lo dice e se ne va lo stesso; lo dà all'incontro successivo
(Pallet c'è sempre).

## Testi in inglese (approvati dall'utente il 22/10, D5)

Tradotti dalla bozza e rivisti: refusi corretti ("c'è l'ha"), qualche frase
accorciata per il box di testo. Maiuscole come negli script di HnS (`ELM`,
`TEAM ROCKET`, `POKéMON`); il nome dell'oggetto resta **TcloneR**, perché
la "TR" è la battuta. L'impaginazione (`\n`, `\l`, `\p`) si fa in sessione B,
con gli screenshot.

**Presentazione** (primo incontro, qualunque tappa):

> Psst… Hey, you! Did PROF. ELM send you? I used to be ELM's assistant.
> Here, take this.
> *[obtained the TcloneR]*
> It's the TcloneR. Use it. You can stea— ahem, CLONE one of your opponent's
> POKéMON per battle. They won't even notice, and they'll challenge you again
> if they see you. The POKéMON will be just like one you caught yourself.
> It's complicated… I'll explain if we meet again. Nobody gets hurt…
> Probably.
> There's a nice reward waiting if you steal… uh, clone a lot of them.

Ad Azalea, dopo il Pozzo Slowpoke, si aggiunge "Quite a commotion around
here, huh?" prima di "Here, take this.". Chiusura ad Azalea: "Now I have to go
find the others. I heard there's a nice lake near MAHOGANY TOWN." (dopo il
Pozzo) oppure "I have to go now." (prima).

**Mahogany** (dentro il covo, o fuori dal negozio dopo):

> Look who it is. How's it going? How many have you got?
> *[premi]*
> There are still plenty of rewards for you. What? Who am I?
> I used to be ELM's assistant. Now I'm with TEAM ROCKET. Fewer rules, more
> money. I don't like it much, but I can do whatever I want.
> I have to go now, but I think we'll meet again soon… in the shadow of a
> tower. Clone 'em all!

**Goldenrod**, durante l'occupazione della Torre Radio:

> I knew you'd be the one to show up. You're giving the higher-ups a lot of
> trouble. I don't think they'll give up easily. Anyway, how many have you
> cloned?

dopo la liberazione:

> I knew it wouldn't last. Now I need a new job… I think I'll head to KANTO.
> What about you? How many have you cloned?

poi, in entrambi i casi:

> *[premi]*
> The other TRAINERS don't notice because we use tachyon particles that…
> Wow, what a blank look… Fine, let's just say CELEBI is involved. OK?
> I've got to run too. See you!

Righe di recupero se Mahogany è stata saltata: in "Sequenza rotta", sotto.

**Cerulean** (una sola versione: la differenza "a trovarmi / ad arrivare
qua" della bozza non dipende da nulla di verificabile):

> Took you long enough to get here. Me? Still looking for a job. Maybe I'll
> go back to PROF. OAK. You?
> *[premi]*
> CELEBI? Let's say we had an unpleasant run-in with CELEBI. It lost some
> hair, and we extracted the particles from it. It's like time travel, so to
> speak, but very, very limited. Imagine what we could have done if we'd
> caught it…
> I have to go. Looks like someone still has it in for TEAM ROCKET.
> Clone 'em all!

**Pallet**, prima visita:

> I came to visit an old friend, and he asked me to stay. Lucky, huh? Better
> this way. Life on the edge isn't for me. Our deal still stands, though.
> You?
> *[premi]*
> Why did I ask you to do this? I thought that if their research moved
> forward, TEAM ROCKET would have more ways to make money too. But that's
> pointless now. I think I'll stay here for a while. Drop by when you're
> around. Maybe I'll have something for you.
> Gotta catch 'em all!

**Pallet**, visite successive:

> Our deal still stands. How many this time?
> *[premi]*
> Drop by again!

### Sequenza rotta: le varianti (scritte per non dimenticarle)

Richiesta dell'utente (22/10): correggere già nelle bozze la frase ripetuta
"I used to be ELM's assistant" e simili. Sono tutte le combinazioni
possibili: Pallet non può mai essere il primo incontro (compare solo dopo
Cerulean).

**Presentazione corta**: la presentazione senza "It's complicated… I'll
explain if we meet again." (resta "Nobody gets hurt… Probably."). Si usa
quando il primo incontro è a Goldenrod o a Cerulean, perché lì la
spiegazione arriva nello stesso dialogo.

**Primo incontro a Mahogany**: presentazione completa, poi (al posto di
"There are still plenty of rewards… What? Who am I? I used to be ELM's
assistant."):

> Oh, one more thing. I'm with TEAM ROCKET now. Fewer rules, more money. I
> don't like it much, but I can do whatever I want.
> I have to go now, but I think we'll meet again soon… in the shadow of a
> tower. Clone 'em all!

**Primo incontro a Goldenrod**, durante l'occupazione: presentazione corta,
poi

> Oh, and I'm with TEAM ROCKET. You're giving the higher-ups a lot of
> trouble, you know. I don't think they'll give up easily.

dopo la liberazione: presentazione corta, poi

> I was with TEAM ROCKET. Was. I knew it wouldn't last. Now I need a new
> job… I think I'll head to KANTO.

in entrambi i casi segue il blocco delle particelle ("The other TRAINERS
don't notice because…" fino a "See you!"), che risponde a "They won't even
notice" della presentazione.

**Goldenrod non primo, Mahogany saltata** (tappa 1 fatta): saluto normale,
premi, poi la riga di recupero prima di "The other TRAINERS…": durante
l'occupazione "Oh, right: I'm with TEAM ROCKET, by the way."; dopo la
liberazione "Oh, right. I was with TEAM ROCKET. Was."

**Primo incontro a Cerulean**: presentazione corta, poi

> I was with TEAM ROCKET, until it fell apart. Now I'm looking for a job.
> Maybe I'll go back to PROF. OAK.
> How does it work? Let's say we had an unpleasant run-in with CELEBI…

e prosegue come il blocco normale da "It lost some hair…" alla fine.

**Cerulean non primo**:
- Goldenrod saltata (tappa 1 o 2): "CELEBI? Let's say…" diventa "How does
  it work? Let's say we had an unpleasant run-in with CELEBI…", perché
  Celebi non è mai stato nominato;
- anche Mahogany saltata (tappa 1): prima di "How does it work?" la riga
  "I was with TEAM ROCKET, until it fell apart."

**Pallet**, prima visita: non cambia. Il perché ("TEAM ROCKET would have
more ways to make money") presuppone solo il Team Rocket, che a quel punto
è sempre stato nominato (Cerulean lo nomina in ogni variante).

**Righe comuni:**

- nessuno scaglione nuovo: "Hmm, not enough for the next reward yet. Keep at
  it!"
- premi finiti: "Impressive. I have nothing left to give you. You've been
  really great. Well done!"
- borsa piena: "Your BAG is full! I'll keep the rest for next time."
- box pieni: "Your PC BOXES are full! I'll keep this one for next time."
- premi: i messaggi standard di `giveitem` ("Obtained 20 QUICK BALLS!") e,
  per i Pokémon, "<NOME> was sent to the PC!".

## Fasi

### Sessione A: motore (C) — fatta il 23/10

Com'è venuta (dettaglio e verifiche in `.claude/rules/trainer_catch.md`):

- D1 fatto: il `filler` è libero e da 104 byte, bitfield da 82 byte.
- `VAR_TCLONER_COUNT` 0x40E5, `VAR_TCLONER_REWARDS` 0x40FC,
  `VAR_TCLONER_STAGE` 0x40C2 (0x40FA/0x40FB non sono libere: randomizer).
- **Deviazione**: `VAR_TCLONER_REWARDS` conta i *passi* (0-12), non gli
  scaglioni (0-10): i tre Pokémon dei 100 sono un passo ciascuno, così con i
  box pieni resta in sospeso solo quello che non entra.
- Per la B: lo script comune è `EventScript_TClonerRewards`
  (`data/scripts/tcloner.inc`), da chiamare dopo "How many have you got?":
  dà tutto il maturato e chiude con la riga comune giusta. Le righe comuni
  sono già lì (impaginazione da rivedere negli screenshot della B).
- Fra il merge della A e quello della B il TcloneR **non si ottiene** in
  gioco (solo da debug, id 903): niente release in mezzo.

1. `ITEM_TCLONER`: tasca oggetti chiave, nessun uso (descrizione e "non si
   può usare ora"), icona ricolorata (D7), nome `TcloneR` nella lista.
2. `CanCatchTrainerMon()`: `CheckBagHasItem(ITEM_TCLONER, 1)` al posto del
   bit; tolta la voce dal challenge menu (`ITEM_FEATURES_TRAINER_CATCH`,
   descrizioni, copia in/da `ChallengeSettings`); bit rinominato e azzerato.
3. Contatore in `CB2_EndTrainerBattle` e `CB2_EndRematchBattle` sul ramo
   `B_OUTCOME_CAUGHT`, con il bitfield se passa la regola di costo (D1). In
   doppia con due allenatori: contare quello a cui apparteneva il Pokémon
   (lato del battler catturato → `opponentA`/`opponentB`), non sempre
   `opponentA`.
4. `special` per le ricompense: dà gli scaglioni maturati uno alla volta,
   ferma e lascia in sospeso con borsa o box pieni; Pokémon diretti al PC.
   Restituisce in una var speciale l'esito (dato qualcosa / niente di nuovo /
   finiti / pieno) per scegliere la riga comune.
5. Level cap: niente (D3). Nuzlocke: controllare cosa fa oggi `TRAINER CATCH`
   con le regole Nuzlocke (`nuzlocke.h`) e mantenerlo.
6. Migrazione `SAVE_VERSION` 9, `CHANGELOG.md`, aggiornare
   `.claude/rules/trainer_catch.md`.
7. Verifica: debug per dare l'oggetto, var del contatore portata a 9/19/99
   via GDB, una cattura → scaglione maturato; stesso allenatore due volte →
   conta una volta (se D1 fatto); senza oggetto la ball è bloccata come
   prima; borsa piena → premio in sospeso; **aprire la squadra e il box**
   dopo cattura e premio (shiny, OT, livello).

### Sessione B: mondo (mappe, script, testi) — fatta il 23/10, prima del merge della A

Su richiesta dell'utente, nella stessa PR della A. Testi quelli approvati
(D5), impaginati negli screenshot; "stea— ahem" è diventato "stea… ahem"
(il font non ha la lineetta). Dettaglio e verifiche in
`.claude/rules/trainer_catch.md`. Il salvataggio dell'utente non era in
sessione: il caso "salvataggio vecchio a Mahogany" è coperto dagli scenari
(primo incontro a Mahogany), non dal `.srm` vero.


1. Cinque oggetti evento (`OBJ_EVENT_GFX_SCIENTIST_M_HNS` o simile) nelle
   mappe `AzaleaTown_hns` (vicino al pozzo, fermo, visibile dall'arrivo:
   D6), `RocketHideout_B1F_hns` all'ingresso / `Mahoganytown_hns` fuori dal
   negozio dopo il covo, `GoldenrodCity_hns` fuori dalla Torre Radio,
   `CeruleanCity_hns` fuori dal Centro (vaga), `PalletTown_Lab_hns` (raggio
   1: D8). Posto e dialogo dai flag della storia (D2), visibilità dalla tappa.
2. Script comune: presentazione se è il primo incontro, altrimenti saluto
   della tappa; `special` dei premi e riga comune; riga di recupero se serve;
   tappa avanzata e NPC nascosti (tutti quelli delle tappe precedenti);
   Pallet sbloccato dopo Cerulean.
3. Testi dalla sezione qui sopra, impaginati e controllati negli screenshot.
4. Verifica: percorso completo con warp di debug in tutti e cinque i posti,
   sia "durante" sia "dopo" ogni evento (flag forzati via GDB); primo
   incontro in ognuna delle varianti di "Sequenza rotta" (Mahogany,
   Goldenrod durante e dopo, Cerulean, Cerulean con tappe saltate); un
   incontro saltato fa sparire quelli precedenti; salvataggio dell'utente a
   Mahogany (caso vecchio reale); screenshot di ogni dialogo.
   **Consegnare all'utente gli screenshot dei casi con la sequenza rotta**
   (primo incontro a Mahogany, primo incontro a Goldenrod): li ha approvati
   sulla carta ma ha chiesto di vedere come vengono davvero.

## Sessione consigliata

**Due sessioni in fila**, la B dopo il merge della A (la B usa lo `special` e
le var della A). *Andata diversamente: l'utente ha chiesto la B prima del
merge, sullo stesso branch.* Il level scaling è indipendente e può andare prima o dopo.

- Modello: il più capace disponibile (Opus). A: sforzo alto (salvataggio,
  lotta, migrazione). B: sforzo medio, ma tanta verifica in emulatore.
- Serve: emulatore (`docs/claude/emulator.md`), il `.srm` dell'utente per il
  caso "salvataggio vecchio". Soulgold non serve: non è un port.
- Le decisioni sono già in questo file: prima di iniziare la B servono solo le
  correzioni dell'utente ai testi.

Prompt da incollare (A):

> Implementa la sessione A di `docs/plans/tcloner.md`, con le decisioni già
> scritte nel piano. Consegna la ROM zippata e apri la PR.

Prompt da incollare (B):

> Implementa la sessione B di `docs/plans/tcloner.md` (la A è in `master`).
> Testi: quelli del piano [con queste correzioni: …]. Ti allego il mio
> salvataggio.

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
