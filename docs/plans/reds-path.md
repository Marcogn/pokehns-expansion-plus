# Piano: Red's Path (le palestre di Kanto rigiocate con la squadra di Red)

**Stato (23/10): piano, nessun codice.** Prima gli avversari (sezione 1,
pronta da rivedere), poi la squadra di Red capo per capo (sezione 2, da
costruire insieme), poi il codice (sezione 4).

## L'idea

Dopo aver battuto un capo palestra di Kanto (o dopo la seconda Lega: da
decidere), un NPC propone la sfida "del viaggio di Red": il giocatore **usa la
squadra di Red di quel momento** contro **lo stesso capo palestra com'era
allora**. Niente catture, niente esperienza, niente zaino: la lotta e basta.
Ogni sfida riparte da zero, quindi l'ordine è libero.

## Decisioni prese

| Tema | Scelta |
| --- | --- |
| Chi usa la squadra di Red | il giocatore (squadra prestata) |
| Capi palestra | Brock, Misty, Surge, Erika, **Koga**, Sabrina, Blaine, **Giovanni** |
| Squadre dei capi | **FireRed/LeafGreen**, con poche variazioni motivate (sezione 1) |
| Squadra di Red | **uno starter** più Pokémon che avrebbe potuto ottenere strada facendo; può cambiare da un capo all'altro (Butterfree all'inizio, Poliwrath più avanti, …). Non più vincolata ai sei del Monte Argento |
| Mosse di Red | livello, MT e MN **ottenibili a quel punto della storia**; niente eccezioni salvo problemi seri |
| Pikachu | se c'è, è `Pikachu-Starter` (base 45/80/50/75/60/120) |
| Espeon | mai: solo al Monte Argento |
| Ordine | libero |
| Zaino | nessuno |
| Premi | l'**asso** di ogni capo, **shiny, Lv70 circa** |
| Storia | non ancora scritta: qui c'è solo la "ciccia" degli script |

## 1. Gli avversari

### Base: le squadre di FireRed/LeafGreen

Da `pret/pokefirered` `src/data/trainer_parties.h`, identiche a quelle che
expansion ha già in `src/data/trainers_frlg.party` (`TRAINER_LEADER_*`).
Specie e livelli sono quelli di Rosso/Blu (`pret/pokered`), tranne l'ultimo
Pokémon di Giovanni (Rhyhorn 50 in FRLG, Rhydon 50 in R/B). Nessuno strumento
tenuto; le cure sono quelle che il capo usa dalla borsa. IV 0. AI FRLG:
`Check Bad Move / Try To Faint / Check Viability`.

| Capo | Cure | Squadra (asso in grassetto) |
| --- | --- | --- |
| Brock | – | Geodude 12: Tackle, Defense Curl · **Onix 14**: Tackle, Bind, Rock Tomb |
| Misty | Super Potion | Staryu 18: Tackle, Harden, Recover, Water Pulse · **Starmie 21**: Swift, Recover, Rapid Spin, Water Pulse |
| Surge | Super Potion, Full Heal | Voltorb 21: Sonic Boom, Tackle, Screech, Shock Wave · Pikachu 18: Quick Attack, Thunder Wave, Double Team, Shock Wave · **Raichu 24**: idem |
| Erika | Hyper Potion, Full Heal | Victreebel 29: Stun Spore, Acid, Poison Powder, Giga Drain · Tangela 24: Poison Powder, Constrict, Ingrain, Giga Drain · **Vileplume 29**: Sleep Powder, Acid, Stun Spore, Giga Drain |
| Koga | Hyper Potion ×2, Full Heal | Koffing 37: Self-Destruct, Sludge, Smokescreen, Toxic · Muk 39: Minimize, Sludge, Acid Armor, Toxic · Koffing 37 (idem) · **Weezing 43**: Tackle, Sludge, Smokescreen, Toxic |
| Sabrina | Hyper Potion ×2, Full Heal | Kadabra 38: Psybeam, Reflect, Future Sight, Calm Mind · Mr. Mime 37: Barrier, Psybeam, Baton Pass, Calm Mind · Venomoth 38: Psybeam, Gust, Leech Life, Supersonic · **Alakazam 43**: Psychic, Recover, Future Sight, Calm Mind |
| Blaine | Hyper Potion ×2, Full Heal | Growlithe 42: Bite, Roar, Take Down, Fire Blast · Ponyta 40: Stomp, Bounce, Fire Spin, Fire Blast · Rapidash 42: idem · **Arcanine 47**: Bite, Roar, Take Down, Fire Blast |
| Giovanni | Hyper Potion ×2, Full Heal | Rhyhorn 45: Take Down, Rock Blast, Scary Face, Earthquake · Dugtrio 42: Slash, Sand Tomb, Mud-Slap, Earthquake · Nidoqueen 44: Body Slam, Double Kick, Poison Sting, Earthquake · Nidoking 45: Thrash, Double Kick, Poison Sting, Earthquake · **Rhyhorn 50**: come il primo |

