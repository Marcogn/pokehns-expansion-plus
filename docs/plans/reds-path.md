# Piano: Red's Path (le palestre di Kanto rigiocate con la squadra di Red)

**Stato (25/10): sessioni A e B fatte.** A1 (24/10): passi 1, 2, 3 e 5
(`SAVE_VERSION 10`, otto capi, otto squadre di Red, voce di debug `Red's
Path…`); A2 (25/10): passi 4, 6 e 7 (agganci di lotta, verifica, regola
`.claude/rules/reds_path.md`); B (25/10): aiutanti, introduzione, scena,
premi, `SAVE_VERSION 11` (vedi "Cosa ha lasciato la B alla C" in fondo).
Resta la C. Avversari (§1), squadre di
Red (§2), premi (§3), script e scena (§4) decisi con l'utente.
Implementazione in tre fasi, quattro sessioni (§5). Le ricerche da cui
vengono le scelte (cosa era ottenibile prima di ogni capo, meccaniche,
fonti) sono in [`docs/analysis/reds-path-research.md`](../analysis/reds-path-research.md):
non servono per implementare.

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
| Storia | tre anni dopo il viaggio di Red, la Lega organizza ogni anno l'evento "Red's Path": **un visore VR interattivo** (che in gioco non si vede) fa rivivere al giocatore le lotte di Red in prima persona, con la sua squadra. Niente viaggio nel tempo (decisione dell'utente, 25/10; dialoghi in sezione 4) |

## 1. Gli avversari

### Base: le squadre di FireRed/LeafGreen

Da `pret/pokefirered` `src/data/trainer_parties.h`, identiche a quelle che
expansion ha già in `src/data/trainers_frlg.party` (`TRAINER_LEADER_*`).
Specie e livelli sono quelli di Rosso/Blu (`pret/pokered`), tranne l'ultimo
Pokémon di Giovanni (Rhyhorn 50 in FRLG, Rhydon 50 in R/B). Nessuno strumento
tenuto; le cure sono quelle che il capo usa dalla borsa. IV 0. AI: era FRLG (`Check Bad
Move / Try To Faint / Check Viability`), dal 26/10 **`Smart Trainer`** (piano
`small-features-1`).

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
   Solo abilità che il Pokémon aveva in Gen 3:

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

Le meccaniche di expansion cambiano alcune mosse e abilità rispetto a
FRLG (Sturdy, Rock Tomb, Giga Drain, legature, Minimize…): tabella
nell'analisi, da tenere presente nella taratura (sessione C).

## 2. La squadra di Red

Scelta capo per capo con l'utente fra i Pokémon e le mosse **ottenibili in
FireRed prima di quella palestra** (learnset e MT di FRLG, non quelli di
expansion). Le motivazioni sono nell'analisi.

### Le otto squadre (fonte per i dati della sessione A)

