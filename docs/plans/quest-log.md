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
