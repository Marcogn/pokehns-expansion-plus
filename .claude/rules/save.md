---
paths:
  - "src/save.c"
  - "include/save.h"
  - "include/global.h"
  - "src/new_game.c"
  - "src/oak_speech_hns.c"
  - "src/option_menu*.c"
  - "include/option_menu.h"
  - "include/constants/flags.h"
---

# Salvataggi: layout, polarità, migrazioni

`struct ChallengeSettings` in `include/global.h` è **fissata a 32 byte** da
`STATIC_ASSERT(..., ChallengeSettingsLayoutPinned)` in `src/save.c`.

- Aggiungi campi **solo in fondo**, così nessun campo esistente cambia offset.
- Bit liberi: **1**, `unusedTrainerCatchBit` (byte 31, bit 7 della struct):
  era `trainerCatch`, tolto quando l'opzione è diventata l'oggetto TcloneR, e
  il passo `saveVersion < 9` lo azzera, quindi si può riusare **senza** nuova
  migrazione. Prima ancora era `unusedDexNavBit` (liberato fondendo `DEXNAV
  SHOW ALL` e `DEXNAV SOULGOLD`). Dopo, **la prossima opzione deve
  ingrandire la struct**, e quello sposta il layout del salvataggio — oppure
  vivere in un flag, come fa Soulgold per le luci. Anche il byte
  `soundAndFieldOptions` (vedi sotto) è pieno dal level scaling.
  Riusare un bit che ha già avuto un significato vuol dire che nei salvataggi
  passati per quelle build può valere 1: per questo il passo
  `saveVersion < 7` lo azzera. Stessa cautela per `FLAG_FORCE_SHINY` (ex
  `FLAG_UNUSED_3`): la storia di upstream è un commit unico, non si può provare
  che una HnS vecchia non l'abbia mai scritto, e la migrazione lo pulisce.
- **Fondere due opzioni è anche il modo di recuperare bit.** Se due toggle non
  si usano mai separati, uno solo costa la metà.
- Dopo ogni aggiunta verifica che compili: l'assert fallisce da sola se sfori.

**Polarità dei bit.** Un salvataggio scritto prima che l'opzione esistesse legge il
bit a **zero**. Quindi se il default deve essere "come prima", memorizza il campo in
modo che zero significhi il comportamento vecchio — anche se il nome viene brutto.
`skipNicknamePrompt` è memorizzato invertito proprio per questo, e la lista di scelte
è ordinata di conseguenza (`sChoices_OnOff` = ON per primo, `sChoices_OffOn` = OFF
per primo). La regola in una riga: **l'etichetta del valore zero deve venire prima**.
`noWildEncounters` segue la stessa logica (0 = incontri attivi = "ON").

La regola vale **anche quando il default è ON**, e allora il campo va memorizzato
invertito: `ENHANCED DEXNAV` è il primo caso. Default ON, un salvataggio vecchio
legge 0, quindi 0 deve essere il lato ON → il campo si chiama `basicDexNav`
(1 = torna al DexNav base) e la lista è `sChoices_OnOff`. Nome brutto, polarità
giusta. Stessa cosa in `new_game.c`, dove il campo va messo a `FALSE` proprio
perché il default è ON.

Le opzioni impostate prima di iniziare una partita passano da
`src/oak_speech_hns.c`, che azzera tutta la struct e poi ricopia a mano i campi
del menu opzioni. **Ogni campo nuovo va aggiunto anche a quella lista**, altrimenti
la scelta fatta dal titolo viene persa in silenzio. Nota: quel memset tocca
**solo** `challengeSettings`, non tutto SaveBlock3, quindi i campi aggiunti fuori
dalla struct non passano di lì.

### `soundAndFieldOptions`: il secondo posto per le opzioni

Dal `SAVE_VERSION 8` (Fast HMs, tema musica di lotta, musica notturna nei
Centri) c'è un secondo contenitore, **in coda a SaveBlock3 dopo `candyJarExp`**
(offset 56): `struct SoundAndFieldOptions`, un byte di bitfield.
`ChallengeSettings` non è stata toccata.

| campo | bit | polarità |
| --- | --- | --- |
| `fastHms` | 1 | 0 = OFF (default) |
| `battleMusicTheme` | 3 | `enum BattleMusicTheme`, 0 = DEFAULT (regionale) |
| `noNightCenterMusic` | 1 | **invertito**: 0 = musica notturna ON (default) |
| `unusedOverworldMonsBit` | 1 | ex `noOverworldMons` (voce `WILD POKéMON` del PC, tolta il 21/10): ignorato, ma nei salvataggi passati per quelle build può valere 1; prima di riusarlo azzerarlo con una migrazione |
| `levelScaling` | 1 | 0 = OFF (default): allenatori, selvatici e lotte di script. Ex `unused`: nessuna build l'aveva scritto, la migrazione `< 8` l'ha azzerato, quindi nessuna migrazione nuova |
| `unusedLevelScalingBit` | 1 | era `wildLevelScaling`, quando il level scaling era due opzioni (una build di poche ore): ignorato, ma un salvataggio di quella build può averlo a 1; prima di riusarlo azzerarlo con una migrazione |

