# Piano: Red's Path (le palestre di Kanto rigiocate con la squadra di Red)

**Stato (23/10): piano, nessun codice.** Decisioni prese con l'utente in chat;
quelle ancora aperte sono in fondo.

## L'idea

Dopo aver battuto un capo palestra di Kanto (o dopo la seconda Lega: da
decidere), un NPC propone la sfida "del viaggio di Red": il giocatore **usa la
squadra di Red di quel momento** contro **lo stesso capo palestra com'era in
Gen 1**, con i Pokémon di allora. Niente catture, niente esperienza, niente
zaino, nient'altro: la lotta e basta. Otto sfide, la squadra cresce fino a
quella del Monte Argento (meno Espeon).

## Decisioni prese

| Tema | Scelta |
| --- | --- |
| Chi usa la squadra di Red | **il giocatore** (squadra prestata), contro il capo palestra di Gen 1 |
| Gioco di riferimento | **Pokémon Giallo**, per Red *e* per i capi palestra |
| Capi palestra | gli 8 di Kanto **di Gen 1**: Brock, Misty, Surge, Erika, **Koga**, Sabrina, Blaine, **Giovanni** (confermato: non Janine e Blue) |
| Squadre dei capi | **aperto**: Giallo o FireRed/LeafGreen, confronto sotto |
| Ordine | **libero**: ogni sfida riparte da zero, la squadra di Red è sempre quella della N-esima |
| Pikachu | **`Pikachu-Starter`** come al Monte Argento (base 45/80/50/75/60/120 contro 35/55/40/50/50/90 del Pikachu normale) |
| Zaino | nessuno: borsa disabilitata in queste lotte |
| Espeon | **mai**: Eevee fino all'ultima sfida, Espeon resta solo al Monte Argento |
| Catture, esperienza | nessuna |
| Storia | non ancora scritta: qui c'è solo la "ciccia" degli script |

## Perché Giallo, e da dove viene la squadra di Red

In R/B/V Red è il giocatore, quindi **nessuna cartuccia di Gen 1 contiene una
squadra di Red**. L'unica squadra di Red nei dati di gioco è quella del Monte
Argento (Crystal, `pret/pokecrystal` `data/trainers/parties.asm`, `RedGroup`):
Pikachu 81, Espeon 73, Snorlax 75, Venusaur 77, Charizard 77, Blastoise 77.

In R/B quella squadra non si mette insieme senza scambi. In **Giallo sì**
(tranne Espeon), e ogni membro ha un punto preciso in cui entra
(`pret/pokeyellow` @ `b6696f24`):

| Membro | Come e quando | Fonte |
| --- | --- | --- |
| Pikachu Lv5 | starter; in Giallo non evolve | – |
| Bulbasaur Lv10 | casa di Melanie a Celestopoli, serve Pikachu felice | `scripts/CeruleanMelaniesHouse.asm` |
| Charmander Lv10 | Percorso 24 | `scripts/Route24.asm` |
| Squirtle Lv10 | Agente Jenny ad Aranciopoli, **dopo la Medaglia Tuono** | `scripts/VermilionCity_2.asm` (`BIT_THUNDERBADGE`) |
| Eevee Lv25 | tetto della Villa di Azzurropoli | `scripts/CeladonMansionRoofHouse.asm` |
| Snorlax Lv30 | Percorso 12 o 16, col Poké Flauto; i due Snorlax chiudono le due strade per Fuchsia, quindi c'è prima di Koga | `scripts/Route12.asm`, `Route16.asm` (`ld a, 30`) |

Evoluzioni di Gen 1: Ivysaur/Charmeleon/Wartortle al 16, Venusaur al 32,
Charizard e Blastoise al 36.

**Le specie e i punti d'ingresso sono dati di gioco; i livelli di Red no.**
Sono una proposta (sotto), da tarare in emulatore.

## I capi palestra (dati esatti di Giallo)

