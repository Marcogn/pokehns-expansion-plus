# Analisi: Red's Path (archivio)

Materiale di ricerca da cui sono uscite le decisioni di
[`docs/plans/reds-path.md`](../plans/reds-path.md). **Non serve per
implementare**: il piano contiene già i dati finali. Utile per la taratura
(sessione C) o se si rimette in discussione una scelta.

## Meccaniche del repo che cambiano le lotte rispetto a FRLG

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

## Le squadre di Red, capo per capo: cosa era ottenibile e perché

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

Learnset: quelli di FRLG (`pokefirered` `src/data/pokemon/level_up_learnsets.h`),
non quelli moderni di expansion. **Prima di Brock non ci sono MT né MN
utilizzabili**: nelle mappe raggiungibili (Biancavilla, Percorsi 1, 2 e 22,
Smeraldopoli, Bosco Smeraldo, Plumbeopoli) l'unica è la MN05 Flash
dell'assistente di Oak sul Percorso 2, inutile qui.

Mosse utili entro il Lv15 (FRLG):

| Pokémon | Mosse |
| --- | --- |
| Charmander | Scratch, Growl · 7 Ember · **13 Metal Claw** (Acciaio: superefficace su Geodude e Onix) |
| Mankey | Scratch, Leer · 6 Low Kick · 11 Karate Chop |
| Butterfree (dal 10) | Confusion · 13 Poison Powder · 14 Stun Spore · 15 Sleep Powder |
| Beedrill (dal 10) | Fury Attack · 15 Focus Energy |
| Pidgey | Tackle · 5 Sand Attack · 9 Gust · 13 Quick Attack |
| Rattata | Tackle, Tail Whip · 7 Quick Attack · 13 Hyper Fang |
| Spearow | Peck, Growl · 7 Leer · 13 Fury Attack |
| Pikachu | Thunder Shock, Growl · 6 Tail Whip · 8 Thunder Wave · 11 Quick Attack · 15 Double Team |

### Brock (approvata)

Brock ha due Pokémon (Geodude 12, Onix 14 con Rock Tomb e Sturdy). **Tre**
Pokémon per Red, livelli fra 12 e 14 come un giocatore che arriva a
Plumbeopoli.

| Pokémon | Lv | Mosse | Abilità (Gen 3) |
| --- | --- | --- | --- |
| Charmander | 14 | Scratch, Growl, Ember, Metal Claw | Blaze |
| Mankey | 12 | Scratch, Leer, Low Kick, Karate Chop | Vital Spirit |
| Butterfree | 12 | Tackle, String Shot, Harden, Confusion | Compound Eyes |

Perché: Charmander è il protagonista e con Metal Claw può vincere da solo come
in *Origins*; Mankey è la risposta classica a Brock; Butterfree copre con
Confusion ma soffre Rock Tomb. Le alternative (Beedrill, Pidgey, Rattata,
Spearow, Pikachu) sono deboli contro Roccia/Terra: vanno bene più avanti.
Da tarare in emulatore.

Natura e IV dei Pokémon di Red (approvato): **natura neutra e IV 15** per
tutti, così la difficoltà dipende da livelli e mosse e non dal caso.

### Fra Brock e Misty: cosa si ottiene

Mappe raggiungibili prima della palestra di Misty, senza medaglie nuove:
Percorso 3, Monte Luna, Percorso 4, Celestopoli, Percorsi 24 e 25 (fino alla
casa di Bill). Pesca e Surf no: l'Amo Vecchio è ad Aranciopoli.

| Dove | Pokémon (livelli) |
| --- | --- |
| Percorso 3 | Pidgey 6-7, Spearow 6-8, Jigglypuff 3-7, Mankey 7, Nidoran♀ 6-7, Nidoran♂ 6-7 |
| Monte Luna | Zubat 7-11, Geodude 7-10, Paras 5-12, Clefairy 8-12 |
| Percorso 4 | Rattata 8-12, Spearow 8-12, Mankey 10-12, **Ekans 6-12 (FR)** / Sandshrew 6-12 (LG) |
| Percorsi 24-25 | Pidgey 11-13, Abra 8-13, Caterpie, Metapod, Weedle, Kakuna 7-9, **Oddish 12-14 (FR)** / Bellsprout 12-14 (LG) |
| Centro Pokémon del Percorso 4 | Magikarp Lv5 (venditore) |

Deciso: **Red gioca FireRed** (il nome lo dice), quindi Oddish ed Ekans sì,
Bellsprout e Sandshrew no.

Altro: lo scambio di Celestopoli (`INGAME_TRADE_JYNX`) vuole un Poliwhirl, che
qui non c'è; i fossili del Monte Luna si rianimano solo a Cinnabar.

