---
paths:
  - "src/pokedex*.c"
  - "include/pokedex*.h"
  - "src/reshow_battle_screen.c"
  - "graphics/pokedex/**"
  - "src/data/pokemon/species_info/**"
---

# Pokédex e schermate aperte da altre schermate

- **VBlank**: aprire una schermata senza installare il proprio `SetVBlankCallback`
  eredita quello del chiamante. È così che il Pokédex aperto dal menu SwSh scorreva
  in diagonale.
- **Aprire una schermata full-callback dall'interno di una lotta**: il
  *meccanismo* funziona — `OpenPokedexInfoScreen(species, returnCallback)` prende
  il main callback, e come `returnCallback` si passa `ReshowBattleScreenAfterMenu`
  (`include/reshow_battle_screen.h`), la stessa strada di borsa e menu squadra,
  che ricostruisce la lotta e finisce su `BattleMainCB2`, che è esattamente ciò
  che lo script di cattura in `battle_script_commands.c` aspetta.
  **Ma per il Pokédex la memoria non basta, ed è una misura, non un'opinione.**
  Censimento dell'heap fatto in emulatore con la pagina "nuova voce" aperta
  dentro una lotta (`HeapHead()` percorso a mano, stampato nella finestra):
  **23 440 byte liberi**, in un unico blocco, su 116 480 totali. La lotta da sola
  ne tiene **~66 000**. La pagina della cattura ne tiene altri 26 924
  (20 608 di buffer finestre da `sNewEntryInfoScreen_WindowTemplates`, 4 096 di
  BG 2/3, 2 220 di `PokedexView`): restituendoli tutti si arriva a 48 144.
  La schermata info ne chiede **46 464 prima di disegnare qualsiasi cosa**
  (31 616 di finestre da `sInfoScreen_WindowTemplates` — di cui 7 168 di
  `WIN_CRY_WAVE` e 2 560 di `WIN_VU_METER`, usate solo dalla schermata del
  verso — più 8 192 di BG 0-3 e 6 656 per `tileset_menu1.4bpp`), poi lo sprite
  del Pokémon, poi la sotto-schermata su cui si naviga. Muore sui 6 656 con
  `out of memory`, e la frammentazione peggiora il conto: i 4 buffer BG occupano
  la testa del buco grande, `WIN_INFO` (20 480) è costretto in coda, e alla
  decompressione resta il blocco più grande sotto i 6 656.
  **Morale: un `out of memory` con un numero preciso va tradotto in un file.**
  6 656 è esattamente `graphics/pokedex/hgss/tileset_menu1.4bpp`; `ls -l` sui
  `.4bpp` dice in dieci secondi quale schermata sta fallendo.
  Anche Soulgold, sulla pagina della cattura, esce e basta: A e B fanno la stessa
  cosa. Non era un port, era una mia aggiunta, ed è stata tolta.
  Se un giorno la si rivuole, le due strade sono (a) un set di finestre ridotto
  quando si apre da una lotta, che recupera i ~9,7 KB della schermata del verso
  ma la disabilita, oppure (b) aprire la voce **dopo** la lotta, quando l'heap è
  libero.
  Nota di metodo: **verifica in emulatore le feature che tocchi, non solo quelle
  che rompi.** Questa era stata consegnata senza una cattura vera di prova.

  **Secondo tentativo, misurato: due muri distinti, uno superato e uno no.**
  Patch di lavoro in `dex-navigation-wip.patch` (non su branch: l'uscita è rotta).
  1. *Memoria della schermata info* — **superato.**
     `sInfoScreen_WindowTemplates` alloca `WIN_CRY_WAVE` (7 168) e
     `WIN_VU_METER` (2 560) che solo la schermata del verso usa: 9 728 byte
     sprecati su ogni pagina. Con un secondo array identico ma con quelle due
     finestre a 1×1 — indici invariati, quindi nessun codice legge fuori
     tabella — la schermata info si apre dentro una lotta e **INFO, STATS, EVO
     e FORMS funzionano davvero**: verificato in emulatore su un Pidgey
     catturato (statistiche base, lista mosse con su/giù, toggle con A, catena
     evolutiva, "no alternate forms").
  2. *Schermata AREA* — **non superabile.** Decomprime l'intero tileset della
     mappa regionale: `out of memory trying to allocate 16384 bytes`, in un
     blocco solo. Non esiste margine del genere sopra una lotta, con nessun
     riordino. In modalità ridotta destra da INFO salta quindi su STATS.
  3. *Uscita verso la lotta* — **rotta, causa ignota.** `B` dalla schermata
     info (e da FORMS) **resetta il gioco**, cioè si arriva alla schermata
     GAME FREAK: sintomo tipico di un salto a callback NULL. Il giro è quello
     documentato sopra (`Task_ExitInfoScreen` distrugge il task →
     `Task_WaitForExitInfoScreenFromSummary` fa `SetMainCallback2`), e lo
     stesso giro **funziona dal menu squadra**. Ipotesi scartate leggendo il
     codice: `ResetTasks()` non è sul percorso; `monSpriteIds` è inizializzato
     a 0xFFFF quindi `ClearMonSprites` non tocca sprite validi; il comando di
     cattura in `battle_script_commands.c` aspetta correttamente tutte e tre
     le condizioni (fade, `BattleMainCB2`, task morto). Da provare la prossima
     volta: **strumentare, non dedurre** — una build che congela invece di
     saltare quando `sExternalReturnCallback` è NULL separa in un colpo
     "callback a zero" da "reshow che crasha".

## Pagina EVO e ordine di `.evolutions`

- **L'ordine di `.evolutions` non è solo estetica.** `GetEvolutionTargetSpecies`
  (`pokemon.c`, modo normale) si ferma alla **prima** voce che soddisfa le
  condizioni: fra evoluzioni per livello che si sovrappongono vince quella più
  in alto. Per Eevee oggi Leafeon/Glaceon per luogo stanno prima di
  Espeon/Umbreon per amicizia, quindi un Eevee affezionato nel Bosco di Lecci
  diventa Leafeon. Spostare le voci per farle vedere nel Pokédex cambierebbe
  questo. Le pietre passano da `EVO_MODE_ITEM_USE` e non hanno il problema.
- La pagina EVO (`PrintEvolutionTargetSpeciesAndMethod`) stampa al massimo 10
  metodi (`MAX_EVO_METHOD_LINES`): ora mette prima il primo metodo di ogni
  evoluzione e poi i doppioni, così restano fuori i doppioni e non una specie.
  Per Eevee i luoghi di Johto (Bosco di Lecci, Tunnel di Ghiaccio) stanno
  prima di quelli di Kanto, così restano fuori Bosco Smeraldo e Isole
  Spumarine: fra loro l'ordine è libero perché le mapsec non si sovrappongono.
  Giorno e notte si scrivono D e N (scelta dell'utente).
  Verificato in mGBA su Eevee: Espeon e Umbreon visibili, freccia e A (CHECK)
  sulla riga giusta.
- Per arrivare alla pagina in fretta: `dexSeen` 0x39C0 e `dexCaught` 0x3A7F
  in SaveBlock1 (bit = numero nazionale - 1), poi nel Pokédex SELECT → OK
  lista solo le specie viste. Il puntatore a SaveBlock1 **cambia** a ogni
  caricamento di mappa: rileggilo prima di ogni scrittura.