Squadre da `data/trainers/parties.asm`. Mosse calcolate con la regola di Gen 1
(`WriteMonMoves`: mosse di livello 1 più learnset fino al livello, tiene le
ultime quattro; `gen1moves.py`) e poi sovrascritte dalle mosse speciali dei
capi palestra di Giallo (`data/trainers/special_moves.asm`, slot per slot).
DV fissi degli allenatori di Gen 1: Att 9, Dif 8, Vel 8, Spe 8, quindi PS 8
(`constants/battle_constants.asm`, `ATKDEFDV_TRAINER $98`,
`SPDSPCDV_TRAINER $88`); Stat Exp zero.

| Capo | Squadra |
| --- | --- |
| **Brock** | Geodude 10: Tackle · Onix 12: Tackle, Screech, Bind, Bide |
| **Misty** | Staryu 18: Tackle, Water Gun · Starmie 21: Tackle, Water Gun, Harden, Bubble Beam |
| **Surge** | Raichu 28: Thunderbolt, Mega Punch, Mega Kick, Growl |
| **Erika** | Tangela 30: Constrict, Bind, Mega Drain, Vine Whip · Weepinbell 32: Razor Leaf, Sleep Powder, Stun Spore, Acid · Gloom 32: Petal Dance, Stun Spore, Sleep Powder, Acid |
| **Koga** | Venonat 44: Toxic, Tackle, Sleep Powder, Psychic · Venonat 46: Toxic, Psybeam, Supersonic, Psychic · Venonat 48: Toxic, Double-Edge, Sleep Powder, Psychic · Venomoth 50: Leech Life, Double Team, Psychic, Toxic |
| **Sabrina** | Abra 50: Flash (solo quella, è così in Giallo) · Kadabra 50: Kinesis, Recover, Psychic, Psywave · Alakazam 50: Psywave, Recover, Psychic, Reflect |
| **Blaine** | Ninetales 48: Flamethrower, Tail Whip, Quick Attack, Confuse Ray · Rapidash 50: Stomp, Growl, Fire Spin, Take Down · Arcanine 54: Flamethrower, Fire Blast, Reflect, Take Down |
| **Giovanni** | Dugtrio 50: Dig, Sand Attack, Fissure, Earthquake · Persian 53: Screech, Double Team, Fury Swipes, Slash · Nidoqueen 53: Earthquake, Tail Whip, Thunder, Double Kick · Nidoking 55: Earthquake, Leer, Thunder, Double Kick · Rhydon 55: Rock Slide, Fury Attack, Horn Drill, Earthquake |

MT che ogni capo dava a Red (rilevante per le mosse di Red, vedi sotto):
Bide, Bubble Beam, **Thunderbolt** (Surge), Mega Drain, Toxic, Psywave,
Fire Blast, Fissure (`scripts/<Città>Gym.asm`).

Nota onesta sulla fedeltà: specie, livelli e mosse sono di Giallo, **le
meccaniche no**. Expansion usa quelle moderne: Speciale diviso in Att. Sp. e
Dif. Sp., statistiche base moderne, tabella dei tipi moderna, mosse OHKO che
falliscono contro un livello più alto. Il DV di Gen 1 (0-15) diventa un IV
raddoppiato (Att 18, il resto 16): è un'approssimazione, non una conversione.

## Alternativa: i capi palestra di FireRed/LeafGreen

Squadre da `pret/pokefirered` `src/data/trainer_parties.h`, identiche a
quelle che expansion ha già in `src/data/trainers_frlg.party`
(`TRAINER_LEADER_*`, controllate una per una). **Specie e livelli sono
quelli di Rosso/Blu** (`pret/pokered`), con una sola differenza: l'ultimo
Pokémon di Giovanni è Rhyhorn 50, non Rhydon 50. Le mosse sono scritte a mano
dal remake, con mosse di Gen 3. IV 0 in entrambe le fonti.