MT a disposizione:
- **MT39 Rock Tomb** (da Brock): la impara Mankey, non Charmander/Charmeleon
  in Gen 3 (`tmhm_learnsets.h`);
- **MT28 Dig** (Celestopoli, dal Rocket dietro la casa svaligiata): il
  poliziotto si sposta con `FLAG_GOT_SS_TICKET`, cioè dopo Bill, che è
  raggiungibile prima di Misty. La imparano Charmander/Charmeleon, Mankey,
  Pikachu; non Butterfree né Oddish;
- da terra (script `..._ItemTMxx` in `map.json`, sfuggiti alla prima
  ricerca): **MT09 Bullet Seed** (Monte Luna 1F; la impara Oddish), MT46
  Thief (Monte Luna B2F), MT05 Roar (Percorso 4), MT45 Attract (Percorso 24),
  MT43 Secret Power (Percorso 25).

Mosse di livello (FRLG) utili fino al Lv24:

| Pokémon | Mosse |
| --- | --- |
| Charmeleon (dal 16) | Scratch, Growl, Ember · 13 Metal Claw · 20 Smokescreen |
| Pikachu | Thunder Shock, Growl · 6 Tail Whip · 8 Thunder Wave · 11 Quick Attack · 15 Double Team · 20 Slam |
| Oddish (Gloom al 21) | Absorb · 7 Sweet Scent · 14 Poison Powder · 16 Stun Spore · 18 Sleep Powder · 23 Acid |
| Butterfree | Confusion · 13 Poison Powder · 14 Stun Spore · 15 Sleep Powder · 18 Supersonic · 23 Whirlwind |
| Mankey (Primeape al 28) | Scratch, Leer · 6 Low Kick · 11 Karate Chop · 16 Fury Swipes · 21 Focus Energy |
| Paras | Scratch · 7 Stun Spore · 13 Poison Powder · 19 Leech Life |
| Kadabra (Abra al 16) | Teleport, Kinesis, Confusion · 18 Disable · 21 Psybeam · 23 Reflect |
| Clefairy | Pound, Growl · 5 Encore · 9 Sing · 13 Double Slap · 17 Follow Me · 21 Minimize |
| Nidoran♂ / ♀ | Double Kick al 12, Poison Sting al 17 |
| Ekans | Wrap, Leer · 8 Poison Sting · 13 Bite · 20 Glare |

### Misty (approvata, Charmander evoluto)

Misty: Staryu 18 e Starmie 21 (Natural Cure, Recover, Water Pulse).
**Quattro** Pokémon, livelli 18-21.

| Pokémon | Lv | Mosse | Abilità (Gen 3) |
| --- | --- | --- | --- |
| Charmeleon | 21 | Scratch, Ember, Metal Claw, Smokescreen | Blaze |
| Pikachu-Starter | 20 | Thunder Shock, Thunder Wave, Quick Attack, Slam | Static |
| Oddish | 19 | Bullet Seed, Poison Powder, Stun Spore, Sleep Powder | Chlorophyll |
| Butterfree | 18 | Confusion, Poison Powder, Stun Spore, Sleep Powder | Compound Eyes |

Perché:
- **Pikachu entra qui**: nel Bosco Smeraldo c'era già, e contro l'acqua è la
  risposta naturale. È anche il primo membro "canonico" dopo lo starter.
- **Oddish** (FireRed, Percorsi 24-25) porta l'erba.
- **Charmeleon** resta l'asso per livello ma qui è in svantaggio: la sfida
  sta nel non doverlo usare in prima linea.
- **Mankey va in panchina**: contro Misty non serve, ed è il primo cambio di
  squadra.
- **Attenzione**: Butterfree con Compound Eyes porta Sleep Powder dal 75% a
  circa il 97%. Contro due soli Pokémon può rendere la lotta banale; se in
  emulatore succede, si toglie Sleep Powder a uno dei due.

Variante "Origins" (Charmander non evoluto): scartata.

Approvato: Oddish ha **Bullet Seed** (MT09) al posto di Absorb, quindi
Bullet Seed, Poison Powder, Stun Spore, Sleep Powder.

### Fra Misty e Surge: cosa si ottiene

Mappe della storia prima della palestra di Surge: Percorsi 5 e 6, Sotterraneo,
Aranciopoli, M/N Anna (dove si prende **Taglio**, necessario per entrare
nella palestra), Percorso 11 con la Grotta Diglett, e con Taglio i Percorsi 2
est e 9. Tunnel Roccioso (Rock Tunnel) e Lavandonia no: in storia vengono dopo Surge.

