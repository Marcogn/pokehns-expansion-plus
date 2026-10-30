---
paths:
  - "src/save.c"
  - "include/save.h"
  - "include/global.h"
  - "src/new_game.c"
  - "src/oak_speech_hns.c"
  - "src/option_menu*.c"
  - "include/option_menu.h"
---

# Salvataggi: layout, polarità, migrazioni

Cronaca di ogni `SAVE_VERSION` (come fu provata, checksum, byte): `docs/claude/save-history.md`.

## Mappa dei blocchi (stato a `SAVE_VERSION 14`)

| Dove | Cosa | Versione |
| --- | --- | --- |
| SB3, `struct ChallengeSettings` | **32 byte fissi** (`ChallengeSettingsLayoutPinned` in `save.c`); campi solo in fondo | — |
| SB3 +56 | `struct SoundAndFieldOptions` (1 byte di bitfield; `sizeof` è 4 per `-mabi=apcs-gnu`) | 8 |
| SB3 +60 | `struct QuestLogSaveData`, 32 byte dalla v13 (`seenHigh` a +24 della struct) | 12, 13 |
| SB3 +92 | `stripMenuOrder[4]` (un nibble per posto, id icona + 1) | 14 |
| SB3 | 96 byte su `SAVE_BLOCK_3_CHUNK_SIZE` 116: **20 liberi** | |
| SB2 +0x28 | `pokedex.tclonerTrainerFlags`, 83 byte (dal vecchio `filler` del Pokédex, ne restano 21) | 9, 10 |
| flag | `FLAG_REDS_PATH_BATTLE` (ex `EXTENDED_86`), `INTRO_SEEN` e `PRIZE_*` (ex `87..95`); liberi `96..99` ma **non dimostrabilmente a zero** | 10, 11 |
| engine flag | `FLAG_FAST_HMS` (`ENGINE_FLAGS_START + 13`); liberi da `+14` | 8 |

`soundAndFieldOptions`: `fastHms` 1 bit, `battleMusicTheme` 3 (0 = regionale),
`noNightCenterMusic` 1 (**invertito**), `levelScaling` 1, più due bit ignorati
(`unusedOverworldMonsBit`, `unusedLevelScalingBit`) che nei salvataggi di build
passate possono valere 1: **il byte è pieno** finché una migrazione non li
azzera. Una nuova opzione: un flag, o un byte nuovo in coda a SB3.

## Bit liberi in `ChallengeSettings`

Resta **1** bit, `unusedTrainerCatchBit` (byte 31, bit 7): la migrazione
`saveVersion < 9` lo azzera, quindi si riusa senza nuova migrazione. Dopo, la
prossima opzione allarga la struct (sposta il layout) o vive in un flag.
Riusare un bit che ha già avuto un significato vuol dire che nei salvataggi
passati per quelle build può valere 1 (per questo `< 7` azzera
`unusedDexNavBit`); stessa cautela per `FLAG_FORCE_SHINY` (ex `FLAG_UNUSED_3`).
**Fondere due opzioni è anche il modo di recuperare bit.**

## Polarità

Un salvataggio scritto prima dell'opzione legge **zero**: zero deve essere il
comportamento di prima, anche se il nome viene brutto. **L'etichetta del valore
zero viene prima nella lista di scelte** (`sChoices_OnOff` = ON per primo,
`sChoices_OffOn` = OFF per primo). `skipNicknamePrompt` è invertito per questo;
`noWildEncounters` 0 = incontri attivi. Vale anche quando il default è ON:
`ENHANCED DEXNAV` è `basicDexNav` (1 = DexNav base), messo a `FALSE` in
`new_game.c`.

## Un campo nuovo: le quattro cose

1. In **fondo** alla struct (niente si sposta).
2. Un `STATIC_ASSERT` sul suo offset in `save.c`.
3. **Un passo `saveVersion < N` in `LoadGameSave()`** (+ `SAVE_VERSION` in
   `include/save.h`): in un salvataggio vecchio quei byte sono spazzatura del
   buffer di settore. `candyJarExp` è in XOR con `encryptionKey`: uno zero
   pulito letto restituisce la **chiave stessa**, un saldo a nove cifre. La
   migrazione è anche il posto per **regalare un oggetto nuovo a chi è oltre il
   punto in cui lo si riceve** (Candy Jar: passo `< 6`).
4. Se è un'opzione scelta dal titolo: aggiungerla in `oak_speech_hns.c`
   (azzera `challengeSettings` e ricopia i campi a mano) **e** in
   `NewGameInitData`, che chiama `ClearSav3()` e restituisce a mano solo
   `challengeSettings` e `savedSoundAndField`. Altrimenti la scelta sparisce in
   silenzio. Un campo che non è un'opzione (quest book, ordine della striscia)
   resta azzerato da `ClearSav3()`, ed è giusto così.

## Trappole

- **`NewGameInitData` non chiama `ClearSav2()`**: azzera SB1 e SB3, di SB2 solo
  i campi azzerati a mano (Pokédex, frontier…). `ClearSav2` gira solo da
  `Sav2_ClearSetDefault` (salvataggio vuoto o corrotto). Un campo di gioco in
  SB2 si azzera a mano: i flag del TcloneR li azzera `ResetPokedex()`.
- Il flag `FLAG_FAST_HMS` è lo specchio a runtime di `fastHms` (lo scrivono
  `Task_Save` e `NewGameInitData`); il campo serve a farlo sopravvivere a
  `ClearSav1()`.
- Alzare `TCLONER_TRAINER_FLAGS_BYTES` o le capienze `QUEST_SAVE_MAX_*` allarga
  la struct: serve un passo nuovo. Il passo 10 azzerò solo il byte 82.
- `GetIconOrder` (`strip_start_menu.c`) rifiuta ciò che non è una permutazione,
  ma spazzatura che lo fosse per caso mescolerebbe il menu: azzerare (passo 14).
- `DrawChoices_Many` mostra solo la scelta corrente: un'opzione con più di 4
  scelte (BATTLE MUSIC ne ha 7) si vede bene solo in emulatore.
- **Provare una migrazione**: porta a mano un `.sav` alla versione N-1 con i
  byte sporchi e ricalcola il checksum del settore (somma di u32 sui soli `size`
  byte di dati, `(s >> 16) + s` a 16 bit, a `0xFF6`; slot attivo = `counter`
  a `0xFFC` più alto), carica, salva, rileggi.