| Capo | Strumenti | Squadra |
| --- | --- | --- |
| **Brock** | – | Geodude 12: Tackle, Defense Curl · Onix 14: Tackle, Bind, Rock Tomb |
| **Misty** | Super Potion | Staryu 18: Tackle, Harden, Recover, Water Pulse · Starmie 21: Swift, Recover, Rapid Spin, Water Pulse |
| **Surge** | Super Potion, Full Heal | Voltorb 21: Sonic Boom, Tackle, Screech, Shock Wave · Pikachu 18: Quick Attack, Thunder Wave, Double Team, Shock Wave · Raichu 24: idem |
| **Erika** | Hyper Potion, Full Heal | Victreebel 29: Stun Spore, Acid, Poison Powder, Giga Drain · Tangela 24: Poison Powder, Constrict, Ingrain, Giga Drain · Vileplume 29: Sleep Powder, Acid, Stun Spore, Giga Drain |
| **Koga** | Hyper Potion ×2, Full Heal | Koffing 37: Self-Destruct, Sludge, Smokescreen, Toxic · Muk 39: Minimize, Sludge, Acid Armor, Toxic · Koffing 37 (come il primo) · Weezing 43: Tackle, Sludge, Smokescreen, Toxic |
| **Sabrina** | Hyper Potion ×2, Full Heal | Kadabra 38: Psybeam, Reflect, Future Sight, Calm Mind · Mr. Mime 37: Barrier, Psybeam, Baton Pass, Calm Mind · Venomoth 38: Psybeam, Gust, Leech Life, Supersonic · Alakazam 43: Psychic, Recover, Future Sight, Calm Mind |
| **Blaine** | Hyper Potion ×2, Full Heal | Growlithe 42: Bite, Roar, Take Down, Fire Blast · Ponyta 40: Stomp, Bounce, Fire Spin, Fire Blast · Rapidash 42: idem · Arcanine 47: Bite, Roar, Take Down, Fire Blast |
| **Giovanni** | Hyper Potion ×2, Full Heal | Rhyhorn 45: Take Down, Rock Blast, Scary Face, Earthquake · Dugtrio 42: Slash, Sand Tomb, Mud-Slap, Earthquake · Nidoqueen 44: Body Slam, Double Kick, Poison Sting, Earthquake · Nidoking 45: Thrash, Double Kick, Poison Sting, Earthquake · Rhyhorn 50: come il primo |

Confronto con Giallo:

| | Giallo | FRLG |
| --- | --- | --- |
| Assi (Brock → Giovanni) | 12, 21, 28, 32, **50, 50, 54, 55** | 14, 21, 24, 29, 43, 43, 47, 50 |
| Curva | salto Erika 32 → Koga 50 | regolare, quella di R/B |
| Squadre | 1-5 Pokémon, temi rifatti (Koga a Venonat, Sabrina ad Abra) | 2-5 Pokémon, quelle di R/B |
| Mosse | regola automatica più poche speciali; ci sono stranezze (Abra con solo Flash, Starmie con Harden) ma anche colpi forti: Raichu Thunderbolt/Mega Kick, Venonat Psychic, Giovanni Thunder, Fissure, Horn Drill, Rock Slide | set pensati a mano e coerenti (Calm Mind, Future Sight, Baton Pass, Recover, Toxic), ma la potenza grezza è spesso più bassa: Shock Wave 60 al posto di Thunderbolt, nessuna OHKO |
| Strumenti | nessuno | cure (Super/Hyper Potion, Full Heal) |
| IV | equivalenti a 16-18 | 0 |

**Lettura onesta:** le mosse di FRLG sono più *sensate*, non più *forti*.
Da Koga in poi i capi di Giallo hanno 7-8 livelli in più e mosse più potenti.
FRLG dà sfide più strategiche (Calm Mind con Baton Pass, Minimize con Acid
Armor, cure) e una curva regolare. Il vantaggio maggiore però è per Red: con
FRLG come riferimento hanno senso i learnset di Gen 3 e le MT dei capi di FRLG
(MT39 Rock Tomb, MT03 Water Pulse, MT34 Shock Wave, MT19 Giga Drain, MT06
Toxic, MT04 Calm Mind, MT38 Fire Blast, MT26 Earthquake: `pokefirered`
`data/maps/<Città>_Gym/scripts.inc`), quindi c'è più margine nello scegliere
le mosse di Red.

