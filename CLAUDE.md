# Note per Claude — `pokehns-expansion-plus`

Fork personale di `pokemonHnS-expansion` (base: pokeemerald-expansion 1.15.2) in cui
si portano feature da **Soulgold** (`/home/user/soulgold`, se presente nella sessione).

---

## 1. Build e consegna

```sh
make hns -j$(nproc) -O          # NON `make modern`: quello è l'altro repo
zip -9 pokemonHnS_<descrizione>_<sha>.zip pokehns.gba
```

**Consegna sempre la ROM completa da 32 MB compressa.** Niente ROM tagliate: il
padding lo toglie la compressione, non il file. Zippata sta intorno ai 18 MB.

Prima di ogni build, se hai toccato direttive del preprocessore:

```sh
python3 /tmp/ppcheck.py <file...>   # controlla il bilanciamento di #if/#endif
```

Uno `#if` non chiuso si manifesta come `'X' used but never defined` su funzioni
che non c'entrano nulla. È già successo.

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

Nella scratchpad ci sono gli script usati: `png.py` (lettura PNG indicizzati),
`render.py` (palette `.gbapal` + writer PNG), `gbapal.py`, `plte.py`.

### Trappola ricorrente: una entry, due significati

Succede di continuo. Esempi reali trovati in questo repo:

- palette del popup abilità: entry 7 è **sia** il pannello del popup **sia** la R
  del tab della ball → risolto spostando i pixel del tab su una entry libera (9);
- entry 5: sul tab MOVE INFO è la didascalia (va chiara), sul tab della ball è la
  lettera R (va scura) → il remap va fatto **per foglio**, non globale;
- barra HP entry 2: binario della barra (scuro) **e** metà bianca della ball di
  cattura (chiara) → remap solo su quell'elemento, con entry di destinazione
  **diversa per skin** (9 su gen4, 1 su gen3).

Prima di scurire/schiarire una entry, controlla **tutti** i disegni che la usano.

---

## 3. Emulatore (verifica vera)

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

---

## 4. Salvataggi

`struct ChallengeSettings` in `include/global.h` è **fissata a 32 byte** da
`STATIC_ASSERT(..., ChallengeSettingsLayoutPinned)` in `src/save.c`.

- Aggiungi campi **solo in fondo**, così nessun campo esistente cambia offset.
- Bit liberi nell'ultimo byte: dopo `noWildEncounters` ne restano **2**.
- Dopo ogni aggiunta verifica che compili: l'assert fallisce da sola se sfori.

**Polarità dei bit.** Un salvataggio scritto prima che l'opzione esistesse legge il
bit a **zero**. Quindi se il default deve essere "come prima", memorizza il campo in
modo che zero significhi il comportamento vecchio — anche se il nome viene brutto.
`skipNicknamePrompt` è memorizzato invertito proprio per questo, e la lista di scelte
è ordinata di conseguenza (`sChoices_OnOff` = ON per primo, `sChoices_OffOn` = OFF
per primo). La regola in una riga: **l'etichetta del valore zero deve venire prima**.
`noWildEncounters` segue la stessa logica (0 = incontri attivi = "ON").

Le opzioni impostate prima di iniziare una partita passano da
`src/oak_speech_hns.c`, che azzera tutta la struct e poi ricopia a mano i campi
del menu opzioni. **Ogni campo nuovo va aggiunto anche a quella lista**, altrimenti
la scelta fatta dal titolo viene persa in silenzio.

---

## 5. Mappa del tema scuro

Per non ri-scoprirla. Accessore unico: `IsDarkUiEnabled()` (`include/option_menu.h`).

