# Piano: cattura nell'overworld (stile Leggende Pokémon: Arceus)

**Stato (21/11): fasi 1-11 fatte; prossima: fase 12 (Opus 5.5).** Il diario completo delle fasi fatte (misure, prove, revisioni) è in [`docs/claude/overworld-catch-history.md`](../claude/overworld-catch-history.md): qui restano decisioni, formule, fasi e l'handoff.

Si appoggia agli OWE già in gioco: leggere prima
[`.claude/rules/overworld_encounters.md`](../../.claude/rules/overworld_encounters.md)
e, per i fatti del port, [`overworld-encounters.md`](overworld-encounters.md).

## 1. Cosa si fa

- **Lancio della Ball nell'overworld**: con **R** il giocatore lancia una Ball
  (o il suo Pokémon, che fa partire la lotta) dritta davanti a sé; se sulla linea c'è un Pokémon selvatico visibile (OWE)
  si tenta la cattura senza entrare in lotta, con un'animazione vera (volo,
  Pokémon risucchiato, scosse, esito).
- **Toccare o farsi toccare** resta com'è: lotta classica.
- **Passo lento** (al posto del gattonare): **B tenuto + direzione**, ovunque,
  anche durante la ricerca DexNav (che oggi usa A tenuto).
- **Percezione migliore**: i Pokémon sentono il giocatore in base a quanto
  rumore fa (corsa, camminata, passo lento), lo vedono in un cono, non lo
  vedono attraverso gli ostacoli né quando è nell'erba alta.
- **Meno Pokémon visibili** e ricambio più lento.
- **Ball speciali meno care.** Niente crafting.
- **Pokémon alfa** (seconda parte, fasi 8-11): più grandi sulla mappa, sempre
  aggressivi, più forti all'inizio della lotta come i totem. Se sono troppo
  forti per le medaglie che hai **non si possono catturare**: si possono
  battere, ma conviene fuggire. Catturati restano alfa per sempre, con
  abilità nascosta, una mossa che la specie non ha salendo di livello e un
  segno nel riassunto.
- **Ricorda mosse a pagamento** per le mosse uovo e tutor, come in Soulgold
  (fase 7): così la mossa di un alfa resta speciale.

## 2. Decisioni prese

Risposte dell'utente (14/11):

| # | Decisione |
| --- | --- |
| D1 | **Comandi** (utente, 16/11; sostituisce la prima proposta "L lancia, A lento"): **R** lancia la Ball o il Pokémon scelto; **L premuto e rilasciato** passa da Ball a Pokémon e viceversa; **L tenuto + ←/→** scorre le Ball o i Pokémon disponibili (il giocatore non si muove mentre L è tenuto); **B tenuto** = passo lento. SELECT resta allo strumento registrato (premuto) e al secondo (tenuto 1 s). R in bici alterna ancora Mach e Acro. La scorciatoia R della ricerca DexNav **sparisce**; il DexNav si apre dal menu Start (icona DexNav, `src/strip_start_menu.c:192`). Mock del 16/11 approvati: modalità Ball e modalità Pokémon con L tenuto. |
| D17 | **Lancio del Pokémon**: in modalità Pokémon, R manda il Pokémon scelto contro il selvatico davanti e parte la lotta con **quel Pokémon in campo per primo** (solo per quella lotta). Scelta solo fra Pokémon non esausti e non uova. Nessun bonus di sorpresa nella prima versione. Senza bersaglio non succede niente. |
| D18 | **Pannello** (Ball o Pokémon scelto, quantità o livello, modalità) **in basso a sinistra** (utente, 16/11; nei mock era a destra): compare **mentre L è tenuto e per ~2 s** dopo un cambio o un lancio, poi sparisce; si nasconde quando si apre un messaggio o un menu. Segue il tema scuro senza codice in più se usa la cornice della palette 14 e il testo della 15, come il banner degli strumenti (`dark_ui.md`): da verificare in emulatore, fase 5. |
| D2 | **Probabilità**: formula normale della lotta a PS pieni, **×2 se il Pokémon non ha notato il giocatore**. Se la cattura fallisce il Pokémon si allerta: fugge o attacca secondo l'indole. |
| D3 | **Densità**: tetti **4 / 2 / 2** (erba / acqua / grotta), **Safari 3**; ricambio a tetto pieno da 240 a **480 frame**. |
| D4 | **Prezzi**: Poké/Great/Ultra restano 10/15/30. Net, Nest, Dive, Dusk, Timer, Repeat da 1000 e Luxury da **3000** (non 1000: `I_PRICE >= GEN_8`) a **100**; Heal da 300 a **50**. |

