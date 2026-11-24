# Emulatore: guida completa

Letta su richiesta da `CLAUDE.md` §3. Non è caricata in automatico.

mGBA gira headless e si pilota. Vale l'investimento: è così che sono stati trovati
i bug che l'analisi statica non vedeva.

```sh
apt-get install -y --no-install-recommends mgba-sdl xvfb xdotool imagemagick
Xvfb :99 -screen 0 800x600x24 &
DISPLAY=:99 SDL_AUDIODRIVER=dummy /usr/games/mgba pokehns.gba &   # in background
```

- Tasti: A=`x`, B=`z`, Start=`Return`, Select=`BackSpace`, L=`a`, R=`s`, frecce.
- Input: `xdotool windowfocus --sync $(xdotool search --class mgba|head -1)` e poi
  `xdotool keydown/keyup` (non `--window`: SDL ignora XSendEvent).
- Screenshot: `import -window root out.png`, poi
  `convert out.png -crop 240x160+280+220 +repage -scale 400% zoom.png`.
- L'emulatore è lento sotto Xvfb: servono attese di 2-3 s fra i tasti.
- Menu debug in overworld: tieni **R** e premi **START**
  (`DEBUG_OVERWORLD_MENU` in `include/config/debug.h`, già TRUE).
  Da lì: `Trainers… → Try Battle` entra in lotta in pochi passi.
- Il salvataggio dell'utente è un `.srm`: copialo come `pokehns.sav` accanto alla ROM.

**Misura i pixel invece di fidarti dell'occhio.** Campionare i colori del crop e
risalire alle entry (`#292929` → `RGB(5,5,5)`) ha risolto in un colpo un problema su
cui l'analisi statica girava a vuoto da un'ora.

Con un salvataggio dell'utente già su una rotta con erba la lotta selvatica si
raggiunge camminando avanti e indietro (~15-20 passi). Attenzione: se l'opzione
`WILD BATTLES` è su OFF non succede niente e sembra un bug dell'emulatore.

Due accorgimenti che fanno risparmiare molto tempo:

- avvia `Xvfb` e `mgba` con `setsid nohup ... &`, altrimenti muoiono con la shell
  e ci si ritrova senza display a metà sessione;
- dopo **ogni** passaggio di menu fai uno screenshot e guardalo prima del passo
  successivo. Contare i `Down` alla cieca porta ad aprire le opzioni dal *title
  screen* invece che dal gioco, e te ne accorgi tre schermate dopo.

**ROM strumentata**: quando una feature non si vede e l'analisi statica non
conclude, la via più rapida è una build usa-e-getta con una condizione forzata
(es. `return FALSE;` in cima a `ShouldHideTypeIcon`) o una posizione forzata al
centro schermo. Due build da tre minuti hanno chiuso un problema su cui il
ragionamento girava a vuoto da un'ora. Ricordati di `cp` del file prima.

## Memoria, musica e ora via GDB

- **Leggere il brano in riproduzione, per verificare la musica.** Con
  `mgba -g` (stub GDB, porta 2345) si legge la memoria con un client di dieci
  righe in Python (`$` + `m<addr>,<len>` + checksum). `gMPlayInfo_BGM` (da
  `nm pokehns.elf`) ha il puntatore all'header del brano al primo campo: lo si
  confronta con gli indirizzi dei simboli `mus_*` (`nm`) e si ha il nome. Per
  l'ora, `mgba` non ha opzioni RTC: si lancia con `faketime -f "@2026-10-12
  23:00:00" /usr/games/mgba ...`. Le stesse due scritture in RAM (`M<addr>,...`)
  permettono di dare mosse a un Pokémon: struct a bitfield cifrata
  (`personality ^ otId`), mosse negli 11 bit bassi dei primi quattro u16 della substruct
  indicata da `sSubstructOffsets[1][personality % 24]` (i 5 alti sono altro),
  checksum a somma di u16 da ricalcolare.

**Più letture della RAM nella stessa sessione**: lo stub di `mgba -g`
accetta una sola connessione e non fa detach (`E07`): un `gdb -batch` per ogni
lettura non va, la seconda connessione resta appesa. Funziona un solo
`gdb-multiarch` sempre aperto, alimentato da una fifo (`tail -f fifo |
gdb-multiarch`), con `continue &` dopo il collegamento e `interrupt` / `dump
binary memory` / `continue &` per ogni lettura. `pokehns.elf` non ha
informazioni di debug: gli indirizzi si prendono con `nm`, gli offset dei campi
compilando un `offsetof` con i flag del Makefile (`-mthumb -mabi=apcs-gnu
-march=armv4t -iquote include -DPOKEMON_HNS`). `gSaveBlock1Ptr` punta dentro
`gSaveblock1` con uno spostamento casuale (ASLR): si legge il puntatore, e lo si
rilegge dopo ogni ritorno al campo.

