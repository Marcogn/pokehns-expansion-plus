# Note per Claude — `pokehns-expansion-plus`

Fork personale di `pokemonHnS-expansion` (base: pokeemerald-expansion 1.15.2) in cui
si portano feature da **Soulgold** (`/home/user/soulgold`, se presente nella sessione).

**Come sono organizzate queste note.** Qui c'è solo ciò che serve in ogni
sessione. Il dettaglio di ogni area (trappole, misure, verifiche fatte) sta in
`.claude/rules/`: ogni file ha un campo `paths:` e Claude Code lo carica da solo
quando leggi o modifichi un file corrispondente. `grep` non lo fa scattare:
se lavori su un'area senza aprirne i file, leggi la regola a mano (indice al §7).
Una nota nuova su un'area va nella sua regola, non qui.

---

## 1. Build e consegna

```sh
make hns -j$(nproc) -O          # NON `make modern`: quello è l'altro repo
zip -9 pokemonHnS_<descrizione>_<sha>.zip pokehns.gba
```

**Consegna sempre la ROM completa da 32 MB compressa.** Niente ROM tagliate: il
padding lo toglie la compressione, non il file. Zippata sta intorno ai 18 MB.

**ROM in chat, patch nel repo.** All'utente si consegna la ROM zippata; quello
che si pubblica è solo la patch BPS, sulle release di GitHub (mai nel git: è
grande quanto la ROM).

```sh
tools/make_patch.sh                     # pokehns.gba -> pokehns.bps
```

La release la fa `.github/workflows/release.yml`: **Actions → Release → Run
workflow**, si sceglie il branch e si scrive la versione (`2.0.6.PLUS.2` →
tag `v2.0.6.PLUS.2`, creato dalla release stessa sul commit compilato). Il
formato scelto dall'utente è quello del titolo, maiuscolo; la prima release è
uscita come `v2.0.6.plus.1` (minuscolo) prima di questa scelta e resta così.
Il numero sul title screen segue la versione da solo: la release la passa a
`make hns HNS_VERSION=...`, che la disegna nelle tile
(`.claude/rules/title_screen.md`). In alternativa basta pushare un tag `v*`.
Il pulsante compare solo quando il file è sul branch di default.

**Ogni port o feature va annotato in `CHANGELOG.md`** sotto la prossima
versione, a titoletti: senza la sezione `## <versione>` la release si ferma
prima della build. Patch BPS, base Emerald e note di release:
`.claude/rules/release.md`.

Uno `#if` non chiuso si manifesta come `'X' used but never defined` su funzioni
che non c'entrano nulla. È già successo: se hai toccato direttive del
preprocessore, controlla il bilanciamento di `#if`/`#endif` prima di compilare.

---

## 2. Regola numero uno: non copiare i valori di Soulgold

Le palette di Soulgold sono tarate sulla **sua** grafica. Copiare un valore su una
grafica diversa quasi sempre finisce su una entry che lì significa un'altra cosa.
Questa è la lezione che è costata più tempo, ripetutamente.

**Deriva sempre dai file di questo repo**, e verifica *prima* di scrivere codice:

- conta gli indici usati davvero dai `.4bpp` (sono i dati che finiscono in ROM,
  i `.png` hanno palette di authoring che possono differire — succede per
  `hns/healthbox_*.png`);
- renderizza offline tile + tilemap + `.gbapal` veri e **guarda l'immagine**;
- solo allora scegli i valori.

Una stessa entry di palette spesso serve a due disegni diversi: prima di
scurirla o schiarirla controlla **tutti** quelli che la usano (esempi reali in
`.claude/rules/battle_ui.md`).

---

## 3. Emulatore (verifica vera)

mGBA gira headless e si pilota: è così che sono stati trovati i bug che
l'analisi statica non vedeva. **Guida completa in `docs/claude/emulator.md`**
(tasti, input, screenshot, menu debug, GDB per leggere/scrivere la RAM, ora
finta, nuova partita): leggila prima della prima verifica della sessione.

```sh
apt-get install -y --no-install-recommends mgba-sdl xvfb xdotool imagemagick
setsid nohup Xvfb :99 -screen 0 800x600x24 &
DISPLAY=:99 SDL_AUDIODRIVER=dummy setsid nohup /usr/games/mgba pokehns.gba &
```

- Dopo **ogni** passaggio di menu fai uno screenshot e guardalo.
- **Misura i pixel invece di fidarti dell'occhio.**
- Quando l'analisi statica non conclude, una **ROM strumentata** usa-e-getta
  (condizione forzata, posizione forzata) chiude il problema in due build.
- Il salvataggio dell'utente è un `.srm`: copialo come `pokehns.sav` accanto alla ROM.

---

## 4. Salvataggi (sintesi)

Dettaglio, offset e storia delle migrazioni: `.claude/rules/save.md`, caricata
quando tocchi `save.c`, `global.h`, `new_game.c`, `oak_speech_hns.c` o il menu
opzioni.