### Variazioni proposte

1. **Giovanni: l'ultimo Rhyhorn 50 diventa Rhydon 50**, come in Rosso/Blu.
   Stesse mosse (le impara entrambi). È anche l'asso che si vince: un Rhyhorn
   Lv70 sarebbe strano, visto che evolve al 42.
2. **Abilità scritte a mano.** FRLG non le specifica, ed expansion allora le
   tira con `personalityHash % 3` (`src/battle_main.c:2195`). Così può uscire
   l'abilità nascosta, che in Gen 3 non esisteva (Arena Trap va bene, ma
   Sheer Force a Nidoking, Magic Guard ad Alakazam, Analytic a Starmie no).
   Proposta, solo abilità che il Pokémon aveva in Gen 3:

   | Pokémon | Abilità | Perché |
   | --- | --- | --- |
   | Geodude | Rock Head | non ha mosse con contraccolpo: nessun effetto |
   | **Onix** | **Sturdy** | in expansion (`B_STURDY` Gen 5+) lascia l'asso a 1 PS da PS pieni: un po' di sfida all'inizio. In alternativa Rock Head |
   | Staryu, Starmie | Natural Cure | coerente con Recover |
   | Voltorb, Pikachu, Raichu | Static | |
   | Victreebel, Tangela, Vileplume | Chlorophyll | l'unica di Gen 3 |
   | Koffing, Weezing | Levitate | l'unica di Gen 3 |
   | Muk | Stench | in Gen 5+ dà il 10% di tentennamento |
   | Kadabra, Alakazam | Synchronize | |
   | Mr. Mime | Soundproof | l'unica di Gen 3 |
   | Venomoth | Shield Dust | l'unica di Gen 3 |
   | Growlithe, Arcanine | Intimidate | |
   | Ponyta, Rapidash | Flash Fire | |
   | Rhyhorn, Rhydon | Rock Head | annulla il contraccolpo di Take Down |
   | **Dugtrio** | **Arena Trap** | legale in Gen 3, ed è la firma di Dugtrio; impedisce di cambiare |
   | Nidoqueen, Nidoking | Poison Point | l'unica di Gen 3 |

3. **IV**: restano 0 come in FRLG finché non c'è la squadra di Red; si
   alzano solo se le lotte risultano troppo facili in emulatore.

Nient'altro, finché le prove non dicono il contrario.

### Cosa cambia comunque rispetto a FRLG (meccaniche del repo)

Le meccaniche sono quelle di expansion con la config di questo repo
(`include/config/battle.h`, `pokemon.h`). Cose che toccano queste squadre:

| Cosa | FRLG | Qui |
| --- | --- | --- |
| Sturdy | blocca solo le mosse OHKO | resta a 1 PS da PS pieni (`B_STURDY`) |
| Rock Tomb | 50 di potenza, 80% di precisione | 60, 95% (`B_UPDATED_MOVE_DATA`) |
| Giga Drain | 60 | 75 |
| Fire Blast | 120 | 110 |
| Self-Destruct | dimezza la Difesa del bersaglio | non la dimezza più (più debole) |
| Rock Blast | 80% | 90% |
| Bind, Fire Spin, Sand Tomb, Constrict | 1/16 per 2-5 turni | 1/8 per 4-5 turni (`B_BINDING_*`) |
| Minimize | +1 elusione | +2 (`B_MINIMIZE_EVASION`) |
| Toxic | può mancare | **uguale**: questo repo tiene `B_TOXIC_NEVER_MISS` a `GEN_3` |
| Alakazam | Dif. Sp. base 85 | 95 (`P_UPDATED_STATS`) |

## 2. La squadra di Red (da costruire capo per capo)

Metodo: per ogni capo, l'elenco di cosa è ottenibile **prima** di quella
palestra in FRLG (selvatici da `pokefirered` `wild_encounters.json`, regali e
scambi in gioco dagli script), poi si sceglie insieme la squadra e le mosse
legali a quel punto.

Prima di Brock, verificato (FR e LG uguali, salvo dove indicato):

| Dove | Pokémon (livelli) |
| --- | --- |
| Percorso 1 | Pidgey 2-5, Rattata 2-4 |
| Percorso 2 | Pidgey 2-5, Rattata 2-5, Caterpie 4-5, Weedle 4-5 |
| Percorso 22 | Mankey 2-5, Rattata 2-5, Spearow 3-5 |
| Bosco Smeraldo | Caterpie, Metapod, Weedle, Kakuna, Pikachu 3-5 |

Butterfree (Metapod al 7, Butterfree al 10) e Beedrill (Kakuna al 7,
Beedrill al 10) sono raggiungibili prima di Brock. Pesca e Surf a Smeraldopoli
e sul Percorso 22 vogliono canne o MN che a quel punto non ci sono.

Da decidere: **quale starter** (Pikachu-Starter, o Bulbasaur, Charmander o
Squirtle come in FRLG) e a che livello sta Red a ogni palestra.

## 3. Premi

L'asso di ogni capo, **shiny, Lv70 circa**, alla prima vittoria:

| Capo | Asso |
| --- | --- |
| Brock | Onix |
| Misty | Starmie |
| Surge | Raichu |
| Erika | Vileplume |
| Koga | Weezing |
| Sabrina | Alakazam |
| Blaine | Arcanine |
| Giovanni | Rhydon (variazione 1) |

Da decidere: mosse (il set FRLG del capo o quelle di livello al 70), natura e
IV, Poké Ball, nome dell'allenatore originale (il capo o il giocatore). Per la
consegna con la squadra piena c'è già il percorso del TcloneR
(`TClonerGiveRewardMon`, che manda al PC).

## 4. Come si costruisce (agganci verificati nel codice)

| Pezzo | Dove | Nota |
| --- | --- | --- |
| squadre di Red | `src/data/battle_partners.party` | stesso formato dei `.party`, ha già `Back Pic:`; 8 voci `PARTNER_RED_*` |
| caricare la squadra | nuova `FillRedsPathParty` accanto a `FillPartnerParty` (`src/battle_partner.c`) | quella esistente riempie solo gli slot 3-5 con al massimo 3 Pokémon |
| salvare/ripristinare la squadra del giocatore | `SavePlayerParty` / `LoadPlayerParty` | come la Battle Frontier; la copia sta in `gSaveBlock1Ptr->playerParty` in RAM, la flash non cambia finché non si salva |
| niente esperienza | `FLAG_DISABLE_EXP_GAIN` (già esiste, `battle_script_commands.c:4147`) | |
| niente zaino | `B_VAR_NO_BAG_USE` | oggi è 0 (spento): serve una var libera |
| sconfitta senza svenire | `B_FLAG_NO_WHITEOUT` | oggi è 0 (spento): serve un flag libero; la squadra va ripristinata comunque |
| level scaling | `IsTrainerExcludedFromScaling` (`src/level_scaling.c`) | queste lotte vanno escluse, o con l'opzione accesa Brock diventa Lv60 |
| sprite di schiena di Red | `TRAINER_PIC_BACK_RED` (FRLG, compilato anche in HnS) | da capire dove la lotta sceglie la schiena del giocatore |
| aspetto dei capi | sprite FRLG già compilati (`TRAINER_PIC_FRONT_LEADER_*_FRLG`) | Koga capopalestra e Giovanni di Smeraldopoli esistono solo in FRLG |
| stato | 8 flag (sfida vinta / premio dato) | `ChallengeSettings` e SaveBlock3 non si toccano |