| Dove | Pokémon (livelli) |
| --- | --- |
| Percorsi 5-6 | Pidgey 13-16, Meowth 10-16, **Oddish 13-16 (FR)** / Bellsprout (LG) |
| Percorso 9 (con Taglio) | Rattata 14-17, Spearow 13-17, **Ekans 11-17 (FR)** / Sandshrew (LG) |
| Percorso 11 | Spearow 13-17, Drowzee 11-15, **Ekans 12-15 (FR)** / Sandshrew (LG) |
| Grotta Diglett | **Diglett 15-22**, Dugtrio 29-31 |
| Amo Vecchio (Aranciopoli) | Magikarp |

Scambi in gioco (`INGAME_TRADE_*`): Nidoran (ingresso nord del Sotterraneo),
Farfetch'd per uno Spearow (Aranciopoli), Nidorino/Nidorina (Percorso 11),
Mr. Mime per un Abra (casa del Percorso 2, con Taglio).

MT: **MT31 Brick Break** e MT44 Rest (M/N Anna), **MT40 Aerial Ace**
(Percorso 9, con Taglio), MN01 Cut. La MT34 Shock Wave arriva da Surge,
quindi dopo.

Mosse utili (FRLG, fino al Lv30):

| Pokémon | Livello | MT compatibili qui |
| --- | --- | --- |
| Charmeleon | 20 Smokescreen · 27 Scary Face | Dig, Brick Break, Aerial Ace, Cut |
| Pikachu | 20 Slam · **26 Thunderbolt** | Dig, Brick Break |
| Oddish / Gloom (21) | 18 Sleep Powder · 23/24 Acid | Bullet Seed, Cut |
| Mankey (Primeape 28) | 16 Fury Swipes · 21 Focus Energy · 26 Seismic Toss | Dig, Brick Break, Rock Tomb, Aerial Ace |
| Diglett (Dugtrio 26) | Sand Attack, Scratch · 5 Growl · 9 Magnitude · **17 Dig** · 21 Fury Swipes · 25 Mud-Slap | Rock Tomb, Aerial Ace, Cut |
| Butterfree | 18 Supersonic · 23 Whirlwind · 28 Gust | Aerial Ace |
| Drowzee | Pound, Hypnosis · 7 Disable · 11 Confusion · 17 Headbutt · 21 Poison Gas | Brick Break |
| Meowth | 10 Bite · 18 Pay Day · 25 Faint Attack | Dig, Aerial Ace |

### Surge (approvata)

Surge: Voltorb 21 (Sonic Boom, Screech, Shock Wave), Pikachu 18, Raichu 24
(Thunder Wave, Double Team, Shock Wave); tutti con **Static** (paralisi al
contatto). **Quattro** Pokémon, livelli 22-25.

| Pokémon | Lv | Mosse | Abilità (Gen 3) |
| --- | --- | --- | --- |
| Charmeleon | 25 | Ember, Metal Claw, Dig, Brick Break | Blaze |
| Diglett | 22 | Sand Attack, Magnitude, Dig, Fury Swipes | Arena Trap |
| Gloom | 22 | Bullet Seed, Poison Powder, Stun Spore, Sleep Powder | Chlorophyll |
| Pikachu-Starter | 23 | Thunder Shock, Thunder Wave, Quick Attack, Slam | Static |

Perché:
- **Diglett entra qui**: immune all'Elettro, e la Grotta Diglett è proprio
  accanto ad Aranciopoli. Resta Diglett (evolve al 26).
- **Charmeleon** con Dig (MT28) colpisce tutti e tre di superefficace.
- **Gloom** (Oddish evoluto al 21) resiste all'Elettro.
- **Pikachu** resta: contro Surge non brilla, ma è il compagno fisso.
- **Butterfree e Mankey in panchina**: Butterfree è debole all'Elettro.
  Mankey con Dig e Brick Break sarebbe un'alternativa a Pikachu.
- **Attenzione a Dig**: fa contatto, quindi rischia la paralisi di Static.
  Fa parte della sfida.

### Fra Surge ed Erika: cosa si ottiene

Storia: Percorsi 9 e 10, Tunnel Roccioso (con Flash), Lavandonia, Percorso 8,
sotterraneo 7-8, Percorso 7, Azzurropoli. Il Covo Rocket sotto il Casinò e la
Torre Pokémon (che vuole lo Spettrosonda del Covo) restano fuori: in storia si
fanno attorno a Erika, e per la sfida li mettiamo dopo. Pesca: solo Magikarp
(Amo Vecchio).

| Dove | Pokémon (FireRed) |
| --- | --- |
| Percorso 10 | Spearow 13-17, Ekans 11-17, Voltorb 14-17 |
| Tunnel Roccioso | Zubat 15-16, Geodude 15-17, Machop 16-17, Mankey 16-17, Onix 13-17 |
| Percorso 8 | Pidgey 18-20, Meowth 18-20, Ekans 17-19, **Growlithe 15-18** |
| Percorso 7 | Pidgey 19-22, Meowth 17-20, Oddish 19-22, Growlithe 18-20 |
| Villa di Azzurropoli (tetto) | **Eevee Lv25** (regalo) |
| Casinò, premi (FR; serve il Portagettoni, che si trova al ristorante) | Abra 9, Clefairy 8, Dratini 18, Scyther 25, Porygon 26 |