Trappole di sessione:

- **`gdb` su un file con `sed -i` perde l'output**: `sed -i` crea un file nuovo
  e `gdb` continua a scrivere su quello cancellato. Leggere da
  `/proc/<pid di gdb>/fd/1`.
- **I breakpoint con `commands ... continue &` perdono colpi**: non usarli come
  prova di "non chiamato", guarda i dati in RAM (o un watchpoint).
- `interrupt` è asincrono: aspetta "SIGINT" nell'output prima dei comandi.
- **Decodificare un Pokémon in RAM**: `struct Pokemon` 100 byte, sicuri a +32, 4
  sottostrutture da 12 byte cifrate con `personality ^ otId`, ordine
  `sSubstructOffsets[tipo][personality % 24]`; IV nel secondo u32 della
  sottostruttura 3, `abilityNum` bit 29-30 del terzo.
- **Misurare in frame, non con l'orologio**: ogni lettura ferma il gioco ~0,7 s;
  contatore `gMain.vblankCounter1` (`gMain` + 0x20).
- Mai `pkill -f` con una parola che compare nel comando stesso: uccide la shell.
- `xdotool search --class mgba` restituisce **due** finestre: i tasti arrivano
  solo all'ultima (`tail -1`).

## Nuova partita per i test

**Nuova partita in mGBA, per i test**: la camera della camera da letto è sul
bordo della mappa (metà schermo nera) e la scena aspetta input: **non è un
freeze**, l'ho scambiato per uno e ho perso tempo con un A/B. Il personaggio
però non cammina finché la scena non finisce; conviene saltare con il debug:
`Flags & Vars → Toggle Fly Flags`, poi `Utilities → Warp to map warp` (gruppo e
numero da `data/maps/map_groups.json`, es. Violet Gym = 3/5).

## Teletrasporti, scenari e NPC che vagano (sessione TcloneR, 23/10)

- **Warp di debug**: `Utilities → Warp to map warp…` chiede gruppo, mappa e
  numero di warp, che si impostano come gli altri campi numerici del menu
  (Destra/Sinistra cambia il passo, +1, +10, +100; Su/Giù il valore). Gruppo e numero da
  `data/maps/map_groups.json` (indice del gruppo in `group_order`, indice
  della mappa nella sua lista: Azalea 0/3, Goldenrod 0/4, Mahogany 0/9,
  Cerulean 0/34); il warp *n* è l'*n*-esimo di `warp_events` del `map.json`.
  `Give X → Give item XYZ` vuole l'id numerico dell'oggetto (TcloneR 903).
- **Molti casi di uno stesso dialogo: scenari in una ROM strumentata.** Uno
  script di debug che incrementa una var libera e, con un `switch`, imposta
  var e flag della storia e fa `warp` davanti all'NPC; un secondo script che
  ripete lo scenario corrente. Sedici varianti di dialogo provate in
  un'ora, contro una nuova partita per ognuna. `debug.inc` va ripristinato
  prima del commit (copia in scratchpad), e la ROM consegnata ricompilata
  senza.
- **Un NPC che vaga (`MOVEMENT_TYPE_WANDER_AROUND`) non si lascia
  pilotare**: premere A alla cieca non lo trova mai davanti, e premere una
  direzione sposta il giocatore. Per controllare i testi conviene una build
  usa-e-getta con l'NPC fermo; il vagare si guarda a parte nella build vera.
- **Fogli di contatto**: un `import` dopo ogni pressione di A e `montage
  t_*.png -tile 4x -geometry 240x160+2+2` danno un dialogo intero in
  un'immagine sola, comodo da leggere e da consegnare all'utente.