La disponibilità dei membri di Red resta quella di Giallo (in FRLG non c'è
Pikachu starter né i tre starter regalati): la scelta FRLG sarebbe un ibrido,
con disponibilità da Giallo e capi e mosse da FRLG. Con FRLG i livelli di Red
sotto vanno rifatti sulla curva 14 → 50.

Per l'aspetto, gli sprite FRLG dei capi (già compilati) vanno d'accordo con
entrambe le scelte.

## La squadra di Red (proposta)

Regola dei livelli, inventata e da tarare: Pikachu, l'asso come al Monte
Argento, sta a +2 circa sull'asso del capo; gli altri scalano per ordine di
arrivo. Il salto fra Erika (32) e Koga (50) è di Giallo, non nostro.

| Sfida | Squadra |
| --- | --- |
| 1 Brock | Pikachu 14 |
| 2 Misty | Pikachu 22, Ivysaur 19, Charmeleon 18 |
| 3 Surge | Pikachu 28, Ivysaur 25, Charmeleon 25 |
| 4 Erika | Pikachu 33, Ivysaur 30, Charmeleon 30, Wartortle 26, Eevee 26 |
| 5 Koga | Pikachu 50, Venusaur 46, Charizard 46, Blastoise 43, Eevee 40, Snorlax 38 |
| 6 Sabrina | Pikachu 52, Venusaur 48, Charizard 48, Blastoise 46, Eevee 43, Snorlax 44 |
| 7 Blaine | Pikachu 55, Venusaur 51, Charizard 51, Blastoise 50, Eevee 46, Snorlax 48 |
| 8 Giovanni | Pikachu 58, Venusaur 54, Charizard 54, Blastoise 53, Eevee 50, Snorlax 51 |

**Le mosse di Red non si possono calcolare con la regola automatica.**
`red.py` lo mostra: col "tiene le ultime quattro" il Pikachu Lv14 perde
Thunder Shock e resta con Growl, Tail Whip, Thunder Wave, Quick Attack; a
Lv50 Charizard ha ancora Leer e Rage. Un giocatore sceglie. Proposta di
regola: **le quattro mosse migliori fra learnset di Giallo fino a quel livello,
MT ricevute dai capi già battuti e MN già ottenute a quel punto** (Taglio,
Volo, Surf, Forza, Flash), scelte a mano e annotate qui. È una scelta
editoriale, non un dato.

### Il problema di Brock

Con soli membri canonici la prima sfida è **Pikachu da solo contro Geodude e
Onix**: Elettro non tocca Terra, Quick Attack e Slam sono resistite, Thunder
Wave fallisce. In Giallo nessuno batte Brock col solo Pikachu (si usano
Mankey, Nidoran o Butterfree). Strade possibili, da decidere dopo una prova in
emulatore:

1. Pikachu da solo a un livello più alto, da misurare (fedele nella squadra,
   meno nel livello);
2. un compagno "di passaggio" catturabile prima di Brock in Giallo, poi messo
   nel box (fedele al viaggio, non alla squadra canonica);
3. accettare che sia la sfida più dura delle otto.

Vale anche la forma: HnS al Monte Argento usa `Pikachu-Starter` (statistiche
base più alte). Usarla qui dà continuità e cambia il bilanciamento di Brock.

## Come si costruisce (agganci verificati nel codice)