- `struct ChallengeSettings` è fissata a 32 byte e ha **1 bit libero**
  (`unusedTrainerCatchBit`, già azzerato dalla migrazione 9: riusabile così
  com'è); `soundAndFieldOptions` è pieno (restano 2 bit ignorati, da azzerare
  con una migrazione prima di riusarli). Dopo quel bit, una nuova opzione va
  in un flag, oppure in un campo nuovo in coda a SaveBlock3 (vedi sotto).
- **Polarità**: un salvataggio vecchio legge zero, quindi zero deve essere il
  comportamento di prima (o il default). L'etichetta del valore zero viene prima
  nella lista di scelte, anche se il nome del campo viene brutto.
- Un campo nuovo in SaveBlock3 vuole tre cose: in **fondo** alla struct, un
  `STATIC_ASSERT` sul suo offset in `save.c`, un passo `saveVersion < N` in
  `LoadGameSave()`.
- Le opzioni scelte dal titolo passano da `oak_speech_hns.c` e sopravvivono a
  `ClearSav3()` in `NewGameInitData`: un campo nuovo va aggiunto in **entrambi**,
  o la scelta sparisce in silenzio.
- **`NewGameInitData` azzera SaveBlock1 e 3, non SaveBlock2**: un campo di
  gioco messo in SaveBlock2 va azzerato a mano (come i flag del TcloneR in
  `ResetPokedex`), o una nuova partita sopra un salvataggio caricato lo eredita.

---

## 5. Tema scuro

Mappa completa (quale palette, quale entry, quali stringhe hanno colori
incorporati) in `.claude/rules/battle_ui.md`. Accessore unico:
`IsDarkUiEnabled()` (`include/option_menu.h`).

---

## 6. Prima di portare qualcosa: controlla se c'è già

**Il report aggiornato di cosa è arrivato da Soulgold e cosa no è
`docs/SOULGOLD_PORT_REPORT.md`**: tienilo in pari a ogni port. Il codice di SG
portato è allineato a **SG 1.1.4** più i fix del §4 di quel report e le tre
feature di SG 1.2 (Fast HMs, tema musica di lotta, musica notturna nei Centri). Un nuovo
giro di sync (HnS e Soulgold insieme) si fa con la skill
`.claude/skills/upstream-sync/` (`/upstream-sync`): lo script
`upstream_status.py` fa fetch e classificazione, e il punto da cui ripartire
per Soulgold sta in `state.json`, non più in uno SHA scritto qui. La maggior
parte di quello che esce è contenuto di SG (squadre, learnset, tratti), non da
portare.

Vale più di qualunque stima: DexNav, icone dei tipi, ricorda mosse e DexNav su
R erano già nel codice, **spenti**. In questo repo un `#if IS_HNS / #else` non è
un dettaglio di piattaforma: spesso è il posto in cui una feature è spenta
senza che nessun flag lo dica.

### Lezioni che valgono ovunque

Ognuna è costata ore; il racconto completo sta nella regola indicata.

- **Porti un file da Soulgold? Confronta le firme delle funzioni che chiama.**
  Un argomento in meno compila, una feature in meno no (`party_menu.md`:
  caramelle applicate due volte).
- **Prima di scrivere un simbolo, guarda se c'è già**:
  `nm --defined-only build/hns/src/<file>.o`. Un simbolo senza prefisso di
  variante è una collisione di link che aspetta.
- **"Rendilo come Soulgold" può cancellare feature**: la divergenza può andare
  nei due sensi (`dexnav.md`). Classifica prima.
- **Non dire "è identico in SG" senza averlo aperto** (`dexnav.md`, grotte).
- **Un diff vuoto fra due estratti vuoti non è una verifica**: controlla che
  l'estratto non sia vuoto (`battle_ui.md`).
- **Leggi la segnalazione dell'utente come una misura, non come un sintomo**
  (`battle_ui.md`, icone dei tipi).
- **Verifica con un caso che fallisce senza la modifica**: una specie già vista
  "confermava" un gate sul Pokédex che non c'era (`dexnav.md`).
- **Verifica le feature che tocchi, non solo quelle che rompi, e guarda il
  prodotto**: il Pokémon catturato era un Bad Egg perché nessuno aveva aperto la
  squadra (`trainer_catch.md`).
- **Un `out of memory` con un numero preciso va tradotto in un file**: `ls -l`
  sui `.4bpp` dice quale grafica sta fallendo (`pokedex.md`).
- **Strumentare, non dedurre**: una build che congela sul caso sospetto separa
  due ipotesi in un colpo.
- Una feature sempre spenta ha dati posizionali di default di expansion, non
  tarati su questo repo.
- Una schermata che non installa il suo `SetVBlankCallback` eredita quello del
  chiamante.
- **`LoadPalette` a schermo nero lo riaccende**: scrive anche il buffer
  sfumato, quindi quei colori tornano pieni finché qualcuno non risfuma. Mai
  caricare palette fra una dissolvenza al nero e la schermata dopo
  (`strip_menu.md`: un fotogramma di tile colorate aprendo il quest book).
  Un lampo di un fotogramma si cattura con 40 `import` di fila subito dopo il
  tasto, confrontando la luminosità media.
