---
paths:
  - "src/pokemon_storage_system.c"
---

# Box del PC: `SORT` e `SEARCH`

Non vengono da Soulgold. Tutto in `src/pokemon_storage_system.c`, sezione
"Box sort and search"; nessun campo nel salvataggio, i Pokémon cambiano solo
casella. Voci nel menu del nome del box (A sul titolo), **nascoste** con un
Pokémon in mano, in multi-move e in MOVE ITEMS (`CanSortOrSearchBoxes`).

- **Indirizzo piatto.** `boxes[][]` è contiguo: `&boxes[0][0] + slot` con
  `slot = box * 30 + posizione` percorre tutti i box in ordine. `BoxPokemon`
  si cifra da solo, quindi spostarlo è una copia di 80 byte.
- **Non leggere i dati nei confronti.** Ogni `GetBoxMonData` decifra il
  Pokémon: le chiavi (numero, specie, livello) si leggono una volta in un
  array, si ordina quello (shell sort, chiave totale con lo slot in fondo) e
  si applica la permutazione per cicli con un solo `BoxPokemon` di appoggio.
  La ricerca fa lo stesso con `slotSpecies[]`.
- **Ordine numerico**: Johto finché `IsNationalPokedexEnabled()` è falso,
  poi nazionale; le specie fuori dal Johto dex vanno dopo, in ordine
  nazionale. Oak lo dice consegnando il nazionale al porto di Olivine; col
  randomizer il nazionale arriva in silenzio da Mr. Pokémon, e lì la frase non
  serve. A parità di specie, livello più alto prima; uova e caselle vuote in
  fondo. `ALL` riempie dal box 1 e chiede conferma.
- **Ricerca per nome**, gruppi di lettere del Pokédex, uova escluse. Le forme
  condividono il nome (RATTATA di Alola è RATTATA), quindi si confronta il
  nome e non la specie.
- **La finestra messaggi copre l'ultima riga del box**, cursore compreso
  (righe tile 16-19), e sopra il box **non c'è spazio per una cornice**:
  misurato, i pulsanti PARTY/CLOSE BOX finiscono a y 15 e la barra del titolo
  parte da y 17, mentre le cornici occupano 6-8 righe del tile in tutti e 20 i
  tipi di `graphics/text_window/`. Un primo tentativo con la riga in alto
  aveva i soli bordi laterali e all'utente non piaceva. I risultati stanno
  quindi in una finestra con cornice completa **sopra l'immagine del pannello
  sinistro** (`sWindowTemplate_SearchResult`, tile 1-8 × 3-9, cornice sulle
  righe 2-10): nome, `n/N`, ⒶNEXT, ⒷEXIT. Riusa i tile del menu (`0x5C`), che
  non è mai aperto insieme. Lo sprite del Pokémon ha **priorità 0** e
  starebbe sopra BG0: va reso invisibile dopo ogni refresh e ripristinato in
  uscita. Nome e livello restano visibili sotto.
  Coprendo l'immagine si perdeva l'unico indizio di shininess (i colori):
  accanto al contatore c'è quindi la scintilla del riassunto
  (`summary_screen/shiny_icon.4bpp`), blittata nella finestra. Usa gli indici
  4 e 5, che in `text_windows.gbapal` (BG pal 15) sono neri e inutilizzati:
  ci si caricano le entry 4-5 di `heart.gbapal`, la sua palette originale.
- Verificato in mGBA con i box riempiti dal debug (`PC/Bag → Fill…`) e 5 ABRA
  clonati via GDB in box 4, 4, 6, 10, 10: `ABRA 1/6 … 6/6` e ritorno al
  primo; MAROWAK nell'ultima riga del box 7 visibile col cursore; ordine Johto e, dopo `Toggle National Dex`, nazionale; riassunto di
  un Pokémon spostato integro.
