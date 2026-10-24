# Piano (bozza): registro delle missioni

**Stato (24/10): raccolta di idee e di fatti, niente di deciso né
pianificato.** Si progetta più avanti, dopo Red's Path.

## L'idea

Un **registro delle missioni** consultabile dal giocatore: per ogni missione,
a che punto è e **cosa fare adesso** (dove andare, chi cercare), passo per
passo. Dentro ci vanno:

1. **la storia principale**: le 16 medaglie (8 di Johto, 8 di Kanto), con
   l'indicazione della prossima palestra;
2. il **Pokédex**, a scaglioni di 50;
3. le **catture dei leggendari**;
4. le **missioni secondarie**: quelle che esistono già (TcloneR, Red's Path)
   e quelle da inventare.

### Un principio da tenere

Quasi tutto lo stato esiste già nel salvataggio: flag delle medaglie
(`FLAG_BADGE01_GET`…`FLAG_BADGE16_GET`) e dei capi battuti
(`FLAG_DEFEATED_*_GYM`, `include/constants/flags_hns.h`), conteggio del
Pokédex, flag degli eventi leggendari, flag e var di Red's Path e del
TcloneR. Il registro dovrebbe **leggere** questi dati per decidere passo e
testo di ogni missione, non duplicarli: così costa poco nel salvataggio e
funziona anche su partite vecchie. Le missioni nuove (inventate) avranno
invece bisogno di flag propri.

### Da progettare (nulla verificato nel codice)

- Dove si apre: oggetto chiave, voce del menu di pausa, pagina del Pokégear.
- Come si presenta: categorie, missioni attive/completate, una riga "cosa
  fare adesso" per missione.
- Se c'è una missione "seguita" in evidenza.
- Se esiste qualcosa di riusabile: nella config di expansion
  (`include/config/`) non c'è un registro missioni.

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

### Riferimento esterno

Il menu missioni più diffuso per i decomp è quello di ghoulslash (branch
`quest-menu`, pokeemerald e pokefirered: tabella `sSideQuests`, voce del
menu di pausa dietro `FLAG_SYS_QUEST_MENU_GET`, `QuestMenu_Init`), da cui è
derivato anche il rifacimento del menu di Unbound
([PokéCommunity](https://www.pokecommunity.com/threads/pokefirered-pokeemerald-quest-menu.434462/),
[Unbound Quest Menu](https://pokecommunity.com/threads/unbound-quest-menu.477500/)).
Non è stato aperto né confrontato col codice: tiene lo stato delle missioni
in flag propri (attiva/completata), cioè l'opposto del principio sopra.

## 1. Storia principale: le 16 medaglie

Una missione per regione, un passo per palestra, con la prossima indicata.

| Johto | Mappa | Kanto | Mappa |
| --- | --- | --- | --- |
| Falkner | `VioletCity_Gym_hns` | Brock | `PewterCity_Gym_hns` |
| Bugsy | `AzaleaTown_Gym_hns` | Misty | `CeruleanCity_Gym_hns` |
| Whitney | `GoldenrodCity_Gym_hns` | Surge | `VermilionCity_Gym_hns` |
| Morty | `EcruteakCity_Gym_hns` | Erika | `CeladonCity_Gym_hns` |
| Chuck | `CianwoodGym_hns` | Janine | `FuchsiaCity_Gym_hns` |
| Jasmine | `OlivineCity_Gym_hns` | Sabrina | `SaffronCity_Gym_hns` |
| Pryce | `MahoganyTown_Gym_hns` | Blaine | `SeafoamIslands_Gym_hns` |
| Clair | `BlackthornCity_Gym_hns` | Blue | `ViridianCity_Gym_hns` |

- **Kanto**: ordine libero, tranne Blue, che torna in palestra solo dopo
  averlo incontrato a Cinnabar con 15 medaglie (`CinnabarIsland_hns/scripts.inc`,
  `VAR_NUM_BADGES == 15`; verificato per Red's Path). Il passo "prossima
  palestra" in Kanto è quindi un elenco di quelle mancanti, con Blue per
  ultimo.
- **Johto**: l'ordine e i blocchi di storia (cosa serve per arrivare a ogni
  palestra) vanno ricavati dagli script di HnS in fase di progettazione:
  non ancora verificati.
- Il testo di ogni passo dice dove andare e, se serve, cosa sblocca la
  strada (una MN, un evento).

## 2. Pokédex a scaglioni di 50

| Pokédex | Voci | Scaglioni |
| --- | --- | --- |
| Johto (`JOHTO_DEX_COUNT`, `include/constants/pokedex.h`) | 282 | 50, 100, 150, 200, 250, 282 |
| Nazionale (tutte le generazioni attive, fino a Pecharunt) | 1025 | 50, 100, … 1000, 1025 |

Da decidere: quale Pokédex conta (Johto, nazionale, o entrambi in due
missioni), visti o catturati, e se ogni scaglione ha un premio (come gli
scaglioni del TcloneR).

## 3. Leggendari

Dove compaiono negli script di HnS (ricerca di `SPECIES_*` nelle mappe
`_hns`; alcune mappe li citano solo nel testo, come la casa di Bill o il
teatro di Amarantopoli: il luogo della lotta va confermato mappa per mappa).

| Pokémon | Mappa |
| --- | --- |
| Articuno | `SeafoamIslands_B1F` |
| Zapdos | `Route10` |
| Moltres | `MtSilver_1F_MoltresRoom` |
| Mewtwo | `CeruleanCave_B2F` |
| Mew | `FarawayIsland_Interior` |
| Raikou, Entei | `BurnedTower_B1F` (vaganti) |
| Suicune | `BurnedTower_B1F`, poi `CianwoodCity`, `Route42`, `VermilionCity`, `Route25`, `Route14` (inseguimento) |
| Lugia | `WhirlIslands_LugiaChamber` |
| Ho-Oh | `TinTower_RoofDay` |
| Celebi | `IlexForest` |
| Kyogre | `Route19_Cave` |
| Groudon | `SeafoamIslands_SecretCave` |
| Rayquaza | `EmbeddedTower` |
| Latias, Latios | `SouthernIsland_Interior` |
| Deoxys | `BirthIsland_Exterior` |
| Jirachi | `MtMoon_Outside` |
| Regirock, Regice, Registeel, Regieleki, Regidrago, Regigigas | `SinjohRuins_*` |
| Arceus | `SinjohRuins_ArceusRoom` |

Ogni leggendario è una missione (o un passo di una missione per gruppo:
uccelli di Kanto, cani, Regi), con le condizioni per farlo comparire. Le
condizioni vanno lette negli script di ciascuna mappa.

## 4. Missioni secondarie

### Esistenti

- **TcloneR** (`docs/plans/tcloner.md`, `.claude/rules/trainer_catch.md`):
  lo scienziato in cinque posti, gli scaglioni di cloni e i premi. Le tappe
  e i contatori ci sono già (`VAR_TCLONER_STAGE`, `VAR_TCLONER_COUNT`,
  `VAR_TCLONER_REWARDS`).
- **Red's Path** ([`reds-path.md`](reds-path.md)): otto sfide in ordine.
  Passi del registro: "batti un capo di Kanto" (prima dell'introduzione) →
  "prossima sfida: <capo> a <città>" (con "batti prima il capo di HnS" se
  serve) → "premio da ritirare" (squadra piena) → completata. Lo stato sono
  i flag degli otto allenatori di Red's Path e dei premi, già previsti nel
  piano.

### Da Red's Path al Monte Argento

Nate dal piano di Red's Path (24/10). Red's Path finisce con Charizard,
Pikachu-Starter, Snorlax, Eevee, Poliwrath, Lapras; al Monte Argento HnS gli
dà Pikachu-Starter, Snorlax, Charizard, Venusaur, Blastoise, Espeon
(`TRAINER_RED_HNS` in `src/data/trainers_hns.party`). Queste missioni colmano
il salto: da dove vengono **Venusaur e Blastoise** (in FireRed, partendo con
Charmander, non si ottengono), quando **Eevee diventa Espeon** (evoluzione di
Gen 2), dove finiscono **Poliwrath, Lapras, Butterfree** e la panchina, e
come si arriva a Red.

Agganci già in HnS: la **casa di Red** a Biancavilla
(`PalletTown_RedsHouse_1F_hns`: la madre dice che Red è via da tanto e non ha
sue notizie), il **laboratorio di Oak** (`PalletTown_Lab_hns`), la casa di
**Bill** sul Percorso 25, la **Villa di Azzurropoli** (dove Red ha avuto
Eevee). In HnS **nessuno script regala gli starter di Kanto** (solo il menu
debug, `data/scripts/debug.inc`).

1. **Il PC di Bill.** Bill custodisce i Pokémon del viaggio di Red
   (Butterfree, Mankey, Diglett/Dugtrio, Gloom, Poliwrath, Lapras: le
   panchine di Red's Path). Il giocatore li vede o combatte una lotta
   speciale con loro (motore di Red's Path). Racconta: dove finisce la
   squadra del viaggio.
2. **Gli starter di Oak.** Red è tornato da Oak dopo la Lega a chiedere i
   due starter rimasti. In *Pokémon Origins* Blue sceglie Squirtle, quindi
   Bulbasaur sarebbe rimasto al laboratorio; per Blastoise serve
   un'invenzione. Il giocatore porta a Red un oggetto di Oak fino al Centro
   Pokémon del Monte Argento. Racconta: Venusaur e Blastoise. Attenzione:
   tocca Oak e l'accesso al Monte Argento (progressione).
3. **Eevee al sole.** La signora della Villa di Azzurropoli riceve la foto
   di un Espeon e chiede notizie; oppure un indizio in Johto. Racconta:
   Espeon.
4. **La lettera alla madre.** Red racconta in poche righe i tre anni dopo il
   viaggio e i nuovi compagni. Racconta tutto, solo testo.
5. **Red contro Blue.** Una nona lotta dopo Red's Path: Red contro Blue
   campione, con la squadra di Giovanni. Le squadre FRLG del campione ci sono
   già (`TRAINER_CHAMPION_FIRST_*` in `src/data/trainers_frlg.party`; per
   Red con Charmander, Blue ha `..._SQUIRTLE`).

Suggerimento iniziale: 1 + 4 come base, 2, 3 e 5 come aggiunte.

### Da inventare

(da aggiungere insieme all'utente)
