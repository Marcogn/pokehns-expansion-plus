# Inventario: HnS expanded (2.0.5) vs Soulgold

Scopo: capire **prima di portare qualunque cosa** su che base sta il nuovo HnS,
quanto è vicino a Soulgold, cosa ha già, e quanto costa ciascuna feature mancante.

Tutto quello che segue è stato letto dal sorgente nei tre working tree, non da
screenshot. Ogni affermazione ha il file e la riga.

Repo confrontati:
- `pokehns-expansion-plus` @ `e4b2efcc` ("Fix synchronize again", 2.0.5) — **target**
- `soulgold` @ `768612c8e` — sorgente di riferimento, sola lettura

---

## 1. Su che base sta il nuovo HnS, e quanto è vicino a Soulgold

### 1.1 Stessa versione di expansion

| | HnS expanded | Soulgold |
|---|---|---|
| `include/constants/expansion.h` | **1.15.2** | **1.15.2** |
| Compressione gfx | LZ77 + `smol` (`.4bpp.smol`, `.bin.smolTM`) | uguale |
| `COMPOUND_STRING` | disponibile (usato in `src/option_menu.c:454`) | disponibile |
| API battler | `enum BattlerId battler` per parametro | uguale |
| Dati mosse/oggetti | `gMovesInfo`, `gItemsInfo` | uguale |

**La premessa da verificare era giusta.** Il muro della sessione precedente —
adattare all'indietro codice expansion su Modern Emerald/Emerald vanilla — non
esiste più. I due `CreateSpriteAt` sono lo stesso codice, `.anims`/`.affineAnims`/
`.callback` NULL hanno il fallback in entrambi.

### 1.2 Ma la parentela è più stretta di così

`soulgold/README.md:28-30` accredita:

> - [HnS Dev Team](https://github.com/PokemonHnS-Development/pokemonHnS) for their amazing work!
> - [smithk200](https://github.com/smithk200/Gold-And-Silver-Gen-3-Decomp) for porting most of HnS to expansion!

Soulgold **è** un discendente di HnS portato su expansion. Non sono due progetti
paralleli: sono due rami dello stesso albero che si sono separati a monte della
2.0 ufficiale.

Misura diretta (`git ls-files` dei due tree, confronto byte a byte):

| area | file comuni | identici | diversi | solo SG | solo HnS |
|---|---|---|---|---|---|
| `src/` | 544 | 197 | 347 | 42 | 38 |
| `include/` | 436 | 237 | 199 | 41 | 30 |
| `graphics/` | 18704 | 16691 | 2013 | 4452 | 1564 |
| `data/` | 5054 | 3131 | 1923 | 5329 | 5858 |
| `test/` | 1007 | 509 | 498 | 88 | 0 |
| **totale** | **28827** | **23802 (82,6 %)** | 5025 | 16999 | 7912 |

### 1.3 Dove invece hanno preso strade diverse

Il nuovo HnS ha tenuto l'impalcatura **Modern Emerald**, Soulgold no:

- `include/constants/global.h:65-81` — HnS è un codebase **multi-gioco**:
  `FIRERED` / `LEAFGREEN` / `POKEMON_HNS` / Emerald, con le macro `IS_FRLG` e
  `IS_HNS`. `IS_HNS` compare in **561 punti** del sorgente. Soulgold non ha
  niente di tutto questo.
- `include/global.h:253-301` — `struct ChallengeSettings` dentro `SaveBlock3`:
  opzioni "Options Plus" + randomizer/challenge di `tx_randomizer_and_challenges`.
  Soulgold non ha `ChallengeSettings`.
- Le opzioni stanno in **posti diversi**:
  - HnS → `gSaveBlock3Ptr->challengeSettings.<campo>` (bitfield)
  - SG → `gSaveBlock2Ptr->options*` (bitfield, `include/global.h:625-647`) e
    alcune in `VAR_*` (`VAR_BATTLE_SPEED`, `VAR_OVERWORLD_SPEEDUP`)
- Il target di build è `make hns` (`Makefile:22`), non `make modern`.

**Conseguenza pratica:** il codice *motore* si copia quasi com'è; il codice
*di opzione* va riscritto nell'architettura di HnS. Che è la parte facile —
vedi 4.6.

---

## 2. Cosa il nuovo HnS ha già

| feature | presente? | dove |
|---|---|---|
| Menù squadra SwSh | **No** | nessun `swsh_party_menu.c`; `grep -ril swsh` trova solo 3 file non correlati |
| Tema scuro | **No** | nessuna occorrenza di `optionsDarkBattleUi` / `DARK_UI` / `IsDarkUiEnabled` |
| Borsa stile Soulgold | **No** (ma vedi 3.2) | `src/graphics.c:1822-1840` |
| Speed-up overworld | **No** | nessun `OverworldSpeedup*`; `CB2_Overworld` (`src/overworld.c:1922`) è vanilla |
| Speed-up battaglia 1×/2×/3× | **No**, ma c'è un `FAST BATTLES` on/off | vedi 2.1 |
| Pokédex dal menù squadra | **No** | nessun `OpenPokedexPlusHGSSAtSpecies`; nessun `Pokedex` in `src/party_menu.c` |

Verificato anche **a monte**: `PokemonHnS-Development/pokehns-expansion@master`
non ha `src/swsh_party_menu.c` né `src/party_menu_dispatch.c` (HTTP 404), e il
suo `include/global.h` ha gli stessi `fastBattle:1` / `newBattleUI:1` del fork.
Il fork non è indietro rispetto all'upstream su queste cose.

Verificato anche **contro expansion 1.15.2 vanilla**
(`raw.githubusercontent.com/rh-hideout/pokeemerald-expansion/expansion/1.15.2`):
`B_BATTLE_SPEED` non esiste (c'è solo `B_WAIT_TIME_MULTIPLIER`, riga 326),
`include/overworld.h` non ha nessun `OVERWORLD_SPEED*`, `src/swsh_party_menu.c`
è 404, `include/constants/party_menu.h` non ha `SWSH`/`STYLE`.
**Le quattro feature sono tutte roba custom di Soulgold, non roba di expansion
che HnS ha semplicemente disattivato.**

### 2.1 `FAST BATTLES` di HnS non è lo speed-up di Soulgold

`gSaveBlock3Ptr->challengeSettings.fastBattle` (`include/global.h:264`) ha
esattamente **due** consumatori:

- `src/battle_script_commands.c:2174` (`Cmd_printstring`-like, attesa messaggio)
- `src/battle_script_commands.c:4945` (`Cmd_pause`)

Entrambi azzerano `gPauseCounterBattle`: salta le pause degli script di
battaglia. **Non tocca il frame rate**, non accelera le animazioni, non ha
livelli. Non si sovrappone a quello di Soulgold, che è un'altra cosa (3.3);
al limite le due opzioni convivono.

---

## 3. Quanta grafica propria di HnS è rimasta

Molta, ed è **il vincolo principale per il tema scuro**.

### 3.1 HnS ha due skin di UI di battaglia, selezionate a runtime

`src/battle_gfx_sfx_util.c:35-38`:

```c
bool8 UseGen4BattleUI(void)
{
    return gSaveblock3.challengeSettings.newBattleUI;
}
```

e da lì `GetSinglesPlayerHealthbox()`, `GetSinglesOpponentHealthbox()`,
`GetDoublesPlayerHealthbox()`, `GetSinglesPlayerHealthboxFrontier()`… scelgono
fra `graphics/battle_interface/hns/` (Gen 3) e `graphics/battle_interface/gen4/`.
Ci sono **59 asset in `hns/` + `gen4/` che in Soulgold non esistono**.

Cioè: **la lezione "se ci sono più skin vanno testate tutte" vale ancora, e
adesso è strutturale, non un dettaglio.** Ogni palette del tema scuro va
derivata due volte.

### 3.2 Le palette non sono trasferibili — numeri

Palette PNG lette dal chunk `PLTE` (entry 0-8, le uniche che il tema scuro tocca):

| # | SG `healthbox_singles_player.png` | HnS `hns/` (Gen 3) | HnS `gen4/` |
|---|---|---|---|
| 0 | 0,0,0 | 0,0,0 | 0,0,0 |
| 1 | 65,65,65 | 65,65,65 | 65,65,65 |
| 2 | 227,227,227 | **255,255,222** | 227,227,227 |
| 3 | 186,186,186 | **222,213,180** | 186,186,186 |
| 4 | 154,154,154 | **197,189,115** | 154,154,154 |
| 5 | 114,108,79 | **123,148,131** | 123,148,131 |
| 6 | 251,251,251 | **82,106,98** | 251,251,251 |
| 7 | 237,202,18 | **32,57,0** | 113,113,113 |
| 8 | 217,183,0 | **57,82,65** | 48,97,219 |

`graphics/battle_interface/dark_healthbox.pal` di Soulgold scurisce **solo** le
entry 1-4 (65→12, 227→43, 186→35, 154→29) e lascia intatte le 5-15.

Su `hns/` quel file rovinerebbe lo schermo: nella skin Gen 3 la cornice è fatta
anche dalle entry 5-8 (i verdi), che resterebbero chiare, mentre le 2-4 (crema)
verrebbero mappate su grigi. Su `gen4/` andrebbe meglio per le 2-4 ma le 7-8
(grigio e blu) resterebbero fuori.

**Conferma numerica della lezione della sessione scorsa: la palette scura va
ri-derivata, per skin, schermata per schermata.** Non è un file da copiare.

### 3.3 Borsa: schermata identica, asset diversi — di nuovo

`sDefaultBagWindows[]` in `src/item_menu.c` è **identico byte per byte** fra i
due repo (tutte e 6 le window, stessi `tilemapLeft/Top/width/height/paletteNum/
baseBlock`, stesso `DUMMY_WIN_TEMPLATE`). Esattamente come sul vecchio HnS.

Le differenze reali sono:
- HnS usa la propria skin: `graphics/bag/hns/{bag_male,bag_female,bag,menu_male,
  menu_female}` sotto `#if IS_HNS` (`src/graphics.c:1821-1834`)
- HnS sceglie la tilemap in base alle tasche: `menu_6.bin` / `menu_8.bin`
  a seconda di `I_COMBINE_BAG_POCKETS` (`src/graphics.c:1836-1840`)
- SG ha in più: `menu_male_dark.pal`, `menu_female_dark.pal`, `scrolling_bg.bin`,
  `key_item_box.png`, i quattro `select_button_{left,right,up,down}`
- SG aggiunge **un solo BG layer** per il campo stellato:
  `{ .bg = 3, .charBaseIndex = 3, .mapBaseIndex = 28, .priority = 3 }` in
  `sBgTemplates_ItemMenu`, caricato a `src/item_menu.c:1143`

### 3.4 Menù squadra

HnS ha il proprio sfondo (`graphics/party_menu/hns/bg.{png,bin}`, sotto
`#if IS_HNS`, `src/graphics.c:1782-1790`). SG ha tre set (`swsh/`, `hgss/`,
`bw/`) e **non ha più una variante Emerald classica**.

---

## 4. Costo per feature mancante

Ordine dal più economico al più caro.

### 4.1 Pokédex dalla pagina del Pokémon selezionato — quasi copia-incolla

Serve: `OpenPokedexPlusHGSSAtSpecies()` (`soulgold/src/pokedex_plus_hgss.c:2065`,
14 righe) + 3 funzioni in `party_menu.c` (`CursorCb_Pokedex`,
`CB2_OpenPartyPokedex`, `CB2_ReturnToPartyMenuFromPokedex`, righe 3453-3470) +
la voce di menù e il gate `GetSetPokedexFlag(..., FLAG_GET_SEEN)` (riga 2934).

I due `pokedex_plus_hgss.c` divergono di 975 righe su ~9100 (≈89 % identici) e
le funzioni toccate non sono fra quelle che divergono.

**Stima: ~60 righe. Basso rischio.**

### 4.2 Speed-up overworld — quasi copia-incolla

- `OverworldSpeedup_AdditionalIterations()` — `soulgold/src/overworld.c:1801-1821`, 21 righe
- 5 costanti in `include/overworld.h:31-39`
- `CB2_Overworld` — 8 righe in più dentro una funzione che in HnS è **vanilla,
  contesto identico** (`pokehns-expansion-plus/src/overworld.c:1922-1933`)
- una riga in `battle_transition.c:303`
- il gancio `FLAG_PREVENT_OVERWORLD_SPEEDUP` e `FLAG_DEXNAV_SEARCHING` non
  esistono in HnS: o si aggiungono o si tolgono dalla condizione

Il tenere premuto R per tornare a 1× è dentro quella funzione.

**Stima: ~60 righe + l'opzione. Basso rischio.**

### 4.3 Borsa di Soulgold — medio, tutto sulla grafica

Codice: il BG layer in più, il caricamento della tilemap scorrevole, l'update
dello scroll. Poche decine di righe (`item_menu.c` diverge di 1378 righe in
totale, ma quasi tutte per altre ragioni — tasche, key item box, register a 4
direzioni).

Il costo vero è decidere **se si vuole la grafica di Soulgold o quella di HnS**:
se si tiene `bag/hns/`, il campo stellato va ri-tarato sulla palette di HnS
(stesso problema di 3.2, in piccolo).

**Stima: ~150 righe di codice + lavoro grafico da quantificare a parte.**

### 4.4 Speed-up battaglia 1×/2×/3× — medio, ma autocontenuto

È una riscrittura di `BattleMainCB2` che esegue N tick logici di battaglia per
frame (`soulgold/src/battle_main.c:1779-1880`):

- `BattleMainCB2` riscritta (~40 righe)
- `RunBattleSoftwareTick()`, `AdvanceBattleFrameRng()`, `CanRunExtraBattleTick()`
  (~70 righe, statiche, tutte nuove)
- `Rogue_GetBattleSpeedScale()` — `soulgold/src/battle_controllers.c:3028-3075`
  (~48 righe). Il nome tradisce l'origine: viene da pokeemerald-rogue.
- `InBattleChoosingMoves()` / `InBattleRunningActions()` — `battle_main.c:3136-3145`
- `gBattleStruct->hasBattleInputStarted:1` — `include/battle.h:618`
- `IsPaletteFadeTransferPending()` in `palette.c` (HnS non ce l'ha)

In HnS `BattleMainCB2` è vanilla (`src/battle_main.c:1864-1880`): il punto di
innesto è pulito. Il tenere premuto L per 1× è dentro `Rogue_GetBattleSpeedScale`.

Il rischio non è la compilazione, è il comportamento: RNG, fade di palette,
animazione di cattura e battaglie link sono già gestiti con i guard di
`CanRunExtraBattleTick()`, ma vanno ritestati su HnS.

**Stima: ~200 righe. Rischio medio, va testato in gioco.**

### 4.5 Tema scuro — il più caro, e non per il codice

96 hunk su 10 file in Soulgold:

| file | occorrenze |
|---|---|
| `src/item_menu.c` | 28 |
| `src/battle_interface.c` | 17 |
| `src/option_menu.c` | 15 |
| `src/battle_bg.c` | 14 |
| `src/battle_script_commands.c` | 9 |
| `src/battle_message.c` | 5 |
| `src/battle_controller_player.c` | 4 |
| `src/battle_gfx_sfx_util.c` | 2 |
| `src/palette.c`, `src/new_game.c` | 1 + 1 |

più ~12 tabelle di palette hardcodate (`sDarkBagStandardMenuPalette`,
`sDarkBattleCommandPalette`, `sDarkHealthBoxTextColor`, `sDarkBattleTextColor`,
`sDarkBattleUiBgColor`, `sDarkIndicatorGfx`, `sDarkDownArrowTiles`…).

Il codice si innesta senza problemi. **Ma ognuna di quelle palette è tarata
sulla grafica di Soulgold, e in HnS ci sono due skin di battaglia più una skin
di borsa propria: vanno ri-derivate tutte, e testate su entrambe le skin.**
`battle_interface.c` fra i due repo diverge già di 1302 righe per conto suo,
quindi i punti di innesto vanno ritrovati a mano.

**Stima: codice modesto, lavoro di palette alto. È la feature che va fatta per
ultima e con un metodo (il trucco dei marker `RGB(i, 0, 31-i)` su
`gPlttBufferFaded`+`gPlttBufferUnfaded`, questa volta ripetuto per skin).**

### 4.6 Menù squadra SwSh — il più grosso, ma meno terribile del previsto

Volume attuale in Soulgold (più grande di quello portato l'altra volta, perché
nel frattempo SG ha aggiunto BW e HGSS):

| file | righe |
|---|---|
| `src/swsh_party_menu.c` | 11816 |
| `src/data/swsh_party_menu.h` | 1821 |
| `src/party_menu_dispatch.c` | 183 |
| `include/party_menu_variant.h` | ~140 |
| `src/hgss_party_menu.c` | 12 |

Il meccanismo è pulito: `party_menu_variant.h` rinomina ~120 funzioni pubbliche
con un prefisso per variante, ogni variante compila la propria copia, e
`party_menu_dispatch.c` espone l'API pubblica e smista a runtime su
`gSaveBlock1Ptr->optionsPartyMenuStyle`. In `party_menu.c` servono solo **14**
condizionali.

**Superficie di dipendenza, misurata:** dei 3321 identificatori usati da
`swsh_party_menu.c` + il suo header dati, **93 su 93 `#include` esistono già in
HnS** tranne due, che sono file nuovi di SG stesso (`party_menu_variant.h`,
`data/swsh_party_menu.h`).

I simboli che HnS davvero non ha, con il numero di punti di chiamata dentro il
menù SwSh:

| simbolo | chiamate | cos'è |
|---|---|---|
| `PARTY_ACTION_MOVE_ITEM` | 10 | oggetti tenuti multipli (`MAX_MON_ITEMS`) |
| `TryMultichoiceFormChange` | 9 | cambi forma di SG |
| `PARTY_ACTION_FUSION` | 7 | fusioni |
| `RemoveHeldItemFromBag` / `AddHeldItemToBag` | 6 + 5 | oggetti multipli |
| `FormChangeTeachMove` | 5 | |
| `CanItemBeTossed`, `TryItemUseFusionChange` | 3 + 3 | |
| `Task_ShinGenome`, `MAX_MON_ITEMS` | 2 + 2 | oggetti custom SG |
| `ChangeRotomForm`, `CanChangeMonPokeball`, `TryChangeMonPokeball`, `ItemUseCB_ChangePokeball`, `GetCurrentExpCapType`, `SetUpFieldMove_Whirlpool`, `CB2_ShowPokemonPCFromParty`, `PokemonPC_SetReturnToPartyCallback`, `CreateMonIcon2`, `PlayerHasMove`, `IsItemInfiniteHold`, `GetSurfablePokemonPartySlot`, `RefreshFollowingPokemon`, `TryDecrementMonLevel`, `ItemUseCB_ShinGenome`, `OpenPokedexPlusHGSSAtSpecies`, `PARTY_MSG_SEND_MON_TO_BOX` | 1 ciascuno | |

Totale ≈ **70 punti di chiamata su 13.600 righe (0,5 %)**. Sono quasi tutti
feature di gameplay di Soulgold che HnS non ha (oggetti tenuti multipli,
fusioni, cambio Poké Ball, exp cap, candy inversa), non incompatibilità di
motore. Si stubbano o si tagliano.

Altri (`Task_AbilityCapsule`, `Task_AbilityPatch`, `Task_Mint`,
`Task_DynamaxCandy`, `DeleteMove`, `DoesMonHaveAnyMoves`, `TryItemHoldFormChange`,
`TryItemUseFormChange`, `GetFormChangeTargetSpecies`) **esistono in HnS** ma
sono `static` in `party_menu.c`: il meccanismo di variante li esporta già.

**Da decidere:** SG non ha più una variante "Emerald classica". Per HnS servirà
una variante classica che usi `graphics/party_menu/hns/bg.png`, cioè compilare
il `party_menu.c` di HnS due volte con le macro di variante. È meccanico, ma
è lavoro in più rispetto a un semplice copia-incolla.

**Stima: ~14.000 righe da trasferire, di cui ~99,5 % senza modifiche, più un
lavoro di variante da progettare. Resta la feature più grossa, ma il grosso è
volume, non attrito.**

### 4.7 Le opzioni: non portare `option_menu.c` di Soulgold

`src/option_menu.c` è l'unico file dove i due repo **divergono del tutto**:
2747 righe di diff su 2018 (SG) + 1231 (HnS). Sono due schermate diverse.

Quella di HnS è molto migliore per i nostri scopi: 3 schede
(`TAB_MAIN`/`TAB_BATTLE`/`TAB_SOUND`), lista scrollabile, e ogni voce è un
descrittore dichiarativo (`src/option_menu.c:453-497`):

```c
[ITEM_BATTLE_NEW_BATTLEUI] = {
    .name         = COMPOUND_STRING("BATTLE UI"),
    .descriptions = sDesc_NewBattleUI,
    .numChoices   = 2,
    .choiceNames  = sChoices_Gen3Gen4,
},
```

più una riga in salvataggio (`:1089`) e una in caricamento (`:1197`).

Quella di SG è nello stile imperativo vecchio, con `X_ProcessInput()` e
`X_DrawChoices()` per ogni opzione: ~80 righe a voce.

**Aggiungere un'opzione in HnS costa ~6 righe contro ~80.** Questa parte va
scritta nell'architettura di HnS, non portata. È un ribaltamento rispetto alla
sessione precedente.

**Attenzione al salvataggio:** le opzioni di HnS stanno in bitfield dentro
`struct ChallengeSettings` (`include/global.h:253-301`), che sta dentro
`SaveBlock3` seguito da `registeredItemHold`. Aggiungere bit in mezzo sposta
tutto quello che segue e rompe i salvataggi esistenti. Vanno accodati in fondo
alla struct, e **va verificato empiricamente quanti bit liberi restano
nell'ultimo byte** — i commenti di offset non sono affidabili (lezione già
pagata: nel vecchio HnS `vars[]` stava a +0x1490, non a 0x139C).

---

## 5. Note operative

- **Toolchain assente in questa sessione.** Non c'è `/opt/devkitpro`, `DEVKITARM`
  è vuoto, niente `arm-none-eabi-gcc`. Va installato prima di qualunque build.
- Il target è `make hns`, non `make modern`. Il `CLAUDE.md` in
  `pokemonHnSEnhanced` si riferisce al repo vecchio.
- HnS non ha test propri (`test/` solo-HnS: 0 file); SG ne ha 88, fra cui
  `test/new_game.c` che verifica i default di `optionsBattleSpeed` e
  `VAR_OVERWORLD_SPEEDUP`. Portando le feature conviene portare anche quelli.
- Il `.srm` di Heart and Soul 2.0.4 dovrebbe caricare nativo su una build di
  questo repo. Da confermare prima di usarlo come base di test.

---

## 6. Ordine proposto (da confermare)

1. Speed-up overworld (4.2) — piccolo, isolato, verifica subito il giro
   opzione → `ChallengeSettings` → effetto
2. Pokédex dal menù squadra (4.1) — piccolo
3. Speed-up battaglia (4.4) — medio, autocontenuto
4. Menù squadra SwSh (4.6) — grosso ma a basso attrito
5. Borsa (4.3) — dipende da una decisione grafica
6. Tema scuro (4.5) — per ultimo, dopo che le altre schermate sono ferme