MT:
- **Centro Commerciale** (2° piano, in vendita): MT05 Roar, MT15 Hyper Beam,
  MT28 Dig, MT31 Brick Break, MT43 Secret Power, MT45 Attract;
- **tetto del Centro Commerciale** (bibite alla bambina): MT16 Light Screen,
  MT20 Safeguard, MT33 Reflect;
- **Casinò** (gettoni): MT13 Ice Beam, MT23 Iron Tail, **MT24 Thunderbolt**,
  MT30 Shadow Ball, **MT35 Flamethrower**;
- MN05 Flash (necessaria per il Tunnel Roccioso).

**Il punto di rottura sono le MT del Casinò**: legali, ma Flamethrower e Ice
Beam a questo punto stravolgono le sfide successive. Regola (approvata):
**Red non usa le MT e i Pokémon del Casinò** (nella storia al Casinò ci va per
il Team Rocket, non per giocare). Non serve comunque: Pikachu impara
Thunderbolt **di livello al 26** e Charmeleon Flamethrower al 34.

Mosse utili (FRLG):

| Pokémon | Livello | MT compatibili fra quelle sopra |
| --- | --- | --- |
| Charmeleon | 27 Scary Face · 34 Flamethrower | Dig, Brick Break, Aerial Ace, Iron Tail, Cut |
| Pikachu | 26 **Thunderbolt** · 33 Agility | Dig, Brick Break, Light Screen, Iron Tail, Flash |
| Butterfree | 23 Whirlwind · 28 **Gust** · 34 Psybeam | Aerial Ace, Safeguard, Hyper Beam |
| Eevee | Tackle, Tail Whip, Helping Hand · 8 Sand Attack · 16 Growl · 23 Quick Attack · 30 Bite | Dig, Iron Tail |
| Growlithe | Bite, Roar · 7 Ember · 13 Leer · 19 Odor Sleuth · 25 Take Down | Dig, Aerial Ace, Iron Tail |
| Machop | 13 Karate Chop · 19 Seismic Toss · 25 Revenge | Dig, Brick Break, Rock Tomb |
| Pidgeotto | 13 Quick Attack · 20 Whirlwind · 27 Wing Attack | Aerial Ace |

### Erika (approvata)

Erika: Victreebel 29, Tangela 24, Vileplume 29 (Chlorophyll; Giga Drain,
Sleep Powder, Stun Spore, Acid, Ingrain). **Quattro** Pokémon, livelli 25-30.

| Pokémon | Lv | Mosse | Abilità (Gen 3) |
| --- | --- | --- | --- |
| Charmeleon | 30 | Ember, Metal Claw, Scary Face, Dig | Blaze |
| Pikachu-Starter | 28 | Thunderbolt, Thunder Wave, Quick Attack, Slam | Static |
| Butterfree | 28 | Confusion, Gust, Stun Spore, Sleep Powder | Compound Eyes |
| Eevee | 25 | Tackle, Sand Attack, Growl, Quick Attack | Run Away |

Perché:
- **Eevee entra qui**: è il regalo della Villa di Azzurropoli, al Lv25 com'è
  in FRLG, e porta verso l'Espeon del Monte Argento (che resta solo lì).
  Contro Erika è debole: è il membro "di storia", non quello di forza.
- **Butterfree torna** dalla panchina: Volante e Psico sono superefficaci su
  tutti e tre (Gust impara al 28).
- **Pikachu ha Thunderbolt** di livello (26), non dal Casinò.
- **Gloom, Diglett e Mankey in panchina**: contro l'Erba non servono.
- Onestà: un giocatore con Charmander ha Erika facile anche in FRLG. Qui la
  difficoltà viene solo dalle polveri di Erika (sonno e paralisi) e da Giga
  Drain più forte (75). Se in prova è banale, si abbassa Charmeleon o si
  toglie Gust.

### Fra Erika e Koga: cosa si ottiene

Storia: Covo Rocket (Spettrosonda), Torre Pokémon, Mr. Fuji e il Poké Flauto,
gli Snorlax dei Percorsi 12 e 16, Percorsi 12-18 (Pista Ciclabile con la Bici),
Fuchsia con la Zona Safari. Zafferanopoli resta fuori: in storia viene dopo
Koga (Silph, Sabrina, dojo con Hitmonlee/Hitmonchan, Lapras).

