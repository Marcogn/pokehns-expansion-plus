# Analisi: quest book e menu di pausa (archivio)

Ricerche del 24/10 da cui sono uscite le decisioni di
[`docs/plans/quest-log.md`](../plans/quest-log.md). **Non serve per
implementare**: il piano ha già i fatti che servono. Qui restano i confronti,
le opzioni scartate e i dettagli che le giustificano.

## Confronto con Soulgold (24/10)

**Soulgold non ha un registro delle missioni.** Verificato sul mirror e
sull'upstream `Eemeliri/soulgold` (`master` = `4d4acbe98`, lo stesso punto
già valutato in `state.json`; nessun branch remoto con un quest log). Le
"quest" di SG sono solo script e var senza interfaccia
(`VAR_BAOBA_QUEST_STATE`, `FLAG_STARTED_VICTINI_QUEST`,
`VAR_POSTGAME_RIVAL_LEGENDARYQ` per Cresselia): sono contenuto, e le
missioni si valutano una per una, non si portano in blocco.

La cosa più vicina in SG sono i **Trofei** (achievement): un elenco di
obiettivi con stato e progresso, che è metà di un registro. Non dice "cosa
fare adesso". Il §5 del port report lo classifica come contenuto di SG;
qui interessa solo il **motore**.

| Pezzo di SG | File | Riusabile per il registro? |
| --- | --- | --- |
| Tabella dichiarativa: id, nome, descrizione, categoria, contatore + soglia, oppure un **predicato** `bool32 (*)(void)` | `src/achievements.c`, `include/achievements.h` | Sì, come forma. I predicati leggono lo stato che c'è già (`FlagGet(FLAG_BADGE0x_GET)`, `GetSetPokedexFlag(..., FLAG_GET_CAUGHT)`, var di script): è il principio scritto sopra. Al registro serve un passo in più: una funzione che restituisce il **passo corrente** (indice di testo), non solo vero/falso. |
| Salvataggio: 48 byte in SaveBlock1 (6 contatori `u32`, 16 byte di bit sbloccati, coda di 6 popup, magic `0xACE1` che inizializza i salvataggi vecchi) | `include/global.h`, `include/constants/achievements.h` | No così com'è. Un registro che legge lo stato esistente non ha bisogno di contatori né di bit "completato": al massimo una missione seguita e i bit "già letto/nuovo". In Hasep un campo nuovo va in coda a SaveBlock3 con migrazione (CLAUDE.md §4), non con un magic. |
| Controllo: `Achievement_CheckAll()` chiamato da cattura, evoluzione, oggetti, strutture lotta; `Achievement_OnTrainerDefeated()` | sparsi in 10+ file | Non serve per il registro: lo stato si calcola all'apertura. Servirebbe solo per avvisare "missione aggiornata". |
| Popup in overworld con coda salvata, agganciato a `OverworldBasic()` | `src/achievements_popup.c` | Possibile, da valutare a parte (usa `comfy_anim`, che in Hasep c'è: `include/comfy_anim.h`). |
| Schermata: lista di 6 righe, icona per categoria (Poké/Mega/Ultra/Master Ball), stato a destra (`Done`, `x/y`, `Locked`), descrizione in basso | `src/achievements_menu.c` | Come riferimento di layout. Prende lo sfondo da `graphics/pokegear/tiles.*` di SG: in Hasep va derivato dalla grafica nostra (CLAUDE.md §2). |
| Ingresso: app `POKEGEAR_APP_TROPHIES` del Pokégear di SG | `src/pokegear.c` | No: Hasep usa il Pokénav di HnS (§5 del port report). |

### Dove aprirlo in Hasep (fatti, nessuna scelta)

- **Pokénav di HnS**: il menu più completo (`POKENAV_MENU_TYPE_UNLOCK_MC_RIBBONS_RADIO`,
  `src/pokenav_menu_handler.c`) usa già 6 voci su `MAX_POKENAV_MENUITEMS` = 6.
  Una settima voce vuol dire alzare il massimo e una grafica di pulsante
  nuova: costo non misurato.
- **Menu di pausa**: c'è già una voce aggiunta da HnS (`MENU_ACTION_DEXNAV`,
  `src/start_menu.c`); una voce in più è la strada più corta.
- **Oggetto chiave**: come la Mappa città.

## Menu missioni esistenti per i decomp (letti nel codice, 24/10)

