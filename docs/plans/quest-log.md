# Backlog: missioni per il registro (logbook)

**Stato (24/10): raccolta di idee, niente di deciso né pianificato.**

L'utente vuole aggiungere più avanti un **registro delle missioni**: un elenco
consultabile dal giocatore, con le missioni in corso e quelle completate.
Questo file raccoglie le missioni candidate man mano che saltano fuori. Il
registro stesso (dove si apre, come si presenta, cosa salva) è da progettare:
nessuna verifica fatta finora nel codice.

Ogni missione indica cosa deve raccontare, i posti e i personaggi di HnS che
userebbe, e il costo stimato (solo testo, script, lotte, dati nuovi).

## Da Red's Path al Monte Argento

Nate dal piano di [Red's Path](reds-path.md) (24/10). Red's Path finisce con
la squadra Charizard, Pikachu-Starter, Snorlax, Eevee, Poliwrath, Lapras;
al Monte Argento HnS gli dà Pikachu-Starter, Snorlax, Charizard, Venusaur,
Blastoise, Espeon (`TRAINER_RED_HNS` in `src/data/trainers_hns.party`).
Queste missioni colmano il salto: da dove vengono **Venusaur e Blastoise**
(in FireRed, partendo con Charmander, non si ottengono), quando **Eevee
diventa Espeon** (evoluzione di Gen 2), dove finiscono **Poliwrath, Lapras,
Butterfree** e la panchina, e come si arriva a Red.

Agganci già in HnS:
- la **casa di Red** a Biancavilla (`PalletTown_RedsHouse_1F_hns`): la madre
  dice che Red è via da tanto e non ha sue notizie;
- il **laboratorio di Oak** (`PalletTown_Lab_hns`);
- la casa di **Bill** sul Percorso 25;
- la **Villa di Azzurropoli**, dove Red ha avuto Eevee;
- in HnS **nessuno script regala gli starter di Kanto** (compaiono solo nel
  menu debug, `data/scripts/debug.inc`).

### 1. Il PC di Bill

Finito Red's Path, Bill dice che Red gli ha lasciato in custodia i Pokémon
del viaggio: Butterfree, Mankey, Diglett/Dugtrio, Gloom, Poliwrath, Lapras
(le panchine delle otto sfide di Red's Path). Il giocatore li vede, o
combatte una lotta speciale con loro, e Bill racconta che Red li ha lasciati
quando è partito per allenarsi da solo.

- Racconta: dove finisce la squadra del viaggio.
- Costo: script e testo; una lotta, se c'è, riusa il motore di Red's Path
  (squadra prestata).

### 2. Gli starter di Oak

Oak racconta che Red è tornato dopo la Lega a chiedere i due starter rimasti,
per allevarli sul Monte Argento. In *Pokémon Origins* Blue sceglie Squirtle,
quindi Bulbasaur sarebbe rimasto al laboratorio; per Blastoise serve
un'invenzione (un secondo Squirtle, uno scambio con Blue). Il giocatore porta
a Red un oggetto di Oak (le Poké Ball degli starter, una lettera) fino al
Centro Pokémon del Monte Argento.

- Racconta: Venusaur e Blastoise.
- Attenzione: tocca Oak e l'accesso al Monte Argento, cioè la progressione
  del gioco.

### 3. Eevee al sole

Espeon è un'evoluzione di Gen 2 (amicizia, di giorno). La signora della
Villa di Azzurropoli che ha dato Eevee a Red riceve la foto di un Espeon e
chiede al giocatore di portarle notizie; oppure un indizio in Johto, dove Red
è passato per arrivare al Monte Argento.

- Racconta: Espeon.
- Costo: script e testo.

### 4. La lettera alla madre

A Red's Path completato, la madre di Red ha ricevuto una lettera (o il
giocatore gliela porta): Red racconta in poche righe i tre anni dopo il
viaggio e i nuovi compagni.

- Racconta: tutto (Venusaur, Blastoise, Espeon, il ritiro sul Monte
  Argento), senza meccaniche nuove.
- Costo: solo testo.

### 5. Red contro Blue

Una nona lotta dopo le otto sfide di Red's Path: Red contro Blue campione,
con la squadra di Giovanni, come alla fine del viaggio di Gen 1. Esce dal
tema dei capi palestra.

- Racconta: la fine del viaggio di Kanto.
- Costo: una lotta in più col motore di Red's Path; squadra di Blue da
  definire (FRLG ha le squadre del campione in `trainers_frlg.party`).

### Suggerimento iniziale

1 + 4 come base (chiudono la squadra di Red's Path e spiegano il resto con
posti e personaggi che HnS ha già); 2, 3 e 5 come missioni aggiuntive del
registro.

## Altre missioni

(da aggiungere)