| Dove | Pokémon (FireRed) |
| --- | --- |
| Torre Pokémon | Gastly 13-19, Haunter 20-25, Cubone 15-19 |
| Percorsi 12 / 16 | **Snorlax Lv30** (uno per percorso, `setwildbattle`) |
| Percorsi 12-15 | Oddish, Gloom 28-30, Pidgey, Pidgeotto 29, Venonat 24-26, Ditto 23-25 |
| Percorsi 16-18 | Rattata, Raticate, Spearow, Fearow, Doduo 18-28 |
| Zona Safari | Nidoran♂/♀, Nidorino, Nidorina, Exeggcute, Rhyhorn, Chansey, Kangaskhan, Scyther, Tauros, Venonat, Venomoth, Paras, Parasect, Doduo (22-33) |
| Amo Buono (casa di Fuchsia) | Poliwag 5-15 (Percorsi 6, 22, 25, Fuchsia, Safari), Goldeen, Horsea, Krabby |
| Super Amo (casa del Percorso 12) | **Poliwhirl 20-30** (Percorsi 6, 22, 25), Gyarados, Psyduck, Dratini/Dragonair (Safari) |

Pietre evolutive in vendita al Centro Commerciale di Azzurropoli (4° piano):
Fuoco, Foglia, Tuono, **Idro**. Quindi **Poliwrath è legale prima di Koga**:
Poliwhirl col Super Amo, Pietraidrica comprata ad Azzurropoli.

MT e MN nuove: MT12 Taunt, MT21 Frustration, MT49 Snatch (Covo Rocket), MT48
Skill Swap (Percorso 12), MT27 Return (portineria nord del Percorso 12), MT18
Rain Dance (Percorso 15), MT11 Sunny Day, MT47 Steel Wing, MT32 Double Team
(Zona Safari), **MN02 Fly** (casa del Percorso 16), **MN03 Surf** (casa
segreta della Safari), **MN04 Strength** (casa del Guardiano). La MT06 Toxic
arriva da Koga, quindi dopo.

Mosse utili (FRLG):

| Pokémon | Livello | MT/MN compatibili |
| --- | --- | --- |
| Charizard (36) | 34 Flamethrower · 36 **Wing Attack** · 44 Slash | Dig, Brick Break, Aerial Ace, Steel Wing, Return, Fly, Strength |
| Pikachu | 33 Agility · **41 Thunder** | Brick Break, Return, Rain Dance, Strength |
| Butterfree | 28 Gust · **34 Psybeam** · 40 Safeguard | Aerial Ace, Return, Double Team |
| Snorlax | 25 Rest · 29 Snore · **33 Body Slam** · 37 Sleep Talk · 41 Block | Brick Break, Rock Tomb, Return, Surf, Strength |
| Poliwrath | Water Gun, Hypnosis, Double Slap · **35 Submission** | **Surf**, Brick Break, Rock Tomb, Return, Strength |
| Eevee | 30 Bite · 36 Baton Pass | Dig, Return |
| Dugtrio (26) | 21 Fury Swipes · 25 Mud-Slap · 26 Sand Tomb · 38 Slash | Dig, Rock Tomb, Aerial Ace |

Abilità di Gen 3 utili qui: **Snorlax** Immunity o Thick Fat, **Poliwrath**
Water Absorb o **Damp** (blocca Self-Destruct ed Explosion).

### Koga (approvata)

Koga: Koffing 37, Muk 39, Koffing 37, Weezing 43 (Koffing e Weezing con
Levitate; Self-Destruct, Sludge, Smokescreen, Toxic, Minimize, Acid Armor).
**Sei** Pokémon, livelli 35-42.

| Pokémon | Lv | Mosse | Abilità (Gen 3) |
| --- | --- | --- | --- |
| Charizard | 42 | Flamethrower, Wing Attack, Scary Face, Dig | Blaze |
| Pikachu-Starter | 41 | Thunderbolt, Thunder, Thunder Wave, Quick Attack | Static |
| Butterfree | 38 | Psybeam, Gust, Stun Spore, Sleep Powder | Compound Eyes |
| Poliwrath | 37 | Surf, Hypnosis, Brick Break, Strength | **Damp** |
| Snorlax | 36 | Body Slam, Amnesia, Rest, Snore | **Immunity** |
| Eevee | 35 | Return, Bite, Quick Attack, Sand Attack | Run Away |

Perché:
- **Snorlax entra qui**: i due Snorlax bloccano le strade per Fuchsia, e Red
  ne ha uno al Monte Argento. Con Immunity Toxic non lo tocca.
- **Poliwrath entra qui**, come volevi: Poliwhirl col Super Amo e Pietraidrica
  (legali, vedi sopra). Damp spegne i Self-Destruct dei Koffing; Surf è la
  MN della Safari.
