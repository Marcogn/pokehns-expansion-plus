# Piano: Red's Path (le palestre di Kanto rigiocate con la squadra di Red)

**Stato (23/10): piano, nessun codice.** Avversari decisi (sezione 1), poi la squadra di Red capo per capo (sezione 2, da
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
| Starter di Red | **Charmander**: in *Pokémon Origins* (File 1) Red lo sceglie e batte l'Onix di Brock proprio con lui |
| Squadra di Red | **uno starter** più Pokémon che avrebbe potuto ottenere strada facendo; può cambiare da un capo all'altro (Butterfree all'inizio, Poliwrath più avanti, …). Non più vincolata ai sei del Monte Argento |
| Mosse di Red | livello, MT e MN **ottenibili a quel punto della storia**; niente eccezioni salvo problemi seri |
| Pikachu | se c'è, è `Pikachu-Starter` (base 45/80/50/75/60/120) |
| Espeon | mai: solo al Monte Argento |
| Ordine | libero |
| Sblocco | **dopo il capo di HnS corrispondente** |
| Zaino | nessuno |
| Premi | l'**asso** di ogni capo, **shiny, Lv70**, con le mosse di livello al 70 |
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

Da decidere dopo la correzione delle MT: Oddish può avere **Bullet Seed** al
posto di Absorb. In expansion è 25 di potenza per 2-5 colpi contro i 20 di
Absorb; contro Misty è un salto netto, quindi la lasciamo fuori solo se la
lotta risulta già facile.

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

### Surge (proposta)

Surge: Voltorb 21 (Sonic Boom, Screech, Shock Wave), Pikachu 18, Raichu 24
(Thunder Wave, Double Team, Shock Wave); tutti con **Static** (paralisi al
contatto). Proposta: **quattro** Pokémon, livelli 22-25.

| Pokémon | Lv | Mosse | Abilità (Gen 3) |
| --- | --- | --- | --- |
| Charmeleon | 25 | Ember, Metal Claw, Dig, Brick Break | Blaze |
| Diglett | 22 | Sand Attack, Magnitude, Dig, Fury Swipes | Arena Trap |
| Gloom | 22 | Absorb, Poison Powder, Stun Spore, Sleep Powder | Chlorophyll |
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

## 3. Premi

L'asso di ogni capo, **shiny, Lv70**, alla prima vittoria, con le mosse di
livello al 70 (quelle che `givemon` dà già da solo: nessun codice in più per
le mosse):

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
Natura: da scegliere asso per asso. Per la consegna con la squadra piena c'è già il percorso del TcloneR
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

1. **Squadra per Surge** (sezione 2).
2. **Bullet Seed** a Oddish contro Misty, al posto di Absorb?
3. **Versione di Red**: FireRed (Oddish, Ekans) o LeafGreen (Bellsprout,
   Sandshrew)? La squadra di Misty usa già Oddish, quindi oggi è FireRed.
4. **Natura** di ogni asso premio.

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
