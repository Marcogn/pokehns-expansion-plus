# Piano: Fast HMs, tema musica di lotta, musica notturna nei Centri (da Soulgold 1.2)

## Contesto

Il sync del 12/10/2026 (PR #9) ha lasciato come candidati al §5 di
`docs/SOULGOLD_PORT_REPORT.md` tre feature nuove di Soulgold 1.2. L'utente le
vuole tutte e tre:

| feature | commit SG | in SG |
| --- | --- | --- |
| Fast HMs | `a9d41a7b9` | `FLAG_FAST_HMS`, salta domanda + messaggio + posa/animazione delle MN |
| Tema musica di lotta | `f81a55ff3` | `VAR_BATTLE_MUSIC_THEME`, 6 temi |
| Musica notturna nei Centri | `c913c4a2d` | sempre attiva, `GetLocationMusic` |

Decisioni dell'utente (12/10):
- musica notturna nei Centri **come opzione, default ON**;
- temi: **DEFAULT (regionale), HGSS JOHTO, HGSS KANTO, FRLG, RSE, DPPT, RANDOM**;
  il tema cambia selvatici, allenatori generici, capipalestra **e Superquattro**;
  boss, rivali, Rocket, campioni, leggendari, link e strutture lotta restano come ora.
- Fast HMs: default OFF come SG.

Regola numero uno (CLAUDE.md §2): **non copiare valori di SG** (id di brani,
flag, var, coordinate). Si copiano solo i `.mid` e la logica, adattati a Hasep.

## Fatti verificati in Hasep (non ri-scoprirli)

- **Brani DP**: Hasep ne ha 11 (`MUS_DP_AZURE_FLUTE` … `MUS_DP_STARK_MOUNTAIN`,
  `include/constants/songs.h`, `DP_MUSIC_END` = `MUS_DP_STARK_MOUNTAIN`,
  `END_MUS` = `DP_MUSIC_END`, i phoneme seguono `END_MUS + 1`). **Mancano**
  `mus_dp_vs_wild`, `mus_dp_vs_trainer`, `mus_dp_vs_gym_leader`,
  `mus_dp_poke_center_night`. In SG usano tutti e quattro `voicegroup191`, e
  `sound/voicegroups/voicegroup191.inc` è **identico byte per byte** fra i due
  repo: servono solo i `.mid` + `midi.cfg` + `song_table.inc` + costanti.
  I `.s` in `sound/songs/midi/` non sono tracciati: li genera la build.
  Righe SG di `midi.cfg`: `mus_dp_vs_wild/vs_trainer/vs_gym_leader.mid: -E -R50 -G191 -V088`,
  `mus_dp_poke_center_night.mid: -E -R50 -G191 -V127`.
- **Tutti gli altri brani dei temi esistono già**: `MUS_HG_VS_WILD/TRAINER/GYM_LEADER`,
  `MUS_HG_VS_*_KANTO`, `MUS_RG_VS_WILD/TRAINER/GYM_LEADER`, `MUS_VS_WILD/TRAINER/GYM_LEADER`.
- **`GetBattleBGM()`** sta in `src/pokemon.c` (~riga 8058) ed è già regionale
  (`#if IS_HNS`, `GetCurrentRegion()`): Johto→HG, Kanto/Alola→HG Kanto,
  Hisui→HG, resto→RSE. È chiamata **una volta** a inizio lotta
  (`PlayBattleBGM`, `PlayMapChosenOrBattleBGM`, `Task_PlayMapChosenOrBattleBGM`),
  quindi RANDOM può scegliere lì.
- **`GetLocationMusic()`** sta in `src/overworld.c` (~riga 1276). Musiche dei
  Centri nelle mappe: `MUS_HG_POKE_CENTER` (16), `MUS_POKE_CENTER` (36),
  `MUS_RG_POKE_CENTER` (69). `MUS_RG_NET_CENTER` (2) **non** è un Centro, escluso.
  `GetTimeOfDay()` / `TIME_NIGHT` in `include/rtc.h`, `include/constants/rtc.h`.
- **Opzioni**: `src/option_menu.c` è il menu a tab di HnS (TAB_MAIN / TAB_BATTLE /
  TAB_SOUND), `MAX_ITEMS_PER_TAB 20`, TAB_MAIN ha **19** voci. Ogni voce ha
  `descriptions` con **una stringa per scelta**. Lettura in `CB2_InitOptionMenu`
  case 6, scrittura in `Task_Save`.
- **Salvataggio** (CLAUDE.md §4): `ChallengeSettings` è fissata a 32 byte e non
  si tocca. `SaveBlock3`: `challengeSettings` a 16, `registeredItemHold` a 48,
  `candyJarExp` a **52** (ultimo campo, `src/save.c` ha gli STATIC_ASSERT),
  `SAVE_BLOCK_3_CHUNK_SIZE` 116. `SAVE_VERSION` = **7** (`include/save.h`).
  `NewGameInitData` (`src/new_game.c`) azzera solo campi specifici di SaveBlock3
  e chiama `ClearSav1()` (che azzera **tutti i flag**).
- **Precedente da imitare**: `FLAG_EVEN_FASTER_JOY` (`ENGINE_FLAGS_START + 0`)
  è uno specchio di `challengeSettings.evenFasterJoy`: lo scrivono `Task_Save` in
  `option_menu.c` e `NewGameInitData` (blocco "Sync engine flags from restored
  challenge settings", ~riga 315), e lo leggono gli script con `goto_if_set`.
  Engine flag liberi: da `ENGINE_FLAGS_START + 13` a `+ 31`.
- **Script MN di Hasep** (`data/scripts/field_move_scripts.inc`, `surf.inc`)
  hanno le stesse etichette di SG (`EventScript_CutTreeCommon`,
  `EventScript_RockSmashCommon`, `EventScript_UseWaterfall`, `EventScript_UseDive`,
  `EventScript_UseDiveUnderwater`, `EventScript_CutGrassCommon`,
  `EventScript_UseRockClimb`, …) e `isfollowerfieldmoveuser`. `src/oras_dowse.c`
  esiste. `FLDEFF_DEFOG` (74) e `FLDEFF_USE_ROCK_CLIMB` (75) esistono.

## Progetto

### 1. Memoria delle opzioni (un byte nuovo in coda a SaveBlock3)

`include/global.h`, **dopo** `candyJarExp`:

```c
// Appended after candyJarExp (SAVE_VERSION 8). Every field reads 0 as its
// default, so a fresh save and a migrated one agree without extra code.
struct SoundAndFieldOptions
{
    u8 fastHms:1;            // 0 = off (default, as in Soulgold)
    u8 battleMusicTheme:3;   // enum BattleMusicTheme, 0 = DEFAULT
    u8 noNightCenterMusic:1; // stored inverted: 0 = night theme ON (default)
    u8 unused:3;
};
...
    u32 candyJarExp;
    struct SoundAndFieldOptions soundAndFieldOptions;
```

- `src/save.c`: `STATIC_ASSERT(offsetof(struct SaveBlock3, soundAndFieldOptions) == 56, ...)`
  con commento come quello di `candyJarExp`.
- `include/save.h`: `SAVE_VERSION` 7 → **8**.
- `src/save.c`, in `LoadGameSave`, nuovo blocco `if (gSaveBlock1Ptr->saveVersion < 8)`:
  `memset` a zero del campo (spazzatura del buffer di settore nei salvataggi vecchi)
  e `FlagClear(FLAG_FAST_HMS)`; poi `saveVersion = 8`. Commento "perché" come i blocchi 6 e 7.
- **Non** serve toccare `oak_speech_hns.c`: il suo `memset` tocca solo
  `challengeSettings`. Verificare comunque leggendo che `NewGameInitData` non
  azzeri il nuovo campo (così le scelte fatte dal titolo sopravvivono).
- Aggiornare il commento di CLAUDE.md §4 (nuovo campo, SAVE_VERSION 8).

### 2. Flag specchio per gli script

- `include/constants/flags.h`: `#define FLAG_FAST_HMS (ENGINE_FLAGS_START + 13)`.
  Prima: `git log -S "ENGINE_FLAGS_START + 13" --oneline` deve essere vuoto
  (il bit non ha mai avuto un significato); la migrazione lo pulisce comunque.
- Scritto in `Task_Save` (`option_menu.c`) come `FLAG_EVEN_FASTER_JOY` e in
  `NewGameInitData` nel blocco "Sync engine flags" (`if (…fastHms) FlagSet(FLAG_FAST_HMS);`).
- Il codice C legge il **flag** (come SG): è lo stato a runtime; il campo serve
  solo a farlo sopravvivere a `ClearSav1()` di inizio partita.

### 3. Menu opzioni (`src/option_menu.c`, `include/option_menu.h`)

- TAB_MAIN: `ITEM_MAIN_FAST_HMS` dopo `ITEM_MAIN_OW_SPEED`, nome `"FAST HMS"`,
  `sChoices_OffOn`, descrizioni: OFF "Show the prompt and animation\nwhen using a field move."
  / ON "Skip the prompt and animation\nwhen using a field move.". TAB_MAIN arriva a
  20 = `MAX_ITEMS_PER_TAB`: aggiungere `STATIC_ASSERT(ITEM_MAIN_COUNT <= MAX_ITEMS_PER_TAB, …)`
  (e lo stesso per le altre due tab).
- TAB_SOUND: `ITEM_SOUND_BATTLE_MUSIC` (`"BATTLE MUSIC"`, 7 scelte
  `sChoices_BattleMusic` = `DEFAULT`, `HG JOHTO`, `HG KANTO`, `FRLG`, `RSE`,
  `DPPT`, `RANDOM` — maiuscolo come le altre voci; descrizione per scelta:
  DEFAULT "Each region keeps its own\nbattle themes.", RANDOM "A random theme\nfor every battle.",
  gli altri "Wild, Trainer, Gym and Elite Four\nbattles use the X themes.") e
  `ITEM_SOUND_NIGHT_CENTER` (`"NIGHT CENTER"`, `sChoices_OnOff`: indice 0 = ON →
  `noNightCenterMusic`, polarità §4; descrizioni "Pokémon Centers play the\nDPPt night theme at night."
  / "Pokémon Centers keep their\nusual theme at night.").
  Controllare a schermo che "BATTLE MUSIC" + "HG KANTO" stiano nella riga.
- Lettura in `CB2_InitOptionMenu` case 6, scrittura in `Task_Save`, accanto alle altre.
- Accessori pubblici in `include/option_menu.h` (vicino a `IsDarkUiEnabled()`):
  `enum BattleMusicTheme GetBattleMusicTheme(void)` (clamp a DEFAULT se ≥ COUNT)
  e `bool32 IsNightCenterMusicEnabled(void)`.

### 4. Brani DP (4 nuovi)

1. Copiare da `/home/user/soulgold/sound/songs/midi/` i quattro `.mid`.
2. `sound/songs/midi/midi.cfg`: le quattro righe con i flag di SG (sopra),
   vicino alle altre `mus_dp_*`.
3. `include/constants/songs.h`: **in coda** dopo `MUS_DP_STARK_MOUNTAIN`
   (`DP_MUSIC_START + 0x0B` … `+ 0x0E`): `MUS_DP_VS_WILD`, `MUS_DP_VS_TRAINER`,
   `MUS_DP_VS_GYM_LEADER`, `MUS_DP_POKE_CENTER_NIGHT`; `DP_MUSIC_END` →
   `MUS_DP_POKE_CENTER_NIGHT`. Mai i numeri di SG (lì sono 679, 706…).
4. `sound/song_table.inc`: quattro righe `song mus_dp_…, MUSIC_PLAYER_BGM, 0`
   **subito dopo** `mus_dp_stark_mountain`, nello **stesso ordine** delle costanti
   (la tabella è posizionale).
5. Controllare che nessun salvataggio memorizzi id ≥ `FIRST_PHONEME_SONG`
   (i phoneme slittano di 4): `grep -rn "savedMusic\|PH_" src include` e leggere.
   Controllare come il Makefile raccoglie i `.mid` (`audio_rules.mk` / wildcard).

### 5. Tema musica di lotta (`src/pokemon.c`, `include/pokemon.h`)

- `include/pokemon.h`: `enum BattleMusicTheme { BATTLE_MUSIC_THEME_DEFAULT,
  _HGSS_JOHTO, _HGSS_KANTO, _FRLG, _RSE, _DPPT, _RANDOM, _COUNT }` e
  `enum BattleMusicCategory { _WILD, _TRAINER, _GYM, _COUNT }`;
  `STATIC_ASSERT(BATTLE_MUSIC_THEME_COUNT <= 8, …)` (campo a 3 bit).
- Tabella `sBattleMusicThemeSongs[THEME][CATEGORY]` con le voci concrete
  (JOHTO: `MUS_HG_VS_WILD/TRAINER/GYM_LEADER`; KANTO: `MUS_HG_VS_*_KANTO`;
  FRLG: `MUS_RG_VS_*`; RSE: `MUS_VS_*`; DPPT: `MUS_DP_VS_*`).
- `static u16 GetThemedBattleBGM(enum BattleMusicTheme theme, enum BattleMusicCategory cat, u16 regionalSong)`:
  DEFAULT → `regionalSong`; RANDOM → un tema concreto a caso fra JOHTO…DPPT
  (usare `Random2()` se esiste in Hasep, così non si tocca l'RNG principale;
  altrimenti `Random()`); altrimenti la tabella.
- In `GetBattleBGM()`: calcolare il tema una volta; se
  `gBattleTypeFlags & (BATTLE_TYPE_FRONTIER | BATTLE_TYPE_TRAINER_HILL)` → DEFAULT (come SG).
  Agganci, ognuno passando il valore di oggi come `regionalSong`:
  - `TRAINER_CLASS_LEADER`, `_LEADER_HNS`, `_LEADER_KANTO_HNS`, `_LEADER_FRLG` → GYM;
  - `TRAINER_CLASS_ELITE_FOUR`, `_ELITE_FOUR_HNS`, `_ELITE_FOUR_FRLG` → GYM;
  - **attenzione**: oggi `LEADER_FRLG`, `ELITE_FOUR_FRLG` e `PROFESSOR_HNS` condividono
    un `case`: separare `PROFESSOR_HNS`, che resta `MUS_RG_VS_GYM_LEADER` sempre;
  - ramo `default:` (allenatori generici) → TRAINER, con il risultato regionale
    di oggi (incluso il caso `MAPSEC_TRAINER_HILL`) come `regionalSong`;
  - ramo selvatici finale → WILD. Leggere anche le righe fra il ramo allenatori e
    quello selvatici (~8170-8189) per eventuali casi speciali da lasciare intatti.
  - Leggendari, link, Aqua/Magma, Rocket, rivali, campioni, Frontier Brain: invariati.

### 6. Musica notturna nei Centri (`src/overworld.c`)

In `GetLocationMusic()`, dopo i casi speciali: letta la musica della mappa, se è
`MUS_POKE_CENTER`, `MUS_RG_POKE_CENTER` o `MUS_HG_POKE_CENTER`,
`GetTimeOfDay() == TIME_NIGHT` e `IsNightCenterMusicEnabled()` →
`MUS_DP_POKE_CENTER_NIGHT`. Leggere `GetCurrLocationDefaultMusic()` e i suoi
chiamanti: se calcola la musica senza passare da `GetLocationMusic`, aggiungere
lo stesso controllo lì (estrarre un helper `static u16 ApplyNightCenterMusic(u16 music)`).
Come in SG la musica non cambia se scocca la notte mentre si è dentro.

### 7. Fast HMs (port di `a9d41a7b9`, adattato)

Leggere `git -C /home/user/soulgold show a9d41a7b9` intero e portarlo hunk per
hunk, controllando ogni etichetta e funzione **nella versione di Hasep**
(CLAUDE.md §6: confrontare le firme delle funzioni chiamate).

- `src/field_effect.c` + `include/field_effect.h`: `bool32 ShouldSkipHmAnimation(void)`
  (lista FLDEFF; verificare che ogni id esista in `include/constants/field_effects.h`),
  early-out in `FldEff_FieldMoveShowMon`, salto della posa in
  `SurfFieldEffect_FieldMovePose`/`SurfFieldEffect_ShowMon` e
  `RockClimb_FieldMovePose`/`RockClimb_ShowMon` (verificare che esistano con quei nomi).
- `src/fldeff_flash.c` (`FieldCallback_Flash`) e `src/fldeff_rocksmash.c`
  (`Task_DoFieldMove_Init`), con `EndORASDowsing()` (verificare il nome in `src/oras_dowse.c`).
- `data/scripts/field_move_scripts.inc` e `data/scripts/surf.inc`: i `goto_if_set
  FLAG_FAST_HMS, …` di SG. Per Strength SG salta a
  `EventScript_PushABoulderLikeBathanielNandy`: trovare l'etichetta equivalente di Hasep.
- **MN solo di HnS**: `grep -rn "Text_WantTo\|MSGBOX_YESNO" data/scripts/*.inc` e
  le mappe per mosse campo che SG non ha (es. Whirlpool, Headbutt): applicare lo
  stesso schema se ci sono, e annotarlo nel report.
- `upstream_status.py`: aggiungere a `PORTED` i file di SG portati
  (`src/field_effect.c`, `src/fldeff_flash.c`, `src/fldeff_rocksmash.c`,
  `data/scripts/field_move_scripts.inc`, `data/scripts/surf.inc`), così il prossimo
  sync ne legge i diff per intero.

### 8. Documentazione

- `docs/SOULGOLD_PORT_REPORT.md`: tre righe nel §1 (feature, opzione, file, note:
  temi diversi da SG con HG JOHTO e Superquattro; notte nei Centri come opzione),
  togliere le tre voci dai candidati del §5.
- `CLAUDE.md` §4: nuovo campo `soundAndFieldOptions` in coda a SaveBlock3,
  migrazione `saveVersion < 8`, flag specchio `FLAG_FAST_HMS`.

## File toccati

`include/global.h`, `include/save.h`, `src/save.c`, `src/new_game.c`,
`include/constants/flags.h`, `src/option_menu.c`, `include/option_menu.h`,
`include/constants/songs.h`, `sound/song_table.inc`, `sound/songs/midi/midi.cfg`,
4 × `sound/songs/midi/mus_dp_*.mid`, `include/pokemon.h`, `src/pokemon.c`,
`src/overworld.c`, `src/field_effect.c`, `include/field_effect.h`,
`src/fldeff_flash.c`, `src/fldeff_rocksmash.c`,
`data/scripts/field_move_scripts.inc`, `data/scripts/surf.inc`,
`docs/SOULGOLD_PORT_REPORT.md`, `CLAUDE.md`,
`.claude/skills/upstream-sync/upstream_status.py`.

## Verifica

1. `make hns -j$(nproc) -O` senza warning; misurare la ROM usata prima e dopo
   (`python3 -c "d=open('pokehns.gba','rb').read(); n=len(d.rstrip(b'\xff')); print(n, n*100/len(d))"`).
   Oggi 94,78%. Sopra il 98% fermarsi e chiedere.
2. `python3 /tmp/ppcheck.py` sui file con direttive toccate, se lo script c'è.
3. **mGBA obbligatorio** (tocca layout e migrazione del salvataggio, CLAUDE.md §3):
   - salvataggio dell'utente (`.srm` → `pokehns.sav`; se non c'è nella sessione,
     chiederlo): si carica, squadra intatta, Candy Jar con lo stesso saldo, le tre
     opzioni lette come FAST HMS OFF / BATTLE MUSIC DEFAULT / NIGHT CENTER ON;
   - menu opzioni: le tre voci si vedono, cambiano, sopravvivono a salva + reset;
   - Fast HMs ON: Taglio su un albero e Surf senza domanda né animazione; OFF come prima;
   - tema DPPT: una lotta selvatica (erba) e una con allenatore (debug: R+START →
     Trainers → Try Battle) → brani DP; DEFAULT → brani di oggi;
   - Centro Pokémon di notte (orologio/fake RTC o debug) → tema notturno DP;
     opzione OFF → tema di sempre;
   - nuova partita impostando le opzioni dal titolo: dopo l'intro sono ancora quelle.
4. Consegna: ROM 32 MB zippata (`pokemonHnS_sg12-options_<sha>.zip`), commit in
   inglese, PR draft con lista "cosa provare".

---

## Esito dell'implementazione (differenze dal piano)

- **§1 salvataggio**: `sizeof(struct SoundAndFieldOptions)` è 4 (ABI apcs-gnu),
  non 1; l'assert sulla size è sostituita da `sizeof(SaveBlock3) <= CHUNK`.
- **§1 `NewGameInitData`**: il piano dava per sicuro che non azzerasse il campo.
  Lo azzerava (`ClearSav3()`): ora salva e ripristina `soundAndFieldOptions`.
- **§3 menu**: il menu disegnava al massimo 4 scelte; aggiunto `DrawChoices_Many`
  per le 7 di `BATTLE MUSIC`.
- **§7 script**: la build `hns` compila `field_move_scripts_hns.inc`, non
  `field_move_scripts.inc` (che è il ramo non-HnS). Lì Taglio, Spaccaroccia,
  Forza, Cascata e Testata non hanno più il sì/no né l'effetto: l'opzione toglie
  il messaggio "usato". Le etichette di SG (`EventScript_PushABoulderLikeBathanielNandy`,
  `EventScript_MoveUpAWaterfall`, …) non esistono qui e sono state create.
  `field_move_scripts.inc` non è stato toccato.

---

## Prompt per la sessione Sonnet

```
Repo: Marcogn/pokehns-expansion-plus (e Marcogn/soulgold in sola lettura, in
/home/user/soulgold: clonalo se manca, serve per i .mid e per i diff di SG).

Parti dal branch origin/ccr-db5c795d-a6wzlo (contiene il piano e i fix del sync
del 12/10) e lavora sul branch che ti assegna la sessione.

Implementa il piano in docs/plans/sg-1.2-options.md, nell'ordine dei suoi
paragrafi 1-8. Prima di iniziare leggi CLAUDE.md per intero: le trappole dei §2,
§4 e §6 sono esattamente quelle in cui questo lavoro può cadere (layout del
salvataggio, polarità dei bit, firme delle funzioni portate da SG, #if IS_HNS).

Regole:
- non copiare valori di Soulgold (id di brani, flag, var): derivali da Hasep;
- non toccare struct ChallengeSettings: le opzioni nuove vanno nel campo in coda
  a SaveBlock3 descritto nel piano, con SAVE_VERSION 8 e la migrazione;
- ogni etichetta di script o funzione di SG che porti, verificala nella versione
  di Hasep prima di usarla; se qualcosa nel piano non torna con il codice, fermati
  e chiedi invece di improvvisare;
- build: make hns -j$(nproc) -O (non make modern). Riporta la % di ROM usata
  prima e dopo;
- verifica in mGBA come da sezione "Verifica" del piano (CLAUDE.md §3 spiega
  come pilotarlo). Se non hai il salvataggio dell'utente, chiedilo. Non chiamare
  "verificato" ciò che hai solo compilato;
- commit e commenti in inglese, conversazione in italiano;
- alla fine: push, PR draft verso master con la lista "cosa provare", ROM 32 MB
  zippata inviata all'utente, report e CLAUDE.md aggiornati come da §8 del piano.
```
