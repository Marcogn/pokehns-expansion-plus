---
paths:
  - "data/scripts/reds_path.inc"
  - "src/battle_partner.c"
  - "include/battle_partner.h"
  - "include/constants/battle_partner.h"
  - "src/data/battle_partners.party"
---

# Red's Path (piano in `docs/plans/reds-path.md`)

Il giocatore rigioca le otto palestre di Kanto con la squadra di Red di quel
momento, contro i capi di FRLG. Fatte le sessioni A1 (dati, salvataggio,
debug) e A2 (agganci di lotta); restano la B (script di mappa, scena, premi) e
la C (taratura). Fino alla B alle lotte si arriva solo dal menu di debug
(`Red's Path… → N`).

## Pezzi e dove stanno

| Cosa | Dove |
| --- | --- |
| otto capi, id 653-660 | `TRAINER_REDS_PATH_*_HNS` (`include/constants/opponents_hns.h`), squadre in `src/data/trainers_hns.party` |
| otto squadre di Red | `PARTNER_RED_*` in `src/data/battle_partners.party`, caricate negli slot 0-5 da `FillRedsPathParty` (`src/battle_partner.c`) |
| la lotta | `RedsPath_EventScript_Battle` (`data/scripts/reds_path.inc`), `VAR_0x8004` = sfida 0-7 |
| ripristino della squadra | `special RedsPath_RestorePlayerParty`: `LoadPlayerParty` più `UpdateFollowingPokemon` (sotto) |
| "lotta in corso" | `FLAG_REDS_PATH_BATTLE` (ex `FLAG_UNUSED_EXTENDED_86`), letto da `IsRedsPathBattle()` |
| "è un capo di Red's Path" | `IsRedsPathTrainer(id)`, per id: level scaling e TcloneR |
| "sfida vinta" | il flag "allenatore battuto" del capo, acceso solo alla vittoria |

## Agganci di lotta (A2), tutti dietro `IsRedsPathBattle()`

| Cosa | Dove |
| --- | --- |
| dorso di Red (quello di FRLG, l'unico) | `PlayerGetTrainerBackPicId` (`battle_controller_player.c`): un punto solo per disegno, slide e palette |
| mugshot di Red FRLG, banda maschile | `Mugshot_SetGfx` e la creazione dello sprite del giocatore (`battle_transition.c`) |
| niente fuga | `CanPlayerForfeitNormalTrainerBattle` (`battle_main.c`): ci passano tutti e tre i chiamanti |
| niente svenimento né soldi persi | **`B_FLAG_NO_WHITEOUT` è `FLAG_REDS_PATH_BATTLE`** (`include/config/battle.h`, solo HnS) |
| niente esperienza | `Cmd_getexp` (`battle_script_commands.c`); senza esperienza niente Candy Jar |
| niente evoluzioni | `TryEvolvePokemon` (`battle_main.c`) salta il giro |

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
**Vittoria**: il premio in soldi del capo arriva come in ogni lotta
(Brock ₽1400, tutti e otto ₽27100). Il piano non dice niente: da decidere
nella B.

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

- Nei testi della lotta il nome resta quello del giocatore ("You are
  challenged by…", "<NOME> is out of usable POKéMON!"): RED compare solo nel
  testo di sconfitta dei capi.
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

Non provato: la sconfitta per tutti e otto (stesso codice), evoluzioni
speciali in lotta (nessun Pokémon di Red ne ha), migrazione di un salvataggio
a versione 9 (fatta nella A1, codice non toccato).