Per tutti: **natura neutra (Hardy), IV 15, EV 0, amicizia 255** (`Happiness:`
nel `.party`; serve a Return, che in expansion dipende dall'amicizia).
Pikachu è sempre `Pikachu-Starter`. Abilità di Gen 3: Charmander/Charmeleon/
Charizard Blaze, Pikachu Static, Mankey Vital Spirit, Butterfree Compound
Eyes, Oddish/Gloom Chlorophyll, Diglett Arena Trap, Eevee Run Away, Poliwrath
Damp, Snorlax Immunity, Lapras Water Absorb.

| Sfida | Squadra (Lv: mosse) |
| --- | --- |
| 1 Brock | Charmander 14: Scratch, Growl, Ember, Metal Claw · Mankey 12: Scratch, Leer, Low Kick, Karate Chop · Butterfree 12: Tackle, String Shot, Harden, Confusion |
| 2 Misty | Charmeleon 21: Scratch, Ember, Metal Claw, Smokescreen · Pikachu 20: Thunder Shock, Thunder Wave, Quick Attack, Slam · Oddish 19: Bullet Seed, Poison Powder, Stun Spore, Sleep Powder · Butterfree 18: Confusion, Poison Powder, Stun Spore, Sleep Powder |
| 3 Surge | Charmeleon 25: Ember, Metal Claw, Dig, Brick Break · Pikachu 23: Thunder Shock, Thunder Wave, Quick Attack, Slam · Diglett 22: Sand Attack, Magnitude, Dig, Fury Swipes · Gloom 22: Bullet Seed, Poison Powder, Stun Spore, Sleep Powder |
| 4 Erika | Charmeleon 30: Ember, Metal Claw, Scary Face, Dig · Pikachu 28: Thunderbolt, Thunder Wave, Quick Attack, Slam · Butterfree 28: Confusion, Gust, Stun Spore, Sleep Powder · Eevee 25: Tackle, Sand Attack, Growl, Quick Attack |
| 5 Koga | Charizard 42: Flamethrower, Wing Attack, Scary Face, Dig · Pikachu 41: Thunderbolt, Thunder, Thunder Wave, Quick Attack · Butterfree 38: Psybeam, Gust, Stun Spore, Sleep Powder · Poliwrath 37: Surf, Hypnosis, Brick Break, Strength · Snorlax 36: Body Slam, Amnesia, Rest, Snore · Eevee 35: Return, Bite, Quick Attack, Sand Attack |
| 6 Sabrina | Charizard 45: Flamethrower, Wing Attack, Slash, Dig · Pikachu 43: Thunderbolt, Thunder, Thunder Wave, Quick Attack · Snorlax 42: Body Slam, Amnesia, Rest, Sleep Talk · Eevee 41: Bite, Return, Quick Attack, Sand Attack · Butterfree 40: Psybeam, Gust, Stun Spore, Sleep Powder · Poliwrath 40: Surf, Hypnosis, Brick Break, Strength |
| 7 Blaine | Charizard 48: Flamethrower, Wing Attack, Slash, Dig · Pikachu 46: Thunderbolt, Thunder, Thunder Wave, Quick Attack · Poliwrath 45: Surf, Hypnosis, Brick Break, Strength · Snorlax 45: Body Slam, Surf, Rest, Sleep Talk · Eevee 44: Bite, Return, Quick Attack, Take Down · Lapras 43: Surf, Ice Beam, Body Slam, Confuse Ray |
| 8 Giovanni | Charizard 52: Flamethrower, Wing Attack, Steel Wing, Slash · Pikachu 50: Thunderbolt, Brick Break, Slam, Quick Attack · Lapras 49: Surf, Ice Beam, Body Slam, Confuse Ray · Poliwrath 49: Surf, Hypnosis, Brick Break, Strength · Snorlax 49: Body Slam, Surf, Rest, Sleep Talk · Eevee 47: Return, Bite, Take Down, Quick Attack |

Ordine nella squadra: quello della tabella (il primo è quello che scende in
campo). I livelli sono da tarare in emulatore (sessione C).

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

Aggiunta dell'utente (24/10): l'introduzione dice che le sfide vanno fatte in
ordine. La prima proposta inglese è superata dal visore (sotto): quella da
usare è la seconda.

**Il visore (decisione dell'utente, 25/10).** Non si torna magicamente
indietro nel tempo: la Lega fa rivivere le lotte di Red con un **visore VR
interattivo**, che in gioco non si vede. Si usa la squadra di Red e si lotta
al posto suo, in prima persona. Testo dell'utente:

> Ogni anno facciamo rivivere l'esperienza delle lotte di Red coi
> capipalestra attraverso questo visore VR interattivo. Avrai accesso alla
> sua squadra e lotterai al posto suo in prima persona. Vuoi provare?

**Introduzione da usare** (proposta inglese, da impaginare nella B):

> What a battle! It's been three years since a TRAINER from PALLET TOWN saved
> the whole region from TEAM ROCKET. Every year the POKéMON LEAGUE holds RED'S
> PATH: with this interactive VR headset you relive RED's battles against the
> GYM LEADERS. You get his team, the POKéMON he had back then, and you fight
> in his place, in first person! There's a great prize for every LEADER you
> beat. Just like RED, you have to take them on in order, starting with BROCK
> in PEWTER CITY. Talk to me whenever you want to try!

Una sola volta in tutto il gioco (un flag), non una per palestra.

**Se il giocatore ne ha saltata una** (parla con l'aiutante della palestra N
ma la prima sfida non vinta è la K < N), l'aiutante indica quella:

> RED'S PATH has to be followed in order! Your next challenge is BROCK, in
> PEWTER CITY.

Se in più il capo di HnS di quella palestra non è ancora battuto (la sfida K
non è ancora offerta), l'aiutante aggiunge (approvato):

> Beat the LEADER there first!

**Le volte successive, se la sfida di questa palestra è la prossima:**

> Want to relive RED's battle with BROCK? → YES / NO
> - YES: "Put on the headset… Here we go!" → parte la sfida
> - NO: "OK! Remember, there are great prizes!"

**Sconfitta:** la lotta si chiude come una normale (niente svenimento, niente
soldi persi: fatto nella A2), si torna all'ingresso davanti all'aiutante:
"Try again!".

**Vittoria con posto in squadra:** congratulazioni e premio.
**Vittoria con squadra piena:** congratulazioni, "Come back with room in your
party!". **Ritorno con posto libero:** "Here's your prize!".

**Dopo tutte e 8:** l'aiutante fa tanti complimenti e basta.

### La scena (dopo il "sì")

1. dissolvenza al nero: il giocatore indossa il visore (non si vede);
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
6. il visore si toglie: di nuovo all'ingresso, davanti all'aiutante, nel punto
   in cui si era lasciato il giocatore: congratulazioni e premio.

Quello che si vede fra 1 e 6 è la simulazione: per questo il giocatore è Red
e Janine e Blue sono Koga e Giovanni. Nessun testo deve far pensare a un
viaggio nel tempo.

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

- **8 allenatori veri** `TRAINER_REDS_PATH_{BROCK,MISTY,SURGE,ERIKA,KOGA,
  SABRINA,BLAINE,GIOVANNI}_HNS` (id 653-660): il loro flag di allenatore
  battuto (`TRAINER_FLAGS_START + id`) è già il "sfida vinta", acceso da
  `trainerbattle` solo alla vittoria. La prossima sfida è la prima con il
  flag spento: basta per il controllo dell'ordine.
- **8 flag** "premio ritirato" (`FLAG_REDS_PATH_PRIZE_<CAPO>`, sessione B).
- **1 flag** "introduzione vista" (`FLAG_REDS_PATH_INTRO_SEEN`). I nove sono
  `FLAG_UNUSED_EXTENDED_87..95`, azzerati dalla migrazione `saveVersion < 11`.
- **1 flag** per `B_FLAG_NO_WHITEOUT` e **1 flag** "lotta di Red's Path in
  corso" (sprite di schiena, mugshot, fuga, TcloneR, level scaling), da
  spegnere sempre dopo la lotta e in `Overworld_ResetBattleFlagsAndVars`.
- Il messaggio finale si deduce dagli 8 flag di allenatore.

### Il limite degli id allenatore: risolto allargando lo spazio del TcloneR

`src/tcloner.c:43` ha
`STATIC_ASSERT(TRAINERS_COUNT <= TCLONER_TRAINER_FLAGS_BYTES * 8)`: con 82
byte (656 bit) e `TRAINERS_COUNT_HNS` a 653, otto allenatori nuovi non
compilano. Ma i bit del TcloneR stanno all'inizio del vecchio filler dei flag
del Pokédex di Emerald, che è di **0x68 = 104 byte** (`struct Pokedex`,
`include/global.h`): ne restano 22 liberi.

Quindi: `TCLONER_TRAINER_FLAGS_BYTES` da 82 a **83** (664 bit, ci stanno gli
id fino a 663), con un passo di migrazione `saveVersion < 10` in
`LoadGameSave()` che azzera il byte nuovo, come la migrazione 9 ha fatto con i
primi 82 ("not provably zero in every save", `src/save.c`). Così ogni capo di
Red's Path è un allenatore normale, con nome, classe, sprite FRLG, mugshot,
abilità e cure scritti nel `.party`, senza sostituzioni a runtime. L'idea
dell'id unico con squadra scelta da una var (scritta prima) è scartata:
avrebbe obbligato a intercettare nome, classe e sprite in ogni punto che legge
`gTrainers[]`.

Il TcloneR non deve comunque contare né clonare questi allenatori.

### Da provare in emulatore

Sconfitta e vittoria, soft reset a metà lotta (la flash deve avere ancora la
squadra vera), evoluzioni a fine lotta, Pokédex (i Pokémon prestati non
devono risultare catturati), reset ripetuti davanti al premio (shiny che
cambia), squadra piena.

**Nota onesta sullo zaino:** con lo zaino del giocatore a fine gioco (Full
Restore, Max Revive, strumenti X) le prime sfide si vincono anche giocando
male. Va bene "per ora", come deciso; se in prova risulta troppo facile, la
soluzione già pronta è `B_VAR_NO_BAG_USE`.

## 5. Fasi e sessioni

Tre fasi in fila (A, B, C; la A divisa in A1 e A2, vedi "Sessione
consigliata"), ognuna sul branch che le assegna
l'ambiente e con la sua PR. Ogni sessione legge prima `CLAUDE.md`, questo
piano e le regole indicate.

### Sessione A: motore della lotta (C, dati, salvataggio)

Regole da leggere: `.claude/rules/save.md`, `trainer_catch.md`,
`level_scaling.md`, guida
dell'emulatore `docs/claude/emulator.md`.

1. **Spazio degli id**: `TCLONER_TRAINER_FLAGS_BYTES` 82 → 83, migrazione
   `saveVersion < 10` che azzera il byte nuovo, `STATIC_ASSERT` aggiornati
   (anche il quest book, `quest-log.md` §5, avrà una migrazione: prende il
   numero dopo, chiunque arrivi prima in `master` usa il 10);
   aggiornare `.claude/rules/save.md`.
2. **Otto capi** in `include/constants/opponents_hns.h` (653-660,
   `TRAINERS_COUNT_HNS` 661) e in `src/data/trainers_hns.party`: squadre FRLG
   della sezione 1 con le variazioni (Rhydon 50), abilità scritte, IV 0, cure,
   AI Smart Trainer (era FRLG), sprite FRLG (`TRAINER_PIC_FRONT_LEADER_*_FRLG`), mugshot, testo
   di sconfitta. Esclusi dal level scaling (`IsTrainerExcludedFromScaling`)
   e dal TcloneR.
3. **Otto squadre di Red** (riepilogo della sezione 2) in
   `src/data/battle_partners.party` come `PARTNER_RED_*`, e una
   `FillRedsPathParty` che le carica negli slot 0-5 di `gPlayerParty`
   (`FillPartnerParty` riempie solo 3-5). Gli `special` per lo script:
   salva squadra, carica squadra di Red N, ripristina squadra.
4. **Agganci di lotta** dietro il flag "lotta di Red's Path in corso":
   dorso `TRAINER_PIC_BACK_RED` in `PlayerGetTrainerBackPicId`, mugshot con
   `TRAINER_PIC_FRONT_RED` (FRLG) in `battle_transition.c`, fuga bloccata in
   `CanPlayerForfeitNormalTrainerBattle`, `B_FLAG_NO_WHITEOUT` assegnato a un
   flag libero, `FLAG_DISABLE_EXP_GAIN`. Controllare che i Pokémon prestati
   non finiscano nel Pokédex come catturati e che non evolvano a fine lotta.
5. **Debug**: una voce che avvia la sfida N senza script di mappa.
6. **Verifica in emulatore** (con la voce di debug): tutte e otto le lotte
   partono con squadre, abilità, mosse e sprite giusti (screenshot);
   vittoria e sconfitta tornano allo script, senza soldi persi e senza
   svenimento; la squadra del giocatore torna intatta in entrambi i casi;
   la fuga è rifiutata; soft reset a metà lotta → al caricamento c'è la
   squadra vera; un salvataggio vecchio (versione 9) passa la migrazione col
   TcloneR intatto.
7. `CHANGELOG.md`, nuova regola `.claude/rules/reds_path.md` (con `paths:`)
   e voce nell'indice di `CLAUDE.md` §7.

Fra il merge della A e quello della B Red's Path non è raggiungibile in gioco
(solo da debug): niente release in mezzo.

### Sessione B: mondo (script, mappe, testi, premi)

Regole da leggere: `reds_path.md` (della A), `field.md`, guida
dell'emulatore.

1. **Aiutante di Erika** in `CeladonCity_Gym_hns` (`OBJ_EVENT_GFX_GIRL_1_HNS`),
   con un dialogo normale da aiutante più Red's Path.
2. **Script comune** (`data/scripts/reds_path.inc`), con la cornice del
   **visore VR** (§4, "Il visore": nessun testo deve parlare di viaggio nel
   tempo): introduzione una volta sola, attivata uscendo dalla palestra dopo il capo di HnS (punto
   esclamativo, l'aiutante si avvicina); domanda sì/no; controllo
   dell'ordine con "Beat the LEADER there first!"; la scena (sezione 4);
   ritorno all'ingresso; premio in squadra (Lv70, mosse di livello, allenatore
   originale il capo, Poké Ball, IV 31, natura casuale, shiny con le
   probabilità normali), squadra piena → torna dopo, "Here's your prize!";
   sconfitta → "Try again!"; complimenti finali dopo le otto.
3. **Agganci** negli otto script delle palestre (aiutanti esistenti). La lotta
   si chiama con `RedsPath_EventScript_Battle` così com'è (vedi "Cosa ha
   lasciato la A2 alla B" in fondo): vittoria e sconfitta si distinguono col
   flag del capo.
4. **Testi inglesi** dalla sezione 4, impaginati e controllati negli
   screenshot.
5. **Verifica in emulatore**: percorso completo con flag forzati (debug o
   GDB): introduzione dopo un capo qualunque; ordine violato, con e senza capo
   di HnS battuto; scena completa con screenshot di ogni passo; vittoria con
   squadra piena e poi libera; reset ripetuti davanti al premio (lo shiny deve
   poter cambiare); sconfitta; complimenti finali. **Consegnare all'utente gli
   screenshot della scena** e la ROM zippata.
6. `CHANGELOG.md`, aggiornare `reds_path.md` e `docs/plans/quest-log.md`
   (Red's Path è una missione del quest book: se cambiano i flag dello
   stato salvato, aggiornare §7 di quel piano).

### Sessione C: taratura

Dopo la B, con le prove dell'utente. Ogni lotta giocata in emulatore con la
squadra di Red, annotando quanto è difficile. Ritocchi già previsti: Brock
(Pikachu assente, Charmander da solo può bastare?), Butterfree e Oddish con
Sleep Powder contro Misty, Erika e Koga probabilmente facili (abbassare
Charmeleon/Charizard, togliere Eevee), IV dei capi (0 → più alti se serve),
lo zaino del giocatore (`B_VAR_NO_BAG_USE` se rende tutto banale). Le
motivazioni delle squadre, per capire cosa si può togliere, sono
nell'analisi. Ogni ritocco va in questo piano e nella regola.

### Sessione consigliata

Una sessione nuova di Claude Code per fase, in fila (B dopo il merge della A,
C dopo la B). **Limite dell'utente: al massimo Opus 5.5 a sforzo medio.**

| Sessione | Modello | Sforzo | Perché |
| --- | --- | --- | --- |
| A1, dati e salvataggio: passi 1-3 e 5 della A | **Opus 5.5** | medio | migrazione del salvataggio |
| A2, agganci di lotta: passi 4, 6, 7 della A | **Opus 5.5** | medio | codice di lotta condiviso con le lotte normali |
| B, mondo | **Opus 5.5** | medio | tanti script e una verifica lunga in emulatore |
| C, taratura | **Sonnet 5.5** | medio | ritocchi piccoli ai dati, molti giri brevi |

In A1 la lotta di debug è ancora una lotta normale (dorso del giocatore, fuga
e sconfitta come sempre): gli agganci arrivano con la A2.

**Cosa ha lasciato la A1 alla A2** (verificato in emulatore il 24/10):

- La lotta passa da `RedsPath_EventScript_Battle` (`data/scripts/reds_path.inc`,
  `VAR_0x8004` = sfida 0-7): `special SavePlayerParty`, `special
  RedsPath_LoadRedsParty` (`FillRedsPathParty`, `src/battle_partner.c`),
  `trainerbattle_no_intro`, `special LoadPlayerParty` (dalla A2 `special
  RedsPath_RestorePlayerParty`, che aggiorna anche il follower). La B la
  chiama così com'è; la voce di debug `Red's Path… → N` pure.
- **Una sconfitta oggi fa svenire e lascia in squadra quella di Red**
  (curata): lo script non arriva a `LoadPlayerParty`. La squadra vera è
  ancora nella flash, un soft reset la riporta (provato). Salvare dopo una
  sconfitta la perderebbe: è il primo punto della A2 (`B_FLAG_NO_WHITEOUT`).
- I Pokémon di Red prendono esperienza e salgono di livello (Charmeleon
  30 → 31 contro Erika, "boosted": OT diverso): `FLAG_DISABLE_EXP_GAIN` è
  della A2. Il ripristino della squadra li butta via comunque.
- OT dei Pokémon di Red: `RED`, ID derivato dal nome come gli altri partner
  (`GetPartnerOtId`). Con OT diverso vale l'obbedienza: con meno di 8
  medaglie di Johto i livelli sopra il tetto disobbediscono. In gioco Kanto
  arriva dopo le 8 medaglie, quindi non succede; da debug su un salvataggio
  con meno medaglie sì (a 6 medaglie il tetto è 70, sopra ogni livello di Red).
- Esclusi già in A1: level scaling (`IsTrainerExcludedFromScaling`) e
  TcloneR (`CanCatchTrainerMon`), tramite `IsRedsPathTrainer`
  (`include/battle_partner.h`), per id allenatore e non con un flag.
- **Randomizer e challenge menu esclusi** (decisione dell'utente, 24/10):
  `FLAG_REDS_PATH_BATTLE` (ex `FLAG_UNUSED_EXTENDED_86`) è il flag "lotta
  di Red's Path in corso" della tabella "Stato salvato". Lo accende lo script
  prima di caricare la squadra di Red e lo spegne dopo `LoadPlayerParty`; lo
  spengono anche `Overworld_ResetBattleFlagsAndVars` (svenimento) e la
  migrazione 10. `IsRedsPathBattle()` (`include/battle_partner.h`) lo legge:
  - `RandomizerFeatureEnabled` restituisce FALSE per tutto (specie e mosse
    dei capi, abilità, tipi, tabella dei tipi);
  - Mirror (scambio e ripristino), `TrainerScalingIVs/EVs`,
    `NoItemTrainer`, `NoItemPlayer`, `LessEscapes`, `BaseStatEqualizer`;
  - Nuzlocke: i Pokémon di Red esausti non vengono cancellati né messi al
    cimitero (come al Bug Contest), e una sconfitta non cancella il
    salvataggio in Hardcore né rimpiazza la squadra.
  Non toccati, perché agiscono solo su esperienza, EV ed evoluzioni della
  squadra di Red, che il ripristino butta via: `LevelCap`, `ExpMultiplier`,
  `NoEVs`, `EvoLimit`; esperienza ed evoluzioni restano un punto della A2.
  Restano attive anche le *modalità* (`tx_Mode_*`: tipi Folletto, abilità
  dei leggendari…), che non sono sfide.
  Verificato in emulatore: con randomizer allenatori, Mirror e
  `TrainerScalingIVs` accesi, Brock di Red's Path ha Geodude Lv12 con PS
  massimi 31 (IV 0) e il giocatore ha Charmander; flag acceso durante la
  lotta, spento dopo, squadra tornata. Controllo: la stessa Erika lanciata
  da `Trainers → Try Battle` (senza flag) esce con uno Slowbro di Galar e la
  squadra specchiata. Nuzlocke non provato in emulatore, solo letto.
- Prima prova di difficoltà, per la C (una partita sola, giocata male):
  Brock ha battuto Charmander, Mankey e Butterfree con l'Onix a circa un
  quarto dei PS (Rock Tomb è 4× su Butterfree e lo mette KO prima che agisca). Serve l'emulatore
(`docs/claude/emulator.md`) e il `.srm` dell'utente (migrazione nella A,
palestre già battute nella B e nella C). Soulgold non serve.

**Cosa ha lasciato la A2 alla B** (verificato in emulatore il 25/10,
dettagli in `.claude/rules/reds_path.md`):

- Agganci fatti come nella tabella "La lotta", con una differenza:
  `B_FLAG_NO_WHITEOUT` **è** `FLAG_REDS_PATH_BATTLE`, non un flag nuovo (un
  bit in più da migrare, e acceso per sbaglio toglierebbe lo svenimento
  ovunque). L'esperienza è bloccata in `Cmd_getexp` da `IsRedsPathBattle()`,
  non con `FLAG_DISABLE_EXP_GAIN`; le evoluzioni a fine lotta sono saltate.
- **Dopo `RedsPath_EventScript_Battle` lo script riprende sia dopo una
  vittoria sia dopo una sconfitta**: la B li distingue con il flag del capo
  (`TRAINER_FLAGS_START + TRAINER_REDS_PATH_*_HNS`, `goto_if_defeated`).
- **Nessun premio in soldi alla vittoria** (decisione dell'utente, 25/10):
  lo script di lotta salta `getmoneyreward` e il suo messaggio
  (`jumpifredspathbattle`).
- **Nei testi della lotta c'è sempre RED** (decisione dell'utente, 25/10):
  "RED is challenged by…", "RED used <oggetto>!", "RED defeated…", e
  `{B_PLAYER_NAME}` vale RED. Le domande al giocatore ("Would you like
  to…") restano in seconda persona.
- Il follower si aggiorna nel ripristino: nella scena della B, se il
  giocatore diventa Red prima della lotta, va nascosto lì (o
  `FLAG_TEMP_HIDE_FOLLOWER`).

**Cosa ha lasciato la B alla C** (verificato in emulatore il 25/10,
dettagli e trappole nella sezione "Mondo" di `.claude/rules/reds_path.md`):

- Fatto come nel §4, con le scelte tecniche rimaste aperte: **Red è un
  oggetto a parte** sopra il giocatore invisibile (cambiare lo sprite del
  giocatore non regge al ritorno dalla lotta), **il capo di Gen 1 è lo
  stesso oggetto con la grafica cambiata** (Janine → Koga, Blue → Giovanni).
  Tutto ciò che dipende dalla palestra sta in `sGyms` (`src/reds_path.c`).
- Testi: la proposta inglese del §4 così com'è; aggiunti "You've already
  relived RED's battle with X!" (sfida già fatta in quella palestra), una
  riga per i premi rimasti in altre palestre a sfide finite, e le
  congratulazioni dopo "RED WINS!" ("Congratulations! You beat X just like
  RED did!"). La fanfara di "RED WINS!" è quella della medaglia.
- **L'aiutante di Erika** è a (7,16), accanto alla statua come a Pewter, con
  un consiglio su Erika prima della medaglia.
- **Seafoam**: si esce da una scala e la casella sopra è un muro, quindi
  l'introduzione scatta solo uscendo dal lato dell'aiutante (19,22)-(20,22);
  dall'altro lato si vede parlandogli. Blaine è quasi sempre il penultimo
  capo, quindi in pratica l'introduzione arriva prima da un'altra palestra.
- OT dei premi a 7 caratteri: `SURGE`, `GIOVANN`. Il premio non passa dal
  randomizer (è l'asso del capo, come deciso); con la sfida "un tipo solo"
  attiva un asso di un altro tipo finisce nel PC come ogni regalo.
- Il quest book (`quest-log.md` §7) ha i nomi dei flag; la sua migrazione
  diventa la 12.
- Per la C: le lotte si raggiungono dagli aiutanti (o dal debug, senza
  scena). Per rigiocare una sfida già vinta basta spegnere il flag
  dell'allenatore (`TRAINER_FLAGS_START + 653..660`).

Prompt da incollare (A1):

> Implementa la sessione A1 di `docs/plans/reds-path.md` (passi 1, 2, 3 e 5
> della sessione A), con le decisioni già scritte nel piano. Ti allego il mio
> salvataggio. Consegna la ROM zippata e apri la PR.

Prompt da incollare (A2, dopo il merge della A1):

> Implementa la sessione A2 di `docs/plans/reds-path.md` (passi 4, 6 e 7
> della sessione A; la A1 è in `master`). Consegna la ROM zippata e apri la
> PR.

Prompt da incollare (B):

> Implementa la sessione B di `docs/plans/reds-path.md` (la A è in `master`).
> Ti allego il mio salvataggio. Consegna la ROM zippata, gli screenshot della
> scena e apri la PR.

Prompt da incollare (C):

> Sessione C di `docs/plans/reds-path.md`: tariamo le otto lotte. [Le mie
> prove: …]

## Domande aperte

Nessuna di design. Restano i ritocchi di bilanciamento della C.

## Scartato

- **I capi di Giallo**: squadre più forti da Koga in poi ma set meno
  sensati e nessuna cura. Dettagli nell'analisi.