Scelte di progetto prese qui, **con un default**: l'utente può cambiarle prima
della fase che le realizza.

| # | Scelta | Default | Fase |
| --- | --- | --- | --- |
| P1 | Quale Ball parte | l'ultima lanciata (`gLastThrownBall`) se è in borsa, altrimenti la prima della tasca Ball; **L tenuto + ←/→** la cambia (D1) | 3, 5 |
| P22 | Andature e rumore (fase 2) | AUTORUN acceso (default delle partite nuove dal 16/11; prima era spento: v. §5.2): direzione sola = **corsa**, sentita a 5 caselle in ogni direzione; B tenuto = **lenta** (`PlayerWalkSlow`, il passo del DexNav), non sentita: conta solo la vista; bici = 5. La **camminata normale** (sentita a 2) esiste solo con AUTORUN spento (P20) | 2 |
| P23 | Forma del cono (fase 2) | oggi è un rettangolo 4×3 uguale per tutti (`CanAwareOWESeePlayer`, `src/wild_encounter_ow.c:2021`). Diventa un cono: larghezza 1 casella a distanza 1, 3 a distanza 2-3, 5 da 4 in poi, fino alla `viewDistance` del comportamento; la vista si ferma contro le caselle che bloccano il passaggio fra il Pokémon e il giocatore | 2 |
| P20 | AUTORUN spento | B tenuto resta la **corsa**, come oggi: senza corsa automatica il passo lento non c'è e la camminata normale si sente a 2 caselle | 2 |
| P21 | BUTTON MODE `L=A` | L vale A (`src/main.c:300`): niente cambio di modalità né scorrimento; R lancia comunque la Ball o il Pokémon già scelto | 3 |
| P2 | Gittata | 5 caselle in linea retta; si ferma al primo ostacolo o dislivello (D16: niente consumo) | 3 |
| P3 | Nuzlocke / One Type / Species Clause | le stesse regole della lotta: dove la cattura sarebbe bloccata la Ball non parte (messaggio); una cattura riuscita segna la zona. **Un lancio fallito non la segna** (utente, 17/11): col lancio conta solo la prima cattura riuscita, diversamente dalla lotta, dove qualunque esito consuma l'incontro. Scelta voluta, non un buco | 3 |
| P4 | Leggendari erranti e sciami | il lancio fallisce sempre e parte la lotta (i loro dati vivono fuori dall'oggetto) | 3 |
| P5 | Zona Safari, Gara Pigliamosche, Azienda Lotta | lancio **spento** nella prima versione | 3 |
| P6 | Esperienza alla cattura | nessuna (in lotta `B_EXP_CATCH` la dà: qui non c'è chi ha partecipato) | 3 |
| P7 | BUTTON MODE `L=A` | sostituita da P21 (il lancio ora è su R) | — |
| P8 | Squadra piena | va al PC come in lotta; box pieni: la Ball rimbalza. In raffica la squadra si può riempire a metà: il controllo è **all'impatto**, non al lancio, e così Nuzlocke e Species Clause (la prima cattura segna la zona, la seconda Ball rimbalza anche se era già in aria) | 3 |

Il ×2 di D2 è una scelta di bilanciamento **nostra**: non è un valore preso da
Leggende Arceus.

Seconda parte, risposte dell'utente (14/11, dopo la prima stesura):

| # | Decisione |
| --- | --- |
| D5 | **Mega fuori** dal gioco per ora: il limite di ROM non è un vincolo per questo piano. |
| D6 | **DexNav resta** (utente, 18/11, fase 6): cerca una specie, catene, nascosti, abilità e mosse; il lancio non lo sostituisce. |
| D7 | **Alfa troppo forte**: non catturabile (né in lotta né con il lancio), ma **si può battere e si può fuggire**. È tarato perché fuggire convenga. |
| D8 | **Alfa all'inizio della lotta**: aumento delle statistiche come i totem. |
| D9 | **Alfa catturato**: resta alfa per sempre (bit nel Pokémon salvato), **abilità nascosta garantita**, **una mossa speciale**, **segno nel riassunto**, **follower più grande**, missioni nel quest book. |
| D10 | **Ricorda mosse**: mosse uovo e tutor solo dopo aver pagato (o con eventi), preso da Soulgold. |
| D11 | Alfa e ricorda mosse **nello stesso piano**, in fasi dopo la cattura. |
| D12 | **Fino a 2 alfa per mappa**, e più frequenti di 1 su 50: con i tetti abbassati (D3) sarebbero troppo rari. |
| D13 | **Missione a livelli**: cattura **1 / 5 / 10 / 30** alfa, con premi crescenti. |
| D15 | **Raffica**: fino a **3 Ball in aria**, intervallo minimo ~½ s fra due lanci, il giocatore resta libero di muoversi. Esito **deciso all'impatto** (Pokémon creato e consegnato subito); le scosse sono solo animazione. Banner **in coda**, uno dopo l'altro; soprannomi chiesti **alla fine** della raffica. Se un Pokémon uscito ti carica e parte la lotta, le catture già decise restano. |
| D16 | **Consumo**: la Ball si consuma solo se colpisce un Pokémon; a vuoto (5 caselle e cade) o contro NPC/muro/cartello torna in borsa. Verso il follower non parte. |
| D14 | **Banner di cattura**: il riquadro in alto degli strumenti ottenuti, con l'icona del Pokémon al posto dell'icona dello strumento, "You caught XXX♂!" e sotto "It was added to your party." o "It was sent to the PC.", con una **fanfara breve**. Prototipo approvato in emulatore il 14/11 (§3). |

Terza parte, risposte dell'utente (18-20/11, dal documento
[Riordino Impostazioni e Sfide](https://claude.ai/artifact/7mFewnXGQH3VQCdSL1afTK),
che tiene la destinazione di ogni voce; le fasi lo rileggono, non lo copiano):

| # | Decisione |
| --- | --- |
| D19 | **Menu riordinati**: Impostazioni in 4 schede (GENERALE, MAPPA, LOTTA, SUONO: la MAIN di oggi ha 20 voci su 20), Sfide in 5 (MODALITÀ con dentro FEATURES, RANDOMIZER, NUZLOCKE, DIFFICOLTÀ, SFIDE). EASY CATCH e LEVEL SCALING vanno in Sfide · DIFFICOLTÀ; CLOCK TYPE e ALT SHINY in Impostazioni · GENERALE; il resto voce per voce nel documento. Spostare una voce non tocca il salvataggio. |
| D20 | **OVERWORLD CATCH** (Impostazioni · MAPPA, ON/OFF, default ON). Spenta: tetti 6/3/3 e ricambio a 240 frame, niente passo lento né udito e cono (B corre), R avvia la ricerca DexNav, L senza pannello; **i prezzi restano**. Gli alfa (fasi 12-14) esistono solo col lancio acceso. |
| D21 | **ARCEUS FORMULAS** (Impostazioni · LOTTA, OFF/ON, default OFF): **una voce sola** per le statistiche di Leggende Arceus (ogni Pokémon, ricalcolate al cambio) e la sua formula di cattura **solo nell'overworld** (in lotta resta quella di oggi); viaggiano insieme (utente, 20/11). Con loro la **formula del danno** di Arceus, se no le prime lotte durano 3-4 volte di più. Livelli d'impegno **derivati** da IV ed EV (niente dati nuovi nel Pokémon); le sfide su IV/EV restano e agiscono tramite i livelli. Il 20/11: **un tasso di cattura unico, 135, per tutte le specie** (la differenza la fanno la Ball del tipo, il livello e il comportamento); **leggendari e misteriosi tengono il loro tasso del gioco**; i **roamer restano come oggi** (R avvia la lotta, dove provano a fuggire), **rango del giocatore = 8 medaglie di Johto + Campione di Johto (0-9)**, colpo alle spalle come in Arceus (colpito da dietro). Ball: Poké/Great/Ultra coi valori di Arceus, le altre col loro effetto × 0,75 (il ×4 per tipo delle Ball di Kurt prende il posto dei tassi per specie); stadi delle statistiche del gioco (§3, documento [Formule di Arceus: scelte](https://claude.ai/artifact/3AR9iBxCBwF5N6RMHweHot), chiuso). Il 21/11: stadi, abilità e strumenti agiscono sul **danno finale** (come AtkMod/DefMod di Arceus), Adattabilità 1,5. |
| D22 | **Salvataggio**: 2 bit, con il passo `saveVersion < 15` che li azzera. **`unusedTrainerCatchBit` non è libero**: vale 1 nel `.sav` dell'utente (slot 0 e 1, letto il 20/11); nessun codice di oggi lo scrive. (Fase 8: quel `.sav` è a `saveVersion` 8, scritto da PLUS.2-5, e il bit a 1 è TRAINER CATCH acceso; il passo 9 non ci era mai passato.) Il secondo bit in coda a SaveBlock3 (20 byte liberi; `autoRun` ha 7 bit a zero nello stesso `.sav`, alternativa). OVERWORLD CATCH salvata invertita (0 = accesa). |

Default della seconda parte, cambiabili prima della fase:

| # | Scelta | Default | Fase |
| --- | --- | --- | --- |
| P9 | Quanti alfa | **1 su 20** Pokémon generati, **al massimo 2 per mappa** (D12); mai shiny e alfa insieme (lo shiny vince); niente alfa in Safari, Gara, Azienda Lotta, acqua (lo sprite da nuoto non scala). Conto: a tetto pieno nasce un Pokémon ogni 480 frame (8 s, D3), cioè 7,5 al minuto: con 1 su 20 un alfa ogni ~2,7 minuti d'erba, con 1 su 50 ogni ~6,7. La fase 12 lo misura in frame e lo ritocca | 12 |
| P17 | Premi della missione (D13) | 1 alfa: 10 Ultra Ball; 5: **da scegliere** (l'Ability Patch si compra già a Violet, `src/shop.c:1000`: non è un premio); 10: Shiny Charm (funziona, `src/pokemon.c:3249`, e nelle mappe di HnS non si ottiene altrove); 30: Master Ball. Si ritirano da un NPC, come lo scienziato di TcloneR (chi e dove: fase 14) | 14 |
| P10 | Livello | quello tirato dalla tabella (già scalato) **+ 10**, senza il tetto `B_WILD_SCALING_MAX_ABOVE_TABLE` | 12 |
| P11 | "Troppo forte" | livello dell'alfa **sopra** `GetLevelScalingCap()` (il tetto delle medaglie anche con LEVEL CAP spento, `src/caps.c`) | 13 |
| P12 | Comportamento | sempre aggressivo, vista più lunga, non sparisce finché sei sulla mappa (come gli shiny) | 12 |
| P13 | Aumento in lotta | +1 a Attacco, Difesa, Att. Sp., Dif. Sp., Velocità, col messaggio dei totem | 13 |
| P14 | Mossa speciale | una mossa uovo della specie a caso, altrimenti una mossa tutor, altrimenti nessuna; sostituisce l'ultima | 13 |
| P15 | Lancio su un alfa troppo forte | la Ball rimbalza **e non si consuma**, con un messaggio; l'alfa ti nota | 13 |
| P18 | Dopo il banner | soprannome chiesto come in lotta (rispetta `skipNicknamePrompt` e il soprannome obbligatorio della Nuzlocke); **niente** pagina del Pokédex | 3 |
| P19 | Fanfara | `MUS_HG_OBTAIN_EGG` (`FANFARE_HG_OBTAIN_EGG`, 131 frame, ~2 s, "hai ricevuto un Pokémon"); il banner si chiude con A o B solo a fanfara finita | 3 |
| P16 | Ricorda mosse MT | restano **sempre** disponibili (D10 parla solo di uovo e tutor; in SG le MT sono spente) | 7 |

## 3. Formule di Arceus (fonti e scelte)

Fonti: Bulbapedia, sezioni "Pokémon Legends: Arceus" di
[Stat](https://bulbapedia.bulbagarden.net/wiki/Stat),
[Catch rate](https://bulbapedia.bulbagarden.net/wiki/Catch_rate) e
[Damage](https://bulbapedia.bulbagarden.net/wiki/Damage) (screenshot
dell'utente del 20/11: da qui rispondono 403), controllate su
[PKHeX](https://github.com/kwsch/PKHeX) (`PA8.cs`, `IGanbaru.cs`), che replica
il codice del gioco. Implementazione e trappole: `.claude/rules/arceus.md`.
Misure e dati scartati: `docs/claude/overworld-catch-history.md`.

**Statistiche**: PS = ⌊(L/100 + 1) × B + L⌋ + ELB; altra = ⌊⌊(L/50 + 1) ×
B / 1,5⌋ × N⌋ + ELB; ELB = round((√B × m + L) / 2,5); m per impegno 0-10: 0,
2, 3, 4, 7, 8, 9, 14, 15, 16, 25. Impegno = base dagli IV (≥ 20 → 1, ≥ 26 →
2, 31 → 3) + `EV × 7 / 252`, massimo 10 (utente, 21/11). Controllo: base
120, Lv 100, impegno 10 → PS 490, altra 390.

**Cattura** (solo col lancio): a = (0,75 × tasso) + 30 × (rango + (3·PSmax −
2·PS)/(3·PSmax) × stato × Ball × comportamento × alle spalle); rango =
rango_giocatore − rango_Pokémon + 1, fra −5 e 0; rango_Pokémon = ⌊(L − 1)/10
+ 1⌋, **+4 se alfa** (fase 13), max 10. P(cattura) = a/255; a < 0
impossibile. Scelte del 20/11: tasso unico **135** (leggendari e misteriosi
col loro), rango del giocatore = medaglie di Johto + Campione (0-9),
comportamento 2 (non notato) o 1, colpo alle spalle ×1,75; Ball: Poké/Great/
Ultra 0,75/1,5/2,25, le altre col loro effetto × 0,75, Master/Beast/Safari/
Sport invariate. Roamer invariati (R avvia la lotta).

**Danno**: base = ((100 + A + 15 × L) × P / (D + 50)) / 5 con A e D
**grezzi**, e il numeratore × finale/grezzo dell'Attacco × grezzo/finale
della Difesa prima delle divisioni (stadi ±6 del gioco, abilità, strumenti,
bonus medaglia: pesano come nella classica, come AtkMod/DefMod di Arceus;
deciso il 21/11). Senza modificatori è la formula troncata a ogni passo.
STAB 1,25 (Adattabilità 1,5, utente 21/11), doppia debolezza ×2,5, doppia
resistenza ×0,4; brutto colpo, meteo, scottatura del gioco.

**Inizio gioco** (utente, 21/11): le pozioni curano come in Arceus (Pozione
60, Superpozione 100, Iperpozione 150; Serebii e Bulbapedia), perché i PS
sono più che raddoppiati; il danno dei Pokémon **selvatici** è ×0,6 / 0,75 /
0,9 con 0 / 1 / 2 medaglie di Johto, pieno dalla terza (allenatori e
giocatore mai).

## 4. Spazio

ROM 95,13% dopo la fase 11 (errore CI al 99,5%); SaveBlock3 96 B su 116;
VRAM sprite nel caso peggiore 622 tile e 10 palette su 1024 e 16 (fase 4).

## 5. Fasi

Una fase per sessione (`/next-phase overworld-catch`).

| # | Fase | Modello | Cancello per passare oltre |
| --- | --- | --- | --- |
| 1 | Densità e prezzi (D3, D4) | Sonnet 5.5 | fatto (§5.1) |
| 2 | Passo lento e percezione | Opus 5.5 | fatto (§5.2) |
| 3 | Nucleo della cattura e banner | Fable 5.2 | fatto (§5.3) |
| 4 | Animazione del lancio | Fable 5.2 | fatto su Opus (§5.4) |
| 5 | Pannello e documentazione | Sonnet 5.5 | fatto (§5.5) |
| 6 | Revisione della prima parte | Opus 5.5 | fatto (§5.6) |
| 7 | Ricorda mosse a pagamento (D10) | Sonnet 5.5 | fatto (§5.7) |
| 8 | Menu e salvataggio 15 (D19, D22) | Opus 5.5 | fatto (§5.8) |
| 9 | Interruttore OVERWORLD CATCH e Ball nei Mart (D20) | Opus 5.5 | fatto (§5.9) |
| 10 | ARCEUS FORMULAS: statistiche e cattura (D21) | Opus 5.5 | fatto (§5.10) |
| 11 | ARCEUS FORMULAS: danno e bilanciamento (D21) | Opus 5.5 | fatto (§5.11) |
| 12 | **Alfa sulla mappa**: bit `alpha` nell'oggetto, tiro P9, livello P10, comportamento P12, **prototipo dello sprite più grande** (se non regge: un'aura o un'icona sopra la testa, e lo si scrive) | **Opus 5.5** | build verde; con il debug che forza l'alfa: compare uno per mappa, ti insegue, non sparisce; sprite guardato su erba, in grotta, accanto ad altri OWE; tile e matrici affini misurate; un salvataggio vecchio carica senza alfa fantasma. `.srm` non necessario |
| 13 | **Alfa in lotta e catturato**: aumento P13, blocco della cattura in lotta e col lancio (D7, P11, P15), bit nel Pokémon salvato con `MON_DATA_IS_ALPHA`, abilità nascosta e mossa P14 alla nascita | **Opus 5.5** | build verde; un alfa sopra il tetto respinge la Ball in lotta e nell'overworld (Ball non consumata), si batte e si fugge; uno sotto il tetto si cattura e **in squadra, dopo salvataggio e ricarica e passaggio nel PC, è ancora alfa** con abilità e mossa giuste; un Pokémon del salvataggio vecchio legge non-alfa e il checksum regge. **Serve il `.srm`** |
| 14 | **Alfa visibile e missione**: segno nel riassunto (tutti e due i temi), follower più grande se la fase 12 l'ha reso possibile, **missione a livelli 1/5/10/30** sul modello di TcloneR (contatore, premi P17, NPC che li consegna, sottomissioni nel quest book, `/add-quest`) | **Sonnet 5.5** | screenshot del riassunto chiaro e scuro con indici contati sui `.4bpp`; follower guardato; con il contatore portato a 0, 1, 5, 10, 30 via debug: stato e "NEW" giusti nel libro, ogni premio ritirato una sola volta; le due var nuove a zero nel `.srm` dell'utente prima della modifica. **Serve il `.srm`** |
| 15 | **Revisione finale e documentazione**: `/code-review` medio sul diff delle fasi 7-14, CHANGELOG (con l'avviso del ricorda mosse), regole, report SG (la fase 7 è un port), ROM consegnata | **Opus 5.5** | build e CI verdi, punti aperti chiusi o motivati. **Serve il `.srm`** |

### 5.1 Fase 1, com'è stata fatta (16/11)

- Tetti 4/2/2, Safari 3, ricambio 480 frame, prezzi delle Ball. Misurato in
  frame su Route 29.

### 5.2 Fase 2, com'è stata fatta (16/11)

- Passo lento su B (anche nella ricerca DexNav), udito per andatura, cono di
  vista con ostacoli ed erba alta, `IsOWEAwareOfPlayer()`.

### 5.3 Fase 3, com'è stata fatta (17/11)

- Lancio con R/L (D1), Pokémon lanciato (D17), esito all'impatto e raffica
  (D15), formula condivisa con la lotta, blocchi P3-P8, banner con fanfara.

### 5.4 Fase 4, com'è stata fatta (17/11)

- Animazione: posa, arco, risucchio, scosse, fino a 3 Ball insieme.

### 5.5 Fase 5, com'è stata fatta (17/11)

- Pannello in basso a sinistra (solo testo), regola `overworld_catch.md`,
  CHANGELOG, README.

### 5.6 Fase 6, com'è stata fatta (18/11)

- `/code-review` delle fasi 1-5: 7 correzioni. DexNav resta (D6).

### 5.7 Fase 7, com'è stata fatta (20/11)

- Ricorda mosse uovo/tutor a pagamento: Egg Move Master a Blackthorn,
  Tutor Move Master a Olivine (House3: posto da confermare con l'utente).

### 5.8 Fase 8, com'è stata fatta (20/11)

- Opzioni in 4 schede, Sfide in 5, OVERWORLD CATCH e ARCEUS FORMULAS,
  `SAVE_VERSION` 15 provato col `.sav` dell'utente.

### 5.9 Fase 9, com'è stata fatta (20/11)

- OVERWORLD CATCH spenta = gioco di prima salvo i prezzi; Ball speciali nei
  Mart per medaglie.

### 5.10 Fase 10, com'è stata fatta (21/11)

- Statistiche di Arceus (36 su 36 uguali al riferimento PKHeX), squadra e PC
  ricalcolati al cambio, cattura di Arceus col lancio.

### 5.11 Fase 11, com'è stata fatta (21/11)

- **Danno di Arceus** con stadi, abilità e strumenti applicati al danno
  finale (§3): così +2 stadi raddoppiano il danno come nella classica (prima
  davano +15%). Adattabilità 1,5. Verificato in mGBA con una ROM
  strumentata contro un riferimento scritto da Bulbapedia: doppia
  resistenza, doppia debolezza, neutro, Att. Sp. +2, Dif. Sp. +2, bonus
  medaglia, opzione spenta; allenatore scalato con BST EQUALIZER e sfide
  IV/EV fino alla fine della lotta. ROM 95,13%.
- **Colpi per KO** (IV 15, EV 0, natura neutra, danno minimo-massimo; nessun
  modificatore):

| Fase | Attaccante → difensore | Mossa | Classica: PS, danno, colpi | Arceus: PS, danno, colpi |
| --- | --- | --- | --- | --- |
| inizio | Cyndaquil Lv 6 → Pidgey Lv 4 | Ember 40 STAB | 17, 7-10, **2-3** | 47, 25-30, **2** |
| inizio | Pidgey Lv 4 → Cyndaquil Lv 6 | Tackle 40 | 21, 2-3, **7-11** | 49, 15-18, **3-4** |
| inizio | Chikorita Lv 8 → Rattata Lv 6 | Tackle 40 | 20, 5-7, **3-4** | 39, 22-26, **2** |
| inizio | Rattata Lv 6 → Chikorita Lv 8 | Tackle 40 STAB | 26, 4-6, **5-7** | 59, 19-22, **3-4** |
| inizio | Totodile Lv 10 → Pidgeotto Lv 9 | Water Gun 40 STAB | 31, 7-9, **4-5** | 81, 25-30, **3-4** |
| inizio | Pidgeotto Lv 9 → Totodile Lv 10 | Gust 40 STAB | 31, 6-7, **5-6** | 69, 25-30, **3** |
| metà | Quilava Lv 22 → Miltank Lv 20 | Flame Wheel 60 STAB | 71, 12-15, **5-6** | 142, 40-47, **4** |
| metà | Miltank Lv 20 → Quilava Lv 22 | Stomp 65 STAB | 60, 21-25, **3** | 101, 56-67, **2** |
| metà | Croconaw Lv 28 → Haunter Lv 27 | Bite 60 ×2 | 65, 44-54, **2** | 95, 114-136, **1** |
| metà | Bayleef Lv 30 → Graveler Lv 28 | Razor Leaf 55 STAB ×4 | 73, 52-64, **2** | 109, 100-117, **1-2** |
| metà | Skarmory Lv 36 → Typhlosion Lv 38 | Steel Wing 70 STAB ×0,5 | 112, 15-18, **7-8** | 160, 35-41, **4-5** |

  Le lotte si accorciano dalle due parti; all'inizio pesano soprattutto i
  colpi subiti (la Difesa conta poco, `D + 50`, il livello molto, `15 × L`).
- **Inizio gioco** (§3, utente 21/11): pozioni di Arceus e selvatici ridotti
  fino alla terza medaglia. Con la riduzione Pidgey → Cyndaquil va da 3-4 a
  5-6 colpi, Rattata → Chikorita da 3-4 a 5-6; una Pozione su Cyndaquil Lv 6
  (49 PS) lo cura di nuovo tutto (prima 20). Verificato in mGBA:
  descrizioni 60/100/150 in borsa, Pozione +60 fuori e dentro la lotta (+20 a
  opzione spenta), colpo selvatico con 0 medaglie ×0,6 (Fury Attack 11 → 7 →
  6), allenatore con 0 medaglie e giocatore ×1.

**Handoff per la fase 12 (Opus 5.5)**
- La fase 12 non tocca le formule. Con l'opzione accesa un alfa (+10
  livelli, P10) picchia forte (`15 × L` nel danno): da ricordare per P13
  nella fase 13.
- Banco di prova del danno: struct in EWRAM scritta da
  `DoMoveDamageCalcVars` quando `ctx->randomFactor` (potenza, A e D grezzi e
  finali, base, tiro, danno); il client RAM e il riferimento Python vanno
  riscritti a ogni container (`docs/claude/emulator.md`).
- Ancora da chiedere all'utente: dalla fase 9 il passo lento con B nella
  ricerca DexNav a cattura spenta e il Mart di Cherrygrove; dalla fase 7 il
  posto del Tutor Move Master.

## 6. Scartato, e perché

- **Crafting delle Ball**: escluso dall'utente, per ora.
- **Gattonare**: non esistono frame del giocatore accovacciato; il passo lento
  c'è già.
- **Strumenti da lancio di Leggende Arceus** (bacche, fango, fumogeni): fuori
  da questo piano.
- **Statistiche, IV e colori come premio dell'alfa** (utente, 14/11): gli IV
  si possono già portare al massimo e gli EV togliere, e un colore diverso si
  confonderebbe con lo shiny.
- **Alfa grande in lotta e nei menu**: gli sprite di lotta sono 64×64 fissi;
  solo la mappa può scalare (fase 12).
- **Mira libera o a parabola regolabile**: il motore si muove a caselle; una
  linea retta nella direzione dello sguardo è leggibile e costa poco.
- **Lancio nel Safari e alla Gara** (P5): hanno Ball contate e regole proprie
  (`gNumSafariBalls`, punteggio della Gara). Si possono aggiungere dopo.
- **Tetto `OWE_SPAWNS_MAX` a 4**: sposterebbe i `localId` degli OWE; bastano
  i tetti per zona.
