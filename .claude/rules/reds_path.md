---
paths:
  - "data/scripts/reds_path.inc"
  - "src/battle_partner.c"
  - "include/battle_partner.h"
  - "include/constants/battle_partner.h"
  - "src/data/battle_partners.party"
  - "src/reds_path.c"
  - "include/constants/reds_path.h"
  - "data/maps/*_Gym_hns/**"
---

# Red's Path (piano in `docs/plans/reds-path.md`)

Il giocatore rigioca le otto palestre di Kanto con la squadra di Red di quel
momento, contro i capi di FRLG. Fatte le sessioni A1 (dati, salvataggio,
debug), A2 (agganci di lotta) e B (aiutanti, scena, premi: sezione "Mondo"
in fondo); resta la C (taratura). Le lotte si fanno dagli aiutanti delle
palestre, o dal menu di debug (`Red's Path… → N`, senza scena).

## Pezzi e dove stanno

| Cosa | Dove |
| --- | --- |
| otto capi, id 653-660 | `TRAINER_REDS_PATH_*_HNS` (`include/constants/opponents_hns.h`), squadre in `src/data/trainers_hns.party` |
| otto squadre di Red | `PARTNER_RED_*` in `src/data/battle_partners.party`, caricate negli slot 0-5 da `FillRedsPathParty` (`src/battle_partner.c`) |
| AI dei capi | `AI: Smart Trainer` (`AI_FLAG_SMART_TRAINER`, senza Prediction) nei `.party` degli otto capi, verificato in `src/data/trainers_hns.h`. Prima era `Basic Trainer` scritto per esteso. Nessuna opzione del challenge menu cambia l'AI |
| la lotta | `RedsPath_EventScript_Battle` (`data/scripts/reds_path.inc`), `VAR_0x8004` = sfida 0-7 |
| ripristino della squadra | `special RedsPath_RestorePlayerParty`: `LoadPlayerParty` più `UpdateFollowingPokemon` (sotto) |
| "lotta in corso" | `FLAG_REDS_PATH_BATTLE` (ex `FLAG_UNUSED_EXTENDED_86`), letto da `IsRedsPathBattle()` |
| "è un capo di Red's Path" | `IsRedsPathTrainer(id)`, per id: level scaling e TcloneR |
| "sfida vinta" | il flag "allenatore battuto" del capo, acceso solo alla vittoria |
| "premio ritirato", "introduzione vista" | `FLAG_REDS_PATH_PRIZE_*`, `FLAG_REDS_PATH_INTRO_SEEN` (ex `FLAG_UNUSED_EXTENDED_87..95`, migrazione 11) |
| aiutante, scena, premi | `src/reds_path.c` (tabella `sGyms`), `data/scripts/reds_path.inc`, costanti in `include/constants/reds_path.h` |

## Agganci di lotta (A2), tutti dietro `IsRedsPathBattle()`