Tutti e due sono per **pret/pokeemerald**, non per expansion: si portano a
mano (lo dice la stessa documentazione di PSF).

### ghoulslash, `quest-menu`

`github.com/ghoulslash/pokeemerald`, branch `quest-menu`, ultimo commit
2023-05-11; 24 file, +2103 righe, di cui `src/quests.c` 1499
([thread](https://www.pokecommunity.com/threads/pokefirered-pokeemerald-quest-menu.434462/)).

- 30 missioni con nome, descrizione **fissa**, persona da cercare, mappa,
  premio (icona dell'oggetto). Stati: sbloccata, completata, una sola
  "attiva".
- Salvataggio: due bitfield da 4 byte e un byte in coda a SaveBlock2.
- Schermata: è il PC degli oggetti di FireRed (cornici `fr_std*`).
- Voce `QUESTS` nel menu di pausa dietro `FLAG_SYS_QUEST_MENU_GET`; comando
  di script `questmenu` (opcode `0xe3` della tabella di pret).
- Superato dal successivo: non vale la pena partire da qui.

### PokemonSanFran, "Unbound Quest Menu"

`github.com/PokemonSanFran/pokeemerald`: branch `unbound-quest-menu`
(allineato a pret il 2026-08-02) e `complex_quests` (RubyRaven6, ultimo
commit 2026-10-20), +3977 righe, `src/quests.c` 2971. Documentazione:
[wiki di PSF](https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu),
elencata fra i feature branch del
[wiki di pret](https://github.com/pret/pokeemerald/wiki/Feature-Branches).
Parte da ghoulslash e imita il menu di Pokémon Unbound.

- **Stati**: bloccata (`????`), sbloccata, attiva, premio da ritirare,
  completata; più "preferita" (Select, va in cima). 5 bit per missione,
  1 bit per sottomissione: 25 byte in SaveBlock2 per 30 + 30.
- **Sottomissioni** (A su una missione): solo bloccata/completata, con
  contatore `x/y`. Esempio della doc: i sette simboli d'oro del Frontier.
- **Filtri** con R (per stato), **ordine alfabetico** con Start.
- Ogni voce: mappa, sprite (NPC, oggetto **o** Pokémon), descrizione
  attiva e descrizione "completata".
- **`complex_quests` è la parte che ci interessa**: ogni missione ha un
  `questVariable` e array `desc[]`, `map[]`, `sprite[]` indicizzati da
  `VarGet(questVariable)` (`GetQuestDesc`, `GetQuestLocation`). È il "cosa
  fare adesso" guidato da una var che già esiste: va bene così com'è per
  Red's Path e per il TcloneR (`VAR_TCLONER_STAGE`). Per medaglie, Pokédex
  e leggendari lo stato non è una var ma una funzione: servirebbe un
  callback al posto della var.

Cose da sapere prima di portarlo:

- **Cambia tutte le liste del gioco**: `ListMenu_ProcessInput`
  (`src/list_menu.c`) viene modificata perché ogni lista ricominci dall'alto
  dopo l'ultima voce. La doc lo segnala; in Hasep toccherebbe anche borsa,
  PC e menu nostri. Va tolto o messo dietro un parametro.
- **Salvataggio in SaveBlock2**, azzerato a mano in `NewGameInitData`
  (`QuestMenu_ResetMenuSaveData`). Da noi un campo nuovo va in coda a
  SaveBlock3 con migrazione (CLAUDE.md §4). Se si legge lo stato esistente,
  i 5 bit per missione si riducono a "preferita" e "nuovo".
- **Nessun controllo di limite**: `desc[VarGet(...)]` su un array di
  `MAX_QUEST_STATES` = 50 voci, ricade sull'indice 0 solo se la voce è
  `NULL`. Una var oltre 49 legge fuori dall'array.
- **ROM**: ogni missione porta 50 + 50 puntatori e 50 sprite anche se ne
  usa 3. Circa 570 byte a missione, ~17 KB per 30: poco, ma la ROM è al
  99,46% con le mega (`.claude/rules/megas.md`).
- Comando di script con opcode nella tabella di pret: in expansion la
  tabella è diversa, va rinumerato. `CreateMonIcon` in expansion ha un
  argomento in meno (FAQ della doc).
- Bug noto della doc: un artefatto per un frame passando da uno sprite NPC
  a uno non NPC, mai riprodotto con regolarità.
- Sfondo: versione modificata di quello FRLG di Skeli; ci sono varianti
  (stile Pokédex Plus, scuro). Va accreditato l'autore. Da noi va comunque
  rifatto sulla nostra grafica e sul tema scuro (CLAUDE.md §2 e §5).

## Il menu di pausa

Hasep e Soulgold hanno tutti e due il menu di pausa classico, una lista
(`BuildNormalStartMenu`, `src/start_menu.c`); HnS aggiunge solo la voce
DexNav. Soulgold non lo ha rifatto: le sue app stanno nel Pokégear.

Rifacimenti esistenti:

- **Full Screen Start Menu** (Archie e Mudskip, Team Aqua's Hideout):
  branch `full_start_menu` di `TeamAquasHideout/pokeemerald`, ultimo
  commit 2024-10-14. Il
  [wiki di pret](https://github.com/pret/pokeemerald/wiki/Full-Screen-Start-Menu-by-Archie-and-Mudskip)
  lo dice compatibile con expansion; con il Pokédex HGSS (Hasep usa
  `pokedex_plus_hgss.c`) va cambiato il ritorno in `Task_ClosePokedex`. Usa
  gli stessi flag del menu classico (voce grigia se manca), salva senza la
  conferma di sovrascrittura. Non aperto nel codice.
- `pollythadon/pokeemerald-expansion`, branch `shattered-emerald`
  (expansion 1.17.1, commit del 2026-10-13): contiene
  `src/unbound_start_menu.c` e `src/rotom_start_menu.c`. Una ricerca lo
  descrive come "alpha"; non aperto, base più nuova della nostra (1.15.2).

Il legame con il registro: in un menu a schermo intero le voci hanno posti
fissi, quindi il registro va previsto nel disegno. Il costo vero è un
altro: ogni schermata aperta dal menu deve tornarci (`CB2_ReturnTo...`),
quindi vanno ritoccati tutti i ritorni, compresi quelli nostri (party menu
SwSh, DexNav, borsa, Pokédex).

## Opzioni per il menu di pausa, prima della decisione

### Menu a schermo intero: cosa si fa

Le basi esistenti sono due, nessuna da prendere così com'è:

- **Team Aqua's Hideout** (`ui_startmenu_full.c`, 1577 righe, compatibile
  con expansion per il wiki di pret): 6 icone fisse (Pokédex, Squadra,
  Borsa, Scheda, Mappa/Pokénav, Opzioni) più icone della squadra con barre
  PS, ora, nome della mappa. **Salva non è un'icona**: sta su Start
  (`Task_StartMenuFullMain`, poi conferma con A, annulla con B); Select
  cambia l'orologio 12/24 h se `FLAG_CLOCK_MODE` è configurato. L e R non
  sono usati. Dai suoi ritorni, la Zona Safari, la Piramide,
  la Battle Pike e la Union Room restano sul menu classico.
- **`pollythadon/pokeemerald-expansion`** (`unbound_start_menu.c`, 1317
  righe, expansion 1.17.1): barra di icone in stile Unbound, 6 per pagina
  con **più pagine**, voci riordinabili dal giocatore (ordine salvato in
  SaveBlock3). È dentro un gioco (`shattered-emerald`), non un feature
  branch; descritto come alpha. **Non è a schermo intero**: è una striscia
  in basso disegnata sopra la mappa (icone a `USM_ICON_YPOS` 128, BG0 del
  campo, `FreezeObjectEvents`), come il menu di Unbound. Le schermate
  tornano con `CB2_ReturnToFieldWithOpenMenu` come oggi. Tasti: frecce
  (a destra dell'ultima icona passa alla pagina dopo), L pagina, R sposta
  voce, B esce; finestre per nome, orologio e suggerimenti.

A Hasep servono 8 icone più Salva su Start, come fa Team Aqua: Pokédex,
DexNav, Squadra, Borsa, Pokégear, Scheda, **Missioni**, Opzioni (più Debug
nelle build di debug e i ritiri di Safari e Bug-Catching Contest). Le 6
caselle fisse di Team Aqua non bastano: o si
ridisegna la griglia, o si usano le pagine dello stile Unbound. Per un menu
"compagno" del quest book di Unbound il secondo è il più vicino.

Il costo vero sono i **ritorni**: `CB2_ReturnToFieldWithOpenMenu` compare in
13 file (`start_menu.c` 5, `naming_screen.c` 4, `swsh_party_menu.c`,
`party_menu.c` e `pokenav.c` 2, `pokedex_plus_hgss.c`, `dexnav.c`,
`item_menu.c`, `region_map.c` e altri 1). Ognuno va portato al nuovo menu,
con le eccezioni dove deve restare la lista classica (Safari, Bug-Catching
Contest, Piramide, …), e verificato in emulatore uno per uno.

### Due strade, scelta dell'utente (24/10)

| | A. Schermo intero (base Team Aqua) | B. Striscia in basso (base stile Unbound) |
| --- | --- | --- |
| Aspetto | schermata propria, griglia di icone | sopra la mappa, 6 icone visibili per pagina |
| Ritorni da riscrivere | 13 file (vedi sopra) | nessuno: restano quelli di oggi |
| Voci | griglia da ridisegnare per 8 | 8 su due pagine, o 6 + scorciatoie |
| Costo stimato | 3-4 sessioni | 1-2 sessioni |

Preferenze dell'utente, valide per tutte e due: **L apre il Pokégear, R le
Opzioni, Salva su Start; niente griglia della squadra** (la voce Squadra
c'è già, mostrarla accanto è doppio). Con L e R come scorciatoie, sulla
striscia il cambio pagina resta alle frecce e il riordino delle voci si
perde o va su un altro tasto. Restano 6 icone (Pokédex, DexNav, Squadra,
Borsa, Scheda, Missioni): con Pokégear e Opzioni sui dorsali stanno in una
pagina sola.

## Altre migliorie viste nel wiki di pret (24/10)

Lette nel codice di Hasep: **due su tre sono già superate da expansion**.

- **Improved Audio Mixer** (Kurausukun, `newmix`): **già presente.**
  `src/m4a_1.s:88` è l'HQ-Mixer rev 4.0 di ipatix; `src/m4a.c` ha
  `SoundMainRAM_Buffer[0xB40]` e `hq_buffer_ptr[0x130]` (18 157 Hz), con
  `PCM_DMA_BUF_SIZE` 2128 e 15 canali DirectSound; le scritture DMA atomiche
  sono in `include/gba/macro.h`. Non c'è niente da portare.
- **BetterBag** (AsparagusEduardo): **superata.** Expansion ha le tasche
  come opzione: `I_COMBINE_BAG_POCKETS` (`include/config/item.h`) è `TRUE`
  in Hasep, cioè 6 tasche (Strumenti, Medicine, Poké Ball, MT/MN, Bacche,
  Strumenti chiave). Con `FALSE` si aggiungono **Strumenti lotta** e
  **Tesori**; i posti totali restano uguali (236 + 92 contro
  200 + 36 + 60 + 32), ma le tasche nuove stanno in mezzo alla
  `struct Bag`, quindi serve una migrazione che ridistribuisca gli oggetti di
  un salvataggio vecchio. Va rifatta anche la grafica della borsa per 8
  tasche. Soulgold ha 8 tasche, ma diverse: Strumenti lotta e
  **Megapietre**, niente Tesori.
- **free_saveblock** (ghoulslash): **superata.** Expansion ha
  `include/config/save.h` con i flag `FREE_*`; Hasep ne usa già quattro
  (seen flag in SaveBlock1, buffer Mystery Event, Bacca Enigma, Dono
  Segreto). Oggi lo spazio non serve: il quest book ci sta (vedi sopra).
  Ogni altro `FREE_*` toglie un campo, e se non è in coda alla struct sposta
  tutto quello che segue: rompe i salvataggi vecchi senza una migrazione.
  `FREE_MATCH_CALL` va lasciato, perché il Pokégear di HnS usa le chiamate.

## Anteprime

Le due anteprime mostrate all'utente (240×160, ingrandite 3×) sono state
generate offline: mappa vera di New Bark Town renderizzata da layout, tileset
e palette del repo (`NewBarkTown_hns_Layout`, `johto_general_hns`,
`new_bark_town_hns`), sprite di Gold (`gold/walking_hns.png`), font
`latin_normal.png` con le larghezze di `src/fonts.c`, glifi dei tasti da
`keypad_icons.png`, icone e fascia da `pollythadon` (`graphics/unbound_start_menu`),
sfondo a Poké Ball da Team Aqua's Hideout. Gli script non sono nel repo:
si rifanno in poche righe di Python con PIL se servono di nuovo.