| Cosa | Dove | Note |
| --- | --- | --- |
| Box messaggio, prompt azione | BG pal 0, entry 15 (fondo), 6 (ombra), 7/9–14 (bordo) | `ApplyDarkBattleUiPalettes()` in `battle_bg.c` |
| Box comandi, mosse, sì/no | BG pal 1 entry 14 (interno cornice) + BG pal 5 entry 8 | tutte e 20 le cornici in `graphics/text_window/` hanno l'interno sulla entry 14, bianca |
| Testo dei box comandi | BG pal 5: fg 14, ombra 13, fondo 8 | entry 8/9/10 di `text.gbapal` sono nere e inutilizzate |
| Cursore ▶ | BG pal 13 (`BATTLE_COMMAND_PAL_NUM`) | i tile 1 e 2 del textbox hanno lo sfondo sulla entry 1, che deve restare bianca per il testo |
| Healthbox / barra HP / shiny | palette sprite in `battle_gfx_sfx_util.c` | 4 palette per skin: normale, shiny, scura, shiny scura |
| Popup abilità + tab R e MOVE INFO | `sDarkAbilityPopUpPalette*` in `battle_interface.c` | palette condivisa fra tre disegni: attenzione |
| Simboli di efficacia accanto ai PP | stringhe in `MoveSelectionDisplayMoveEffectiveness` (`battle_controller_player.c`) | hanno il colore incorporato: solo l'highlight va da 14 a 8 |

Tutti i punti che ricaricano `gBattleTextboxPalette` su BG pal 0 chiamano subito dopo
`LoadBattleMenuWindowGfx()`, quindi gli override vivono solo lì dentro.

**Differenza voluta rispetto a Soulgold:** SG tiene chiara la healthbox shiny anche
in tema scuro e per farlo riscrive i pixel di ogni sprite della barra HP. Qui vanno
scuri tutti i box insieme, così la palette della barra è uno scambio secco e nessuno
sprite ha stato per-sprite che possa restare sbagliato dopo un cambio.

**Attenzione alle stringhe con i colori incorporati.** `gText_BattleYesNoChoice` e
`gText_BattleSwitchWhich` avevano `{BACKGROUND}`/`{TEXT_COLORS}` dentro il testo, che
scavalcano il colore scelto da `BattlePutTextOnWindow`. In Soulgold quelle stringhe
sono testo puro. Se una finestra resta chiara pur essendo nella lista di
`IsDarkBattleCommandWindow`, guarda prima la stringa.

---

## 6. Prima di portare qualcosa: controlla se c'è già

Vale più di qualunque stima. Cose date per mancanti che invece c'erano, spente:

- **DexNav**: `src/dexnav.c` (2691 righe), `include/config/dexnav.h`, le 12 grafiche,
  `data/scripts/dexnav.inc` e la voce `MENU_ACTION_DEXNAV` nel menu Start ci sono
  già. `DEXNAV_ENABLED` è `FALSE` e i cinque flag/var sono a 0.
- **Ricorda mosse dal riassunto**: è una feature di expansion 1.15.2
  (`P_ENABLE_MOVE_RELEARNERS`), non di Soulgold. Hasep ne aveva però solo metà:
  `ShowRelearnPrompt()` disegnava il prompt e **nessuno gestiva START**.
  Come lo configura Soulgold, verificato: `P_TM_MOVES_RELEARNER` **FALSE** (MT
  mai disponibili), mosse uovo dietro `FLAG_EGG_MOVES_UNLOCKED` (Egg Move Master,
  Blackthorn City House 3, ¥88.888) e tutor dietro `FLAG_TUTOR_MOVES_UNLOCKED`
  (Tutor Move Master, Olivine City House 4, ¥44.444). È per questo che lì un
  Pokémon di livello basso non vede mosse fuori scala: qui quei due cancelli non
  ci sono per scelta.
  Soulgold apre il relearner **solo dalla pagina Battle Moves** e forza
  `showContestInfo` a FALSE.
- **Icone dei tipi in lotta**: `src/type_icons.c` c'è, `B_SHOW_TYPES` era
  `SHOW_TYPES_NEVER`.
- **Mente**: tutte e 21 già in vendita al negozio di fiori di Goldenrod, dietro
  medaglia 3 e dietro il toggle `MODE_MINTS` del challenge menu.
