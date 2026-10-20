---
paths:
  - "src/item_menu.c"
  - "src/item_menu_icons.c"
  - "graphics/bag/**"
---

# Borsa: icone nella lista

- **Le icone blittate nella lista della borsa: la palette del tema è la
  trappola.** `WIN_ITEM_LIST` usa **BG pal 1** ed è riempita con `PIXEL_FILL(0)`.
  Quella palette **cambia col tema**, e cambia solo in parte. Misurata da
  `graphics/bag/soulgold/menu_male.gbapal` e `menu_male_dark.gbapal`:

  | entry | chiaro | scuro | |
  | --- | --- | --- | --- |
  | 0 | — | — | trasparente: si vede la borsa dietro |
  | 1 | **nero** | **bianco** | è `TEXT_COLOR_WHITE`, il colore del testo |
  | 2, 3, 6, 7, 8 | chiari | scuri | cambiano |
  | 4, 5, 9, 10, 11, 12, 13, 14, 15 | uguali | uguali | **fissi nei due temi** |

  Quindi: un pixel sull'indice **1** si inverte col tema, uno su **11** o **15**
  no. Due bug distinti, stessa radice:
  - **badge `SEL`** (`graphics/bag/select_button*.png`): i 154 pixel di sfondo
    stavano sull'indice **10**, opaco, e in tema scuro erano un rettangolo
    bianco. In Soulgold stanno sullo **0**. Rimappato 10 → 0: il badge si fonde
    con la lista, e il contorno (indice 1) esce bianco su scuro e nero su
    chiaro, leggibile in entrambi;
  - **badge `HM`** (`graphics/bag/hm.png`): qui il rimappaggio 10 → 0 **non
    basta**, ed è stato il mio errore: quei 36 pixel sono solo gli **angoli
    arrotondati**. Il riquadro vero sono i **162 pixel sull'indice 1**, cioè il
    piatto del badge, che per forza si inverte col tema — nero in chiaro,
    bianco in scuro. **Soulgold ha esattamente lo stesso difetto**
    (`0:36, 1:162, 11:32, 15:26`, identico): non c'era niente da portare, la
    soluzione andava inventata. Piatto spostato sull'indice **11** (viola) e
    ombra delle lettere sulla **15** (rosa), entrambe fisse: badge viola con
    "HM" rosa, **identico nei due temi**. La rimappatura va fatta in **un solo
    passaggio** con un dizionario `{1: 11, 11: 15}`, non in due sostituzioni in
    fila, o il piatto nuovo finisce anche lui sul 15.

  Metodo che ha risolto e che vale la pena rifare: dumpare la `.4bpp` come
  **mappa di indici** (16×16 di cifre esadecimali) e renderizzarla offline con
  le `.gbapal` vere dei **due** temi affiancate. Dieci secondi, e si vede subito
  quale indice è il riquadro. Solo i `.png` sono tracciati: i `.4bpp` li genera
  la build.
