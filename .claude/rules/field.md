---
paths:
  - "data/scripts/**"
  - "data/maps/**"
  - "data/event_scripts.s"
  - "asm/macros/event.inc"
  - "src/shop.c"
  - "src/field_effect.c"
  - "src/fldeff_*.c"
  - "src/overworld.c"
---

# Script di campo, MN, negozi, nickname

- **Script delle MN: la build `hns` usa `data/scripts/field_move_scripts_hns.inc`**,
  non `field_move_scripts.inc` (`data/event_scripts.s`, `#if IS_HNS`). Il piano
  del port di Fast HMs citava il secondo e dava per uguali le etichette di SG:
  non lo sono. Nel file `_hns` Taglio, Spaccaroccia, Forza, Cascata e Testata
  hanno **già** il sì/no e il `dofieldeffect` commentati, e Strength mostra il
  messaggio dentro `EventScript_ActivateStrength`. Prima di toccare uno script di
  MN controlla quale dei due file è compilato (`nm pokehns.elf | grep
  EventScript_CutTree` e confronta con le etichette).

- **Negozi**: tutti i Poké Mart normali usano `pokemart 0`, cioè un inventario unico
  scalato sui medaglieri (`sShopInventories` in `src/shop.c`). Per vendere qualcosa
  in una sola città si **appendono** extra a quella lista (`sMartExtras`), non si dà
  una lista fissa alla città. Nessun tetto al numero di oggetti: `MAX_ITEMS_SHOWN 8`
  è solo quante righe si vedono.
- **Nickname**: la domanda dei regali sta nei singoli script di mappa. È dietro la
  macro `asknickname` (`asm/macros/event.inc`) che, con l'opzione spenta, non mostra
  niente e lascia `VAR_RESULT` a NO. I due prompt del *valutatore di nickname* usano
  un testo proprio e non vanno toccati.

- **Mente**: tutte e 21 già in vendita al negozio di fiori di Goldenrod, dietro
  medaglia 3 e dietro il toggle `MODE_MINTS` del challenge menu.

## Descrizione dell'oggetto ottenuto (`src/overworld.c`, piano `small-features-1` fase 2)

`OW_SHOW_ITEM_DESCRIPTIONS` vale `OW_ITEM_DESCRIPTIONS_ALWAYS`: nessun campo di
salvataggio (`FIRST_TIME` vorrebbe 113 byte, vedi il piano).

- Comandi `showitemdescription` / `showberrydescription` / `hideitemdescription`:
  il riquadro è la finestra (1,1) 28x3 tile, l'icona è uno sprite a (26,24).
  `EventScript_ObtainedItem`, `EventScript_PickUpItem` e
  `EventScript_PutHiddenItemInPocket` (`obtain_item.inc`), più alberi di bacche
  e di Ghicocche (`headerType` 1, che scrive in `gStringVar3`).
- **Alberi di bacche in HnS**: si compila il ramo `#if IS_HNS` di
  `berry_tree.inc` (`BerryTree_EventScript_Hns_PickBerry`, raccoglie e
  ripianta), non quello vanilla. Il riquadro va mostrato **prima** di
  `ObjectEventInteractionPlantBerryTree`, che richiama `GetBerryTreeData` e
  rimette in `VAR_0x8006` i dati del nuovo albero. L'id dell'oggetto lo scrive
  `ObjectEventInteractionPickBerryTree` (ramo HnS): `GetBerryTreeData` ci
  lascia il **numero** di bacche, che `ScriptShowItemDescription` leggerebbe
  come id. Nessuna mappa HnS ha alberi di Ghicocche.
- A capo **per pixel**: `StringCopy` + `StripLineBreaks` +
  `BreakStringAutomatic(196, 2, FONT_SMALL, HIDE_SCROLL_PROMPT)`; il vecchio
  conteggio di 32 caratteri non limitava le righe. Una riga sola va a y=4.
- La palette dell'icona ha il tag suo `ITEM_PALETTE_TAG` (`ITEM_TAG |
  BLEND_IMMUNE_FLAG`); `DestroyItemIconSprite` libera tile e palette con i
  rispettivi tag. Tile e palette con tag diversi: se se ne cambia uno, va
  cambiato anche nel `Free*ByTag`.
- Con grotta buia o Piramide (`GetFlashLevel() > 0 || InBattlePyramid_()`)
  l'icona si crea due volte (la seconda come finestra OBJ).
- Un oggetto aggiunto a mano in uno script deve chiamare `hideitemdescription`
  dopo ogni `showitemdescription`, altrimenti il riquadro resta.

## Testata: aspettare la lotta prima di `releaseall` (26/10)

- In `field_move_scripts_hns.inc` la Testata faceva `special
  RockSmashWildEncounter` e subito `goto EventScript_EndHeadbutt`
  (`releaseall`): il `waitstate` dopo era irraggiungibile (import iniziale di
  HnS, `2e22dff0`). Ora `goto_if_eq VAR_RESULT, FALSE, ...` e `waitstate`,
  come Spaccaroccia.
- **Perché conta**: lo spawn degli OWE genera il Pokémon **dentro
  `gEnemyParty`** e poi lo azzera (`SetSpeciesInfoForOWE`,
  `wild_encounter_ow.c`), e gira solo a controlli sbloccati. Uno script che
  avvia una lotta e fa `releaseall` prima di `waitstate` lascia 89
  fotogrammi di transizione a controlli sbloccati: se in quel momento nasce
  un OWE, la lotta parte con l'avversario azzerato ("wild !", Lv0, sviene
  subito). **Regola**: ogni script che crea un selvatico e poi avvia la
  lotta deve tenere i controlli bloccati fino a `waitstate`/`dowildbattle`.
- Misura (ROM strumentate, salvataggio dell'utente, Route 43, Testata forzata
  al 100% e spawn OWE forzato): script vecchio, Testata = Pineco, lotta =
  specie 0, 89/89 fotogrammi sbloccati; script nuovo, Exeggcute = Exeggcute,
  0/89; lotta da contatto con un OWE (controllo) 0/89.
- Gli "alberelli con i puntini rossi" di Route 43 sono alberi da Testata, non
  alberi di bacche.
