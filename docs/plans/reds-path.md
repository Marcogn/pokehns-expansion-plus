# Piano: Red's Path (le palestre di Kanto rigiocate con la squadra di Red)

**Stato (24/10): piano, nessun codice.** Avversari decisi (sezione 1); squadra
di Red decisa fino a Blaine (sezione 2); script e
scena decisi (sezione 4); premi (sezione 3).

## L'idea

Dopo aver battuto un capo palestra di Kanto, l'aiutante della palestra
propone la sfida "del viaggio di Red": il giocatore **usa la squadra di Red di
quel momento** contro **lo stesso capo palestra com'era allora**. Niente
catture, niente esperienza: la lotta e basta. Le medaglie di HnS si prendono
in (quasi) qualunque ordine, ma **le sfide di Red's Path vanno fatte
nell'ordine del viaggio di Red**: Brock, Misty, Surge, Erika, Koga, Sabrina,
Blaine, Giovanni. (Il modello dell'utente: il DLC di *Breath of the
Wild* in cui si rigioca una sfida con gli strumenti dei campioni di allora.)

## Decisioni prese

| Tema | Scelta |
| --- | --- |
| Chi usa la squadra di Red | il giocatore (squadra prestata) |
| Capi palestra | Brock, Misty, Surge, Erika, **Koga**, Sabrina, Blaine, **Giovanni** |
| Squadre dei capi | **FireRed/LeafGreen**, con poche variazioni motivate (sezione 1) |
| Starter di Red | **Charmander**: in *Pokémon Origins* (File 1) Red lo sceglie e batte l'Onix di Brock proprio con lui |
| Squadra di Red | **uno starter** più Pokémon che avrebbe potuto ottenere strada facendo; può cambiare da un capo all'altro (Butterfree all'inizio, Poliwrath più avanti, …). Non più vincolata ai sei del Monte Argento |
| Mosse di Red | livello, MT e MN **ottenibili a quel punto della storia**; niente eccezioni salvo problemi seri |
| Pikachu | se c'è, è `Pikachu-Starter` (base 45/80/50/75/60/120) |
| Espeon | mai: solo al Monte Argento |
| Ordine | **obbligato** (Brock → Misty → Surge → Erika → Koga → Sabrina → Blaine → Giovanni), anche se le medaglie di HnS sono in ordine libero (tranne Blue, sempre ultimo). Cambiato dall'utente il 24/10 (prima era libero) |
| Sblocco | **dopo il capo di HnS corrispondente** |
| Zaino | **quello attuale del giocatore** (per ora) |
| Fuga | **impossibile**: si vince o si perde |
| Sconfitta | **nessuna perdita di soldi**, la lotta si chiude normalmente e si torna dall'aiutante |
| Versione di Red | **FireRed** (Oddish, Ekans; niente Bellsprout e Sandshrew) |
| Premi | l'**asso** di ogni capo, **Lv70**, mosse di livello al 70, **non shiny ma nemmeno bloccato**: probabilità normali, così si può cacciare lo shiny resettando |
| Storia | tre anni dopo il viaggio di Red, la Lega organizza ogni anno l'evento "Red's Path" (dialoghi in sezione 4) |

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

### Variazioni (approvate)

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

Proposta: **Red gioca FireRed** (il nome lo dice), quindi Oddish ed Ekans sì,
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
| Oddish | 19 | Absorb, Poison Powder, Stun Spore, Sleep Powder | Chlorophyll |
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

### Da Red's Path al Monte Argento

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

## 3. Premi

L'asso di ogni capo, **Lv70**, alla prima vittoria, con le mosse di livello
al 70 (quelle che `givemon` dà già da solo). **Non shiny, ma con le
probabilità normali**: niente shiny lock, così chi vuole può resettare
finché non esce. Natura **casuale**.

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

