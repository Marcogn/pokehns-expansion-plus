# Report: cosa è arrivato da Soulgold, dove sta, cosa no e perché

Documento vivo. Sostituisce come riferimento
[`INVENTORY_SOULGOLD_VS_HNS_EXPANDED.md`](INVENTORY_SOULGOLD_VS_HNS_EXPANDED.md),
che resta com'era: è l'analisi fatta **prima** del port (HnS 2.0.5) e serve a
capire da dove si è partiti, non lo stato attuale.

I dettagli tecnici, le trappole e le misure stanno in `CLAUDE.md` (§5–§7bis);
qui c'è l'elenco, con il rimando al punto giusto.

---

## 0. Stato della sincronizzazione (28/09/2026)

| repo | allineato a | note |
| --- | --- | --- |
| `pokehns-expansion-plus` ← `PokemonHnS-Development/pokehns-expansion` | `167aa6d` (2.0.6) | 0 commit mancanti, verificato con `git rev-list --count HEAD..upstream/master` |
| `Marcogn/soulgold` ← `Eemeliri/soulgold` | `dcba9e032` (27/09) | merge pulito, `make modern` compila |
| codice di Soulgold portato in Hasep | Soulgold **1.1.4** (`a374881df`) + i fix elencati al §4 | `dexnav.c`, `candy_jar.c`, `swsh_party_menu.c`, `type_icons.c` e il codice degli speed-up in SG **non sono cambiati** dopo 1.1.4 |

Il branch `claude/mega-type-stones` ha 3 commit **non ancora in master**
(megaevoluzioni con le type stone di SG, due tasche nuove nella borsa). Vedi §5.

---

## 1. Portato da Soulgold

| feature | opzione | file principali | note |
| --- | --- | --- | --- |
| Speed-up overworld | `OW SPEED` 1×–4× | `src/overworld.c`, `src/battle_transition.c`, `include/overworld.h` | le cutscene restano a 1× |
| Speed-up lotta | `BATTLE SPEED` 1×–3× | `src/battle_main.c`, `src/battle_controllers.c`, `src/palette.c` | più tick software per frame |
| Menu squadra SwSh | `PARTY MENU` | `src/swsh_party_menu.c`, `src/party_menu_dispatch.c`, `include/party_menu_variant.h` | due varianti compilate insieme, CLAUDE.md §7 |
| Schermata borsa | — | `src/item_menu.c`, `src/item_menu_icons.c`, `graphics/bag/soulgold/` | |
| Tema scuro | `DARK UI` | `battle_bg.c`, `battle_interface.c`, `battle_gfx_sfx_util.c`, `item_menu.c` | mappa completa in CLAUDE.md §5 |
| Healthbox shiny | — | `src/battle_gfx_sfx_util.c` | in tema scuro diventa scura anche lei, diversamente da SG (voluto, §6) |
| Shiny Genome | — | `swsh_party_menu.c` (`Task_ShinGenome`), Mart di Viridian | |
| Start menu compatto | — | `src/menu.c`, `src/start_menu.c` | da SG 1.1.4 |
| Candy Jar | — | `src/candy_jar.c`, `src/item_use.c`, `src/save.c` (migrazione `saveVersion < 6`) | CLAUDE.md §4 e §6 |
| DexNav, metà "ricerca" | `ENHANCED DEXNAV` | `src/dexnav.c` | la metà "mostra tutti" **non** è di SG, §6 |
| Icone dei tipi, comportamento | — | `src/type_icons.c`, `include/type_icons.h` | l'arte è di SG; layout in singola diverso, §6 |
| Relearner solo dalla pagina mosse | — | `src/pokemon_summary_screen.c` | la feature è di expansion, la restrizione di SG |

## 2. Accesi e completati qui (non scritti qui, non di Soulgold)

DexNav (`DEXNAV_ENABLED` era FALSE), relearner dal riassunto
(`P_ENABLE_MOVE_RELEARNERS`, mancava la gestione di START), icone dei tipi
(`B_SHOW_TYPES` era NEVER), Pokédex HGSS. Dettagli in CLAUDE.md §6.

## 3. Aggiunto qui

Pokédex dal menu squadra, `EASY CATCH`, `NICKNAMES`, `WILD BATTLES`, toggle del
follower nel menu SwSh, badge `SEL`/`HM` leggibili nei due temi (difetto
presente anche in SG), prezzi e negozi (README).

---

## 4. Aggiornamenti di Soulgold dopo il port: cosa è stato applicato