### Il limite degli id allenatore (da risolvere prima di scrivere)

`src/tcloner.c:43` ha
`STATIC_ASSERT(TRAINERS_COUNT <= TCLONER_TRAINER_FLAGS_BYTES * 8)`, con 82 byte
= 656 bit, e oggi `TRAINERS_COUNT_HNS` è 653. **Otto allenatori nuovi non
compilano.** Strade:

1. un solo id `TRAINER_REDS_PATH_HNS` (653 → 654, ci sta), con squadra, nome
   e sprite scelti da una var al momento della lotta (come fa la Frontier);
2. allargare lo spazio del TcloneR, se nella struct `Pokedex` ce n'è
   (`include/constants/tcloner.h`): da misurare;
3. riusare id di allenatori irraggiungibili, se ce ne sono: da verificare.

La 1 è la mia preferita. In tutti i casi il TcloneR non deve agire in queste
lotte.

### Flusso dello script (bozza)

1. l'NPC propone la sfida N (solo se il capo di HnS è battuto);
2. `SavePlayerParty`, carica `PARTNER_RED_N`, accende i flag/var di lotta;
3. `trainerbattle` contro il capo di FRLG;
4. **in ogni caso** `LoadPlayerParty` e spegne flag/var;
5. prima vittoria → flag e premio.

Rischi da provare in emulatore: sconfitta, soft reset a metà lotta (la flash
deve avere ancora la squadra vera), evoluzioni a fine lotta, Pokédex (i
Pokémon prestati non devono risultare catturati).

## Domande aperte

1. **Quando si sblocca**: dopo il capo di HnS corrispondente o dopo la
   seconda Lega?
2. **Variazioni** della sezione 1: vanno bene Rhydon, la tabella delle
   abilità (in particolare Sturdy a Onix e Arena Trap a Dugtrio) e gli IV a 0?
3. **Starter** di Red.
4. **Dettagli dei premi** (sezione 3).

## Scartato: i capi di Giallo

Prima proposta, superata dalla scelta FRLG. Squadre da `pret/pokeyellow`
`data/trainers/parties.asm` con le mosse speciali di
`data/trainers/special_moves.asm` (`reds-path/gen1moves.py` rifà il calcolo
delle mosse di livello). Assi 12, 21, 28, 32, 50, 50, 54, 55: da Koga in poi
7-8 livelli sopra FRLG e mosse più forti (Thunderbolt, Psychic, Thunder,
Fissure), ma set meno sensati (Abra con la sola Flash) e nessuna cura. In
Giallo Bulbasaur, Charmander, Squirtle ed Eevee sono regalati e Snorlax è al
Lv30 (`scripts/CeruleanMelaniesHouse.asm`, `Route24.asm`,
`VermilionCity_2.asm`, `CeladonMansionRoofHouse.asm`, `Route12.asm`): utile
se un giorno si volesse la squadra del Monte Argento.

## Fonti

- `pret/pokefirered`: `src/data/trainer_parties.h` (capi),
  `src/data/wild_encounters.json` (selvatici),
  `data/maps/<Città>_Gym/scripts.inc` (MT dei capi: 39 Rock Tomb, 03 Water
  Pulse, 34 Shock Wave, 19 Giga Drain, 06 Toxic, 04 Calm Mind, 38 Fire Blast,
  26 Earthquake).
- `pret/pokered`, `data/trainers/parties.asm`: squadre di R/B.
- `pret/pokeyellow` @ `b6696f24`: sezione scartata.
- `pret/pokecrystal`, `data/trainers/parties.asm`: squadra di Red al Monte
  Argento.