- **Charizard** al 36, come da evoluzione; Wing Attack di livello.
- **Butterfree** con Psybeam (34) è superefficace su tutti e quattro.
- **Eevee** resta: è il futuro Espeon del Monte Argento. Return (MT27) è la
  mossa più forte che ha a questo punto.
- **Panchina**: Dugtrio (Ground non tocca Koffing e Weezing per Levitate),
  Gloom, Mankey.
- Onestà: Immunity e Damp tolgono a Koga le sue due armi migliori, e sei
  contro quattro è tanto. Se in prova è facile: si toglie Eevee o si
  abbassano i livelli di 2-3.

### Fra Koga e Sabrina: cosa si ottiene

Storia: Zafferanopoli (le guardie lasciano passare col Tè della Villa di
Azzurropoli), Silph SpA, Dojo. Con Surf ci sono i Percorsi d'acqua, ma in
erba nuova solo Tangela (Percorso 21, che però porta a Cinnabar: dopo).

| Dove | Cosa |
| --- | --- |
| Silph SpA 7F | **Lapras Lv25** (regalo) |
| Dojo di Zafferanopoli | **Hitmonlee o Hitmonchan Lv25** (uno a scelta) |
| Surf ovunque | Tentacool 5-40 |

MT nuove: **MT06 Toxic** (da Koga), **MT29 Psychic** (casa di Mr. Psychic,
Zafferanopoli), MT01 Focus Punch (Silph 5F), MT08 Bulk Up (Silph 7F), MT41
Torment (Silph 4F). La MT04 Calm Mind arriva da Sabrina, quindi dopo.

Mosse utili (FRLG):

| Pokémon | Livello | MT nuove compatibili |
| --- | --- | --- |
| Charizard | 44 Slash | Focus Punch, Toxic |
| Pikachu | 41 Thunder | Focus Punch, Toxic |
| Snorlax | 37 **Sleep Talk** · 41 Block · 45 Covet | Psychic, Focus Punch, Toxic |
| Eevee | 30 **Bite** · 36 Baton Pass · 42 Take Down | Toxic |
| Butterfree | 34 Psybeam · 40 Safeguard | **Psychic**, Toxic |
| Poliwrath | 35 Submission | Bulk Up, Focus Punch, Psychic, Toxic |
| Lapras | 13 Body Slam · 19 Confuse Ray · 25 Perish Song · **31 Ice Beam** | Psychic, Toxic |

Nota su Sabrina in expansion: Kadabra e Alakazam hanno **Synchronize**, che
rimanda paralisi, veleno e scottatura a chi li causa (il sonno no). Mr. Mime
ha **Soundproof**: Snore non lo tocca.

### Sabrina (approvata)

Sabrina: Kadabra 38 (Psybeam, Reflect, Future Sight, Calm Mind), Mr. Mime 37
(Barrier, Psybeam, Baton Pass, Calm Mind), Venomoth 38 (Psybeam, Gust, Leech
Life, Supersonic), Alakazam 43 (Psychic, Recover, Future Sight, Calm Mind).
**Sei** Pokémon, livelli 40-45 (stessa squadra di Koga: approvato).

| Pokémon | Lv | Mosse | Abilità (Gen 3) |
| --- | --- | --- | --- |
| Charizard | 45 | Flamethrower, Wing Attack, Slash, Dig | Blaze |
| Pikachu-Starter | 43 | Thunderbolt, Thunder, Thunder Wave, Quick Attack | Static |
| Snorlax | 42 | Body Slam, Amnesia, Rest, Sleep Talk | Immunity |
| Eevee | 41 | Bite, Return, Quick Attack, Sand Attack | Run Away |
| Butterfree | 40 | Psybeam, Gust, Stun Spore, Sleep Powder | Compound Eyes |
| Poliwrath | 40 | Surf, Hypnosis, Brick Break, Strength | Damp |

Perché:
- **Stessa squadra di Koga, più alta**: Red non prende nulla di nuovo a
  Zafferanopoli. Lapras e Hitmon arrivano a Lv25, troppo indietro rispetto al
  resto della squadra per entrare subito.
- **Eevee ha il suo momento**: Bite è Buio, l'unica mossa superefficace sugli
  Psico che Red ha a questo punto (senza Shadow Ball del Casinò).
- **Snorlax** con Amnesia regge gli attacchi speciali; **Sleep Talk** al posto
  di Snore, che contro Mr. Mime non funziona.
- **Charizard** sistema Venomoth (Fuoco e Volante).
- **Poliwrath** è debole agli Psico (Lotta): resta in squadra come rischio,
  non come arma. In alternativa Lapras, se si vuole un cambio di squadra.
- Psychic (MT29) non serve: contro Psico è resistito.
- **Synchronize** fa parte della sfida: Thunder Wave su Alakazam paralizza
  anche Pikachu.

