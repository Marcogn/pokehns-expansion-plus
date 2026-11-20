# Piano: cattura nell'overworld (stile Leggende Pokémon: Arceus)

**Stato (20/11): fasi 1-6 fatte (tetti e prezzi; passo lento e percezione; nucleo del lancio; animazione; pannello e documentazione; revisione della prima parte); prossima: fase 7 (Sonnet 5.5). Il 20/11 sono entrate le fasi 8-11 (menu, OVERWORLD CATCH, ARCEUS FORMULAS) e gli alfa sono passati a 12-14.**

Si appoggia agli OWE già in gioco: leggere prima
[`.claude/rules/overworld_encounters.md`](../../.claude/rules/overworld_encounters.md)
e, per i fatti del port, [`overworld-encounters.md`](overworld-encounters.md).

## 1. Cosa si fa

- **Lancio della Ball nell'overworld**: con **R** il giocatore lancia una Ball
  (o il suo Pokémon, che fa partire la lotta) dritta davanti a sé; se sulla linea c'è un Pokémon selvatico visibile (OWE)
  si tenta la cattura senza entrare in lotta, con un'animazione vera (volo,
  Pokémon risucchiato, scosse, esito).
- **Toccare o farsi toccare** resta com'è: lotta classica.
- **Passo lento** (al posto del gattonare): **B tenuto + direzione**, ovunque,
  anche durante la ricerca DexNav (che oggi usa A tenuto).
- **Percezione migliore**: i Pokémon sentono il giocatore in base a quanto
  rumore fa (corsa, camminata, passo lento), lo vedono in un cono, non lo
  vedono attraverso gli ostacoli né quando è nell'erba alta.
- **Meno Pokémon visibili** e ricambio più lento.
- **Ball speciali meno care.** Niente crafting.
- **Pokémon alfa** (seconda parte, fasi 8-11): più grandi sulla mappa, sempre
  aggressivi, più forti all'inizio della lotta come i totem. Se sono troppo
  forti per le medaglie che hai **non si possono catturare**: si possono
  battere, ma conviene fuggire. Catturati restano alfa per sempre, con
  abilità nascosta, una mossa che la specie non ha salendo di livello e un
  segno nel riassunto.
- **Ricorda mosse a pagamento** per le mosse uovo e tutor, come in Soulgold
  (fase 7): così la mossa di un alfa resta speciale.

## 2. Decisioni prese

Risposte dell'utente (14/11):

| # | Decisione |
| --- | --- |
| D1 | **Comandi** (utente, 16/11; sostituisce la prima proposta "L lancia, A lento"): **R** lancia la Ball o il Pokémon scelto; **L premuto e rilasciato** passa da Ball a Pokémon e viceversa; **L tenuto + ←/→** scorre le Ball o i Pokémon disponibili (il giocatore non si muove mentre L è tenuto); **B tenuto** = passo lento. SELECT resta allo strumento registrato (premuto) e al secondo (tenuto 1 s). R in bici alterna ancora Mach e Acro. La scorciatoia R della ricerca DexNav **sparisce**; il DexNav si apre dal menu Start (icona DexNav, `src/strip_start_menu.c:192`). Mock del 16/11 approvati: modalità Ball e modalità Pokémon con L tenuto. |
| D17 | **Lancio del Pokémon**: in modalità Pokémon, R manda il Pokémon scelto contro il selvatico davanti e parte la lotta con **quel Pokémon in campo per primo** (solo per quella lotta). Scelta solo fra Pokémon non esausti e non uova. Nessun bonus di sorpresa nella prima versione. Senza bersaglio non succede niente. |
| D18 | **Pannello** (Ball o Pokémon scelto, quantità o livello, modalità) **in basso a sinistra** (utente, 16/11; nei mock era a destra): compare **mentre L è tenuto e per ~2 s** dopo un cambio o un lancio, poi sparisce; si nasconde quando si apre un messaggio o un menu. Segue il tema scuro senza codice in più se usa la cornice della palette 14 e il testo della 15, come il banner degli strumenti (`dark_ui.md`): da verificare in emulatore, fase 5. |
| D2 | **Probabilità**: formula normale della lotta a PS pieni, **×2 se il Pokémon non ha notato il giocatore**. Se la cattura fallisce il Pokémon si allerta: fugge o attacca secondo l'indole. |
| D3 | **Densità**: tetti **4 / 2 / 2** (erba / acqua / grotta), **Safari 3**; ricambio a tetto pieno da 240 a **480 frame**. |
| D4 | **Prezzi**: Poké/Great/Ultra restano 10/15/30. Net, Nest, Dive, Dusk, Timer, Repeat da 1000 e Luxury da **3000** (non 1000: `I_PRICE >= GEN_8`) a **100**; Heal da 300 a **50**. |

Scelte di progetto prese qui, **con un default**: l'utente può cambiarle prima
della fase che le realizza.