"Tutto come il capo palestra" (decisione dell'utente): **allenatore originale
il capo** (nome e sesso), **Poké Ball** (i Pokémon degli allenatori di FRLG
non hanno una ball diversa). Unica eccezione, approvata: **IV 31** (quelli
del capo in FRLG sono 0, e un premio Lv70 con IV 0 sarebbe debolissimo).
Consegna (decisione dell'utente, pensata per la caccia allo shiny): il premio
va **sempre in squadra, mai al PC**. Con la squadra piena l'aiutante dice di
tornare con un posto libero; quando si torna, consegna con una frase breve.
Quindi servono due stati per capo: *sfida vinta* e *premio ritirato*. Chi
caccia lo shiny salva davanti all'aiutante con la squadra piena, libera un
posto, ritira e resetta se non è shiny. Lo shiny si tira al momento di
`givemon`, dopo il reset: da verificare in emulatore che due reset di fila non
diano lo stesso Pokémon (il seme dell'RNG dipende dal momento in cui si
preme).

## 4. Script e scena (decisi dall'utente il 24/10)

### Dove: l'aiutante di ogni palestra di Kanto

Ogni sfida vive nella palestra di HnS corrispondente, e la propone l'aiutante
all'ingresso (`OBJ_EVENT_GFX_GYM_GUY_HNS`):

| Palestra di HnS | Capo in HnS | Sfida di Red's Path | Aiutante |
| --- | --- | --- | --- |
| `PewterCity_Gym_hns` | Brock | Brock | c'è |
| `CeruleanCity_Gym_hns` | Misty | Misty | c'è |
| `VermilionCity_Gym_hns` | Surge | Surge | c'è |
| `CeladonCity_Gym_hns` | Erika | Erika | **manca: va aggiunta un'aiutante donna** (la palestra di Erika è solo femminile), con lo sprite della fioraia di Fiordoropoli che dà la Sprinkler: `OBJ_EVENT_GFX_GIRL_1_HNS` |
| `FuchsiaCity_Gym_hns` | Janine | **Koga** | c'è |
| `SaffronCity_Gym_hns` | Sabrina | Sabrina | c'è |
| `SeafoamIslands_Gym_hns` | Blaine | Blaine | c'è |
| `ViridianCity_Gym_hns` | Blue | **Giovanni** | c'è |

La sfida di una palestra si può fare quando:
1. il capo di HnS di quella palestra è battuto (`FLAG_DEFEATED_<CITTÀ>_GYM`,
   in `include/constants/flags_hns.h`), che è anche il momento in cui
   l'aiutante comincia a parlarne;
2. **tutte le sfide di Red's Path che la precedono sono vinte.**

Ordine delle medaglie in HnS, verificato: libero, tranne Smeraldopoli. Blue
torna in palestra solo quando lo si incontra a Cinnabar con 15 medaglie
(`CinnabarIsland_hns/scripts.inc:36`, `VAR_NUM_BADGES == 15`), quindi è
sempre l'ultimo capo di Kanto, come Giovanni nel nostro ordine. Un giocatore
può però battere Blaine prima di Brock: per questo serve il controllo
dell'ordine.

### Dialoghi (bozza; il gioco è in inglese)

Testo dell'utente in italiano, proposta inglese sotto. Il testo esatto si
rifinisce in fase di codice (larghezza della finestra, `\n`/`\p`).

**Prima volta, dopo aver battuto un qualunque capo di Kanto.** Uscendo, prima
della porta, l'aiutante ha il punto esclamativo, si avvicina e dice:

> Bella sfida! Sono passati 3 anni da quando un allenatore di Pallet Town ha
> salvato l'intera nazione dal Team Rocket. Come ogni anno la Lega Pokémon
> organizza l'evento Red's Path, in cui gli allenatori possono ricreare la
> sfida di Red contro i capipalestra usando solo i Pokémon che aveva quando li
> ha sconfitti la prima volta. Non perderla, ci sono bellissimi premi per
> ciascun capopalestra sconfitto in questo modo! Parla con me, organizzerò la
> lotta con il capopalestra.

> What a battle! It's been three years since a TRAINER from PALLET TOWN saved
> the whole region from TEAM ROCKET. Every year the POKéMON LEAGUE holds RED'S
> PATH: TRAINERS relive RED's GYM battles using only the POKéMON he had when
> he first beat each LEADER. Don't miss it, there's a great prize for every
> LEADER you beat this way! Just like RED, you have to take them on in order,
> starting with BROCK in PEWTER CITY. Talk to me and I'll set up the battle.

Aggiunta dell'utente (24/10): l'introduzione dice che le sfide vanno fatte in
ordine.

Una sola volta in tutto il gioco (un flag), non una per palestra.

**Se il giocatore ne ha saltata una** (parla con l'aiutante della palestra N
ma la prima sfida non vinta è la K < N), l'aiutante indica quella:

> RED'S PATH has to be followed in order! Your next challenge is BROCK, in
> PEWTER CITY.

Se in più il capo di HnS di quella palestra non è ancora battuto (la sfida K
non è ancora offerta), l'aiutante aggiunge (approvato):

> Beat the LEADER there first!

**Le volte successive, se la sfida di questa palestra è la prossima:**

> Are you ready for RED'S PATH? → YES / NO
> - YES: "Then let's go!" → parte la sfida
> - NO: "OK! Remember, there are great prizes!"

**Sconfitta:** la lotta si chiude come una normale, si torna all'ingresso
davanti all'aiutante: "Try again!".

**Vittoria con posto in squadra:** congratulazioni e premio.
**Vittoria con squadra piena:** congratulazioni, "Come back with room in your
party!". **Ritorno con posto libero:** "Here's your prize!".

**Dopo tutte e 8:** l'aiutante fa tanti complimenti e basta.

### La scena (dopo il "sì")

1. dissolvenza al nero;
2. nella palestra il giocatore diventa **Red** (sprite overworld
   `OBJ_EVENT_GFX_RED_NORMAL_HNS`), davanti al capo con lo **sprite giusto**:
   Koga (`OBJ_EVENT_GFX_KOGA_HNS`) al posto di Janine, Giovanni
   (`OBJ_EVENT_GFX_GIOVANNI_HNS`) al posto di Blue, gli altri col proprio
   sprite HnS. Gli sprite overworld FRLG in HnS **non** sono compilati
   (`#if IS_FRLG` in `object_event_graphics_info_pointers.h`);
3. ritorno dal nero, scritta **"RED VS BROCK"** (ad esempio), si preme A;
4. **mugshot** e lotta;
5. dopo il messaggio di sconfitta del capo, di nuovo nella scena: **"RED
   WINS!"**, fanfara, dissolvenza al nero;
6. di nuovo all'ingresso, davanti all'aiutante, nel punto in cui si era
   lasciato il giocatore: congratulazioni e premio.

Da decidere in fase di codice: Red come oggetto separato con il giocatore
nascosto, oppure il giocatore con lo sprite cambiato; il capo di Gen 1 come
oggetto con grafica variabile (`OBJ_EVENT_GFX_VAR_*`) o come oggetto nascosto
a parte.

### La lotta

| Cosa | Come | Dove si aggancia |
| --- | --- | --- |
| squadra di Red | 8 voci `PARTNER_RED_*` in `src/data/battle_partners.party`, caricate in `gPlayerParty` da una nuova `FillRedsPathParty` | `src/battle_partner.c` (`FillPartnerParty` riempie solo gli slot 3-5) |
| squadra del giocatore | `SavePlayerParty` prima, `LoadPlayerParty` **sempre** dopo | come la Battle Frontier; la copia sta in RAM, la flash non cambia finché non si salva |
| sprite di schiena | `TRAINER_PIC_BACK_RED` (l'unico dorso di Red: quello di FRLG) | `PlayerGetTrainerBackPicId` (`src/battle_controller_player.c`), un punto solo |
| mugshot | Red **FRLG** (`TRAINER_PIC_FRONT_RED`, compilato), coerente col dorso FRLG | `src/battle_transition.c:2666` usa `PlayerGenderToFrontTrainerPicId`; la palette della banda (`sPlayerMugshotsPals`, riga 2324) dipende dal sesso |
| sprite del capo in lotta | **FRLG** (decisione dell'utente): `TRAINER_PIC_FRONT_LEADER_{BROCK,MISTY,LT_SURGE,ERIKA,KOGA,SABRINA,BLAINE,GIOVANNI}_FRLG`, tutti compilati in HnS (`src/data/graphics/trainers.h`, senza `#if`) | |
| zaino | quello del giocatore: nessun aggancio | |
| fuga | impossibile | `B_RUN_TRAINER_BATTLE` è `TRUE` in questo repo: va bloccata in `CanPlayerForfeitNormalTrainerBattle` (`src/battle_main.c`) |
| sconfitta | nessun soldo perso, ritorno allo script | `B_FLAG_NO_WHITEOUT`, oggi 0: serve un flag libero. Con il flag acceso lo script di sconfitta salta `getmoneyreward` (`jumpifnowhiteout`, `data/battle_scripts_1.s:4202`) e `battle_setup.c` torna allo script invece di svenire |
| niente esperienza | `FLAG_DISABLE_EXP_GAIN` | già esiste |
| niente catture/TcloneR | il TcloneR non deve agire in queste lotte | `src/tcloner.c` |
| level scaling | escluse | `IsTrainerExcludedFromScaling` (`src/level_scaling.c`) |

### Stato salvato

Solo flag, niente struct da allargare:

- 1 flag "introduzione vista";
- 8 flag "sfida vinta" (la prossima sfida è la prima non vinta: basta
  questo per il controllo dell'ordine);
- 8 flag "premio ritirato";
- il messaggio finale si deduce dagli 8 "premio ritirato" (o "sfida vinta").

### Il limite degli id allenatore

`src/tcloner.c:43` ha
`STATIC_ASSERT(TRAINERS_COUNT <= TCLONER_TRAINER_FLAGS_BYTES * 8)`, con 82 byte
= 656 bit, e oggi `TRAINERS_COUNT_HNS` è 653: **otto allenatori nuovi non
compilano**. Proposta: un solo id `TRAINER_REDS_PATH_HNS` (654, ci sta) con
squadra, nome e sprite scelti da una var al momento della lotta, come fa la
Frontier. Alternative: allargare lo spazio del TcloneR (da misurare) o
riusare id irraggiungibili (da verificare).

### Da provare in emulatore

Sconfitta e vittoria, soft reset a metà lotta (la flash deve avere ancora la
squadra vera), evoluzioni a fine lotta, Pokédex (i Pokémon prestati non
devono risultare catturati), reset ripetuti davanti al premio (shiny che
cambia), squadra piena.

**Nota onesta sullo zaino:** con lo zaino del giocatore a fine gioco (Full
Restore, Max Revive, strumenti X) le prime sfide si vincono anche giocando
male. Va bene "per ora", come deciso; se in prova risulta troppo facile, la
soluzione già pronta è `B_VAR_NO_BAG_USE`.

## Domande aperte

1. **Giovanni** (sezione 2): l'ultimo capo.

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