- **`make hns` non è una build RELEASE: un `assertf` che fallisce mostra al
  giocatore una schermata blu.** Va bene solo per errori di programma, mai per
  stati che il gioco raggiunge (VRAM piena, tabella oggetti piena)
  (`overworld_encounters.md`).
- Con `OW_GFX_COMPRESS` ogni oggetto tiene in VRAM tutto il suo foglio: una
  mappa con molti Pokémon-oggetto più gli OWE può lasciare 10 tile libere su
  1024, e uno sprite che non si crea non deve lasciare uno script in attesa.

---

## 7. Indice delle regole per area

In `.claude/rules/`, caricate da sole quando apri i file indicati.

| Regola | Area |
| --- | --- |
| `release.md` | patch BPS, base Emerald, note di release |
| `save.md` | `ChallengeSettings`, `soundAndFieldOptions`, polarità, migrazioni |
| `battle_ui.md` | tema scuro in lotta, palette condivise, icone dei tipi |
| `dexnav.md` | DexNav: divergenze con SG, grotte, Flash, creeping, schermata |
| `relearner.md` | ricorda mosse, learnset gen 3 / gen 7 |
| `party_menu.md` | varianti SwSh/classico, follower, oggetti dal menu SwSh |
| `candy_jar.md` | Candy Jar |
| `field.md` | script MN `_hns`, negozi, nickname, mente |
| `music.md` | riverbero m4a |
| `bag.md` | icone e badge nella lista della borsa |
| `pokedex.md` | aprire il Pokédex da una lotta: limiti di memoria misurati |
| `trainer_catch.md` | cattura dei Pokémon degli allenatori: TcloneR, contatore, premi |
| `megas.md` | spazio in ROM delle megaevoluzioni |
| `title_screen.md` | "+" e versione sulla schermata del titolo |
| `pc_box.md` | `SORT` e `SEARCH` nei box del PC |
| `overworld_encounters.md` | Pokémon visibili nell'overworld: port, indoli, tetti, VRAM (piano in `docs/plans/`) |
| `level_scaling.md` | level scaling di allenatori, selvatici (con tetto e margine) e lotte di script; soldi |
| `quest_book.md` | quest book: schermata, testi, immagini, missioni di prova |
| `strip_menu.md` | menu di pausa a striscia: quando c'è, tasti, salvataggio, VRAM, Flash |
| `reds_path.md` | Red's Path: capi, squadre di Red, agganci di lotta, aiutanti, scena e premi nelle palestre |

**Skill del repo** (`.claude/skills/`, si chiamano con `/<nome>`):
`upstream-sync`, `next-phase` (prossima fase di un piano, e se serve il
`.srm`), `new-plan` (piano a fasi con modello per fase), `add-quest`,
`doc-sync` (documentazione in pari con l'ultimo commit), `new-issue`,
`pr-review` (review di una PR pronta, con i controlli di questo repo).

Fuori dalle regole, da leggere quando serve: `docs/claude/emulator.md`,
`docs/SOULGOLD_PORT_REPORT.md`. I piani in `docs/plans/` contengono le
decisioni; **`docs/analysis/` è l'archivio delle ricerche da cui vengono**:
non serve per implementare, aprilo solo se il piano rimanda lì o se una
scelta va rimessa in discussione.

---

## 8. Rotture preesistenti (non tue)

Verificate su `master` pulito, **prima** di qualunque modifica:

- `make all`, `make firered`, `make leafgreen` falliscono su `src/mom_savings.c`;
- `make check` fallisce in entrambe le varianti; `test/save.c` fissa dimensioni stale.

La CI ereditata costruiva proprio quei target, e mai `hns`: era rossa su
`master` e non proteggeva nulla. Ora `.github/workflows/build.yml` costruisce
**solo** `make hns` e scrive nel riepilogo quanto dei 32 MB è usato (la parte
prima del padding finale a `0xFF`): **avviso dal 98%, errore dal 99,5%**.
Le mega da sole porterebbero al 99,46% (`.claude/rules/megas.md`), quindi
avvisano ma passano. Il job `build` resta come cancello per
eventuali regole di protezione del branch. Se un giorno servono di nuovo gli
altri target, vanno prima riparati, non solo riaccesi.

La CI compila con `UNUSED_ERROR=0`, come la ROM consegnata. Quello ereditato
da expansion era 1, e con 1 `make hns` fallisce su **40** funzioni e variabili
mai usate in 15 file, tutte di HnS spente dietro `IS_HNS`: un errore su codice
che nessuno chiama, non su qualcosa che una modifica ha rotto.

---

## 9. Stile

- Commenti e messaggi di commit **in inglese**, conversazione con l'utente in italiano.
- Commenti che spiegano **perché**, non cosa: soprattutto quando un valore è stato
  derivato, di' da dove. Nelle palette è d'obbligo tenere il valore originale a
  fianco (`// was RGB(...)`).
- Commit descrittivi: cosa è rotto, perché, come si è verificato.
- Branch di sviluppo: quello assegnato dalla sessione. Non pushare altrove senza permesso.
