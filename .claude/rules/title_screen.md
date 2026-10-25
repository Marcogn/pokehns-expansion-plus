---
paths:
  - "src/title_screen.c"
  - "graphics/title_screen/**"
  - "tools/version_strip.py"
  - "tools/version_header.py"
  - "graphics_file_rules.mk"
---

# Schermata del titolo: il "+" e la versione

La scritta "HEART & SOUL" è uno sprite 8bpp da 64×64
(`graphics/title_screen/hns/emerald_version.png`), usato come due sprite da
64×32 affiancati: metà sinistra = righe 0-31, metà destra = righe 32-63.
Indici: 0 trasparente, 4 nero (bordo), 7 grigio scuro (fondo), 15 bianco
(lettere); testo sulle righe 43-53 della metà destra, punta della pillola
alla colonna 29.

Il "+" sta **fuori** dalla pillola, che resta identica all'originale. È una
scelta dell'utente: dentro l'ovale, sia "+" sia "PLUS" nel font del banner
non stavano bene. Nero (indice 4), 9×9 a tratto 3, contorno bianco di 1 px
(indice 15, sugli 8 vicini): 11×11, righe 43-53, colonne 32-42, cioè 2
colonne trasparenti dopo la punta.

**Centrato sul logo, non sullo schermo.** HnS allinea titolo e versione al
logo: misurato in mGBA, logo (giallo + contorno blu) x 42-190, centro 116.
Pillola + "+" senza spostamento: x 76-172, centro 124; per questo le due X
in `src/title_screen.c` scendono di **8** (misurato dopo: x 68-164, centro
116). Se si cambia la larghezza del "+", si rifà questa misura.

**Versione sotto PRESS START** (`press_start.png`, stesso file del testo
PRESS START): è una striscia di 20 tile = 160×8 px, disposta nel foglio come
riga 1 x40-127 seguita da riga 2 x0-71 (sono 5 sprite 32×8, tile 21-40).
Una colonna x della striscia è la x 48+x a schermo. Il font ha **solo
maiuscole e cifre**: per questo la versione è `v2.0.6.PLUS.1` e non "plus".
Testo centrato su 116 come il logo: x 79-153.

**La striscia si genera in build** (`tools/version_strip.py`, regola in
`graphics_file_rules.mk`): gbagfx fa il `.4bpp` dal PNG e lo script ci
ridisegna sopra la striscia, senza Pillow. La versione è `HNS_VERSION`; di
default l'ultimo tag `v*` (`git describe`), e la release passa quella che sta
per taggare. Senza né l'una né l'altro (clone senza tag, come la CI di build)
resta la striscia disegnata nel PNG. Un file `build/hns_version.txt` cambia
solo quando cambia la versione, ed è quello che fa rifare il foglio. Le
minuscole vengono rese maiuscole; un carattere senza glifo ferma la build.

Glifi, tutti nella tabella dello script: v, punto, 0 1 2 6, P L U S copiati
dalla striscia fatta a mano (rigenerare `v2.0.6.PLUS.1` la riproduce **pixel
per pixel**: è il test da rifare se si tocca la tabella); 3 4 5 7 8 9
disegnati poi nello stesso schema (7 righe: contorno, corpo colorato per
riga 5, 5, 4, 3, 3 con il 2 sugli angoli arrotondati, contorno). Glifi
adiacenti condividono la colonna di contorno; il punto ha la sua.
Spazio: 160 px di striscia, ma centrata su 116 ne stanno **137**. Oltre, lo
script la sposta per restare dentro (verificato in mGBA con
`1234.5678.9.PLUS.PLUS`, 147 px, x 48-194); oltre 160 si ferma.
R, E, T e A di PRESS START non sono nella tabella: T e A hanno larghezze
irregolari, e finora non servivano.

**La stessa versione nel menu di salvataggio** (issue #39; l'issue diceva
menu Start, l'utente ha cambiato idea): `tools/version_header.py` scrive
`src/data/hns_version.h` (generato, in `.gitignore`) con `HNS_VERSION_TEXT`,
normalizzato come la striscia del titolo (`v` minuscola, resto maiuscolo).
Senza versione scrive **`dev build`**: il titolo invece tiene la striscia
del PNG (`v2.0.6.PLUS.1`), quindi in un clone senza tag i due non
coincidono; con `HNS_VERSION` (la release) sì. Lo riscrive solo se il testo
cambia. `scaninc` non elenca un header che ancora non esiste: per questo
`start_menu.o` lo nomina a mano in `graphics_file_rules.mk`.
`ShowSaveInfoWindow` (`start_menu.c`): finestra alta 12 tile invece di 10
(finisce al baseBlock 176, prima del Sì/No a 0x125; cornice fino alla riga
13, il riquadro dei dialoghi parte dalla 14). Il numero è nel font stretto:
nel normale `v2.0.6.PLUS.7` lasciava 5 px all'etichetta. Se non entra
comunque, l'etichetta "VERSION" sparisce e resta il numero; oltre 20
caratteri la build si ferma. Verificato in mGBA con `v2.0.6.PLUS.7`,
`v2.0.6.PLUS.10`, `v12.34.56.PLUS.99` (senza etichetta) e `dev build`, con
e senza Pokédex, e un salvataggio completo.
