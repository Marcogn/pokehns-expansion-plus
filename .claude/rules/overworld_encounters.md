---
paths:
  - "src/wild_encounter.c"
  - "src/wild_encounter_ow.c"
  - "include/wild_encounter_ow.h"
  - "include/config/wild_encounter.h"
  - "src/data/pokemon/wild_encounter_ow_behavior.h"
  - "src/event_object_movement.c"
  - "src/field_control_avatar.c"
  - "src/field_effect_helpers.c"
  - "include/config/overworld.h"
  - "docs/plans/overworld-encounters.md"
  - "docs/plans/overworld-encounters/**"
---

# Pokémon visibili nell'overworld: in corso

Piano, stato, fatti verificati e decisioni in
[`docs/plans/overworld-encounters.md`](../../docs/plans/overworld-encounters.md):
port della PR #8434 di expansion (dal diff del merge `10ae7f9`, che usa
ancora i nomi `OW_` come noi, più le sole correzioni arrivate fino a
`expansion/1.16.4`; **non** da `master`, che dipende da `mass_outbreak.h`)
più un tiro pesato per indole,
sul modello del mod Wilds of Kanto. Le indoli scelte dall'utente sono in
`docs/plans/overworld-encounters/temperaments.json`.

## Stato: fase 2 fatta (21/10), tutto spento

Codice della PR più #9925, #9968, #10066, #10753 nel ramo, con
`WE_OW_ENCOUNTERS FALSE` (`include/config/wild_encounter.h`, tutti i valori
ai default upstream). ROM: 31 832 868 → 31 855 284 byte (+22 416, 94,87% →
94,94%). Build pulita senza warning nuovi.

## Adattamenti alla nostra base (1.15.2)

- `enum Species` non esiste da noi: nel codice portato è `u32`.
- `gParties[B_TRAINER_1]` → `gEnemyParty`.
- Le sei costanti `MOVEMENT_TYPE_*_OWE` partono da `0x54`, non `0x53`
  come upstream: `0x53` da noi è `MOVEMENT_TYPE_TOWER_BEAM` di HnS.
- `ComputePlayerShinyOdds()` (nuova, estratta da `CreateBoxMon`) usa
  `GetShinyOdds()` come il codice che sostituisce, non `SHINY_ODDS`: senza,
  l'opzione SHINY CHANCE del challenge menu sparirebbe in silenzio.