- **`swsh_party_menu.c`** si è portato dietro roba di Soulgold mai agganciata
  (`Task_ShinGenome` era già lì). Controlla con
  `nm --defined-only build/hns/src/swsh_party_menu.o` prima di scrivere un doppione:
  un simbolo senza prefisso di variante è una collisione di link che aspetta.

Corollario sulle coordinate: quando una feature è sempre stata spenta, i suoi
dati posizionali sono i default di expansion, non valori tarati su questo repo.
`sTypeIconPositions` era così.

**Non si ricavano per differenza dalle coordinate di Soulgold.** Ci ho provato e
ho sbagliato due volte. La healthbox è creata con subpriority **1**, le icone dei
tipi con **255**: tutto ciò che si sovrappone alla box viene disegnato *dietro* e
non si vede mai. L'arte della healthbox di Soulgold è più stretta di quella di
Hasep, quindi il suo x=93 qui cade dentro la box (bordo destro misurato a
**x=106**: a 103 c'è ancora il contorno bianco, a 110 c'è già lo sfondo). Il
valore giusto si **misura campionando i pixel di un frame vero**, non si deriva.

Trappola di metodo, costata un'ora: avevo "verificato" che la geometria dei
sotto-sprite fosse identica fra i due repo con
`diff <(awk "/nome/,/^};/" a.c) <(awk "/nome/,/^};/" b.c)`. Il nome non esisteva
in **nessuno** dei due file, quindi awk non stampava nulla da entrambe le parti e
il diff tornava vuoto: l'ho letto come "identici". **Un diff vuoto fra due estratti
vuoti non è una verifica.** Controlla sempre che l'estratto non sia vuoto.

---

## 7. Altre cose da sapere

- **Menu squadra SwSh**: due varianti compilate insieme. `tools/gen_party_menu_variant.py`
  rigenera `include/party_menu_variant.h` e `src/party_menu_dispatch.c`. Lo script è
  idempotente (toglie i prefissi prima di riapplicarli) e verifica che ogni
  sostituzione compaia esattamente una volta.
- **Negozi**: tutti i Poké Mart normali usano `pokemart 0`, cioè un inventario unico
  scalato sui medaglieri (`sShopInventories` in `src/shop.c`). Per vendere qualcosa
  in una sola città si **appendono** extra a quella lista (`sMartExtras`), non si dà
  una lista fissa alla città. Nessun tetto al numero di oggetti: `MAX_ITEMS_SHOWN 8`
  è solo quante righe si vedono.
- **Nickname**: la domanda dei regali sta nei singoli script di mappa. È dietro la
  macro `asknickname` (`asm/macros/event.inc`) che, con l'opzione spenta, non mostra
  niente e lascia `VAR_RESULT` a NO. I due prompt del *valutatore di nickname* usano
  un testo proprio e non vanno toccati.
- **VBlank**: aprire una schermata senza installare il proprio `SetVBlankCallback`
  eredita quello del chiamante. È così che il Pokédex aperto dal menu SwSh scorreva
  in diagonale.

---

## 8. Rotture preesistenti (non tue)

Verificate su `master` pulito, **prima** di qualunque modifica:

- `make all`, `make firered`, `make leafgreen` falliscono su `src/mom_savings.c`;
- `make check` fallisce in entrambe le varianti; `test/save.c` fissa dimensioni stale;
- la CI **non costruisce mai** il target `hns`, quindi oggi non protegge da nulla.

Se qualcuno chiede di sistemare la CI, il lavoro utile è restringerla a `hns`.

---

## 9. Stile

- Commenti e messaggi di commit **in inglese**, conversazione con l'utente in italiano.
- Commenti che spiegano **perché**, non cosa: soprattutto quando un valore è stato
  derivato, di' da dove. Nelle palette è d'obbligo tenere il valore originale a
  fianco (`// was RGB(...)`).
- Commit descrittivi: cosa è rotto, perché, come si è verificato.
- Branch di sviluppo: quello assegnato dalla sessione. Non pushare altrove senza permesso.