| # | Scelta | Default | Fase |
| --- | --- | --- | --- |
| P1 | Quale Ball parte | l'ultima lanciata (`gLastThrownBall`) se è in borsa, altrimenti la prima della tasca Ball; **L tenuto + ←/→** la cambia (D1) | 3, 5 |
| P22 | Andature e rumore (fase 2) | AUTORUN acceso (default delle partite nuove dal 16/11; prima era spento: v. §5.2): direzione sola = **corsa**, sentita a 5 caselle in ogni direzione; B tenuto = **lenta** (`PlayerWalkSlow`, il passo del DexNav), non sentita: conta solo la vista; bici = 5. La **camminata normale** (sentita a 2) esiste solo con AUTORUN spento (P20) | 2 |
| P23 | Forma del cono (fase 2) | oggi è un rettangolo 4×3 uguale per tutti (`CanAwareOWESeePlayer`, `src/wild_encounter_ow.c:2021`). Diventa un cono: larghezza 1 casella a distanza 1, 3 a distanza 2-3, 5 da 4 in poi, fino alla `viewDistance` del comportamento; la vista si ferma contro le caselle che bloccano il passaggio fra il Pokémon e il giocatore | 2 |
| P20 | AUTORUN spento | B tenuto resta la **corsa**, come oggi: senza corsa automatica il passo lento non c'è e la camminata normale si sente a 2 caselle | 2 |
| P21 | BUTTON MODE `L=A` | L vale A (`src/main.c:300`): niente cambio di modalità né scorrimento; R lancia comunque la Ball o il Pokémon già scelto | 3 |
| P2 | Gittata | 5 caselle in linea retta; si ferma al primo ostacolo o dislivello (D16: niente consumo) | 3 |
| P3 | Nuzlocke / One Type / Species Clause | le stesse regole della lotta: dove la cattura sarebbe bloccata la Ball non parte (messaggio); una cattura riuscita segna la zona. **Un lancio fallito non la segna** (utente, 17/11): col lancio conta solo la prima cattura riuscita, diversamente dalla lotta, dove qualunque esito consuma l'incontro. Scelta voluta, non un buco | 3 |
| P4 | Leggendari erranti e sciami | il lancio fallisce sempre e parte la lotta (i loro dati vivono fuori dall'oggetto) | 3 |
| P5 | Zona Safari, Gara Pigliamosche, Azienda Lotta | lancio **spento** nella prima versione | 3 |
| P6 | Esperienza alla cattura | nessuna (in lotta `B_EXP_CATCH` la dà: qui non c'è chi ha partecipato) | 3 |
| P7 | BUTTON MODE `L=A` | sostituita da P21 (il lancio ora è su R) | — |
| P8 | Squadra piena | va al PC come in lotta; box pieni: la Ball rimbalza. In raffica la squadra si può riempire a metà: il controllo è **all'impatto**, non al lancio, e così Nuzlocke e Species Clause (la prima cattura segna la zona, la seconda Ball rimbalza anche se era già in aria) | 3 |

Il ×2 di D2 è una scelta di bilanciamento **nostra**: non è un valore preso da
Leggende Arceus.

Seconda parte, risposte dell'utente (14/11, dopo la prima stesura):

| # | Decisione |
| --- | --- |
| D5 | **Mega fuori** dal gioco per ora: il limite di ROM non è un vincolo per questo piano. |
| D6 | **DexNav resta** (utente, 18/11, fase 6): cerca una specie, catene, nascosti, abilità e mosse; il lancio non lo sostituisce. |
| D7 | **Alfa troppo forte**: non catturabile (né in lotta né con il lancio), ma **si può battere e si può fuggire**. È tarato perché fuggire convenga. |
| D8 | **Alfa all'inizio della lotta**: aumento delle statistiche come i totem. |
| D9 | **Alfa catturato**: resta alfa per sempre (bit nel Pokémon salvato), **abilità nascosta garantita**, **una mossa speciale**, **segno nel riassunto**, **follower più grande**, missioni nel quest book. |
| D10 | **Ricorda mosse**: mosse uovo e tutor solo dopo aver pagato (o con eventi), preso da Soulgold. |
| D11 | Alfa e ricorda mosse **nello stesso piano**, in fasi dopo la cattura. |
| D12 | **Fino a 2 alfa per mappa**, e più frequenti di 1 su 50: con i tetti abbassati (D3) sarebbero troppo rari. |
| D13 | **Missione a livelli**: cattura **1 / 5 / 10 / 30** alfa, con premi crescenti. |
| D15 | **Raffica**: fino a **3 Ball in aria**, intervallo minimo ~½ s fra due lanci, il giocatore resta libero di muoversi. Esito **deciso all'impatto** (Pokémon creato e consegnato subito); le scosse sono solo animazione. Banner **in coda**, uno dopo l'altro; soprannomi chiesti **alla fine** della raffica. Se un Pokémon uscito ti carica e parte la lotta, le catture già decise restano. |
| D16 | **Consumo**: la Ball si consuma solo se colpisce un Pokémon; a vuoto (5 caselle e cade) o contro NPC/muro/cartello torna in borsa. Verso il follower non parte. |
| D14 | **Banner di cattura**: il riquadro in alto degli strumenti ottenuti, con l'icona del Pokémon al posto dell'icona dello strumento, "You caught XXX♂!" e sotto "It was added to your party." o "It was sent to the PC.", con una **fanfara breve**. Prototipo approvato in emulatore il 14/11 (§3). |

Terza parte, risposte dell'utente (18-20/11, dal documento
[Riordino Impostazioni e Sfide](https://claude.ai/artifact/7mFewnXGQH3VQCdSL1afTK),
che tiene la destinazione di ogni voce; le fasi lo rileggono, non lo copiano):

| # | Decisione |
| --- | --- |
| D19 | **Menu riordinati**: Impostazioni in 4 schede (GENERALE, MAPPA, LOTTA, SUONO: la MAIN di oggi ha 20 voci su 20), Sfide in 5 (MODALITÀ con dentro FEATURES, RANDOMIZER, NUZLOCKE, DIFFICOLTÀ, SFIDE). EASY CATCH e LEVEL SCALING vanno in Sfide · DIFFICOLTÀ; CLOCK TYPE e ALT SHINY in Impostazioni · GENERALE; il resto voce per voce nel documento. Spostare una voce non tocca il salvataggio. |
| D20 | **OVERWORLD CATCH** (Impostazioni · MAPPA, ON/OFF, default ON). Spenta: tetti 6/3/3 e ricambio a 240 frame, niente passo lento né udito e cono (B corre), R avvia la ricerca DexNav, L senza pannello; **i prezzi restano**. Gli alfa (fasi 12-14) esistono solo col lancio acceso. |
| D21 | **ARCEUS FORMULAS** (Impostazioni · LOTTA, OFF/ON, default OFF): **una voce sola** per le statistiche di Leggende Arceus (ogni Pokémon, ricalcolate al cambio) e la sua formula di cattura **solo nell'overworld** (in lotta resta quella di oggi); viaggiano insieme (utente, 20/11). Con loro la **formula del danno** di Arceus, se no le prime lotte durano 3-4 volte di più. Livelli d'impegno **derivati** da IV ed EV (niente dati nuovi nel Pokémon); le sfide su IV/EV restano e agiscono tramite i livelli. Il 20/11: **tassi di cattura di Arceus** (dati del gioco, `personal_la` di PKHeX; 80 delle 251 specie di Kanto e Johto li hanno), **rango del giocatore = 8 medaglie di Johto + Campione di Johto (0-9)**, colpo alle spalle come in Arceus (colpito da dietro). Ball una per una, specie senza tasso di Arceus e stadi delle statistiche: documento [Formule di Arceus: scelte](https://claude.ai/artifact/3AR9iBxCBwF5N6RMHweHot). |
| D22 | **Salvataggio**: 2 bit, con il passo `saveVersion < 15` che li azzera. **`unusedTrainerCatchBit` non è libero**: vale 1 nel `.sav` dell'utente (PLUS.11, slot 0 e 1, letto il 20/11) benché il passo 9 lo azzeri; nessun codice di oggi lo scrive. Il secondo bit in coda a SaveBlock3 (20 byte liberi; `autoRun` ha 7 bit a zero nello stesso `.sav`, alternativa). OVERWORLD CATCH salvata invertita (0 = accesa). |

Default della seconda parte, cambiabili prima della fase:

| # | Scelta | Default | Fase |
| --- | --- | --- | --- |
| P9 | Quanti alfa | **1 su 20** Pokémon generati, **al massimo 2 per mappa** (D12); mai shiny e alfa insieme (lo shiny vince); niente alfa in Safari, Gara, Azienda Lotta, acqua (lo sprite da nuoto non scala). Conto: a tetto pieno nasce un Pokémon ogni 480 frame (8 s, D3), cioè 7,5 al minuto: con 1 su 20 un alfa ogni ~2,7 minuti d'erba, con 1 su 50 ogni ~6,7. La fase 12 lo misura in frame e lo ritocca | 12 |
| P17 | Premi della missione (D13) | 1 alfa: 10 Ultra Ball; 5: **da scegliere** (l'Ability Patch si compra già a Violet, `src/shop.c:1000`: non è un premio); 10: Shiny Charm (funziona, `src/pokemon.c:3249`, e nelle mappe di HnS non si ottiene altrove); 30: Master Ball. Si ritirano da un NPC, come lo scienziato di TcloneR (chi e dove: fase 14) | 14 |
| P10 | Livello | quello tirato dalla tabella (già scalato) **+ 10**, senza il tetto `B_WILD_SCALING_MAX_ABOVE_TABLE` | 12 |
| P11 | "Troppo forte" | livello dell'alfa **sopra** `GetLevelScalingCap()` (il tetto delle medaglie anche con LEVEL CAP spento, `src/caps.c`) | 13 |
| P12 | Comportamento | sempre aggressivo, vista più lunga, non sparisce finché sei sulla mappa (come gli shiny) | 12 |
| P13 | Aumento in lotta | +1 a Attacco, Difesa, Att. Sp., Dif. Sp., Velocità, col messaggio dei totem | 13 |
| P14 | Mossa speciale | una mossa uovo della specie a caso, altrimenti una mossa tutor, altrimenti nessuna; sostituisce l'ultima | 13 |
| P15 | Lancio su un alfa troppo forte | la Ball rimbalza **e non si consuma**, con un messaggio; l'alfa ti nota | 13 |
| P18 | Dopo il banner | soprannome chiesto come in lotta (rispetta `skipNicknamePrompt` e il soprannome obbligatorio della Nuzlocke); **niente** pagina del Pokédex | 3 |
| P19 | Fanfara | `MUS_HG_OBTAIN_EGG` (`FANFARE_HG_OBTAIN_EGG`, 131 frame, ~2 s, "hai ricevuto un Pokémon"); il banner si chiude con A o B solo a fanfara finita | 3 |
| P16 | Ricorda mosse MT | restano **sempre** disponibili (D10 parla solo di uovo e tutor; in SG le MT sono spente) | 7 |

## 3. Fatti verificati (14/11, commit `d6520855`)

**Non c'è nulla di già scritto**: nessun lancio di Ball, nessun proiettile
nell'overworld, nessun `#if IS_HNS` che lo spegne. Non è un port da Soulgold.
Nemmeno fuori: nessun progetto GBA con sorgente pubblico ha il lancio in
overworld (ricerca del 15/11 in
[`docs/analysis/overworld-catch-research.md`](../analysis/overworld-catch-research.md));
HnS-Legends lo ha ma è solo una patch BPS, da provare e non da riusare.

Cosa c'è già e si riusa:

- **Passo lento**: `PlayerWalkSlow()` e `gPlayerAvatar.creeping`
  (`src/field_player_avatar.c:906-945`), oggi solo con `DN_FLAG_SEARCHING` e A
  tenuto. Attenzione al blocco della corsa (`:947`): con AUTORUN acceso
  (`autoRun == 0`) è vero anche senza B e fa `return`, quindi il passo lento
  va provato **prima**, come già fa il ramo del DexNav.
- **Tasti oggi**: `FieldGetPlayerInput` (`src/field_control_avatar.c:111`)
  non legge L; `struct FieldInput` ha bit liberi (`input_field_1_1` ecc.). R
  (`:259-287`, ramo `#if IS_HNS`) alterna Mach/Acro in bici e altrimenti avvia
  la ricerca DexNav sulla specie registrata: il secondo uso va tolto (D1).
  SELECT premuto = `UseRegisteredKeyItemOnField(FALSE)`, tenuto 60 frame =
  `(TRUE)`, il secondo strumento registrato (`registeredItemHold`, SaveBlock3;
  `src/item_menu.c:2451`). **B tenuto** oggi, con AUTORUN acceso (non è il default: §5.2;
  `autoRun == 0`), fa camminare invece di correre; con AUTORUN spento fa
  correre (`src/field_player_avatar.c:947-968`). Il passo lento del DexNav è
  su A tenuto (`:933-945`) e va spostato su B (D1).
- **Percezione attuale**: `CanAwareOWESeePlayer` (`src/wild_encounter_ow.c:2021`).
  Due regole sole: corsa o bici entro `activeDistance` (5) = notato in ogni
  direzione; altrimenti un rettangolo davanti al Pokémon, 4 caselle × 3
  (`viewDistance`, `viewWidth`). `CanOWEReachPlayer` (`:2078`) controlla solo
  il dislivello: gli ostacoli non contano. I sei comportamenti hanno **tutti
  gli stessi numeri** (`src/data/pokemon/wild_encounter_ow_behavior.h`).
  `struct BehaviorOWE` ha 6 bit di padding e campi da 4 bit (massimo 15).
- **"Mi ha notato"** oggi è implicito nello stato della macchina di movimento
  (`sprite->sTypeFuncId` ≥ 7, `SetSavedOWEMovementState`,
  `src/event_object_movement.c:12366`): la fase 2 deve esporlo con una
  funzione, perché serve al ×2 di D2.
- **L'OWE non contiene un Pokémon**: l'oggetto tiene specie, shiny, sesso
  (nel `graphicsId`), livello (`sOverworldEncounterLevel`) e categoria. Il
  Pokémon vero nasce in `StartWildBattleWithOWE` (`:574-624`) con
  `PickWildMonNature`, `CreateMonWithIVs`, `TryApplyPokeblockBonus`. La
  cattura deve passare **dalla stessa costruzione** (funzione comune estratta,
  non una copia).
- **Formula di cattura**: `ComputeCaptureOdds`, `ComputeBallData`,
  `ComputeBallShakeOdds` (`src/battle_script_commands.c:10923-11034`) sono
  `static` e leggono `gBattleMons`, `gBattleTypeFlags`, `gLastUsedItem`. Va
  separata la parte pura (specie, livello, PS, stato, Ball, livello del primo
  Pokémon in squadra) e chiamata da tutti e due i lati. Dentro ci sono già:
  EASY CATCH (`IsGuaranteedCatchEnabled`), malus medaglie, bonus livello
  basso. `B_CRITICAL_CAPTURE` è `FALSE`.
- **Dopo la cattura in lotta** succede: `GiveCapturedMonToPlayer`
  (`src/pokemon.c:5422`), `HandleSetPokedexFlag`, soprannome
  (`Cmd_trygivecaughtmonnick`, con `IsNuzlockeNicknamingActive` e
  `skipNicknamePrompt`), `GAME_STAT_POKEMON_CAPTURES`, Ball scritta nel
  Pokémon, effetti di Friend/Heal Ball. La fase 3 fa l'elenco completo
  leggendo `Cmd_givecaughtmon` e `BattleScript_SuccessBallThrow`.
- **Blocchi di sfida**: `SetNuzlockeChecks` (`src/nuzlocke.c:445`) legge
  `gEnemyParty[0]` e imposta `NuzlockeIsCaptureBlocked`,
  `NuzlockeIsSpeciesClauseActive`, `OneTypeChallengeCaptureBlocked`.
- **Grafica della Ball nell'overworld già in ROM**: `gObjectEventPic_*Ball`
  (`src/data/object_events/object_event_graphics.h:407-458`), una per tipo,
  con palette `OBJ_EVENT_PAL_TAG_BALL_*` (`OW_FOLLOWERS_POKEBALLS`), e le
  azioni `MOVEMENT_ACTION_ENTER_POKEBALL` / `EXIT_POKEBALL` dei follower.
  **Nessun file grafico nuovo è necessario.**
- **Densità**: `OWE_SPAWNS_MAX*` (`include/wild_encounter_ow.h:4-14`),
  `OWE_SPAWN_TIME_REPLACEMENT` 240 (`src/wild_encounter_ow.c:57`).
  `OWE_SPAWNS_MAX` dimensiona gli array degli slot e i `localId` 247-252.
- **Prezzi**: `src/data/items.h`. Le Ball con Ghicocca hanno il prezzo
  commentato (0): non si toccano.
- Lo spawn si ferma con i controlli bloccati (`UpdateOverworldWildEncounter`,
  `:440`). **Con la raffica (D15) i controlli restano liberi**, quindi questa
  protezione non c'è più: spawn e cattura usano entrambi `gEnemyParty[0]`.
  Dall'analisi non si intrecciano, perché ciascuno crea, usa e azzera il
  Pokémon nello stesso frame; la fase 3 lo **prova** (spawn forzato nello
  stesso frame di un impatto, ROM strumentata) invece di fidarsi.

**Banner di cattura (prototipo del 14/11, ROM usa-e-getta, non nel git)**:

- Riquadro e icona riusano quelli della descrizione dell'oggetto ottenuto
  (`ScriptShowItemDescription`, `src/overworld.c:3901`; `field.md`):
  finestra (1,1) 28×3 tile, base 8, palette 15, cornice
  `DrawStdFrameWithCustomTileAndPalette(.., 0x214, 14)`. Il Pokémon è
  l'**icona della squadra** (`LoadMonIconPalette` + `CreateMonIcon` con
  `SpriteCB_MonIcon`, animata) centrata a (26, 20) e `oam.priority` 0: lo
  sprite di lotta 64×64 non entra in un riquadro alto 3 tile. Testo in
  `FONT_SMALL` da x = 40 (a 28, come gli strumenti, l'icona copre le prime
  lettere), riga 2 a y = 12.
- **Il font piccolo non ha i simboli di sesso**: "♂" esce come un "8". Il
  simbolo va stampato a parte in `FONT_NORMAL`, blu
  (`TEXT_COLOR_BLUE`/`LIGHT_BLUE`) o rosso (`RED`/`LIGHT_RED`), alla x
  misurata con `GetStringWidth`; esce 1-2 pixel più basso del testo, da
  allineare. Senza sesso (Magnemite) niente simbolo.
- Provati: Pikachu ♂ in squadra, Eevee ♀ al PC, Magnemite, Crabominable (12
  lettere, ci sta con margine). Chiusura pulita: finestra e icona tolte,
  palette liberata, mappa intatta.
- Nel gioco vero il banner va mostrato **dopo** che il Pokémon è stato dato
  al giocatore (serve sapere se è finito in squadra o al PC, e in che box).
  Con la raffica (D15) i banner vanno in una coda (al massimo 3) e si
  mostrano uno alla volta; il prototipo ne gestiva uno solo e bloccava i
  comandi.

Fatti per la seconda parte (alfa e ricorda mosse):

- **Bit nell'oggetto**: `struct ObjectEvent` ha `padding:2` accanto a `shiny`
  (`include/global.fieldmap.h:289`): un bit diventa `alpha`. Gli altri campi
  che gli OWE riusano sono pieni (`sOverworldEncounterLevel` usa il bit 7 per
  il non-sparire, `src/wild_encounter_ow.c:41-47`). Gli oggetti finiscono in
  SaveBlock1: la fase 12 controlla che un salvataggio vecchio legga 0 lì.
- **Bit nel Pokémon salvato**: bit non usati da nessun file, verificato con
  `grep`: `PokemonSubstruct0` `unused_02:6`, `unused_04:3`, `unused_0A:2`;
  `PokemonSubstruct1` `unused_04:5`, `unused_06:3`; `BoxPokemon` `unused_13`,
  `unused_1E` (`include/pokemon.h:130-170, 255-276`). Meglio uno delle
  sottostrutture: sono cifrate e coperte dal checksum. `CreateMon` azzera
  tutto, quindi un Pokémon vecchio legge 0 = non alfa. **Da controllare in
  fase 13**: che expansion non abbia già preso quel bit nelle versioni dopo la
  1.15.2 (un `/upstream-sync` futuro lo sovrascriverebbe) e che l'app
  compagna di PokeDaisy non lo legga (`hasep_symbols.md`).
- **Aumento da totem già nel motore**: `settotemboost`
  (`asm/macros/event.inc:2252`, `ScriptSetTotemBoost` in
  `src/battle_main.c:6577`) riempie `gQueuedStatBoosts`; in lotta lo applica
  `BS_GetTotemBoost` col messaggio. Gli alfa partono da codice, non da script:
  si scrive in `gQueuedStatBoosts` prima della lotta.
- **Blocco della cattura in lotta**: `GetBallThrowableState`
  (`src/item_use.c:1265`) ha già un elenco di motivi (Nuzlocke, One Type,
  `FLAG_NO_WILD_CATCHING`): si aggiunge `BALL_THROW_UNABLE_ALPHA`.
- **Missione a livelli: il modello c'è già**, TcloneR (`src/tcloner.c:26`,
  `trainer_catch.md`): un contatore in una var (`VAR_TCLONER_COUNT`), un'altra
  var per i premi ritirati (`VAR_TCLONER_REWARDS`), una tabella `sRewards` di
  soglie e premi, lo stato `QUEST_STATE_REWARD` del quest book e una
  sottomissione per premio (**le due liste vanno tenute allineate a mano**,
  `quest_book.md`). Gli alfa servono due var: `vars.h` ne segna diverse come
  `VAR_UNUSED_*`, ma `trainer_catch.md` avverte che alcune vecchie "libere"
  **non sono dimostrabilmente a zero** nei salvataggi: la fase 14 le controlla
  sul `.srm` dell'utente. Il contatore sale a ogni alfa catturato, in lotta e
  col lancio (stessa funzione per i due lati, fase 13).
- `setwildbossbattle` (`src/scrcmd.c:3383`) è una lotta di script con mosse
  scelte, senza aumento di statistiche: non serve agli alfa.
- **Sprite più grande**: il motore scala già sprite di oggetti con matrici
  affini (`src/event_object_movement.c:1531`, riflessi). Le matrici affini
  sono 32 e gli OWE in acqua ne usano una (`ST_OAM_AFFINE_ERASE`). Un oggetto
  scalato oltre i suoi 32×32 viene tagliato se non è in doppia dimensione:
  **prototipo prima di promettere**, in fase 12.
- **Ricorda mosse oggi**: `P_ENABLE_MOVE_RELEARNERS` **TRUE**
  (`include/config/summary_screen.h:13`) apre tutto; `P_FLAG_EGG_MOVES` e
  `P_FLAG_TUTOR_MOVES` sono **0**. Il codice li legge già
  (`src/move_relearner.c:1217, 1284`), e anche `data/scripts/move_relearner.inc`.
  In Soulgold (verificato in passato, `relearner.md`): MT spente, uovo dietro
  `FLAG_EGG_MOVES_UNLOCKED` (Egg Move Master, Blackthorn City House 3,
  ¥88.888), tutor dietro `FLAG_TUTOR_MOVES_UNLOCKED` (Tutor Move Master,
  Olivine City House 4, ¥44.444). **Qui** `BlackthornCity_House3_hns` ha già
  il Cancella mosse e un ricorda mosse a Squama Cuore
  (`BlackthornCity_MoveRelearnersHouse_EventScript_MoveRelearner`, riga 107),
  mentre `OlivineCity_House4` non esiste (ci sono House1-3): dove mettere il
  maestro dei tutor si decide in fase 7, guardando le mappe. Il sorgente di SG non è
  in questa sessione: la fase 7 lo aggancia (`add_repo Marcogn/soulgold`).
- **Polarità del ricorda mosse**: i flag nuovi valgono 0 nei salvataggi
  vecchi, quindi chi aggiorna **perde** l'accesso libero a mosse uovo e tutor
  finché non paga. È l'effetto voluto (D10), ma va scritto nel CHANGELOG.

Trappole già note che qui mordono (dalla regola degli OWE):

- VRAM: nel Safari restavano 10 tile su 1024; `OWE_SPRITE_TILE_HEADROOM` ne
  riserva 32. Lo sprite della Ball e gli effetti devono starci, e **uno sprite
  che non si crea non deve lasciare uno script in attesa**.
- Mai `assertf` su stati raggiungibili (schermata blu).
- Un OWE in acqua ha lo sprite vero nascosto e uno da nuoto sopra, più 2
  palette: il risucchio nella Ball va provato a parte.
- `gEnemyParty[0]` azzerato: guardare il Pokémon catturato in squadra (il
  precedente è il Bad Egg di `trainer_catch.md`).

### Fonti per ARCEUS FORMULAS (20/11; screenshot di Bulbapedia mandati dall'utente)

Bulbapedia risponde 403 da qui: le tre sezioni "Pokémon Legends: Arceus" di
[Stat](https://bulbapedia.bulbagarden.net/wiki/Stat),
[Catch rate](https://bulbapedia.bulbagarden.net/wiki/Catch_rate) e
[Damage](https://bulbapedia.bulbagarden.net/wiki/Damage) sono trascritte qui
dagli screenshot dell'utente (20/11). Le immagini non sono nel git.

**Statistiche** (Bulbapedia; uguali a [Pokébip](https://www.pokebip.com/page/jeux-video/legendes-pokemon-arceus/statistiques-niveau-effort)
e [Poképédia](https://www.pokepedia.fr/Calcul_des_statistiques)):

- PS = ⌊(L/100 + 1) × B + L⌋ + ELB
- altra statistica = ⌊⌊(L/50 + 1) × B / 1,5⌋ × N⌋ + ELB, N = 0,9 / 1 / 1,1
- ELB = **round**((√B × m + L) / 2,5): all'intero più vicino (chiude il dubbio
  fra Pokébip e Poképédia). m per impegno 0-10: 0, 2, 3, 4, 7, 8, 9, 14, 15,
  16, 25 (la tabella di Bulbapedia salta la riga 1; il 2 viene dalle altre due
  fonti).
- Gli IV servono solo a dare l'impegno iniziale, fino a 3: 0-20 → 0, 21-25 →
  1, 26-30 → 2, 31 → 3 (Pokébip; il caso IV 20 resta da confermare sulla
  pagina Effort level, conta solo per quel valore).
- Controllo: base 120, Lv 100, impegno 10 → PS 490, altra 390 (anche
  ts-pokeapi).

**Cattura** (Bulbapedia, Catch rate):

- a = (0,75 × rate + outbreak) + 30 × (rank + (3·PSmax − 2·PS)/(3·PSmax) ×
  status × ball × behavior × backstrike × berry)
- outbreak = 10 per un Pokémon di uno sciame (Mass Outbreak), se no 0.
- rank = rank_giocatore − rank_Pokémon + 1, **fra −5 e 0**; rank_Pokémon =
  ⌊(livello − 1)/10 + 1⌋, +4 se alfa, al massimo 10. rank_giocatore è il rango
  del Team Galassia (da noi: dalle medaglie, D21).
- status 2 con uno stato; backstrike 1,75 se colpito alle spalle; berry 1,3
  se mangia una Bacca Lampone.
- Ball (base / potenziata): Poké 0,75 / 0,75; Great 1,5 / 1,5; Ultra 2,25 /
  2,25; Heavy 1 / 1,25; Leaden 1,75 / 2; Gigaton 2,5 / 2,75; Feather 1 / 1,25;
  Wing 1,25 / 2; Jet 2 / 2,75 (Heavy/Leaden/Gigaton potenziate se non ti ha
  visto o in lotta; Feather/Wing/Jet se vola).
- behavior: base 2, reazione docile 2, timida 1, aggressiva 1, cauto 2,
  mangia 2,5 (bacca 2,5, torta 4), dorme 4, stordito 3, riposa 2,5, fugge 1.
- **Scosse**: a si ferma a 255; b = 65535 × (1 − √(1 − a/255)); due
  controlli contro un numero fra 0 e 65535, ne basta uno. a < 0 = cattura
  impossibile. **Ne segue P(cattura) = a/255 esatta** (due controlli con
  p = 1 − √(1 − a/255)).
- Il primo Pokémon iniziale fallisce sempre al primo tentativo (dettaglio di
  Arceus, da non portare).

**Danno** (Bulbapedia, Damage; ogni moltiplicazione e divisione arrotonda per
difetto salvo dove detto):

- Danno = ((100 + A + 15 × Livello) × Potenza / (D + 50)) / 5 × (AtkMod ×
  DefMod) × random × Tipo × Altri
- Altri = Meteo × Brutto colpo × STAB × Scottatura/Congelamento × Sonnolenza
  × Fissato (attacco) × Fissato (difesa) × Concentrato
- A, D: Attacco o Att. Sp., Difesa o Dif. Sp. effettivi; AtkMod 1,5 / 1 /
  0,66 se le statistiche offensive sono alzate / neutre / abbassate, DefMod
  1,5 / 1 / 0,66 se le difensive sono abbassate / neutre / alzate (Arceus non
  ha i sette stadi: solo su, giù o niente); random intero 85-100 / 100; Tipo
  0,4 / 0,5 / 1 / 2 / 2,5 (doppia debolezza 2,5, non 4); STAB 1,25; brutto
  colpo 1,5; Fuoco sotto la pioggia 0,75; scottatura 0,5 sugli attacchi fisici.

**Effetto misurato** (calcolo; IV 15 EV 0 contro impegno 0, natura neutra,
mossa senza STAB ed efficacia 1, danno massimo, attaccante e difensore della
stessa specie):

| Base, livello, potenza | Classica: colpi per KO | Statistiche Arceus, danno classico | Arceus intero |
| --- | --- | --- | --- |
| 50, Lv 5, 40 | 4 | 12 | 4 |
| 50, Lv 25, 40 | 6 | 9 | 3 |
| 50, Lv 50, 80 | 4 | 4 | 2 |
| 100, Lv 5, 40 | 5 | 23 | 7 |
| 100, Lv 50, 80 | 5 | 6 | 3 |
| 100, Lv 100, 80 | 5 | 5 | 4 |

Le statistiche da sole allungano molto le lotte iniziali; con il danno di Arceus
le lotte iniziali restano simili e quelle di metà gioco si accorciano.

Cattura con la formula di Arceus (PS pieni, comportamento base ×2, rango del
giocatore = medaglie): Sentret Lv 3 con Poké Ball 81%; con 6 medaglie,
Furret Lv 21 32% (alle spalle 37%, Ultra 44%), Pinsir Lv 22 19% (Ultra 31%);
**Pinsir Lv 22 con 0 medaglie: impossibile con la Poké Ball** (a < 0), 7%
con la Ultra. Con la formula di oggi Furret era 12% / 28% e Pinsir 6% / 12%
(fase 6).

**Deciso il 20/11** (D21): tassi di cattura di Arceus; rango del giocatore =
medaglie di Johto + 1 da Campione di Johto (0-9: con 9 anche un Pokémon di
rango 10, un alfa di livello alto, dà rango 0, il massimo); colpo alle spalle
di Arceus (Pokémon colpito da dietro, ×1,75; il ×2 di D2 sparisce con
l'opzione accesa).

**Tassi di Arceus**: `PKHeX.Core/Resources/byte/personal/personal_la` di
[PKHeX](https://github.com/kwsch/PKHeX) (dati estratti dal gioco), voci da
0xB0 byte per numero nazionale, tasso al byte 0x08, presente nel gioco al bit 6
del byte 0x21 (`PersonalInfo8LA.cs`). Letto il 20/11: 226 specie, di cui 48 di
Kanto e 32 di Johto. **Rifatti specie per specie, non scalati**: Onix 45 → 235,
Scyther 45 → 215, Machop 180 → 245, Heracross 45 → 85, Gyarados 45 → 55,
Pikachu 190 → 145. Da confrontare con una seconda fonte in fase 10.

**Ancora da decidere** (tendine nel documento
[Formule di Arceus: scelte](https://claude.ai/artifact/3AR9iBxCBwF5N6RMHweHot); default tra parentesi): ogni Ball (Poké/Great/Ultra coi valori di
Arceus, le altre con il loro effetto del gioco × 0,75, Master invariata); le 171
specie senza tasso di Arceus (tasso del gioco); stadi delle statistiche (tenere
gli stadi del gioco, AtkMod = DefMod = 1, con STAB 1,25 e Tipo 2,5 / 0,4 di
Arceus). Comportamento: base 2, chi ti ha notato e carica o fugge 1.

## 4. Spazio misurato (build pulita di `d6520855`)

| | Usato | Limite | Libero |
| --- | --- | --- | --- |
| ROM | 31 902 628 B (95,08%) | 32 MB, errore CI al 99,5% | 1 651 804 B. Le mega sono fuori per ora (D5); se tornassero (`megas.md`, 1 574 944 B) la ROM andrebbe al 99,77%, già oltre il limite senza questa feature |
| SaveBlock3 | 96 B | 116 | 20 B |
| VRAM sprite | 622 tile, 10 palette (Route 29 di notte, 4 OWE + follower + 3 Ball di tipi diversi in aria; fase 4) | 1024 tile, 16 palette | 402 tile, 6 palette |

Costo previsto della prima parte: **solo codice** (stima 8-15 kB, da
rimisurare a ogni fase), **zero byte di salvataggio** (`gLastThrownBall` è già
in EWRAM, nessuna opzione nuova), nessuna grafica nuova.
Seconda parte: **due flag** nuovi per il ricorda mosse (`save.md`: liberi
`FLAG_*` e engine flag da `+14`), **un bit** in ogni Pokémon salvato (bit già
riservato, struct invariata), un bit nell'oggetto; grafica forse un'icona del
segno alfa per il riassunto. Nessun campo nuovo in SaveBlock3.

## 5. Fasi

Una fase per sessione (`/next-phase overworld-catch`). Modelli scelti fra
Fable, Opus e Sonnet come chiesto dall'utente: Fable solo dove un errore
costa di più del suo prezzo.

| # | Fase | Modello | Cancello per passare oltre |
| --- | --- | --- | --- |
| 1 | **Densità e prezzi** (D3, D4): `OWE_SPAWNS_MAX_LAND/WATER/CAVE/SAFARI`, `OWE_SPAWN_TIME_REPLACEMENT`, prezzi in `items.h`. `OWE_SPAWNS_MAX` resta 6 (dimensiona slot e `localId`) | **Sonnet 5.5** | build verde; in emulatore il conteggio degli OWE su una rotta d'erba non supera 4 in 60 s di gioco (misurato in frame) e 2 in grotta; screenshot di un negozio con i prezzi nuovi. `.srm` non necessario |
| 2 | **Passo lento e percezione**: B tenuto = passo lento con AUTORUN acceso, anche nella ricerca DexNav (al posto di A tenuto), P20 con AUTORUN spento; rumore per andatura (corsa/bici 5, camminata 2, passo lento 0: solo vista); cono di vista con numeri diversi per comportamento; ostacoli che coprono la vista; giocatore nell'erba alta non visto oltre 1 casella; funzione pubblica `IsOWEAwareOfPlayer()` | **Opus 5.5** | build verde; prove in emulatore con posizioni lette via GDB: da dietro a passo lento si arriva adiacenti senza essere notati, camminando si viene notati a 2, correndo a 5, di fronte alla distanza di vista, dietro un ostacolo mai. A premuto davanti a un NPC o un cartello interagisce ancora; nella ricerca DexNav il passo lento con B non fa scappare il bersaglio; con AUTORUN spento B fa ancora correre. `.srm` non necessario |
| 3 | **Nucleo della cattura**, senza animazione (Ball istantanea, messaggio): comandi di D1 (R lancia, L premuto cambia modalità, L tenuto + ←/→ scorre senza muovere il giocatore, scorciatoia DexNav su R tolta, Mach/Acro su R in bici invariato), **lancio del Pokémon** (D17), bersaglio sulla linea (P2), consumo solo a bersaglio colpito (D16), **esito all'impatto e coda per la raffica** (D15: fino a 3 lanci pendenti, intervallo minimo, banner e soprannomi in coda), Pokémon costruito dalla funzione comune con la lotta, formula condivisa con ×2 se non allertato (D2), tutto ciò che la lotta fa dopo una cattura, blocchi P3-P8, reazione al fallimento; **banner di cattura con fanfara** (D14, P18, P19, dal prototipo del §3) | **Fable 5.2** | build verde; `ComputeCaptureOdds` in lotta dà gli stessi numeri di prima su 3 casi (ROM strumentata). In emulatore: ogni tasto di D1 provato a piedi, in bici, con AUTORUN spento e con `L=A`; un Pokémon lanciato scende in campo per primo e la squadra torna nell'ordine di prima; banner guardato per squadra, PC, maschio, femmina, senza sesso e nome lungo; cattura con Master Ball, **guardando il Pokémon in squadra** (specie, livello, shiny, Ball giusti, non Bad Egg), Ball scalata, Pokédex segnato; **raffica di 3** con 2 presi e 1 fuggito, con 3 presi a squadra da 5 (uno in squadra, due al PC), con la seconda Ball su una zona Nuzlocke appena usata (rimbalza, non consumata); lancio a vuoto e contro un NPC (non consumata); spawn forzato nello stesso frame di un impatto; un lancio fallito allerta il Pokémon; zona Nuzlocke già usata, box pieni e Safari rifiutano il lancio; un OWE in acqua o catturato o escluso con il motivo scritto. **Serve il `.srm` dell'utente** (squadra e borsa vere, impostazioni di sfida) |
| 4 | **Animazione**: posa di lancio (derivata dai frame che il giocatore ha già, guardando l'immagine), Ball in volo ad arco con lo sprite del suo tipo, risucchio, caduta, 0-3 scosse da `ComputeBallShakeOdds` (decise all'impatto, D15), cattura o uscita con "!"; **fino a 3 Ball animate insieme**, e una lotta che parte mentre altre Ball tremano le chiude prima della transizione | **Fable 5.2** | build verde; tile e palette libere misurate nel caso peggiore **con 3 Ball di tipi diversi in aria** (se non ci stanno, il limite scende e si scrive), nessuno script in attesa se uno sprite non si crea (provato forzando il fallimento); sequenza di screenshot guardati per cattura, fuga dopo 1 e dopo 3 scosse, nelle 4 direzioni, di giorno e in grotta. `.srm` utile, non necessario |
| 5 | **Rifinitura e documentazione**: pannello di D18 (Ball o Pokémon, quantità o livello, modalità) dai mock del 16/11, rifatto con font e finestre del gioco, suoni, messaggi; `CHANGELOG.md`, `README.md`, regola `.claude/rules/overworld_catch.md` (con `paths:`), indice in CLAUDE.md §7, `docs/SOULGOLD_PORT_REPORT.md` (feature nuova, non un port) | **Sonnet 5.5** | build verde; pannello guardato con 1, 2 e 0 tipi di Ball in borsa e con 1 e 6 Pokémon, in tutti e due i temi; sparisce dopo ~2 s; testi riletti contro il codice; `docs_validate` verde |
| 6 | **Revisione della prima parte**: `/code-review` medio sul diff delle fasi 1-5, ogni punto aperto provato in emulatore, bilanciamento (quante Ball per una cattura a inizio gioco e a metà), ROM consegnata zippata. Qui si decide anche se togliere il DexNav (D6) | **Opus 5.5** | build e CI verdi, percentuale di ROM annotata, punti aperti chiusi o motivati. **Serve il `.srm` dell'utente** |
| 7 | **Ricorda mosse a pagamento** (D10), port da Soulgold: `P_ENABLE_MOVE_RELEARNERS` FALSE, `P_TM_MOVES_RELEARNER` resta TRUE (P16), due flag, due maestri con il loro prezzo (dove: dalle mappe, v. §3), testo nel riassunto quando la voce è chiusa | **Sonnet 5.5** | build verde; nel riassunto le mosse uovo/tutor non compaiono prima e compaiono dopo aver pagato (screenshot); le MT sì in tutti e due i casi; un salvataggio vecchio parte chiuso. **Serve il `.srm`** (Pokémon con mosse uovo vere) |
| 8 | **Menu e posto per le voci nuove** (D19, D22): schede nuove nei due menu dal documento (al massimo 20 righe ciascuna), voci spostate, OVERWORLD CATCH e ARCEUS FORMULAS presenti ma ancora senza effetto, `SAVE_VERSION` 15 con il passo che azzera i 2 bit, titolo (`oak_speech_hns.c`) e `NewGameInitData`; `save.md` corretta sul bit che non era libero | **Opus 5.5** | build verde; col `.srm` dell'utente: prima e dopo il caricamento `ChallengeSettings` e `SoundAndFieldOptions` uguali byte per byte salvo i 2 bit, che leggono 0; salvato e ricaricato, checksum giusto; ogni scheda guardata nei due temi, nessuna oltre 20 righe; una New Game tiene le scelte fatte al titolo. **Serve il `.srm`** |
| 9 | **Interruttore OVERWORLD CATCH** (D20): spenta = il gioco di prima del piano salvo i prezzi; accesa = com'è oggi. Cambio a mappa aperta senza Pokémon di troppo (`TryRemoveOWEAboveCap` lavora già al contrario) | **Opus 5.5** | build verde; spenta: R avvia la ricerca DexNav sulla specie registrata, B corre, L non fa nulla, fino a 6 OWE in erba in 60 s (misurato in frame), nessuna Ball lanciata; accesa: le prove della fase 6 (riva, mancato, fermo, Pokémon mandato) danno gli stessi esiti. `.srm` utile |
| 10 | **ARCEUS FORMULAS: statistiche e cattura** (D21). Formule e default da decidere nel §3 (fonti complete dal 20/11); le statistiche in `CalculateMonStats` dietro l'opzione (livelli d'impegno da IV/EV, squadra ricalcolata al cambio e al caricamento con i PS in proporzione, box al ritiro) e la cattura di Arceus in `ComputeOverworldCaptureOdds` (rango dalle medaglie, colpo alle spalle al posto del ×2) | **Opus 5.5** | build verde; 3 Pokémon (Lv 5, 25, 50) calcolati a mano e letti in RAM, opzione accesa e spenta, PS e stato conservati al cambio; probabilità di cattura su 3 casi uguale al calcolo a mano (ROM strumentata); il `.srm` dell'utente carica con l'opzione accesa e spenta. **Serve il `.srm`** |
| 11 | **ARCEUS FORMULAS: danno e bilanciamento** (D21): formula del danno di Arceus dietro la stessa opzione (fonte nel §3), con BST EQUALIZER e le sfide su IV/EV provate insieme; misura dei colpi per KO a inizio e metà gioco con l'opzione accesa e spenta; CHANGELOG e regola (`overworld_catch.md` o una nuova `arceus.md`) | **Opus 5.5** | build verde; danno su 3 casi uguale al calcolo a mano (ROM strumentata); tabella dei colpi per KO accesa/spenta annotata, con l'utente a decidere se ritoccare; l'opzione accesa non rompe le lotte con allenatori scalati (`LEVEL SCALING`). `.srm` utile |
| 12 | **Alfa sulla mappa**: bit `alpha` nell'oggetto, tiro P9, livello P10, comportamento P12, **prototipo dello sprite più grande** (se non regge: un'aura o un'icona sopra la testa, e lo si scrive) | **Opus 5.5** | build verde; con il debug che forza l'alfa: compare uno per mappa, ti insegue, non sparisce; sprite guardato su erba, in grotta, accanto ad altri OWE; tile e matrici affini misurate; un salvataggio vecchio carica senza alfa fantasma. `.srm` non necessario |
| 13 | **Alfa in lotta e catturato**: aumento P13, blocco della cattura in lotta e col lancio (D7, P11, P15), bit nel Pokémon salvato con `MON_DATA_IS_ALPHA`, abilità nascosta e mossa P14 alla nascita | **Opus 5.5** | build verde; un alfa sopra il tetto respinge la Ball in lotta e nell'overworld (Ball non consumata), si batte e si fugge; uno sotto il tetto si cattura e **in squadra, dopo salvataggio e ricarica e passaggio nel PC, è ancora alfa** con abilità e mossa giuste; un Pokémon del salvataggio vecchio legge non-alfa e il checksum regge. **Serve il `.srm`** |
| 14 | **Alfa visibile e missione**: segno nel riassunto (tutti e due i temi), follower più grande se la fase 12 l'ha reso possibile, **missione a livelli 1/5/10/30** sul modello di TcloneR (contatore, premi P17, NPC che li consegna, sottomissioni nel quest book, `/add-quest`) | **Sonnet 5.5** | screenshot del riassunto chiaro e scuro con indici contati sui `.4bpp`; follower guardato; con il contatore portato a 0, 1, 5, 10, 30 via debug: stato e "NEW" giusti nel libro, ogni premio ritirato una sola volta; le due var nuove a zero nel `.srm` dell'utente prima della modifica. **Serve il `.srm`** |
| 15 | **Revisione finale e documentazione**: `/code-review` medio sul diff delle fasi 7-14, CHANGELOG (con l'avviso del ricorda mosse), regole, report SG (la fase 7 è un port), ROM consegnata | **Opus 5.5** | build e CI verdi, punti aperti chiusi o motivati. **Serve il `.srm`** |

Perché questi modelli: le fasi 1 e 5 sono costanti, dati e testo con un
cancello che prende gli errori. La 2 tocca un invariante nascosto (la macchina
a stati del movimento). La 4 è passata a Fable con la raffica (D15): tre
animazioni concorrenti, la VRAM condivisa con icone ed effetti e una lotta che
può partire nel mezzo. La 3
apre il codice di lotta, crea un Pokémon fuori dalla lotta e incrocia Nuzlocke
e salvataggio: è l'unico punto in cui un errore silenzioso arriva nel
salvataggio dell'utente. Nella seconda parte la 13 tocca il Pokémon salvato, ma
con un bit già riservato e un cancello che salva, ricarica e passa dal PC:
basta Opus. La 7 è un port con sorgente chiaro, la 14 grafica e contenuto.
Le fasi 8-11 (20/11) sono tutte su Opus: la 8 cambia il salvataggio, la 9 il
movimento e R, la 10 e la 11 i numeri di ogni Pokémon e di ogni lotta. Vengono
prima degli alfa perché gli alfa dipendono da tutte e tre le cose: esistono solo
col lancio, hanno un rango in più nella cattura di Arceus e un livello +10 che
con le statistiche di Arceus vale un'altra cosa.

### 5.1 Fase 1, com'è stata fatta (16/11)

- **Cosa**: tetti 4/2/2 e Safari 3 (`include/wild_encounter_ow.h`), ricambio a
  tetto pieno 480 frame (`OWE_SPAWN_TIME_REPLACEMENT`), prezzi in
  `src/data/items.h` (Net, Nest, Dive, Dusk, Timer, Repeat, Luxury 100; Heal
  50). I 480 frame non entravano nel contatore: `sOWESpawnCountdown` è ora
  `u16` con segnale `0xFFFF` (`.claude/rules/overworld_encounters.md`).
  `OWE_SPAWNS_MAX` resta 6. README e `CHANGELOG.md` (2.0.6.PLUS.12) aggiornati.
- **Verificato** (banco usa-e-getta: mGBA con `-g`, GDB che ogni ~1 s di gioco
  scarica la tabella oggetti e `vblankCounter1`; teletrasporto di debug per
  ricaricare la mappa; non nel git):
  - Route 29, 70 campioni in 4024 frame (~67 s): ROM nuova **4 generati in
    tutti i campioni**; ROM con i tetti vecchi, stessa misura: da 3 a 6, **più
    di 4 in 64 campioni su 70**.
  - Grotta dell'Unione 1F, 70 campioni in 4029 frame: **massimo 2**. In una
    schermata si vedevano tre Pokémon: due erano decorativi di mappa (vedi
    handoff), non spawn.
  - Prezzi letti **dalla ROM** (`gItemsInfo`, `0x087b32bc`, 0x2C byte a voce,
    `price` primo `u32`): nuova Heal 50, Net/Nest/Dive/Dusk/Timer/Repeat/Luxury
    100, Poké 10, Great 15, Ultra 30, Premier 20, Quick 50; originale Heal
    300, le sei 1000, Luxury 3000.
  - Un salvataggio con 6 OWE, fatto con la ROM vecchia, si carica sulla nuova.
- **Non verificato**: tetti acqua (2) e Safari (3), che passano dalla stessa
  `GetOWESpawnCap`; la grotta con i vecchi tetti; **la schermata di un
  negozio** che il cancello chiedeva: queste Ball non si vendono a Johto e
  Kanto e il teletrasporto di debug ai negozi di Hoenn va in errore anche sulla
  ROM originale (`emulator.md`), quindi i prezzi sono letti dalla ROM.
- **Dopo `/code-review` (16/11)**: corretti quattro punti. (1) Sopra il tetto
  il ricambio uno per uno teneva l'eccesso finché non si cambiava mappa:
  `TryRemoveOWEAboveCap` toglie il più vecchio non shiny ogni 30 frame.
  Prova: lo stesso salvataggio con 6 OWE, caricato senza ricaricare la mappa,
  prima restava a 6 per 641 frame, ora è a 4 dal primo campione per 1514
  frame (ROM finale). (2) Con OWE dell'altra zona (Pokémon di terra quando si inizia il
  Surf) il ricambio è a 30 frame invece di 480 (`IsAnyOWEFromOtherArea`,
  shiny esclusi perché non vengono mai tolti):
  **non provato in emulatore** (serve una rotta con acqua e il Surf). (3)
  Commento di `OWE_SPAWNS_MAX` corretto. (4) `STATIC_ASSERT` dei tetti
  contro `OWE_SPAWNS_MAX`. Lasciati, con il motivo: conteggi ripetuti (non
  per frame, schema di prima), Luxury senza `I_PRICE` (decisione D4),
  CHANGELOG sulle Ball non in vendita (punto aperto qui sotto).
- ROM finale: 95,08% usato.

**Handoff per la fase 2 (Opus 5.5)**
- Non serve il `.srm`. Per misurare posizioni da GDB: `currentCoords` a
  +0x10 (x) e +0x12 (y, `s16`), `previousCoords` a +0x14, `facingDirection`
  a +0x18 (`struct ObjectEvent`, `include/global.fieldmap.h`); il resto della
  ricetta (offset, `vblankCounter1`, tasti con `xdotool`) in `emulator.md` e
  nella regola.
- **Punto aperto per l'utente**: le Ball speciali **non si vendono in nessun
  negozio di Johto/Kanto** (solo Rustboro, Verdanturf e Mossdeep a Hoenn e la
  Casa 2 di Melemele a Alola: mappe che il debug non raggiunge). Il nuovo
  prezzo oggi si vede solo nel valore di vendita. Decidere dove si vendono
  (`sMartExtras`, `src/shop.c:999-1008`, come la Quick Ball ad Azalea) è una
  scelta di contenuto, non di questa fase.
- **Fase 3**: le mappe di HnS hanno **Pokémon decorativi** (oggetti
  `OBJ_EVENT_GFX_MON_BASE+…`, `trainerType` 0): sono sul percorso di un
  lancio ma non sono selvatici. Il bersaglio va scelto con
  `IsOverworldWildEncounter`, e gli altri vanno trattati come ostacoli o
  ignorati (da decidere lì). Occupano anche slot di oggetti e VRAM.
- Da provare quando capita un Surf: il ricambio veloce con OWE dell'altra
  zona (punto 2 della revisione qui sopra).
- Il commento di `include/config/wild_encounter.h:8` cita
  `HasepAreOWEsAllowedHere()`, che non esiste più (gli OWE sono attivi
  ovunque): da togliere alla prima occasione.

### 5.2 Fase 2, com'è stata fatta (16/11)

- **Cosa**: passo lento su B, udito per andatura, cono di vista con
  ostacoli ed erba alta, `IsOWEAwareOfPlayer()`. Fatti e trappole nella
  regola nuova `.claude/rules/overworld_catch.md` (la regola degli OWE era a
  7,3 KB); CHANGELOG (2.0.6.PLUS.12), README, `dexnav.md` aggiornati.
  - `PlayerNotOnBikeMoving`: B tenuto = `PlayerWalkSlow` + `creeping` con
    AUTORUN acceso; **durante una ricerca DexNav B tenuto è il passo lento
    sempre**, a terra e in Surf, anche con AUTORUN spento (correre lì fa
    comunque scappare il bersaglio); A non fa più il creeping. Fuori dalla
    ricerca, AUTORUN spento: B corre come prima (P20). Velocità del Surf
    invariate.
  - `CanAwareOWESeePlayer`: udito (corsa/bici = `activeDistance` 5,
    camminata `OWE_HEARING_WALK` 2, passo lento mai, attraverso gli ostacoli),
    poi cono (1/3/5 di larghezza a distanza 1/2-3/4+, limitato da
    `viewWidth`), linea di vista bloccata da collisioni di mappa e oggetti che
    non sono OWE né follower, erba alta = visto solo dalla casella davanti.
  - Numeri per comportamento: insegue, fugge, sparisce 5×5; osserva 4×5;
    si avvicina 4×3 (curioso, poco guardingo); vaga invariato (non nota mai).
    Erano tutti 4×3. Vincolo `viewDistance <= activeDistance` scritto nel
    file dei dati (motivo nella regola).
- **Fatto nuovo, contro P22**: in una partita nuova **AUTORUN è spento**
  (`src/new_game.c:123`, `autoRun = 1`; 0 = acceso). Verificato anche in
  emulatore: partita nuova, B tenuto correva (7 caselle in ~100 frame),
  senza B camminava (4). Quindi con i default il passo lento non c'è: vedi
  punti aperti.
- **Verificato** (ROM strumentata usa-e-getta, non nel git: tetto 1 OWE, il
  Pokémon resta fermo e non cambia direzione, comportamento forzato, un
  comando in RAM lo sposta a un offset dal giocatore con una direzione e
  registra il frame e le posizioni in cui nota; mGBA + GDB sulla Route 29,
  partita nuova; il ricambio bloccato col bit "non sparire"):
  - **Vista, giocatore fermo** (niente udito), comportamento "osserva" (4×5):
    159 posizioni nelle 4 direzioni (offset avanti 0-5, di lato ±3),
    **159/159 uguali** al modello calcolato sulla griglia delle collisioni
    letta dalla RAM. Notato a 4 di fronte, non a 5; mai alle spalle; il
    cono si allarga come previsto; casi coperti da alberi non notati. NPC in
    mezzo: (4,−1) con l'NPC sulla linea non vede (6 prove, NPC rimasto lì),
    il simmetrico (4,+1) sì.
  - **Vista a 5**, comportamento "fugge" (5×5, scritto da GDB nel
    `movementType`): 189 posizioni, **189/189** col modello, notato a 5 di
    fronte (anche a ±2 di lato) e mai a 6.
  - **Erba alta**: giocatore nell'erba, 161 posizioni: notato **solo** dalle
    3 caselle adiacenti di fronte (la quarta è del follower), 161/161 col
    modello.
  - **Udito**, Pokémon girato dall'altra parte, giocatore che si avvicina in
    linea retta: corsa (DASH acceso) notata **a 5** sia con AUTORUN spento
    (B) sia acceso (senza B); camminata (AUTORUN spento, senza B) **a 2**;
    passo lento (AUTORUN acceso, B) arrivato **adiacente senza essere
    notato**. Passo lento: 3 caselle in ~120 frame, contro 4 della camminata
    e 7 della corsa in ~100.
  - **ROM vera** (quella consegnata, identica byte per byte a quella
    provata): AUTORUN spento, senza B cammina (4 caselle in ~100 frame,
    niente DASH), con B **corre ancora** (7, DASH); AUTORUN acceso, B = passo
    lento (3, `creeping` 1), senza B corsa (DASH); con il flag della ricerca
    DexNav acceso a mano e AUTORUN spento, B = passo lento (2, `creeping` 1)
    e A una camminata (`creeping` 0). A davanti all'NPC della Route 29 apre
    il suo dialogo.
  - **Senza la modifica** (ROM di partenza, stesso salvataggio): AUTORUN
    acceso, B = camminata normale (4 caselle, `creeping` 0). La percezione
    di prima (rettangolo 4×3, niente erba né ostacoli) non è stata rifatta
    in emulatore.
  - **Non verificato**: una ricerca DexNav **vera** con `ENHANCED DEXNAV`
    spento (con l'opzione accesa il bersaglio non scappa mai): provato solo
    che B alza `creeping` col flag della ricerca, che è ciò che la fuga legge
    (`src/dexnav.c:1293`). Accendere il flag a mano fa partire dopo qualche
    passo una lotta (Hoothoot): il DexNav gira senza una ricerca vera, è un
    artefatto della prova. Surf non provato.
  - **AUTORUN acceso di default** (aggiunto dopo la scelta dell'utente):
    senza salvataggio, al menu principale `autoRun` = 0 e OPTIONS mostra
    AUTORUN ON; il salvataggio di prova della Route 29 (fatto con AUTORUN
    spento) caricato sulla stessa ROM legge ancora 1.
- ROM: 31 903 524 B, 95,08% (+896 B sulla fase 1).

- **Dopo `/code-review` (16/11)**: corretti quattro punti. (1) Il salto da
  una sporgenza esce prima dell'azzeramento di `creeping`: dopo un passo
  lento era silenzioso; ora lo spegne. (2) Gli oggetti `invisible` non
  coprono più la vista. (3) `IsPlayerInOWEViewCone` legge la tabella dei
  comportamenti una volta sola (gira ogni frame). (4) Condizione dell'erba
  semplificata (`forward > 1`, equivalente). Lasciati, col motivo: AUTORUN
  ereditato da una New Game sopra un salvataggio (le opzioni del titolo si
  ricopiano di proposito: scritto nel CHANGELOG e nella regola); il ripiego
  `SPECIES_NONE` dell'udito (caso che non capita, un OWE ha sempre una
  specie); `IsOWEAwareOfPlayer` senza chiamanti (è per la fase 3).
  Riprovato con la ROM strumentata ricompilata sul codice corretto: erba
  alta 161/161 (notato solo dalle 3 caselle adiacenti di fronte), terreno
  normale "osserva" 161/161 col modello. Il salto con `creeping` spento
  **non** è provato in emulatore (serve una sporgenza con un OWE vicino).

**Handoff per la fase 3 (Fable 5.2)**
- Serve il `.srm` (cancello della fase 3).
- `IsOWEAwareOfPlayer(owe)` è il "ha notato" del ×2 di D2. Un OWE "vaga"
  (`OWE_IGNORE_PLAYER`) non nota mai: con D2 avrà sempre il ×2.
- **AUTORUN acceso nelle partite nuove** (utente, 16/11, dopo la
  segnalazione qui sopra): `SetDefaultChallengeSettings` mette `autoRun = 0`.
  Gira solo a salvataggio vuoto o corrotto, quindi i salvataggi esistenti
  tengono il loro valore; le opzioni del titolo lo ricopiano già
  (`oak_speech_hns.c`). Verifica in §5.2.
- I muretti/staccionate della Route 29 risultano collisione nella griglia
  (`MapGridGetCollisionAt`), quindi coprono la vista anche se bassi.
- Ricetta di prova (ROM strumentata) riusabile per il bersaglio del lancio:
  §5.2 qui sopra e la regola.
- Il commento di `include/config/wild_encounter.h:8` su
  `HasepAreOWEsAllowedHere()` è ancora da togliere.

### 5.3 Fase 3, com'è stata fatta (17/11)

- **Cosa**: `src/overworld_catch.c` (nuovo), script in
  `data/scripts/overworld_catch.inc`. Fatti e trappole nella regola
  `.claude/rules/overworld_catch.md`, sezione "Lancio"; ricette di prova in
  `docs/claude/emulator.md`. CHANGELOG (2.0.6.PLUS.12), README, `dexnav.md`.
  - Tasti di D1: R lancia (ricordato 16 frame, così a metà passo non si
    perde), L premuto cambia modalità, L tenuto + ←/→ scorre e ferma il
    giocatore; scorciatoia DexNav su R tolta; in bici R resta Mach/Acro.
  - Un lancio è un record con un timer di volo (8 + 8 frame a casella) e poi
    60 frame di "assestamento" al posto delle scosse: la fase 4 ci appende
    gli sprite. Esito all'impatto, Pokémon consegnato subito, banner e
    soprannomi in coda dopo l'ultimo record.
  - Formula di cattura separata dai dati di lotta (`struct CaptureParams`),
    Pokémon costruito da `CreateMonFromOWE` (comune con la lotta), regole di
    sfida da `GetCaptureChallengeChecks`, strumento tenuto da
    `GiveWildMonHeldItem`: quattro refactor di codice esistente.
  - Dopo la cattura, come in lotta: Ball nel Pokémon, amicizia della Friend
    Ball, strumento tenuto, OT, squadra o PC, Pokédex (visto e catturato),
    `GAME_STAT_POKEMON_CAPTURES`, flag Nuzlocke della zona, soprannome. Non
    fatti di proposito: esperienza (P6), pagina del Pokédex (P18), scelta
    "squadra o Box" a squadra piena (P8), TV e catena DexNav.
- **Scelte prese qui** (cambiabili, la 6 rivede il bilanciamento):
  - **Quick Ball**: il suo ×5 vale solo su un Pokémon che non ti ha notato
    (qui non ci sono turni: sarebbe stato ×5 sempre). Timer Ball: mai bonus.
  - **Il tipo "vaga" non reagisce** a una Ball fallita e tiene il ×2.
  - **Pokémon lanciato**: la lotta parte da dove siete, senza
    l'avvicinamento dello script normale (su 5 caselle porterebbe a riva un
    Pokémon che nuota).
  - **Roamer e sciami** (P4): R avvia la lotta e la Ball **non parte** (il
    piano diceva "il lancio fallisce sempre": così non si spreca una Ball).
  - **Acqua**: non ferma il lancio (nessuna casella d'acqua ha il bit di
    collisione); il "dislivello" di P2 è la collisione dei bordi.
  - **`EASY CATCH`** vale anche qui, come in lotta.
- **Verificato** in mGBA + GDB col `.sav` dell'utente (Route 43, squadra da
  6 al 38, 37 Poké Ball; `EASY CATCH` e NICKNAMES OFF accesi, cambiati in
  RAM dove serviva). "ROM di prova" = usa-e-getta, identica salvo gli OWE a
  riposo fermi (non nel git). Bersagli messi sulla linea e specie cambiate
  scrivendo l'oggetto da GDB.
  - **Formula in lotta** (ROM strumentata: le funzioni di prima del
    refactor, da `2dcd0dc6`, girano accanto alle nuove a ogni lancio):
    **15 casi, 15 uguali**. Mewtwo Lv 23: Poké 1/1, Love 4/4 (ramo dei tipi
    di HnS), Quick 1/1 dopo il primo turno, Timer 1/1. Skarmory Lv 23: Quick
    32/32 al primo turno, Dusk 28/28 di notte, Nest 13/13, Level 32/32, Poké
    8/8, Ultra 16/16, Net 8/8, Fast 8/8; con lo stato scritto in RAM: 5 PS su
    70 23/23, 5 PS + paralisi 34/34, 20 PS + sonno con Dusk 175/175.
  - **Master Ball, guardando il Pokémon**: Pikachu ♂ in squadra (slot
    liberato in RAM): Lv 21, Master Ball, checksum valido, OT e ID del
    giocatore, "met at Lv 21, Route 43" nel riassunto, non Bad Egg; Pokédex
    0→1, catture +1, Master Ball −1; soprannome "AA" dalla schermata dei
    nomi, salvato. Venonat ♀ a squadra piena: al PC.
  - **Banner**: squadra e PC, ♂ blu, ♀ rosso, senza sesso (Magnemite, niente
    simbolo), nome da 12 lettere (Crabominable); si chiude con A solo a
    fanfara finita; chiusura pulita. Tema scuro dell'utente: cornice scura.
  - **Raffica**: 2 presi + 1 fallito (il terzo, allertato, mi ha caricato:
    lotta, fuga, poi i due banner in ordine); 3 presi con squadra da 5 (uno
    in squadra, due al PC, tre banner). **Bug trovato qui**: la coda contava
    anche i record già atterrati e rifiutava il terzo lancio.
  - **Nuzlocke, seconda Ball già in volo**: la Ball 1 (volo allungato in
    RAM) atterra dopo che la 2 ha catturato e segnato la zona: rimborso
    (breakpoint su `AddBagItem`: Master Ball, 1), Pokémon ancora sulla mappa.
  - **Rifiuti al lancio**, con messaggio e Ball non usata: zona Nuzlocke
    usata, Box pieni a squadra piena, flag Safari.
  - **Non consumata**: linea vuota, NPC, Pokémon decorativo; lotta partita
    col bersaglio mentre la Ball volava (36→37 dopo la fuga).
  - **Fallito → allertato**: inseguitore di spalle, giocatore fermo, non
    allertato per 3 s; dopo la Ball fallita allertato entro 16 frame, si
    gira e viene. **Bug trovato qui**: il bit di allerta restava acceso su
    un OWE già allertato o "vaga" (ora non si alza).
  - **Spawn nello stesso giro dell'impatto** (conto alla rovescia scritto a
    `timer − 1`, breakpoint d'ordine): 3 prove, ogni volta la specie giusta
    nel Box e il nuovo OWE nato.
  - **Tasti**: L premuto alterna la modalità; L + →/← scorre Ball (ordine
    della tasca, con giro) e Pokémon; L + ↓ per 0,5 s: giocatore fermo; con
    `L=A` L non alterna e R lancia; AUTORUN spento: R lancia; flag della
    bici: R alterna Mach/Acro e non lancia.
  - **Pokémon lanciato**: il secondo della squadra entra per primo da 5
    caselle; dopo la fuga la squadra è nell'ordine di prima.
  - **Acqua** (sintetico: metatile dello stagno scritto sotto il bersaglio,
    Lapras): sprite da nuoto visibile, cattura, oggetto e sprite spariti.
  - **ROM finale** sul salvataggio intatto: cattura con Poké Ball (EASY
    CATCH), banner, chiusura; con EASY CATCH spento un lancio fallito e
    l'allerta. Il resto è sulla ROM di prova.
- **Non verificato in emulatore**: salita vera in bici (flag scritti in
  RAM) e un OWE d'acqua vero preso in Surf; roamer e sciami (nessuno nel
  salvataggio); rifiuti One Type e Species Clause (stessa funzione della
  zona, altri rami) e Gara/Pike/Piramide/Trainer Hill; soprannome
  obbligatorio della Nuzlocke; lotta doppia selvatica con Pokémon lanciato;
  banner in grotta buia (la seconda icona come finestra OBJ è scritta ma non
  provata); amicizia della Friend Ball e strumento tenuto (i due Pokémon
  guardati non ne avevano); allineamento del simbolo di sesso guardato a 3×,
  non misurato in pixel.
- ROM: 31 908 340 B, 95,09% (+4 816 B sulla fase 2).

- **Dopo `/pr-review` (17/11, PR #60)**: 9 punti dalla revisione generica,
  tutti verificati sul codice. Corretti 6: (1) i banner partivano **prima**
  della gestione del passo e su un frame di passo saltavano contapassi e
  script della casella; (2) Start con una Ball in volo apriva il menu e un
  salvataggio la perdeva (ora Start è ignorato finché vola, < 1 s); (3) R
  premuto durante un messaggio tornava come lancio alla chiusura; (4) L
  tenuto attraverso un menu cambiava modalità al rilascio; (5) la palette
  dell'icona del banner era liberata anche se condivisa (icona della ricerca
  DexNav); (6) due commenti superati. Provati in emulatore (1-4 e il banner).
  Lasciato: il doppio ciclo sulla squadra (stile). **Deciso
  dall'utente (17/11)**: un lancio **fallito** non consuma l'incontro
  Nuzlocke della zona (in lotta qualunque esito lo consuma): col lancio conta
  solo la prima cattura riuscita. Resta così (P3).

**Handoff per la fase 4 (Fable 5.2)**
- **Non c'è ancora niente a schermo** per il lancio né per la scelta: solo
  suoni (`SE_BALL_THROW`, `SE_BALL` presa, `SE_BALL_OPEN` uscita,
  `SE_BALL_BOUNCE_1` rimbalzo, `SE_SWITCH`/`SE_SELECT` per L). Il pannello è
  della fase 5.
- Dove agganciare gli sprite: `struct PendingThrow` in `overworld_catch.c`.
  `OverworldCatch_TryThrow` crea il record, `ResolveImpact` decide (il
  numero di scosse è il ritorno di `RollShakes`, oggi buttato),
  `THROW_SETTLE_FRAMES` è la durata finta delle scosse, `ReturnBall` i casi
  a vuoto. Oggi alla cattura l'OWE è tolto **all'impatto**
  (`RemoveCaughtOWE`): con l'animazione va nascosto nella Ball e tolto alla
  fine, e un fallito va fatto riuscire prima dell'allerta.
- I timer di volo sono `u8` e fermi a controlli bloccati; una lotta che
  parte lascia i record com'erano (al ritorno: bersaglio sparito = Ball
  resa). La fase 4 deve chiudere gli sprite prima della transizione.
- Un lancio senza bersaglio oggi non fa nulla: il "vola 5 caselle e cade"
  di D16 è animazione, fase 4.
- La lotta da Pokémon lanciato parte senza avvicinamento: se la fase 4
  anima l'uscita del Pokémon, lo script è `OverworldCatch_EventScript_Battle`.
- Per le prove: il salvataggio dell'utente ha `EASY CATCH` acceso;
  `emulator.md`, sezione del 17/11.
- **Punti per l'utente** (non bloccano): le scelte elencate sopra (Quick
  Ball, "vaga", roamer, lotta a distanza); le Ball speciali ancora non in
  vendita a Johto/Kanto (handoff della fase 1).

### 5.4 Fase 4, com'è stata fatta (17/11)

- **Modello**: assegnata a Fable 5.2, fatta su Opus 5.5 (cambio deciso
  dall'utente a sessione iniziata).
- **Cosa**: tutto in `src/overworld_catch.c` più quattro agganci
  (`battle_setup.c`, `field_control_avatar.c`, `wild_encounter_ow.c`,
  `GetRunningDirectionAnimNum` esportata). Stati, trappole e misure nella
  regola `.claude/rules/overworld_catch.md`, "Lancio"; metodo di prova in
  `docs/claude/emulator.md`, sezione del 17/11. CHANGELOG (PLUS.12).
  - Il record della fase 3 ora ha stati di animazione (volo, apertura,
    chiusura, caduta, scosse, click o uscita; a vuoto: caduta e Ball a
    terra) e gli sprite sono appesi ai record. Esito e Pokémon consegnato
    restano all'impatto (D15); `THROW_SETTLE_FRAMES` non c'è più.
  - Sprite delle Ball dei follower (nessuna grafica nuova); arco con
    altezza 4 + 2 a casella sopra la retta mano-bersaglio; la Ball insegue
    il bersaglio che si muove.
  - Pokémon congelato e nascosto dentro la Ball dal raggio in poi; alla
    cattura l'oggetto è tolto a fine apertura (non a fine scosse: un
    oggetto nascosto per 3 s attraversava spawn, despawn e collisioni).
  - **D16 completato**: a vuoto la Ball vola 5 caselle e cade, contro un
    muro o un NPC lo colpisce e cade; torna in borsa. Verso il follower non
    parte. Start bloccato finché una Ball è fuori (prima solo in volo).
  - Posa: frame della corsa tenuto 16 frame con un'azione `DELAY_16`.
- **Verificato** in mGBA + GDB, ROM di prova (OWE fermi a comando, OWE messo
  sulla linea, esito forzato, sprite forzato a fallire; non nel git), col
  `.sav` dell'utente (Route 43, giorno, PLUS.11) e una partita nuova (Route
  29 di notte, Union Cave 1F). Foto frame per frame con un breakpoint su
  `OverworldCatch_Update`.
  - **Cattura**: verso sud (Girafarig), est (Sentret, inseguito mentre
    fuggiva), nord (Doduo), in grotta (Geodude ♀ al PC); banner dopo le
    scosse.
  - **Fuga dopo 1 scossa** (Flaaffy: esce, "!", carica, lotta Lv 23) e
    **dopo 3** (Furret, "vaga": esce e resta).
  - **A vuoto**: contro un albero a 2 caselle (cade a 1,5 e torna) e a
    piena gittata verso nord; `RemoveBagItem` e `AddBagItem` una volta
    ciascuno. Ovest: posa e Ball contro l'albero.
  - **Follower davanti**: nessun record creato.
  - **3 Ball di tipi diversi in aria** (Poké, Great, Ultra; Route 29, 4 OWE
    + follower): tile 606 → 622 su 1024, palette libere 8 → 6. Il tetto di 3
    resta.
  - **Sprite che non si crea** (forzato): niente a schermo, l'OWE sparisce
    all'impatto, il record si libera nello stesso frame, banner "sent to
    the PC". Nessuno script in attesa.
  - **Lotta durante le scosse**: Ball fallita a 2 caselle, poi Pokémon
    mandato (L) contro un secondo OWE a 4: la Ball a terra si chiude prima
    della transizione, il Pokémon dentro riappare; dopo la fuga record
    liberi e Pokémon visibile.
  - **Campo ricostruito**: Ball a vuoto lanciata da R + START, poi warp dal
    menu debug: record liberato, Ball resa.
  - Posa letta in RAM: `animNum` 0x17 (corsa est) dai frame 4 a 19, poi la
    faccia torna da sola.
- **Bug trovati qui e corretti**: (1) l'OWE spariva all'impatto invece che
  sul raggio (nascosto a ogni frame dal primo); (2) la posa non si vedeva
  mai: `PlayerFaceDirection` riscrive l'animazione a ogni frame, e
  `ObjectEventIsMovementOverridden` dà occupata l'azione finita del
  giocatore fermo (serve `ClearHeldMovementIfFinished` prima); (3) arco
  troppo alto (28 px a 3 caselle, ora ~20) e Ball che partiva dalla testa.
- **Non verificato in emulatore**: OWE in acqua (sprite da nuoto nascosto
  dal flag, scritto non provato), Ball in volo che attraversa una lotta e
  atterra dopo (ricreata all'impatto: scritto, non provato), notte in
  grotta buia con Flash. Il "?" del Pokémon liberato durante la transizione
  di una lotta è voluto (scongelato subito; tenerlo congelato rischiava di
  lasciarlo fermo dopo la lotta).
- ROM: 31 911 700 B, 95,10% (+3 360 B sulla fase 3), dopo le due revisioni.

- **Dopo `/pr-review` (17/11, PR #61)**: 7 punti dalla revisione generica,
  verificati sul codice. Corretti 6: (1) **una seconda Ball in volo verso un
  Pokémon già nella prima lo catturava di nuovo** (l'OWE ora resta fino alla
  chiusura, nella fase 3 spariva all'impatto): diventa un lancio a vuoto; (2)
  nei 12 frame fra impatto e raggio lo stesso Pokémon era ancora un bersaglio
  (anche per il Pokémon lanciato, che avrebbe aperto una lotta con un
  Pokémon già dato); (3) lo spawn poteva ricambiare il Pokémon nella Ball con
  uno della stessa specie, preso per lui: flag "non sparire" finché è dentro;
  (4) `CreateBattleStartTask_Debug` senza l'aggancio; (5) la guardia della
  coda contava solo le Ball in volo, ora vuole anche un record libero; (6)
  un lancio a vuoto senza sprite bloccava Start ~1,3 s per niente. Scartato:
  `GiveCaughtMon` che fallisce dopo un tiro riuscito (non raggiungibile:
  `GetCatchBlock` all'impatto controlla i box pieni prima). Provati (1) e
  (3): due Ball allo stesso Flaaffy, `CreateMonFromOWE` 1 volta,
  `AddBagItem` 1 volta (la seconda resa), nessuna cattura in coda; byte del
  livello del Farfetch'd `0x96` nella Ball e `0x16` dopo l'uscita.

- **Seconda `/pr-review` (17/11, sul commit delle correzioni)**: 9 punti.
  Corretti: (1) dopo una cattura, una Ball ancora in volo verso lo stesso
  slot poteva agganciare un Pokémon della stessa specie appena nato lì (con
  un'Esca il conto alla rovescia dello spawn è 0): diventa un lancio a vuoto
  alla rimozione; (2) la Ball attraversava un Pokémon già colpito ma ancora
  visibile (12 frame prima del raggio): ora lo colpisce come un muro e cade;
  (3) tolta la guardia "record libero" aggiunta prima, che nascondeva il
  messaggio di blocco (il ciclo sui record basta e non perde Ball);
  prototipo doppio; CHANGELOG e `overworld_encounters.md` (il flag "non
  sparire" vale anche per la Ball). Scartati: flag rimasto acceso se
  `GetThrowTarget` smette di riconoscere un oggetto vivo (non succede: NULL
  vuol dire sparito o sostituito); suono e posa di un lancio a vuoto senza
  sprite (stato degenere, il suono conferma il tasto). Provato (2): seconda
  Ball nella finestra, `CreateMonFromOWE` 1, `AddBagItem` 1, 1 cattura.

**Handoff per la fase 5 (Sonnet 5.5)**
- Il pannello di D18 può restare a schermo mentre una Ball è fuori (il
  giocatore è libero); le Ball hanno la priorità OAM del giocatore e
  sottopriorità 1, da tenere presente se il pannello usa sprite.
- `OverworldCatch_IsBallOut()` è il "c'è un lancio in corso" giusto per il
  pannello; `sCatch.monMode` per la modalità.
- R + START del debug ora lancia anche una Ball a vuoto (la combinazione
  preme R prima di Start): innocuo, la Ball torna; se dà fastidio si
  esclude R tenuto con Start.
- Suoni già presenti: `SE_BALL_THROW` lancio, `SE_BALL_TRADE` apertura,
  `SE_BALL_BOUNCE_1` caduta, `SE_BALL` scossa, `SE_RG_BALL_CLICK` presa,
  `SE_BALL_OPEN` uscita. La fase 5 li rivede solo se l'utente li vuole
  diversi.
- Punti per la fase 6 (bilanciamento): la Ball insegue un bersaglio che
  fugge (come nella fase 3, dove bastava che esistesse ancora): più
  generoso di Leggende Arceus.

### 5.5 Fase 5, com'è stata fatta (17/11)

- **Modello**: Sonnet 5.5, come assegnato.
- **Cosa**: `UpdatePanel`/`DrawPanel`/`HidePanel` in `src/overworld_catch.c`
  (regola `.claude/rules/overworld_catch.md`, "Pannello"); CHANGELOG
  (PLUS.12), README (riga nuova in "Written from scratch"), report SG
  (feature di questo fork, non un port). Indice in CLAUDE.md §7: già c'era.
  - **Scelta di progetto**: i mock del 16/11 **non sono nel repo**, quindi il
    pannello è stato disegnato qui: finestra di testo 12x3 a (2,16), riga 1 il
    nome (Ball o soprannome), riga 2 "Ball ×N" o "Lv. N"; "No Balls" e
    "Nobody can fight" quando non c'è niente da scegliere. **Solo testo, senza
    icona**: niente sprite, quindi niente palette OBJ né priorità da
    spartire con le Ball in volo. Se l'utente vuole l'icona della Ball o del
    Pokémon (come nel banner) è un'aggiunta separata.
  - Su con L tenuto o per 120 frame dopo cambio, scorrimento o lancio (anche a
    vuoto); giù con controlli bloccati (script, messaggio, menu Start) e con
    `L=A`. Il rilascio di L senza scorrere cambia modalità (D1), quindi in
    prova serve scorrere prima di rilasciare.
- **Verificato** in mGBA + GDB con una ROM di prova usa-e-getta (un `Cheat()`
  in `OverworldCatch_Update` pilotato da una variabile scritta da GDB: dà
  flag, 6 Pokémon e 3 tipi di Ball, poi toglie Ball o Pokémon; non nel git),
  partita nuova salvata in gioco, Route 29:
  - Ball con 3 tipi (Poké ×12, Great ×5), con 1 tipo (lo scorrimento non
    cambia), con 0 (`Ball` / `No Balls`); Pokémon con 6 (Rattata Lv 25,
    Crabominable Lv 55 da 13 lettere, Pikachu Lv 30), con 1, con tutti a 0 PS
    (`Pokémon` / `Nobody can fight`).
  - **Tema scuro**: cornice e fondo scuri, testo chiaro (guardato a 4×). Vale
    **dopo** il ricaricamento delle pal 14/15: acceso a mappa aperta, il
    pannello restava chiaro finché non è passato un messaggio/menu.
  - Sparisce da solo (visibile a 1 s, assente a 3,5 s), si nasconde aprendo
    il menu Start, ricompare dopo un lancio a vuoto.
  - **Bug trovato qui**: la prima versione ridisegnava con `PIXEL_FILL(0)`
    dopo `SetStandardWindowBorderStyle` (che riempie con 1): il fondo era
    trasparente e il testo aveva uno sfondo bianco a strisce. Ora `PIXEL_FILL(1)`.
- **Non verificato**: il pannello con una Ball in volo (stessa finestra BG0, nessuno
  sprite: non dovrebbe interagire); `docs_validate` in CI (in locale lo script
  non stampa errori); testi riletti contro il codice e non con un giro in grotta
  buia. Suoni non toccati (l'handoff diceva di rivederli solo su richiesta).
- ROM: 31 913 684 B, 95,11% (+1 984 B sulla fase 4).
- **Preesistente, non toccato**: la riga "Overworld speed-up" del README
  chiude con "R throws a Ball at wild Pokémon on the map", fuori posto (non è
  una nota dell'`OW SPEED`).

**Handoff per la fase 6 (Opus 5.5)**
- **Serve il `.srm` dell'utente** (cancello della fase).
- Il pannello non è stato provato con un salvataggio vero, ma non scrive nel
  salvataggio e non ha stato fuori da `sCatch`.
- Per le prove al pannello e al lancio senza passare dai menu: una ROM di prova
  con `Cheat()` come sopra e un'ora di gioco; la partita nuova si salva in gioco
  (Start → Salva) per non rifare l'intro a ogni build.
- Punti già in lista per la 6: bilanciamento (Ball per una cattura a inizio
  e a metà gioco), la Ball che insegue un bersaglio che fugge, la scelta DexNav
  (D6), e il testo "Lv. N" del pannello (FONT_SMALL non ha il glifo `{LV}`: non
  provato, si è usato "Lv. ").

### 5.6 Fase 6, com'è stata fatta (18/11)

- **Modello**: Opus 5.5, come assegnato. Salvataggio dell'utente (PLUS.11,
  Route 43, 6 medaglie, `EASY CATCH` acceso).
- **`/code-review` medio** sul diff `d6520855..HEAD` (fasi 1-5, senza il
  commit HASEPSYM): 10 punti, verificati sul codice e dove serviva in
  emulatore. Corretti 7:
  1. **Pannello sopra la finestra DexNav** (stessa base 8 e stessa riga):
     riprodotto sulla build di prima (L tenuto durante una ricerca:
     "FURRET" sparisce, "Lv. 21" rovinato, poi un buco nella finestra);
     ora il pannello è spento con `DN_FLAG_SEARCHING`. Provato dopo: finestra
     intatta.
  2. **Ordine nel frame**: il pannello si nascondeva a fine CB2, dopo script
     e task. Il caso segnalato (banner coperto) **non si riproduce** (`lock`
     aspetta un frame prima del banner: provato sulla build di prima, banner
     pulito); resta vero per ciò che uno script disegna nel frame del blocco
     (un cartello: per lettura, non visto). Ora `OverworldCatch_UpdatePanel`
     gira prima di `ScriptContext_RunScript`.
  3. **Il lancio ignorava i dislivelli** (contro P2): ora un dislivello ferma
     la Ball e un Pokémon a un'altra altezza è saltato. Provato con un trucco
     (altezza di OWE e giocatore scritte): Ball sopra il Pokémon, resa; Ball
     caduta subito (~0,25 s contro >1,2) con il giocatore "più in alto".
  4. Il bit di allerta forzata (Ball fallita) saltava anche `CanOWEReachPlayer`.
  5. Icona del banner: se la prima non si creava, la seconda (grotta buia)
     restava a schermo; palette ora liberata con `FieldEffectFreePaletteIfUnused`
     (fa anche `ResetPaletteColorMapType`).
  6. `gOverworldThrowLeadSlot` azzerato a ogni frame a controlli liberi
     (difensivo: lo script della lotta tiene i controlli fino alla lotta, il
     caso non è raggiungibile oggi).
  7. Commento della Friend Ball che parlava della Heal Ball.
  Scartati: `TryRemoveOWEAboveCap` "senza animazione" (falso:
  `RemoveObjectEvent` → `OnOverworldWildEncounterDespawn` la fa, e un
  Pokémon nella Ball ha il flag "non sparire"); Start ignorato in silenzio
  con una Ball fuori (voluto, al massimo ~4 s; il salvataggio del menu
  debug resta, è solo debug).
- **Decisioni dell'utente (18/11)**: D6, il DexNav resta. **La Ball non
  insegue più** chi si sposta: va dove il Pokémon era al lancio e lo colpisce
  se all'arrivo è a meno di mezza casella (`IsOWEAtLanding`). Provato: OWE a
  3 caselle che parte di lato al lancio → Ball resa, nessuna cattura; OWE
  fermo a 2 → catturato.
- **Bilanciamento** (formula vera, PS pieni, probabilità per Ball e lanci
  medi; ×2 = non notato). A inizio gioco (Lv 3-7, 0-1 medaglie) quasi sempre
  al primo colpo grazie al bonus dei livelli bassi. A metà (6 medaglie, Lv
  21-22): Furret 12% / 28% (8,1 / 3,6 lanci), Flaaffy 17% / 34%, Girafarig
  8% / 17%, Pinsir 6% / 12% con la Poké Ball; Ultra Ball: Furret 28% / 50%,
  Pinsir 12% / 28%. Le Ball costano 10/15/30: l'utente ha scelto di non
  ritoccare i numeri.
- **Punti aperti delle fasi 4-5**: "Lv. N" del pannello letto bene in
  FONT_SMALL (squadra vera: Vileplume, Quilava, Scizor Lv 38); pannello
  visibile con una Ball a terra; `docs_validate` verde in locale. **Non
  provati**: OWE in acqua, Ball in volo che attraversa una lotta, grotta buia
  con Flash (codice non toccato in questa fase).
- **Verificato** in mGBA + GDB col `.sav` dell'utente, due ROM di prova (build
  di prima e corretta, con un `Cheat()` che mette un OWE fermo davanti, ne
  cambia l'altezza o lo fa spostare al lancio; non nel git).
- ROM consegnata: 31 914 068 B, **95,11%** (+384 B sulla fase 5, dopo la `/pr-review`).

- **`/pr-review` (18/11, PR #65, sul commit `c5b818d2`)**: 9 punti dalla
  revisione generica. Corretti 6: (1) **i dislivelli fermavano ogni lancio
  fra riva e acqua** (acqua 1, riva 3 nelle mappe di HnS, p. es. Lake of Rage
  e Route 43): introdotto in questa fase, ora acqua e terreno sono un livello
  solo (`GetThrowLevel`), anche per l'oggetto saltato; (2) colpito/mancato
  per **casella** (`currentCoords` alla caduta = casella del lancio) invece
  che per pixel: un Pokémon a metà passo al lancio era mancato finendo il
  passo, uno che salta sul posto (gli OWE che si avvicinano) era mancato; (3)
  `GetObjectGroundPos` senza `y2`; (4) il bit dell'allerta forzata si consuma
  al primo controllo (prima aspettava il giocatore e scattava da lontano); (5)
  CHANGELOG: le sporgenze non fermano la Ball. Opzionale, non fatto: palette
  dell'icona del banner senza nessuno sprite creato (tabella degli sprite
  piena: non raggiunto). Scartati con il percorso rifatto: lo slot del Pokémon
  lanciato azzerato troppo presto (lo script tiene i controlli fino alla
  lotta) e la finestra DexNav aperta nello stesso frame del pannello (ogni
  apertura passa da un blocco dei controlli nel CB1).
  Provato in mGBA col `.sav` dell'utente su Route 43, riva in (10,12): Ball
  dalla riva a un Pokémon in acqua → presa (Flaaffy); un OWE che parte di
  lato al lancio → mancato, Ball resa; fermo → preso; in Surf, Pokémon
  mandato (Quilava, scelto con L) contro uno in acqua → lotta con Quilava in
  campo; lotta da contatto subito dopo → Vileplume in campo (primo della
  squadra). Il caso che falliva prima della correzione non è stato rifatto in
  emulatore: lo mostrano i dati della mappa (riva 3, acqua 1) contro il
  vecchio `IsElevationMismatchAt(3, casella 1)` = vero.

- **`/pr-review` della PR #66 (20/11, su `803c87b1`)**: 7 punti. Corretti 2:
  il bit dell'allerta forzata usava `CanOWEReachPlayer` (acqua ≠ riva), quindi
  un Pokémon in acqua colpito dalla riva non notava mai il giocatore; ora usa
  il livello del lancio (`OverworldCatch_AreLevelsApart`) e si consuma solo su
  un altro livello. Regola: rimessi follower, linea vuota e acqua senza
  collisione. Scartati: bersaglio oltre un confine di mappa (l'OWE della mappa
  vecchia viene tolto comunque), ponte ad altezza 3 sull'acqua (sotto un ponte
  si passa solo con caselle 15), controllo del bit fuori da `Step3` (solo con
  `WE_OWE_UNRESTRICT_SIGHT`, FALSE). Lasciati come scelta: il colpo per casella
  nel fotogramma in cui il Pokémon parte, il `y2` del Surf per la mano.
  **Scoperto in emulatore**: un OWE dell'altra zona (in acqua col giocatore a
  terra) viene ricambiato in ~30 frame dopo l'uscita dalla Ball
  (`IsAnyOWEFromOtherArea`, fase 1): i lanci fra riva e acqua toccano solo
  Pokémon rimasti dalla zona di prima. La correzione dell'allerta non si è
  potuta vedere per questo (il Pokémon sparisce prima di guardare); letta sul
  codice. Il caso "presa dalla riva" del 18/11 usava un OWE di terra messo in
  acqua dal trucco.

**Handoff per la fase 7 (Sonnet 5.5)**
- La fase 7 è un port da Soulgold (ricorda mosse a pagamento): non tocca
  `overworld_catch.c`. Leggere `relearner.md` e il §3 per i maestri.
- Il salvataggio dell'utente (PLUS.11) è quello della fase 6: Route 43, 6
  medaglie; serve alla 7 per le mosse uovo vere.
- Ancora non provati in emulatore (da fare se la 9 o la 15 passano di lì):
  Ball in volo durante l'inizio di una lotta,
  grotta buia con Flash, un cartello letto entro 2 s da un lancio.

## 6. Scartato, e perché

- **Crafting delle Ball**: escluso dall'utente, per ora.
- **Gattonare**: non esistono frame del giocatore accovacciato; il passo lento
  c'è già.
- **Strumenti da lancio di Leggende Arceus** (bacche, fango, fumogeni): fuori
  da questo piano.
- **Statistiche, IV e colori come premio dell'alfa** (utente, 14/11): gli IV
  si possono già portare al massimo e gli EV togliere, e un colore diverso si
  confonderebbe con lo shiny.
- **Alfa grande in lotta e nei menu**: gli sprite di lotta sono 64×64 fissi;
  solo la mappa può scalare (fase 12).
- **Mira libera o a parabola regolabile**: il motore si muove a caselle; una
  linea retta nella direzione dello sguardo è leggibile e costa poco.
- **Lancio nel Safari e alla Gara** (P5): hanno Ball contate e regole proprie
  (`gNumSafariBalls`, punteggio della Gara). Si possono aggiungere dopo.
- **Un'opzione per spegnere la feature**: non c'è posto nel salvataggio senza
  allargare una struct, e chi non la vuole non preme R.
- **Tetto `OWE_SPAWNS_MAX` a 4**: sposterebbe i `localId` degli OWE; bastano
  i tetti per zona.