- `WE_OWE_PREVENT_SPECIAL_MOVEMENT_DESPAWN` aggiunta a mano (viene da #9968).

## Trappole trovate nel port

- **Gli hunk entrati "con fuzz" vanno riletti.** `patch -F3` ha messo
  `CleanIncompatibleGenderSpecies` *dentro* lo `switch` di `IsValidGender`
  (`pokemon.c`) e l'`.include` di `wild_encounter.inc` sotto il commento
  `@ FRLG scripts` invece che dopo `apricorn_tree.inc`. Compilavano entrambi.
- **`TryAndDespawnOldestGeneratedOWE_ToFreeObject` con OWE spenti** restituiva
  `FALSE`, che per `GetAvailableObjectEventId` vuol dire "slot libero": a
  tabella dei 16 oggetti piena uno spawn avrebbe usato un indice non
  inizializzato. Upstream l'ha corretto in 1.16.4 dentro un refactor di
  `GetAvailableObjectEventId` che non abbiamo preso; qui `return TRUE`.
  Il refactor 1.16.4 di quella funzione **non** va preso così com'è: cambia
  la firma e l'ordine di ricerca degli slot per tutti gli oggetti.
- **`DespawnOWEOnBattleStart`** gira a ogni inizio lotta (`battle_setup.c`)
  e indicizzava `gObjectEvents` con il risultato "non trovato"
  (`OBJECT_EVENTS_COUNT`): lettura, e potenziale `ClearObjectEvent`, dei
  `.sbss` di `field_region_map.o`/`field_specials.o` che seguono l'array.
  Guardia aggiunta. Lo stesso schema (`&gObjectEvents[GetObjectEventIdByLocalId(...)]`
  senza controllo) resta in `wild_encounter_ow.c` nelle funzioni sugli slot
  OWE (`GetOldestActiveOWESlot`, `SortOWEAges` e vicine): girano solo con OWE
  accesi, **da guardare in fase 3**.
- `GetAvailableObjectEventId`: con la tabella piena e un `localId` di OWE
  generato, cade su `*objectEventId = 16` e restituisce `FALSE`. Upstream
  non ci arriva perché controlla prima `CheckCanLoadOWE`; da tenere presente
  se si cambia lo spawn.
- **`trainer_type` dei massi di HnS**: i massi di Forza che cadono nelle buche
  (Ice Path B1F) mettono un flag (`FLAG_HIDE_ICEPATH_B2F_BOULDER1..4`,
  `0x2F7-0x2FA`) nel campo `trainer_type`. `ObjectEvent.trainerType` è `u8`,
  e upstream riconosce un OWE da `trainerType == 0xFF`
  (`TRAINER_TYPE_OW_WILD_ENCOUNTER`). Oggi i byte bassi sono `F7-FA`: un
  flag nuovo `0x?FF` usato così diventerebbe un Pokémon selvatico.
- Gli OWE generati usano i `localId` da `LOCALID_OW_ENCOUNTER_END` (252) in
  giù, `OWE_SPAWNS_MAX` di numero: con 6 (D4) diventano 247-252, ancora
  sotto `OBJ_EVENT_ID_NPC_FOLLOWER` (253) e sopra Berry Blender (236-240).
  Le mappe HnS arrivano a 64 oggetti (Bellchime Trail).
- **DexNav**: l'aggancio upstream (`DespawnAllOverworldWildEncounters` in
  `Task_DexNavExitAndSearch`) copre solo la ricerca dalla schermata. Da noi
  la ricerca parte anche da **R** (`TryStartDexNavSearch` →
  `InitDexNavSearch`, `field_control_avatar.c`) e c'è la ricerca dei nascosti
  (`FlagSet(DN_FLAG_SEARCHING)` in `dexnav.c`, ~riga 2957): nessuno dei due
  fa sparire gli OWE. Da sistemare in fase 3, probabilmente in
  `InitDexNavSearch`.
- `UpdateOverworldWildEncounter` (ogni frame, da `OverworldBasic`) chiama
  `CheckCurrentWildMonHeaderForOWE` **prima** di guardare
  `WE_OW_ENCOUNTERS`: costo per frame anche a feature spenta. Non toccato
  (upstream), ma è il primo posto dove guardare se il frame rate cala.
- `ShouldDisableRandomEncounters()` (`field_control_avatar.c`) usa
  `WE_VANILLA_RANDOM`, di compilazione: l'interruttore della fase 5 ha
  bisogno di un controllo a runtime lì.

## Riflessi (#9925), l'unica modifica sempre attiva voluta

Offset del tag del riflesso `0x2000 → 0x0800`, raw `0x4000 → 0x3000`,
controlli `== 0xFF`. Corregge anche una seconda collisione che il piano non
diceva: il vecchio `HIGH_BRIDGE_PAL_TAG 0x4010` era il tag della palette di
Pidgey in overworld (`16 + OBJ_EVENT_MON`).

Verifica (mGBA, ora finta con `faketime`, stesso salvataggio caricato nelle
due ROM): Violet City, isolotto del ponte (26,25), giocatore + follower
Bulbasaur + NPC sopra il laghetto. Riquadro dei riflessi di giocatore e
follower: **0 pixel diversi** fra base e nuova (39 colori nel riquadro).
Il caso shiny + normale della stessa specie resta da provare in fase 3.

## Prove della fase 2 in emulatore

Nuova partita, Bulbasaur dal debug, warp su Route 29: incontro a passi
nell'erba alta (Sentret Lv2), fuga, ritorno in mappa con follower; i
Pokémon-oggetto già sulle mappe (Route 29, isolotto di Violet) si vedono e
riflettono come prima; salvataggio e caricamento.

Mappa ASCII dei tile riflettenti di un layout (per trovare un punto di
prova): HnS ha **640** metatile primari (`include/fieldmap.h`), non 512;
la collisione sono i bit 10-11 di ogni u16 di `map.bin`.