**Il byte è pieno** (i due bit non usati, `unusedOverworldMonsBit` e
`unusedLevelScalingBit`, vanno prima azzerati con una migrazione). La prossima opzione: un flag, o un byte nuovo
in coda a SaveBlock3 con il suo `STATIC_ASSERT` e il suo passo di migrazione.

- **Flag specchio.** `FLAG_FAST_HMS` (`ENGINE_FLAGS_START + 13`) è lo stato a
  runtime, come `FLAG_EVEN_FASTER_JOY`: lo leggono gli script (`goto_if_set`) e
  `ShouldSkipHmAnimation()`. Lo scrivono `Task_Save` e `NewGameInitData`. Il
  campo serve solo a farlo sopravvivere a `ClearSav1()` di inizio partita.
  Engine flag liberi: da `+14`.
- **Trappola trovata verificando: `NewGameInitData` chiama `ClearSav3()`.** Il
  piano diceva "il memset di `oak_speech_hns.c` tocca solo `challengeSettings`",
  vero, ma `NewGameInitData` azzera **tutta** SaveBlock3 e restituiva a mano
  solo `challengeSettings`. Un campo nuovo fuori da quella struct va quindi
  salvato e ripristinato **anche lì** (`savedSoundAndField`), altrimenti la scelta
  fatta dal titolo sparisce in silenzio. Verificato: dal titolo FAST HMS ON +
  DPPT + NIGHT CENTER OFF, nuova partita, byte `0x1B` e flag 2413 TRUE.
- **`sizeof(struct SoundAndFieldOptions)` è 4, non 1**: con `-mabi=apcs-gnu` ogni
  struct è arrotondata a una word. L'offset resta 56 e `SaveBlock3` arriva a 60
  byte; l'assert che conta è `sizeof(SaveBlock3) <= SAVE_BLOCK_3_CHUNK_SIZE`.
- Migrazione `saveVersion < 8` in `LoadGameSave`: azzera il campo e il flag.
- **L'opzione `BATTLE MUSIC` ha 7 scelte e il menu ne disegnava al massimo 4**:
  `DrawChoices_Many` mostra solo la scelta corrente fra `<` e `>`. Le prime
  schermate mostravano la riga vuota; l'ho visto solo in emulatore.

### `SAVE_VERSION 9`: TcloneR

Azzera `unusedTrainerCatchBit`, le tre var `VAR_TCLONER_*` e il bitfield
`pokedex.tclonerTrainerFlags` (SaveBlock2 + 0x28, 82 byte nel vecchio
`filler` del Pokédex: `STATIC_ASSERT` su offset e dimensione di
`struct Pokedex`). Verificato su un `.sav` riportato a versione 8 a mano
(dettagli in `.claude/rules/trainer_catch.md`). Per sporcare un `.sav` serve
il checksum per settore: somma di u32 sui soli `size` byte di dati (non la
coda di SaveBlock3), `(s >> 16) + s` troncato a 16 bit, a `0xFF6`; lo slot
attivo è quello col `counter` (`0xFFC`) più alto.

### C'è già un sistema di migrazione dei salvataggi: usalo

Scoperto portando il Candy Jar, dopo aver scritto a mano un marker magico che
non serviva. `LoadGameSave()` in `src/save.c` confronta
`gSaveBlock1Ptr->saveVersion` con `SAVE_VERSION` (`include/save.h`) e applica in
sequenza i blocchi `if (saveVersion < N)`. Una partita nuova nasce già a
`SAVE_VERSION`, quindi i blocchi girano solo sui salvataggi vecchi.

Per aggiungere un campo a SaveBlock3 servono quindi tre cose, non una:

1. il campo **in fondo** alla struct (niente si sposta, e `SAVE_BLOCK_3_CHUNK_SIZE`
   è 116, quindi finché si sta sotto resta tutto nella coda del sector 0);
2. un `STATIC_ASSERT` sul suo offset in `save.c`, che inchioda "in fondo";
3. **un passo di migrazione**, perché in un salvataggio scritto prima quei byte
   contengono spazzatura del buffer di settore.

Il punto 3 non è teorico. `candyJarExp` è memorizzato in XOR con
`encryptionKey` come i soldi: leggere anche uno zero pulito restituisce la
**chiave stessa**, cioè un saldo a nove cifre, e il primo uso del barattolo
avrebbe consegnato 999 caramelle di ogni taglia. Verificato in emulatore che
dopo la migrazione il saldo parte da 0.

La migrazione è anche il posto giusto per **regalare un oggetto nuovo a chi è
già oltre il punto in cui lo si riceve**: il Candy Jar arriva dall'assistente di
Elm a Violet City, che un salvataggio a 3 medaglie non rivedrà mai più, quindi
il passo `saveVersion < 6` lo mette in borsa se `FLAG_RECEIVED_TOGEPI_EGG` è
alzato.