| Pezzo | Dove | Nota |
| --- | --- | --- |
| squadre di Red | `src/data/battle_partners.party` | stesso formato dei `.party`, ha già `Back Pic:`; 8 voci `PARTNER_RED_*` |
| caricare la squadra | nuova `FillRedsPathParty` accanto a `FillPartnerParty` (`src/battle_partner.c`) | quella esistente riempie solo gli slot 3-5 con al massimo 3 Pokémon: serve una variante per 0-5 |
| salvare/ripristinare la squadra del giocatore | `SavePlayerParty` / `LoadPlayerParty` | come la Battle Frontier; la copia va in `gSaveBlock1Ptr->playerParty` in RAM, la flash non cambia finché non si salva |
| niente esperienza | `FLAG_DISABLE_EXP_GAIN` (già esiste, `battle_script_commands.c:4147`) | |
| niente zaino | `B_VAR_NO_BAG_USE` | oggi è 0 (spento): serve una var libera |
| sconfitta senza svenire | `B_FLAG_NO_WHITEOUT` | oggi è 0 (spento): serve un flag libero; la squadra va ripristinata comunque |
| level scaling | `IsTrainerExcludedFromScaling` (`src/level_scaling.c`) | queste lotte vanno escluse, o con l'opzione accesa Brock diventa Lv60 |
| sprite di schiena di Red | `TRAINER_PIC_BACK_RED` (FRLG, compilato anche in HnS) | da capire dove la lotta sceglie la schiena del giocatore |
| aspetto dei capi "di allora" | sprite FRLG compilati: `TRAINER_PIC_FRONT_LEADER_BROCK_FRLG`, `..._KOGA_FRLG`, `..._GIOVANNI_FRLG`, … | Koga capopalestra e Giovanni di Smeraldopoli esistono solo in FRLG |
| stato | 8 flag (una per sfida) più quelle dei premi | `ChallengeSettings` e SaveBlock3 non si toccano |

### Il limite degli id allenatore (da risolvere prima di scrivere)

`src/tcloner.c:43` ha
`STATIC_ASSERT(TRAINERS_COUNT <= TCLONER_TRAINER_FLAGS_BYTES * 8)`, con 82 byte
= 656 bit, e oggi `TRAINERS_COUNT_HNS` è 653. **Otto allenatori nuovi non
compilano.** Strade:

1. un solo id `TRAINER_REDS_PATH_HNS` (653 → 654, ci sta) con squadra, nome e
   sprite scelti da una var al momento della lotta (come fa la Frontier);
2. allargare lo spazio del TcloneR, se nella struct `Pokedex` ce n'è
   (`include/constants/tcloner.h`): da misurare;
3. riusare id di allenatori irraggiungibili, se ce ne sono: da verificare.

La 1 è la mia preferita: non tocca il TcloneR e le flag per sfida servono
comunque a parte. In tutti i casi il TcloneR non deve agire in queste lotte.

### Flusso dello script (bozza)

1. l'NPC propone la sfida N (solo se il capo di HnS è battuto e la sfida N no);
2. `SavePlayerParty`, carica `PARTNER_RED_N`, accende i flag/var di lotta;
3. `trainerbattle` contro il capo di Gen 1;
4. **in ogni caso** `LoadPlayerParty` e spegne flag/var;
5. vittoria → flag della sfida, premio.

Rischi da provare in emulatore: sconfitta, fuga dal menu, soft reset a metà
lotta (la flash deve avere ancora la squadra vera), evoluzioni a fine lotta
(con `FLAG_DISABLE_EXP_GAIN` non dovrebbero partire, da vedere), Pokédex (i
Pokémon prestati non devono risultare catturati).

## Domande aperte

1. **Quando si sblocca**: dopo il capo di HnS corrispondente o dopo la
   seconda Lega?
2. **Capi palestra**: Giallo o FRLG (confronto sopra).
3. **Brock**: quale delle tre strade, dopo una prova in emulatore (con
   `Pikachu-Starter` il conto cambia, ma Elettro contro Terra resta zero).
4. **Mosse di Red**: va bene la regola "migliori quattro fra learnset, MT e
   MN disponibili"?
5. **Premi**: asso shiny del capo, soldi o altro; per sfida e per
   completamento.

## Fonti

- `pret/pokeyellow` @ `b6696f24b3be84fe64cf702f5a15ba40c66b7f39`: squadre,
  mosse speciali, learnset, DV, regali, MT dei capi.
- `pret/pokecrystal`, `data/trainers/parties.asm`: squadra di Red al Monte
  Argento in Gen 2.
- `pret/pokered`, `data/trainers/parties.asm`: squadre di R/B, per confronto
  (Brock 12/14, Surge 21/18/24, …).
- `pret/pokefirered`, `src/data/trainer_parties.h`: squadre dei capi di FRLG
  (stesse di `src/data/trainers_frlg.party`).

Gli script `reds-path/gen1moves.py` e `reds-path/red.py` rifanno i calcoli da
un checkout di `pokeyellow`.
