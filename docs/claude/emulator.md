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
accetta una sola connessione e non fa detach. Serve un `gdb-multiarch`
sempre aperto dietro una fifo; ricetta e trappole in
`.claude/rules/overworld_encounters.md` ("GDB su mGBA, come funziona
davvero").

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