### Fra Sabrina e Blaine: cosa si ottiene

Storia: Percorsi 19-20 e 21 in Surf, Isole Spumarine (con Strength), Cinnabar,
Villa Pokémon (la Chiave Segreta apre la palestra).

| Dove | Pokémon (FireRed) |
| --- | --- |
| Isole Spumarine | Zubat, Golbat 22-30, Psyduck, Golduck 26-40, **Seel 25-35**, Dewgong 32-40, Horsea (Surf); **Articuno Lv50** (B4F) |
| Villa Pokémon | Rattata, Raticate 26-38, Grimer 28, Koffing 28-30, Weezing 32-34, **Growlithe 30-32**, Ditto 30 |
| Percorso 21 | Tangela 17-28 |
| Laboratorio di Cinnabar | fossili rianimati **al Lv5** (Omanyte, Kabuto, Aerodactyl): troppo bassi per entrare |

Scambi: Seel, Electrode, Tangela (laboratorio di Cinnabar). MT nuove: **MT14
Blizzard** e **MT22 SolarBeam** (Villa Pokémon B1F). La MT38 Fire Blast
arriva da Blaine, quindi dopo.

Mosse utili (FRLG):

| Pokémon | Livello | Note |
| --- | --- | --- |
| Lapras | Water Gun, Growl, Sing · 7 Mist · 13 Body Slam · 19 Confuse Ray · 25 Perish Song · **31 Ice Beam** · 37 Rain Dance · 43 Safeguard · 49 Hydro Pump | regalo della Silph a Lv25; Surf con la MN |
| Dugtrio | 38 Slash · 51 Earthquake | Dig già noto |
| Snorlax | Surf (MN03) compatibile | |

Abilità che contano contro Blaine: Ponyta e Rapidash hanno **Flash Fire**
(assorbono il Fuoco e si potenziano), Growlithe e Arcanine **Intimidate**.

### Blaine (approvata)

Blaine: Growlithe 42 (Bite, Roar, Take Down, Fire Blast), Ponyta 40 e
Rapidash 42 (Stomp, Bounce, Fire Spin, Fire Blast), Arcanine 47 (Bite, Roar,
Take Down, Fire Blast). Fire Blast in expansion è 110. **Sei** Pokémon,
livelli 43-48 (Lapras al posto di Butterfree: approvato).

| Pokémon | Lv | Mosse | Abilità (Gen 3) |
| --- | --- | --- | --- |
| Charizard | 48 | Flamethrower, Wing Attack, Slash, Dig | Blaze |
| Pikachu-Starter | 46 | Thunderbolt, Thunder, Thunder Wave, Quick Attack | Static |
| Poliwrath | 45 | Surf, Hypnosis, Brick Break, Strength | Damp |
| Snorlax | 45 | Body Slam, Surf, Rest, Sleep Talk | Immunity |
| Eevee | 44 | Bite, Return, Quick Attack, Take Down | Run Away |
| Lapras | 43 | Surf, Ice Beam, Body Slam, Confuse Ray | Water Absorb |

Perché:
- **Lapras entra qui, Butterfree esce**: Butterfree contro il Fuoco è
  debole; Lapras (regalo della Silph, allenato da Lv25) porta l'Acqua e
  resterà utile contro Giovanni (Acqua e Ghiaccio contro Terra). È l'ultimo
  cambio di squadra e porta Red alla sua squadra quasi definitiva.
