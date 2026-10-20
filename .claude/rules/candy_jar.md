---
paths:
  - "src/candy_jar.c"
  - "include/candy_jar.h"
  - "src/item_use.c"
  - "include/constants/items.h"
  - "src/data/items.h"
  - "data/maps/VioletCity_PokemonCenter_hns/**"
---

# Candy Jar

- **Candy Jar**: portato da Soulgold così com'è (`src/candy_jar.c`,
  `include/candy_jar.h`, `ItemUseOutOfBattle_CandyJar` e i tre helper in
  `src/item_use.c`, l'aggancio in `src/battle_script_commands.c`). Tre punti
  dove HnS ha costretto a divergere:
  - **l'enum degli oggetti di HnS è implicito**, quello di Soulgold è numerato
    a mano (`ITEM_CANDY_JAR = 887`). Qui una voce inserita a metà lista sposta
    l'id di ogni oggetto successivo e riscrive la borsa di ogni salvataggio:
    `ITEM_CANDY_JAR` va **in fondo**, subito prima di `ITEMS_COUNT`;
  - il campo in SaveBlock3 e la migrazione, vedi `.claude/rules/save.md`;
  - lo script: SG chiama `VioletPCGiveExpJar` subito dopo l'uovo, qui la riga
    va accanto al `giveitem ITEM_EXP_SHARE_SMALL` che già c'è in
    `data/maps/VioletCity_PokemonCenter_hns/scripts.inc`.
  Le caramelle finiscono in `POCKET_MEDICINE`, non fra gli oggetti: per questo
  `CloseCandyJarMessage` ricostruisce la lista di `GetItemPocket(ITEM_EXP_CANDY_XS)`
  e non quella a schermo.
  Verificato in emulatore sul salvataggio 2.0.6 dell'utente: barattolo in borsa
  grazie alla migrazione, saldo a 0, uno Zubat Lv17 ha versato **107** punti
  (833/7 = 119, meno il 10%), il barattolo ha prodotto 1 EXP. CANDY XS e ne sono
  restati 7. L'art è quella del Powder Jar, come in SG.
