---
paths:
  - "data/scripts/**"
  - "data/maps/**"
  - "data/event_scripts.s"
  - "asm/macros/event.inc"
  - "src/shop.c"
  - "src/field_effect.c"
  - "src/fldeff_*.c"
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