- **Dopo un riavvio del container Xvfb non c'è più** (`Can't open display`)
  mentre gli strumenti installati restano: rilanciare `Xvfb` prima di `mgba`.
- Modificare un `.sav` a mano (checksum per settore): `.claude/rules/save.md`,
  sezione `SAVE_VERSION 9`.

## Misurare gli OWE e trappole di sessione (overworld-catch fase 1, 16/11)

- **`mgba -g` non fa partire il gioco finché GDB non si collega**: schermo
  nero e nessun tasto. Si collega `gdb-multiarch` (`target remote :2345`,
  `continue &`) e **poi** si pilotano i menu con `xdotool`.
- **Un salvataggio fatto su una mappa con OWE li ripristina** (stanno in
  SaveBlock1): prima di contare gli spawn, ricarica la mappa con il
  teletrasporto di debug. Quel salvataggio è anche il caso di prova di
  `TryRemoveOWEAboveCap` (6 OWE salvati con i tetti vecchi).
- **Un tocco di `xdotool` di 0,15 s gira soltanto il giocatore**: per fare un
  passo servono ≥ 0,35 s di `keydown`.
- **Il teletrasporto di debug alla Verdanturf Mart (gruppo 37, mappa 3) manda
  in errore anche la ROM originale** (`Jumped to invalid address 01D001D0`):
  le mappe di Hoenn non si raggiungono dal debug. Negozi di Johto: Azalea
  Mart = gruppo 4, mappa 1.
- `Give X → Give item XYZ` parte da 1 e il numero a schermo è l'id (Nest Ball
  8, Heal Ball 6, Luxury Ball 14); **un prezzo si verifica meglio leggendo la
  ROM**: `gItemsInfo` (da `nm`), struct da 0x2C byte, `price` è il primo `u32`.

## Debug: variabili e stati (sessione small-features-1, 26/10)

- `Flags & Vars → Set Var XYZ…` mostra l'indice **decimale** (0x4000 = 16384):
  la riga sotto dà l'esadecimale, controllala (0x40C2 = 16578). Passo del
  campo: Destra/Sinistra 1 → 10 → 100.
- `Party → Edit Pokemon → Inflict Status1 → Party-wide` dà uno stato a tutta
  la squadra (c'è anche Frostbite); `Give X → Give item XYZ` vuole l'id
  decimale (Ice Heal 46). Il cursore dei menu debug si azzera a ogni apertura.
- La nuova partita: le pagine delle opzioni si scorrono con **R**, l'ultima
  chiede conferma; poi la scena della camera resta ferma ma R+Start apre il
  debug lo stesso.

## Lanci, bersagli e ordine nel frame (overworld-catch fase 3, 17/11)

- **Il salvataggio dell'utente ha `EASY CATCH` acceso**: ogni Ball cattura.
  Per provare un fallimento va spento (`challengeSettings` byte 31, bit
  0x08; `gSaveblock3 + 16 + 31`). Bit di sfida utili, stesso blocco:
  Nuzlocke byte 6 bit 0x04, flag delle zone dal byte 12, `skipNicknamePrompt`
  byte 31 bit 0x10. Si trovano compilando una `const struct` con un solo
  campo a 1 e guardando `.rodata`.
- **Spostare un OWE da GDB**: `currentCoords` e `previousCoords` (+0x10,
  +0x14); la grafica resta dov'era ma la logica usa le coordinate. Specie:
  `graphicsId` (+4) = `0x4000 | specie` (`0x1000` femmina). Fra due comandi
  però cammina: per prove a tempo serve una ROM usa-e-getta con
  `WanderAround_Step4` che torna subito al passo 1.
- **Due eventi nello stesso giro del ciclo**: breakpoint (senza `commands`)
  sulle due funzioni e, dal primo colpo, anche sulla funzione chiamata una
  volta per frame fra le due: se il secondo evento arriva prima di un
  secondo colpo di quella, è lo stesso giro. `vblankCounter1` da solo non
  basta: avanza anche dentro un frame lento.
- **Confrontare una formula prima e dopo un refactor**: nella ROM di prova
  si tiene la versione vecchia (`git show <commit>:file`, rinominata) accanto
  alla nuova e si scrivono i due risultati in EWRAM a ogni chiamata.
- In lotta R lancia `gBallToDisplay`: scriverlo da GDB e premere R evita
  la borsa. Stato di un battler: `gBattleMons[i]` da 136 byte, `hp` +42,
  `status1` +80.
- Un `g.py`/`ram.py` per comando ferma il gioco ogni volta: due pressioni
  "a 0,5 s" possono distare pochi frame di gioco. Per vincolare l'ordine si
  allunga un timer in RAM invece di contare sul tempo.

## Animazioni frame per frame (overworld-catch fase 4, 17/11)

- **Carica l'ELF in GDB** (`file pokehns.elf`) prima di mettere breakpoint:
  senza, GDB non sa che il codice è Thumb e i breakpoint non scattano mai
  (il gioco gira libero e lo script aspetta per sempre).
- **GDB in modo sincrono**: `continue` (non `continue &`) e, per fermarlo,
  `kill -INT $(pgrep -x gdb-multiarch)` invece del comando `interrupt`.
  Mescolare comandi asincroni da una fifo dà "Cannot execute this command
  while the target is running" e comandi persi.
- **Una foto per frame**: breakpoint su una funzione chiamata una volta a
  frame (`OverworldCatch_Update`) con `commands` che contano i frame,
  fanno `eval "shell import ... %03d.png", $f` e premono i tasti con
  `shell xdotool keydown/keyup` a frame precisi; `continue` dentro i
  `commands` finché il contatore non arriva. 240 frame in ~20 s.
- **Leggere lo stato di uno sprite**: `gSprites` (da `nm`), struct da 0x44,
  `animNum` a +0x2A, `animPaused` = bit 6 di +0x2C. È così che si è visto che
  la posa veniva sovrascritta a ogni frame, cosa che a occhio non si vedeva.
- **Prove in campo aperto**: gli OWE aggressivi già allertati caricano il
  giocatore fra un comando e l'altro e fanno partire una lotta (durante la
  quale `OverworldCatch_Update` non gira). Conviene un punto con pochi
  inseguitori (Route 43 del salvataggio dell'utente, di giorno) e ricordarsi
  in che modalità (Ball/Pokémon) è rimasto L.

## Partita nuova veloce e ROM con trucchi (overworld-catch fase 5, 17/11)

- Toolchain assente in un container nuovo: `apt-get update` e poi
  `gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi libpng-dev`;
  senza `update` i pacchetti dell'emulatore non si trovano.
- L'intro di una partita nuova costa ~4 minuti di `xdotool`: una volta in
  overworld si salva in gioco (Start → Start nel menu a strisce → Sì) e le build
  successive ripartono da Continue (il `.sav` accanto alla ROM).
- **ROM con trucchi**: una `Cheat()` chiamata da `OverworldCatch_Update` che
  legge una variabile `volatile u8` (indirizzo da `nm`, in IWRAM) scritta da
  GDB (`interrupt`, `set var *(unsigned char*)0x... = N`, `continue &`) dà
  flag, Pokémon e Ball, o li toglie. Si compila e si ripristina il sorgente.
- Il tema scuro scritto in RAM (`gSaveblock3 + 16 + 31`, bit 0x04) non si
  vede finché le pal 14/15 non si ricaricano: la prima schermata dopo è chiara.

## Un client RAM senza GDB e passi contati (overworld-catch fase 9, 20/11)

- **Al posto di `gdb-multiarch`**: un demone Python tiene l'unica connessione
  allo stub di `mgba -g` (porta 2345; `+` per ogni pacchetto, `c` per
  ripartire, byte `0x03` per fermare, `m addr,len` / `M addr,len:hex`) e
  serve le richieste su un socket unix. Ogni lettura ferma e riparte il
  gioco da sé. **Letture oltre ~256 byte danno `E06`**: spezzarle; le
  risposte possono essere compresse (`X*N` = X ripetuto N-29 volte).
  Avviarlo **dopo** che mgba ascolta (connection refused se troppo presto).
- **Camminare di una casella**: `keydown`, attendere che `currentCoords`
  (+0x10) cambi, `keyup`. Con AUTORUN acceso un tocco di 0,3 s fa 2-4
  caselle; il primo tocco in una direzione nuova può solo girare.
- **Il Mart di Cherrygrove** a inizio storia vende solo Potion e Antidote:
  la lista per medaglie si prova a Violet (gruppo 3, mappa 1).
- Una prova che "tiene L per guardare il pannello" e lo rilascia senza
  scorrere **cambia modalità** Ball/Pokémon: tenerne il conto.
- **Spostare un OWE accanto al giocatore fa partire lotte** (i cacciatori ti
  toccano): per provare la collisione con la Ball a terra si scrive un
  record di `sCatch.throws` (48 byte: stato a 0, timer a 1, `spriteId` 64 a
  12, `landX/Y` a 34, bit `landBlocks` a 38) in un posto senza OWE.

## Screenshot del README (`docs/screenshots/`, 23/11)

Fatti da una nuova partita con il debug, senza `.srm`: nove PNG a 2x (`convert
-filter point -resize 200%`), mostrati a 260 px di larghezza nel README.
- Un clone senza tag disegna la striscia "PLUS.1" del PNG sotto PRESS START:
  per il titolo, `make hns HNS_VERSION=v<ultima release>` (rifà solo il foglio).
- Squadra: `Give X → Pokémon (Basic)` (specie e livello con Destra = passo
  ×10, Su = +1); Ball: `PC/Bag → Fill… → Fill Pocket Poké Balls` (999 di
  ognuna, la prima è la Strange Ball: L + Destra per cambiarla).
- `Utilities → Cheat start` è quello di Emerald: mette le otto medaglie (il quest
  book di Johto risulta finito) e i flag di Hoenn; va bene per le foto, non per i test.
- Il cambio d'ora del debug vuole il Fake RTC: per la luce del giorno si salva e si
  riapre `mgba` con `faketime -f "@AAAA-MM-GG 12:30:00"`.
- Erba con OWE vicina a un warp: Route 30, warp 0 (gruppo 0, mappa 12), due passi a
  destra. Il nord della Route 29 (warp 0) è solo alberi.
- PRESS START lampeggia: più scatti a 0,4 s e si tiene quello con la scritta.