| Cosa | Dove |
| --- | --- |
| dorso di Red (quello di FRLG, l'unico) | `PlayerGetTrainerBackPicId` (`battle_controller_player.c`): un punto solo per disegno, slide e palette |
| mugshot di Red FRLG, banda maschile | `Mugshot_SetGfx` e la creazione dello sprite del giocatore (`battle_transition.c`) |
| niente fuga | `CanPlayerForfeitNormalTrainerBattle` (`battle_main.c`): ci passano tutti e tre i chiamanti |
| niente svenimento né soldi persi | **`B_FLAG_NO_WHITEOUT` è `FLAG_REDS_PATH_BATTLE`** (`include/config/battle.h`, solo HnS) |
| niente esperienza | `Cmd_getexp` (`battle_script_commands.c`); senza esperienza niente Candy Jar |
| niente evoluzioni | `TryEvolvePokemon` (`battle_main.c`) salta il giro |
| niente premio in soldi | `jumpifredspathbattle` (`BS_JumpIfRedsPathBattle`) in testa a `BattleScript_LocalBattleWonReward` salta `getmoneyreward` e il messaggio |
| sempre RED nei testi | `BattleStringGetPlayerName` (`{B_PLAYER_NAME}`) e `GetRedsPathBattleString` (`battle_message.c`) |

**Perché `B_FLAG_NO_WHITEOUT` non ha un flag suo** (il piano ne prevedeva
uno): sarebbe un altro bit di cui non si può dimostrare che sia a zero nei
salvataggi vecchi, con una migrazione 11 (già promessa al quest book), e
acceso per sbaglio toglierebbe lo svenimento in **tutte** le lotte con
allenatori. `FLAG_REDS_PATH_BATTLE` è già spento dalla migrazione 10, lo
accende solo lo script intorno a queste lotte e lo spegne anche
`Overworld_ResetBattleFlagsAndVars`. Il prezzo: un futuro `setflag
B_FLAG_NO_WHITEOUT` spegnerebbe anche randomizer e sfide. Se servirà,
separarli allora.

**Sconfitta**: `CB2_EndTrainerBattle` torna allo script
(`CB2_ReturnToFieldContinueScriptPlayMapMusic`) senza accendere il flag del
capo; lo script di lotta salta `getmoneyreward` (`jumpifnowhiteout`) e la
squadra non viene curata, ma tanto la sostituisce il ripristino. Lo script dopo
`trainerbattle_no_intro` riprende sia dopo una vittoria sia dopo una
sconfitta: la B li distingue con il flag del capo.
**Vittoria**: niente premio in soldi (decisione dell'utente): si salta
`getmoneyreward` e il suo messaggio, si passa a Pay Day e Raccogli, che
la squadra di Red non ha.

**Evoluzioni**: senza esperienza non si sale di livello, ma
`EVO_MODE_BATTLE_SPECIAL` fa evolvere anche senza (colpi critici, danni
subiti), e la scena di evoluzione segna la specie nuova come catturata nel
Pokédex. Per questo il giro si salta del tutto.

## Trappola: il follower

Tornando in mappa `ReturnToFieldLocal` chiama `UpdateFollowingPokemon`
quando in squadra c'è ancora quella di Red: dopo una sconfitta sono tutti KO e
il follower viene **tolto**; dopo una vittoria diventerebbe un Pokémon di Red.
`LoadPlayerParty` da solo non lo aggiorna, e camminare nemmeno: fino al
prossimo cambio di mappa il giocatore restava senza follower (visto in
emulatore, c'era già nella A1). Per questo lo script chiama
`RedsPath_RestorePlayerParty`, che dopo il ripristino chiama
`UpdateFollowingPokemon`. Il follower ricompare al primo passo (nasce
invisibile).

## Cose da sapere

- **RED nei testi della lotta** (decisione dell'utente): su questa base i
  testi di lotta si rivolgono al giocatore con "You" e il nome compare quasi
  mai. Le righe che una lotta di Red's Path può mostrare sono tre ("You are
  challenged by…", "You used <oggetto>!", "You defeated…"): `GetRedsPathBattleString`
  le scambia, per puntatore, con versioni "RED …" appena prima
  dell'espansione. In più `{B_PLAYER_NAME}` vale RED. Le domande al giocatore
  ("Would you like to…") restano in seconda persona: nel visore è lui che
  gioca. Un testo nuovo con "You" che compaia in queste lotte va aggiunto lì.
- La cornice è un **visore VR** (decisione dell'utente, piano §4): niente
  viaggio nel tempo nei testi.
- OT dei Pokémon di Red: `RED`, ID derivato dal nome (`GetPartnerOtId`).
  Vale l'obbedienza: con meno di 8 medaglie di Johto i livelli sopra il tetto
  disobbediscono. In gioco non succede (Kanto viene dopo), da debug sì.
- Randomizer, sfide del challenge menu e Nuzlocke esclusi (A1): l'elenco è nel
  piano, in "Cosa ha lasciato la A1 alla A2".

## Verificato in emulatore (A2, 25/10)

Salvataggio dell'utente (2.0.6.PLUS.4, 6 medaglie, squadra di sei Lv38),
debug `Red's Path… → N`, RAM letta via GDB (squadra in `gPlayerParty`, soldi in
XOR con `encryptionKey`, flag):

- **Otto lotte**: mugshot di Red FRLG con la banda e lo sprite FRLG del capo,
  dorso di Red, squadra di Red come nel piano (specie e livelli nel menu
  squadra), primo Pokémon del capo giusto. Vittoria istantanea dal menu
  debug di lotta: flag del capo acceso, squadra del giocatore identica byte
  per byte (md5 dei 600 byte di `gPlayerParty`), `FLAG_REDS_PATH_BATTLE`
  spento.
- **Sconfitta vera** contro Brock (solo Growl e Harden): nessun Centro, si
  resta sul posto, soldi invariati (23638), flag di Brock spento, squadra
  identica byte per byte.
- **Fuga**: "No! There's no running from a Trainer battle!". Controllo: in
  una lotta normale (Brent, Route 43) RUN offre la resa.
- **Esperienza**: Geodude KO con Metal Claw → "fainted" e subito "sent out
  ONIX", nessuna riga di esperienza. Controllo: con Brent "gained 0 Exp.
  Points" (tetto) e la riga dell'Exp. Share.
- **Soft reset** a metà lotta (Misty, flag acceso, squadra di Red in RAM): al
  caricamento la squadra vera e il flag spento.
- **Pokédex**: dopo le otto lotte, catturati identici al salvataggio
  originale; Charmander, Charizard, Lapras e Snorlax usati in lotta e
  rimasti non visti; unico cambio, Victreebel visto (il primo Pokémon di
  Erika).
- **Follower**: Vileplume tornato dopo la vittoria (prima della correzione
  spariva, anche camminando).

Dopo la richiesta dell'utente (soldi e RED), stessa prova su Brock: "RED is
challenged by LEADER BROCK!", "RED used POTION!", "RED defeated LEADER
BROCK!", poi la frase di Brock e subito la mappa, senza la riga dei soldi;
soldi invariati (23638) e flag di Brock acceso. Prima della modifica: "You
defeated…" e "You got ₽1400 for winning!".

Non provato: la sconfitta per tutti e otto (stesso codice), evoluzioni
speciali in lotta (nessun Pokémon di Red ne ha), migrazione di un salvataggio
a versione 9 (fatta nella A1, codice non toccato).

## Mondo (sessione B, 25/10)

Ogni palestra chiama lo script comune con `VAR_0x8004` = sfida: l'aiutante
(dopo il capo di HnS) va a `RedsPath_EventScript_Aide`, le righe davanti
all'uscita a `RedsPath_EventScript_IntroTrigger`. Tutto quello che dipende
dalla palestra (capo, aiutante, posto della scena, sprite di Gen 1, asso,
flag) sta in `sGyms` in `src/reds_path.c`. Gli stati dell'aiutante
(`RedsPath_GetAideState`): offerta, premio in attesa, fuori ordine, già fatta
qui, premi ancora altrove, tutto finito; "Beat the LEADER there first!" quando
il capo di HnS della prossima sfida non è battuto.

**La scena**: `RedsPath_EnterScene` a schermo nero sposta giocatore e camera
davanti al capo (`MoveCameraAndRedrawMap`, prima marcata `unused`), rende
invisibile il giocatore e crea **Red come oggetto a parte**
(`LOCALID_REDS_PATH_RED` 241); Janine e Blue prendono lo sprite di Koga e
Giovanni. `RedsPath_ExitScene` rimette tutto e riporta il giocatore dov'era.

Trappole trovate in emulatore:

- **Non cambiare la grafica del giocatore.** Era la prima versione: al
  ritorno dalla lotta `SetPlayerAvatarObjectEventIdAndObjectId` ricava stato
  e sesso dell'avatar dalla grafica (Red → maschio, a piedi) e rimette lo
  sprite del giocatore: dopo la lotta si vedeva di nuovo il giocatore, e una
  giocatrice sarebbe diventata maschio fino al cambio mappa. Gli oggetti
  normali invece tengono la grafica cambiata (Koga resta Koga dopo la lotta).
- **Il capo va cercato dopo lo spostamento**: dalla porta Janine e Blue sono
  fuori schermo, e un oggetto fuori vista non esiste. Chi entra in vista con
  lo spostamento non è congelato dal `lockall` (Koga si guardava intorno):
  `FreezeObjectEvents()` alla fine di `RedsPath_EnterScene`.
- **Gli script delle righe d'innesco**: expansion li prova "a secco"
  (`RunScriptImmediatelyUntilEffect`) e li avvia solo se fanno qualcosa.
  Uno special non dichiarato fa partire lo script vero; se poi finisce con
  `end` senza `releaseall`, il passo del giocatore resta "tenuto" e il
  giocatore non si muove più né apre il menu (visto due volte). Ora
  `RedsPath_IsGymLeaderBeaten` è `requests_effects=TRUE` (non ha effetti), le
  uscite anticipate non avviano nulla e quella che spegne l'innesco passa da
  `releaseall`.
- `msgbox` scrive `VAR_RESULT` (`waitbuttonpress`): l'esito del premio va
  salvato prima del messaggio, o `Common_EventScript_GiftMon` non mostra
  "received".
- `CreateMon` non dà mosse: `GiveMonInitialMoveset` (il primo Onix ne era
  senza).
- I cammini dell'aiutante sono costruiti in RAM (`gRedsPathAideApproach`):
  `applymovement` fa rientrare il follower, che è voluto, perché sta dietro
  il giocatore spesso proprio dove l'aiutante si ferma (a Viridian ci finiva
  sopra con `applymovement2`).
- **Seafoam**: si esce da una scala, e la casella sopra è un muro; l'innesco
  è solo a (19,22) e (20,22), dal lato dell'aiutante. Chi esce dall'altro
  lato vede l'introduzione parlandogli (o in un'altra palestra: Blaine è
  quasi sempre il penultimo capo).
- OT dei premi: 7 caratteri, quindi `SURGE` e `GIOVANN`.

### Verificato in emulatore (B, 25/10)

Salvataggio dell'utente (versione 8 → 11), ROM strumentata con script di
debug per gli scenari (debug.inc ripristinato prima del commit), RAM via GDB:

- introduzione: uscendo da Pewter, Fuchsia, Viridian e Seafoam l'aiutante ha
  il «!», si avvicina, parla e torna; entrando (verso nord) non scatta; con
  il capo di HnS non battuto non parte nessuno script e si cammina sulla riga
  senza blocchi; una volta sola (poi `VAR_TEMP_9` = 1);
- le otto scene, lotte vinte in ordine (vittoria istantanea dal debug di
  lotta): Red davanti al capo giusto, Koga e Giovanni compresi, prima e dopo
  la lotta; "RED WINS!", ritorno davanti all'aiutante, congratulazioni;
- sconfitta (Blaine, PS azzerati via GDB): niente "RED WINS!", "Try again!",
  squadra del giocatore di nuovo a sei, sfida offerta di nuovo;
- squadra piena → "Come back with room in your party!"; con un posto
  libero: "Here's your prize!", "received RHYDON!"; Rhydon Lv70 con OT
  `GIOVANN`, ID del capo, Poké Ball, mosse di livello, PS 248 (= IV 31);
  Onix, ritirato prima della correzione delle mosse: OT `BROCK`, PS 150;
- tre reset davanti all'aiutante: tre personalità diverse con lo stesso OT,
  cioè lo shiny si ritira a ogni reset;
- fuori ordine con e senza capo di HnS battuto, "già fatta qui", premi
  ancora altrove, complimenti finali;
- migrazione 11: `.sav` portato a versione 10 con i nove flag accesi →
  versione 11 e flag spenti, bit vicini intatti.

Non provato: giocatrice (Red è un oggetto a parte apposta), richiesta del
soprannome (l'opzione era spenta), reset a metà scena (non si può salvare
dentro uno script), innesco a Cerulean, Vermilion, Celadon e Saffron (stesso
codice, posizioni controllate sulla mappa delle collisioni).
