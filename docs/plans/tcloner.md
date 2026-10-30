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


## Appendice: regola dell'area com'era prima dello sfoltimento (small-features-1 fase 3, 26/10)

## Cattura dei Pokémon degli allenatori (TcloneR, ex `TRAINER CATCH`)

**Dal 23/10 il permesso è l'oggetto chiave `ITEM_TCLONER` in borsa**, non più
l'opzione `TRAINER CATCH` del challenge menu (tolta; piano e dialoghi in
`docs/plans/tcloner.md`, sessioni A e B fatte). Tutto quello che segue sul motore della cattura vale identico: è
cambiata solo la riga in `CanCatchTrainerMon()`.

### TcloneR: contatore e premi (sessione A)

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

#### Verificato in emulatore (23/10, ROM strumentata: script di debug 3/4/5 impostano il contatore, 7 chiama i premi, 8 lo mostra)

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

### Lo scienziato (sessione B)

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

#### Verificato in emulatore (23/10, ROM strumentata: script di debug 7/8 = scenario successivo/stesso, che imposta tappa e stato della storia e teletrasporta davanti allo scienziato; per Cerulean e Pallet una build con lo scienziato fermo, perché vagando non si lascia pilotare)

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

### Motore della cattura (da `TRAINER CATCH`, invariato)

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
- **Niente cattura nelle lotte di scena (26/10)**: quel `releaseall; end` va
  bene per chi ti vede o con cui parli (dopo c'è al massimo la registrazione
  del numero, i `trainerbattle_single` con event script), ma in una lotta
  dentro una scena salta il resto: al covo di Mahogany i grunt dell'allarme
  restavano in mappa e la statua non passava a 2, con Silver e gli admin la
  storia non avanzava. `IsCutsceneTrainerBattle()` (`battle_util.c`) blocca
  i modi `SINGLE_NO_INTRO_TEXT`, `TWO_TRAINERS_NO_INTRO` ed `EARLY_RIVAL`,
  tranne le classi Leader/Leader Kanto/Elite Four/Champion (vanilla e
  `_HNS`), dove dopo c'è solo il premio e si rifà parlandoci. Scelta
  dell'utente fra questa regola e "niente cattura contro il Team Rocket":
  i grunt normali del covo restano catturabili. Censimento per classe dei
  `trainerbattle_no_intro` (mappe `_hns` e `data/scripts`): 21 Silver,
  7 grunt, 5 admin/Giovanni, 8 Kimono Girl, Red due volte e Steven, 77
  capi/Superquattro/Campione, il resto sparso. I `trainerbattle_single` con
  event script sono tutti registrazioni del numero: lasciati catturabili.
  Effetto collaterale accettato: anche i quattro allenatori finti della
  palestra di Fuchsia (no_intro, dopo c'è solo un messaggio) non si clonano più.
  Verificato in emulatore sul salvataggio dell'utente con una ROM strumentata
  (script di debug 7 = `trainerbattle_no_intro` con Falkner, 8 = con
  `TRAINER_GRUNT_17_HNS`, entrambi seguiti da un `msgbox`): sul grunt
  "Don't be a thief!", Master Ball 20 → 20, vinta la lotta compare il
  messaggio dopo; su Falkner "Gotcha! PIDGEY was caught!" e lo script
  finisce come prima, senza il messaggio dopo.
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