- **Poliwrath** con Surf è il protagonista di questa lotta.
- **Snorlax** prende Surf (MN), superefficace; resta con Immunity (è lo stesso
  Snorlax di Koga, l'abilità non cambia).
- **Charizard** tiene Flamethrower: contro Flash Fire è un errore da evitare,
  ed è parte della sfida. Dig è la sua mossa utile qui.
- **Eevee** prende Take Down (42).
- Alternativa a Lapras: **Dugtrio** (Dig, Slash; Terra superefficace), ma
  contro Giovanni non servirebbe.

### Fra Blaine e Giovanni: cosa si ottiene

Dopo Blaine, in Gen 1, resta solo la palestra di Smeraldopoli. Le Isole Sevii
(Bill porta il giocatore a Primisola dopo Blaine) sono un'aggiunta di FRLG,
non del viaggio di Red: le lascio fuori. Unica novità: **MT38 Fire Blast** da
Blaine.

Nota su Pikachu: contro Giovanni tutti i Pokémon sono Terra, e Pikachu non
impara Surf in FRLG. Deve arrangiarsi con mosse non Elettro: **Brick Break**
(MT31, superefficace su Rhyhorn e Rhydon), Slam, Quick Attack.

### Giovanni (approvata)

Giovanni (con la variazione approvata): Rhyhorn 45 (Take Down, Rock Blast,
Scary Face, Earthquake), Dugtrio 42 (**Arena Trap**; Slash, Sand Tomb,
Mud-Slap, Earthquake), Nidoqueen 44 (Body Slam, Double Kick, Poison Sting,
Earthquake), Nidoking 45 (Thrash, Double Kick, Poison Sting, Earthquake),
**Rhydon 50** (Take Down, Rock Blast, Scary Face, Earthquake). **Sei**
Pokémon, livelli 47-52.

| Pokémon | Lv | Mosse | Abilità (Gen 3) |
| --- | --- | --- | --- |
| Charizard | 52 | Flamethrower, Wing Attack, Steel Wing, Slash | Blaze |
| Pikachu-Starter | 50 | Thunderbolt, Brick Break, Slam, Quick Attack | Static |
| Lapras | 49 | Surf, Ice Beam, Body Slam, Confuse Ray | Water Absorb |
| Poliwrath | 49 | Surf, Hypnosis, Brick Break, Strength | Damp |
| Snorlax | 49 | Body Slam, Surf, Rest, Sleep Talk | Immunity |
| Eevee | 47 | Return, Bite, Take Down, Quick Attack | Run Away |

Perché:
- **Stessa squadra di Blaine**: è la squadra definitiva del viaggio.
- **Lapras e Poliwrath** (Acqua, Ghiaccio) sono le armi; **Snorlax** ha Surf.
- **Charizard** è immune alla Terra (Volante) ma soffre Rock Blast; Steel Wing
  (MT47, Zona Safari) è superefficace su Rhyhorn e Rhydon.
- **Pikachu tiene Thunderbolt** anche se qui non tocca nessuno: è la sua
  mossa, e riconoscerlo è parte della sfida. Brick Break è la sua mossa utile.
- **Arena Trap** di Dugtrio impedisce di cambiare: chi entra contro di lui
  deve reggere.

## Da Red's Path al Monte Argento

La squadra di Red al Monte Argento in HnS (`src/data/trainers_hns.party`):

| `TRAINER_RED_HNS` | `TRAINER_RED_POSTOBC_HNS` (rivincita) |
| --- | --- |
| Pikachu-Starter 93 | Pikachu-Starter 95 |
| Snorlax 77 | Snorlax 85 |
| Charizard 79 | Charizard 87 |
| Venusaur 79 | Venusaur 87 |
| Blastoise 79 | Blastoise 86 |
| Espeon 80 | Tauros-Paldea-Combat 85 |

Red's Path finisce (da Blaine in poi) con Charizard, Pikachu-Starter,
Snorlax, Eevee, Poliwrath, Lapras. In comune col Monte Argento: Charizard,
Pikachu, Snorlax, ed Eevee che diventerà Espeon (evoluzione di Gen 2, fuori
dal viaggio di Kanto). Venusaur e Blastoise **non sono ottenibili** in una
partita a FireRed partita con Charmander (FRLG dà un solo starter, niente
regali come in Giallo): si spiegano nei tre anni fra il viaggio e il Monte
Argento, al posto di Poliwrath e Lapras.

## Scelta dei modelli per le sessioni (24/10)

Costi per milione di token in/out (tabella dei modelli della skill
`claude-api`, aggiornata al 6/10/2026): Opus 5.5 $4/$20, Sonnet 5.5
$2/$10. Le scelte vengono dal posizionamento ufficiale dei modelli e dal
rischio di ogni sessione, non da prove su questo repo. Fable 5.1 escluso per
il costo.

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

- *Pokémon Origins*, File 1 "Red" (2013): Red sceglie Charmander e batte
  Brock con lui ([Wikipedia](https://en.wikipedia.org/wiki/Pok%C3%A9mon_Origins),
  [Bulbapedia](https://bulbapedia.bulbagarden.net/wiki/Red_(Origins))).
- `pret/pokefirered` @ `ce6406d8`: `src/data/pokemon/level_up_learnsets.h` (learnset),
  `tmhm_learnsets.h` (MT), `data/maps/*` (regali, scambi, MT, blocchi),
  `src/data/trainer_parties.h` (capi),
  `src/data/wild_encounters.json` (selvatici),
  `data/maps/<Città>_Gym/scripts.inc` (MT dei capi: 39 Rock Tomb, 03 Water
  Pulse, 34 Shock Wave, 19 Giga Drain, 06 Toxic, 04 Calm Mind, 38 Fire Blast,
  26 Earthquake).
- `pret/pokered`, `data/trainers/parties.asm`: squadre di R/B.
- `pret/pokeyellow` @ `b6696f24`: sezione scartata.
- `pret/pokecrystal`, `data/trainers/parties.asm`: squadra di Red al Monte
  Argento.
