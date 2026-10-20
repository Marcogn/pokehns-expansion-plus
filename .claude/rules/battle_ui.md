---
paths:
  - "src/battle_*.c"
  - "include/battle_*.h"
  - "src/type_icons.c"
  - "include/type_icons.h"
  - "graphics/battle_interface/**"
  - "graphics/types/**"
  - "graphics/text_window/**"
---

# UI di lotta: palette, tema scuro, icone dei tipi

## Una entry, due significati (esempi della regola del §2 di CLAUDE.md)

Succede di continuo. Esempi reali trovati in questo repo:

- palette del popup abilità: entry 7 è **sia** il pannello del popup **sia** la R
  del tab della ball → risolto spostando i pixel del tab su una entry libera (9);
- entry 5: sul tab MOVE INFO è la didascalia (va chiara), sul tab della ball è la
  lettera R (va scura) → il remap va fatto **per foglio**, non globale;
- barra HP entry 2: binario della barra (scuro) **e** metà bianca della ball di
  cattura (chiara) → remap solo su quell'elemento, con entry di destinazione
  **diversa per skin** (9 su gen4, 1 su gen3).

Prima di scurire/schiarire una entry, controlla **tutti** i disegni che la usano.

## Mappa del tema scuro

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

## Icone dei tipi in lotta

- **Icone dei tipi in lotta**: `src/type_icons.c` c'è, `B_SHOW_TYPES` era
  `SHOW_TYPES_NEVER`.

Corollario sulle coordinate: quando una feature è sempre stata spenta, i suoi
dati posizionali sono i default di expansion, non valori tarati su questo repo.
`sTypeIconPositions` era così.

La healthbox è creata con subpriority **1**, le icone dei tipi con **255**:
tutto ciò che si sovrappone alla box viene disegnato *dietro* e non si vede mai.

**Errore mio da non ripetere.** Dalla segnalazione «compare per un secondo e poi
sparisce, esce e rientra dalla hpbox» avevo concluso che la posizione di partenza
(`{20, 26}`, il default di expansion) finisse dentro la box, e avevo spostato le
icone a **destra** della healthbox. Sbagliato: quel default mette l'icona a
**sinistra** della box, dove c'è spazio libero, e «esce e rientra dalla hpbox»
descriveva l'animazione che funzionava. L'unico vero bug era
`tHideIconTimer` che non si azzerava mai. La frase dell'utente conteneva già la
risposta: **leggere la segnalazione come una misura, non come un sintomo**.

**Come si misura davvero.** `import` + `convert -sample 240x160!` dà il
framebuffer 1:1, e `convert ... txt:-` ne stampa i pixel: da lì i bordi si leggono
senza interpretare. In singola il bordo **sinistro** della box è una linea
verticale a **x 12** su tutte le righe, quello destro sta a **x 101** sulla riga
del nome. Restano quindi 12 px liberi a sinistra; lo sprite è largo 8 e la x
memorizzata è il **centro**, quindi riposo 7 = icona su x 3..10.

**Disposizione attuale:** singola a **sinistra** della box (`{17, 26}`, la slide
toglie 10), doppia a **destra** come Soulgold (offset +66 dall'origine della box
su entrambi i lati, identico a SG). La scaletta di 4 px della seconda icona vale
solo dove le icone stanno a destra, quindi è attiva solo in doppia: in singola i
12 px non bastano e 4 px in un verso o nell'altro tagliano il bordo schermo o
nascondono mezzo glifo dietro la box.

**Tre differenze di comportamento (non di coordinate) trovate rispetto a SG:**

- l'arte in `graphics/types/battle_icons*.png` è **identica** a quella di SG ed è
  **asimmetrica**. `ShouldFlipTypeIcon` di expansion sceglieva il lato giocatore
  in singola e quello avversario in doppia — non possono essere giusti entrambi:
  qui le icone avversarie uscivano specchiate in singola e non in doppia. SG
  specchia sempre sul lato avversario, ma **perché SG disegna sempre a destra
  della box**. La regola vera non è "lato avversario", è **"da che parte della
  healthbox sta l'icona"**: qui in singola sta a sinistra, quindi il glifo
  specchiato guardava via dalla box. Ora `ShouldFlipTypeIcon` specchia solo dove
  si usa il layout di SG, cioè in doppia (`UseDoubleBattleCoords`); in singola
  l'arte esce come è disegnata e guarda verso la box. Scelta estetica
  dell'utente, non un bug di SG. Verificato per misura: il frame di un
  Misdreavus a schermo coincide con il tile di `battle_icons1.4bpp` reso
  offline, non con il suo specchio;
- SG indenta di **4 px** la seconda icona di un doppio tipo sul lato avversario
  (`SetTypeIconXY`), la scaletta in stile HGSS. Expansion le impila a filo. Qui
  la scaletta è attiva solo in doppia, per lo spazio (vedi sopra);
- le direzioni di `GetTypeIconSlideMovement` e `GetTypeIconHideMovement` in
  singola sono speculari a quelle di SG, ed è corretto così: qui l'icona sta
  dall'altro lato della box, quindi esce verso sinistra e si ritrae verso destra,
  rientrando sotto la healthbox. Il segno del riposo è `x - 10`, per questo la
  entry vale 17 e non 7. Il ramo doppie è identico a SG.

Per farlo funzionare serve anche `tHorizontalPosition` (`data[4]`) in
`include/type_icons.h`: la slide deve agganciarsi alla x di riposo **del singolo
sprite**, non alla entry di tabella condivisa, altrimenti l'indent di 4 px viene
riassorbito. Oggi `src/type_icons.c` differisce da quello di SG solo per la
tabella delle coordinate e per il `sprite->tHideIconTimer = 0;`.

Trappola di metodo, costata un'ora: avevo "verificato" che la geometria dei
sotto-sprite fosse identica fra i due repo con
`diff <(awk "/nome/,/^};/" a.c) <(awk "/nome/,/^};/" b.c)`. Il nome non esisteva
in **nessuno** dei due file, quindi awk non stampava nulla da entrambe le parti e
il diff tornava vuoto: l'ho letto come "identici". **Un diff vuoto fra due estratti
vuoti non è una verifica.** Controlla sempre che l'estratto non sia vuoto.