Analizzati tutti i commit di SG da `768612c8e` (base dell'inventario) a
`dcba9e032`. Quasi tutto è contenuto di SG (squadre, learnset, abilità,
documentazione). Quello che riguarda anche Hasep:

| cambio in SG | in Hasep | perché |
| --- | --- | --- |
| Start menu con più di 8 voci, testo scuro nella borsa (1.1.4) | **già presente** | portato in una sessione precedente |
| Court Change scambia anche `numHazards` (1.1.4) | **applicato** | Hasep aveva lo stesso bug: le code degli hazard passavano di lato, i contatori no, e `AreAnyHazardsOnSide` legge i contatori. Anche expansion upstream ha il fix |
| `ConvertTimeToDateTime` con differenze negative (`a4f12d543`) | **applicato** | HnS azzerava i campi negativi: niente blocco, ma giorno (e giorno della settimana) potevano uscire sbagliati di uno. SG normalizza l'intera differenza |
| Uova shiny più probabili con genitori shiny (`93b51c041`, `e42b2b6f0`) | **applicato, adattato** | ×4 con un genitore shiny, ×8 con due, come SG. In Hasep la shininess è già decisa da `CreateBoxMon`, quindi è una seconda possibilità a probabilità maggiorata (`TryBoostEggShininessFromParents` in `daycare.c`) |
| Caramelle Esp. ordinate per taglia (`e42b2b6f0`) | **applicato** | `CompareItemsByType` in `item_menu.c` |
| `Cmd_switchoutabilities` (Natural Cure / Regenerator) | non applicato | il bug di SG nasce dal suo sistema a tratti multipli; in Hasep l'abilità è una sola e lo `switch` è esclusivo |
| `MonGainEVs` (moltiplicatori applicati più volte) | non applicato | il bug sta nel codice multi-strumento di SG, che Hasep non ha |
| Fix del "fantasma" dopo il Rocket Arcade | non applicato | in Hasep il Rocket Arcade non c'è |

---

## 5. Non portato, e perché

**Bloccato o rimandato per un motivo misurato**

- **Megaevoluzioni**: la ROM passa al 99,46% dei 32 MB (CLAUDE.md §7bis). Il
  lavoro sta su `claude/mega-type-stones`, non ancora unito.
- **Pokédex completo dalla pagina della cattura**: in lotta non c'è abbastanza
  memoria (misura in CLAUDE.md §7). Anche SG, su quella pagina, esce e basta.

**Candidati non ancora fatti**

- **Luci giorno/notte disattivabili** (`OW lighting`, `Battle lighting`, SG
  1.1.4): due opzioni che spengono la fusione dei colori per l'ora del giorno.
  In SG costano due flag. Da valutare: in Hasep le opzioni vivono in
  `ChallengeSettings`, e resta un bit libero solo (§4 di CLAUDE.md), quindi
  la strada più economica sono i flag come in SG.

**Contenuto e design di Soulgold, legati alle sue mappe e al suo bilanciamento**

Achievement, Hidden Grotto, Title Defense, lotte boss, Battle Arcade / Rocket
Arcade, minigiochi (blackjack, gacha, pachinko, pinball, derby, snake, flappy
bird, block stacker, Voltorb Flip roguelike), puzzle delle Rovine d'Alfa,
level scaling, mosse degli allenatori, sistema di tratti/abilità innate, squadre,
learnset, BGM delle strutture lotta, menu Docs in gioco (è la documentazione
di SG). Non sono feature di interfaccia: portarli vuol dire importare il
gioco di un altro.

**HnS ha già un equivalente proprio**

Pokégear (HnS usa `pokenav_radio.c`), Voltorb Flip (`voltorb_flip.c`).

**Non ancora valutati** (esistono solo in SG, nessuna analisi fatta)

`hgss_party_menu.c`, `ui_birch_case.c`, `field_mugshot.c`, `path_finding.c`,
`emulator_check.c`, `replay_options.c`, la ripresa della musica dopo la lotta,
l'unione dei messaggi di aumento/calo statistiche.

---

## 6. Divergenze volute rispetto a Soulgold

- **Tema scuro e healthbox shiny**: qui diventano scure tutte le box insieme,
  SG tiene chiara quella shiny riscrivendo i pixel di ogni sprite (§5).
- **DexNav**: la divergenza va nei due sensi. Hasep ha la riga dei nascosti, il
  Pokéblock della Safari, il movimento in acqua/grotta, che SG non ha; SG ha la
  ricerca che non fallisce, che qui sta dietro `ENHANCED DEXNAV` (§6).
- **Icone dei tipi in singola**: a sinistra della healthbox e non specchiate;
  in doppia come SG (§6).
- **Evoluzioni regionali**: restano legate alla regione. Raichu/Exeggutor/Marowak
  di Alola solo alle Isole Alola, le forme di Hisui solo a Sinjoh
  (`GetRegionForSectionId` in `include/regions.h`); le forme di Galar non si
  ottengono evolvendo, ma dall'NPC nella casa di Bill (`Route25_BillsHouse_hns`,
  `ConvertToRegionalForm`). Scelta dell'utente, 28/09/2026.
