# Report: cosa è arrivato da Soulgold, dove sta, cosa no e perché

Documento vivo. Sostituisce come riferimento
[`INVENTORY_SOULGOLD_VS_HNS_EXPANDED.md`](INVENTORY_SOULGOLD_VS_HNS_EXPANDED.md),
che resta com'era: è l'analisi fatta **prima** del port (HnS 2.0.5) e serve a
capire da dove si è partiti, non lo stato attuale.

I dettagli tecnici, le trappole e le misure stanno in `.claude/rules/` (un file per area, indice in `CLAUDE.md` §7);
qui c'è l'elenco, con il rimando al punto giusto.

---

## 0. Stato della sincronizzazione (20/10/2026)

| repo | allineato a | note |
| --- | --- | --- |
| `pokehns-expansion-plus` ← `PokemonHnS-Development/pokehns-expansion` | `167aa6d` (2.0.6) | 0 commit mancanti anche al 20/10, verificato con `git rev-list --count HEAD..upstream/master` |
| `Marcogn/soulgold` ← `Eemeliri/soulgold` | `4d4acbe98` (20/10) | merge pulito di 18 commit, `make modern` compila |
| codice di Soulgold portato in Hasep | Soulgold **1.1.4** (`a374881df`) + i fix elencati al §4 + le tre feature di SG 1.2 (Fast HMs, tema musica di lotta, musica notturna nei Centri, §1), valutato fino a `4d4acbe98` | dopo 1.1.4 in SG è cambiato solo `swsh_party_menu.c` (selezione squadra per le strutture lotta, fix applicato, §4); `dexnav.c`, `candy_jar.c`, `type_icons.c` e gli speed-up no |

Il branch `claude/mega-type-stones` ha 3 commit **non ancora in master**
(megaevoluzioni con le type stone di SG, due tasche nuove nella borsa). Vedi §5.

---

## 1. Portato da Soulgold

| feature | opzione | file principali | note |
| --- | --- | --- | --- |
| Speed-up overworld | `OW SPEED` 1×–4× | `src/overworld.c`, `src/battle_transition.c`, `include/overworld.h` | le cutscene restano a 1× |
| Speed-up lotta | `BATTLE SPEED` 1×–3× | `src/battle_main.c`, `src/battle_controllers.c`, `src/palette.c` | più tick software per frame |
| Menu squadra SwSh | `PARTY MENU` | `src/swsh_party_menu.c`, `src/party_menu_dispatch.c`, `include/party_menu_variant.h` | due varianti compilate insieme, `.claude/rules/party_menu.md` |
| Schermata borsa | — | `src/item_menu.c`, `src/item_menu_icons.c`, `graphics/bag/soulgold/` | |
| Tema scuro | `DARK UI` | `battle_bg.c`, `battle_interface.c`, `battle_gfx_sfx_util.c`, `item_menu.c` | mappa completa in `.claude/rules/battle_ui.md` |
| Healthbox shiny | — | `src/battle_gfx_sfx_util.c` | in tema scuro diventa scura anche lei, diversamente da SG (voluto, §6) |
| Shiny Genome | — | `swsh_party_menu.c` (`Task_ShinGenome`), Mart di Viridian | |
| Start menu compatto | — | `src/menu.c`, `src/start_menu.c` | da SG 1.1.4 |
| Candy Jar | — | `src/candy_jar.c`, `src/item_use.c`, `src/save.c` (migrazione `saveVersion < 6`) | `.claude/rules/save.md` e `.claude/rules/candy_jar.md` |
| DexNav, metà "ricerca" | `ENHANCED DEXNAV` | `src/dexnav.c` | la metà "mostra tutti" **non** è di SG, §6 |
| Icone dei tipi, comportamento | — | `src/type_icons.c`, `include/type_icons.h` | l'arte è di SG; layout in singola diverso, §6 |
| Relearner solo dalla pagina mosse | — | `src/pokemon_summary_screen.c` | la feature è di expansion, la restrizione di SG |
| Fast HMs (SG 1.2, `a9d41a7b9`) | `FAST HMS`, default OFF | `src/field_effect.c`, `src/fldeff_flash.c`, `src/fldeff_rocksmash.c`, `data/scripts/field_move_scripts_hns.inc`, `data/scripts/surf.inc` | salta domanda, messaggio "usato" e posa/banner. **Gli script sono quelli `_hns`**, non `field_move_scripts.inc` (quello è il ramo non-HnS): Taglio, Spaccaroccia, Forza, Cascata e Testata qui non avevano già né domanda né animazione, quindi per loro l'opzione toglie solo il messaggio. Surf, Sub, Scalaroccia e Defog hanno anche domanda e/o posa. Flag specchio `FLAG_FAST_HMS`, campo `soundAndFieldOptions.fastHms`, `.claude/rules/save.md` |
| Tema musica di lotta (SG 1.2, `f81a55ff3`) | `BATTLE MUSIC`: DEFAULT, HG JOHTO, HG KANTO, FRLG, RSE, DPPT, RANDOM | `src/pokemon.c` (`GetBattleBGM`), `include/pokemon.h`, `src/option_menu.c` | diverso da SG: DEFAULT è il tema **regionale** di Hasep, c'è HG JOHTO, e il tema vale anche per la **Superquattro**. Restano fissi boss, rivali, Rocket, campioni, leggendari, link, strutture e il Professor Oak (`PROFESSOR_HNS`, che prima condivideva il `case` con i capipalestra FRLG). 4 brani DP nuovi (`mus_dp_vs_wild/trainer/gym_leader`, `mus_dp_poke_center_night`) da SG, id derivati da Hasep |
| Musica notturna nei Centri (SG 1.2, `c913c4a2d`) | `NIGHT CENTER`, default ON | `src/overworld.c` (`ApplyNightCenterMusic`) | in SG è sempre attiva, qui è un'opzione. Vale per le tre musiche di Centro (RSE, FRLG, HGSS) |
| Level scaling, il motore (`f690a8587`, branch `feature/level-scaling` di fisham-org) | `LEVEL SCALING`, default OFF (in SG sono due opzioni, trainer e selvatici; qui una, per scelta dell'utente) | `src/level_scaling.c`, `include/level_scaling.h`, `include/config/level_scaling.h`, agganci in `battle_main.c`, `wild_encounter.c`, `dexnav.c`, `scrcmd.c`, `battle_script_commands.c` | portato solo il motore: le regole per allenatore di SG (`level_scaling_rules.h`, mai lette) no. Regole di HnS decise dall'utente (`docs/plans/level-scaling.md`): mai sotto il livello scritto, selvatici −2 invece di −8, lotte di script al level cap, soldi sul livello vero più un bonus; dal 23/10 i selvatici non superano il level cap né la tabella di più di 15. `.claude/rules/level_scaling.md` |

## 2. Accesi e completati qui (non scritti qui, non di Soulgold)

DexNav (`DEXNAV_ENABLED` era FALSE), relearner dal riassunto
(`P_ENABLE_MOVE_RELEARNERS`, mancava la gestione di START), icone dei tipi
(`B_SHOW_TYPES` era NEVER), Pokédex HGSS. Dettagli in `.claude/rules/` (`dexnav.md`, `battle_ui.md`, `relearner.md`).

Pokémon selvatici visibili nell'overworld: **non vengono da Soulgold** (SG
non li ha) ma da pokeemerald-expansion (PR #8434, 1.16), portati qui con
le indoli per famiglia, i tetti per zona e le correzioni di VRAM.
Dettagli in `.claude/rules/overworld_encounters.md` e
`docs/plans/overworld-encounters.md`.

## 3. Aggiunto qui

Pokédex dal menu squadra, `EASY CATCH`, `NICKNAMES`, `WILD BATTLES`, toggle del
follower nel menu SwSh, badge `SEL`/`HM` leggibili nei due temi (difetto
presente anche in SG), prezzi e negozi (README), `SORT` e `SEARCH` nei box
del PC (SG non li ha; `.claude/rules/pc_box.md`).

Quest book (schermata di PokemonSanFran, `complex_quests`) e menu di pausa a
striscia (`unbound_start_menu.c` di pollythadon): **non vengono da Soulgold**
(SG non ha né l'uno né l'altro). Dettagli in `.claude/rules/quest_book.md`,
`.claude/rules/strip_menu.md` e `docs/plans/quest-log.md`.

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

Sync del 12/10/2026, commit di SG da `c913c4a2d` a `73a66207e` (SG 1.2), letti
diff per diff:

| cambio in SG | in Hasep | perché |
| --- | --- | --- |
| Mirror Armor, "non può calare oltre" (`39f2cc303`) | **applicato** | stesso bug in Hasep: `BattleScript_MirrorArmorReflectWontFall` copiava l'attaccante in `gBattlerTarget` e tornava al chiamante senza ripristinarlo, quindi le istruzioni successive dello script della mossa (es. il secondo calo di Parting Shot) agivano sull'attaccante. Ora `savetarget`/`restoretarget`. Letto dal codice, non riprodotto in gioco |
| Bad Dreams lascia `sFIXED_ABILITY_POPUP` alzato (`39f2cc303`) | **applicato** | stesso script in Hasep: dal secondo turno il popup dell'abilità non compariva più |
| Revive / cure a metà o a un quarto in lotta con PS massimi 1-3 (`39f2cc303`) | **applicato** | `BS_ItemRestoreHP`: `maxHP / 2` su Shedinja (1 PS) fa 0, cioè rianimato a 0 PS. Ora almeno 1. Il ramo Sitrus/`tx_Mode_New_Citrus` di Hasep è rimasto com'era |
| Imposter dopo essere andato KO per gli hazard (`39f2cc303`) | **applicato** | `DoSwitchInEvents`: il controllo "è vivo?" stava solo prima del blocco, quindi chi moriva sugli hazard eseguiva lo stesso gli eventi successivi del blocco (abilità d'ingresso). Ora è controllato a ogni evento |
| Conteggio squadra dopo una cattura con scambio (`39f2cc303`) | **applicato** | `GiveCapturedMonToPlayer`: dopo lo scambio dalla lotta lo slot liberato è in mezzo, e `gPlayerPartyCount = i + 1` contava meno Pokémon di quelli presenti. Ora `CalculatePlayerPartyCount()`. `GiveMonToPlayer` (regali da script) non ha buchi in mezzo e non è stato toccato |
| Pensione: Everstone sul padre straniero (`39f2cc303`) | **applicato** | stesso codice in Hasep: un padre di forma regionale straniera con Pietrastante imponeva la **sua specie** all'uovo anche se la madre era di un'altra specie. Ora solo se la specie base coincide |
| Menu squadra SwSh, scelta squadra per le strutture (`c913c4a2d`) | **applicato** | due pezzi: START conferma anche una squadra incompleta (come già il pulsante Conferma, poi la validazione chiede il minimo), e `UnselectLastBattleEntry` non legge più `gSelectedOrderFromParty[max - 1] - 1` = 255 quando l'ultima casella è vuota. In Hasep il secondo caso si raggiungeva già dal pulsante Conferma rispondendo NO |
| `gLeveledUpInBattle` azzerato dopo lo scambio di cattura (`39f2cc303`) | **già presente** | |
| Simple che va in overflow sopra +6 (`39f2cc303`) | non applicato | in SG nasce dai suoi tratti che moltiplicano i boost; in Hasep il boost più grande per mossa è +3, raddoppiato +6, nessun overflow |
| Aura Shield / Omega / Infiltrator, Neutralizing Gas sui tratti, Ability Shield sui tratti (`c913c4a2d`) | non applicato | sistema a tratti multipli di SG |
| `IsRuinStatusActive` senza l'eccezione Neutralizing Gas (`c913c4a2d`) | non applicato | conta solo se un Pokémon ha Neutralizing Gas **e** un'abilità Ruin insieme, cioè con i tratti di SG |
| Maneuverability `<= 60` (`c913c4a2d`) | non applicato | abilità di SG |
| Musica notturna nei Centri Pokémon (`c913c4a2d`) | **applicato** | come opzione, §1 |
| Tema della musica di lotta (`f81a55ff3`) | **applicato** | adattato (regionale, HG JOHTO, Superquattro), §1 |
| Palette del box PC con trasferimento parziale (`f81a55ff3`) | non applicato | SG toglie una sua ottimizzazione (`transferWholePlttFrames`) che in Hasep non è mai esistita |
| Fast HMs (`a9d41a7b9`) | **applicato** | adattato agli script `_hns`, §1 |
| Sprite di surf regionali (`73a66207e`) | **già presente** | SG li ha presi da HnS 2.0: i 30 PNG e le voci in `surfable_pokemon.h` ci sono già |
| Level scaling dei rivali (`39f2cc303`), versione 1.2 (`20089aa08`), MT75 Agility, testi, squadre, learnset, tratti, documentazione (`1d09352b9`, `b7d1534e3`, `29b53d739`, `f6a7d7c2b`, `ad91d8aa5`, `23412857c`, `145247fb4`) | non applicato | contenuto di SG. Il refuso di Motor Drive sta in una `longDescription` che Hasep non ha |

Sync del 20/10/2026, commit di SG da `8033e448c` a `4d4acbe98`, letti diff
per diff:

| cambio in SG | in Hasep | perché |
| --- | --- | --- |
| `animCmdIndex` azzerato quando il follower rientra nella ball (`c279a23b1`, crash di Mega Greninja dopo il surf) | **applicato** | stesso codice in Hasep, che ha anche lui gli sprite di surf (`surfable_pokemon_graphics.h`): `FollowerSetGraphics` cambia la tabella di animazioni e un indice rimasto da quella vecchia può leggere oltre la fine della nuova. `ExitPokeball_Step1` faceva già lo stesso azzeramento. Letto dal codice, non riprodotto in gioco |
| Palette dell'icona tenuta in mano tornando dal riassunto nel PC (`2fab8e856`) | non applicato | corregge il sistema di palette per-Pokémon del PC di SG (`LoadMovingMonIconPalette`, `GetStorageMonIconPalette`, `chooseBoxSwapPal`), che Hasep non ha: qui `CreateMovingMonIcon` non forza nessuna palette |
| Sfondo Rayquaza anche per Zapdos (`11667e07c`) | non applicato | scelta estetica di SG; Hasep ha gli override di Emerald (Groudon/Kyogre/Rayquaza), non quelli di SG |
| Icona di Mega Staraptor (`f55a6fc89`) | non applicato | megaevoluzioni spente in Hasep (`.claude/rules/megas.md`) |
| Docs del sito, CSS, export della squadra (`8033e448c`, `c98530c7d`, `1d304cea2`, `3a378f5e7`, `4d4acbe98`), refusi nelle abilità, testi, mappe, squadre, learnset, tratti (`d039d054b`, `08c15c8ea`, `11a7a74f7`, `a984e5913`, `acfbed1df`, `c37387967`, `16db3f5dc`, `c159c5bf6`, `2672ba3e4`) | non applicato | contenuto di SG |

Trovato e **non** corretto (non è nel diff di SG, presente in entrambi):
`CheckBattleEntriesAndGetMessage` in `swsh_party_menu.c` confronta specie e
strumenti di tutte le `maxBattlers` caselle anche quando le ultime sono vuote,
leggendo `gPlayerParty[255]`. Con una squadra incompleta può dare un falso
"stesso Pokémon / stesso strumento". Raggiungibile solo nelle strutture con
minimo inferiore al massimo.

---

## 5. Non portato, e perché

**Bloccato o rimandato per un motivo misurato**

- **Megaevoluzioni**: la ROM passa al 99,46% dei 32 MB (`.claude/rules/megas.md`). Il
  lavoro sta su `claude/mega-type-stones`, non ancora unito.
- **Pokédex completo dalla pagina della cattura**: in lotta non c'è abbastanza
  memoria (misura in `.claude/rules/pokedex.md`). Anche SG, su quella pagina, esce e basta.

**Candidati non ancora fatti**

- **Luci giorno/notte disattivabili** (`OW lighting`, `Battle lighting`, SG
  1.1.4): due opzioni che spengono la fusione dei colori per l'ora del giorno.
  In SG costano due flag. Da valutare: in Hasep le opzioni vivono in
  `ChallengeSettings`, che ora è piena; il posto più economico è
  `soundAndFieldOptions` (oggi pieno: resta un solo bit ignorato (`unusedLevelScalingBit`), da azzerare con una migrazione prima di riusarlo, `.claude/rules/save.md`) con un flag specchio come
  per Fast HMs.

**Contenuto e design di Soulgold, legati alle sue mappe e al suo bilanciamento**

Achievement, Hidden Grotto, Title Defense, lotte boss, Battle Arcade / Rocket
Arcade, minigiochi (blackjack, gacha, pachinko, pinball, derby, snake, flappy
bird, block stacker, Voltorb Flip roguelike), puzzle delle Rovine d'Alfa,
level scaling come **regole** (il motore è portato, §1), mosse degli allenatori, sistema di tratti/abilità innate, squadre,
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
- **Flash e DexNav**: in SG avere la MN Flash in borsa illumina del tutto le
  grotte buie (livello 0), qui Flash si usa e lascia il raggio (livello 1),
  come in HGSS. Il controllo "troppo buio" del DexNav, identico nei due repo
  (`> 0`), in SG quindi non scatta mai, qui bloccava Dark Cave anche con
  Flash: ora rifiuta solo sopra 1 (`IsTooDarkToSearch`, `.claude/rules/dexnav.md`).
- **Icone dei tipi in singola**: a sinistra della healthbox e non specchiate;
  in doppia come SG (§6).
- **Evoluzioni regionali**: restano legate alla regione. Raichu/Exeggutor/Marowak
  di Alola solo alle Isole Alola, le forme di Hisui solo a Sinjoh
  (`GetRegionForSectionId` in `include/regions.h`); le forme di Galar non si
  ottengono evolvendo, ma dall'NPC nella casa di Bill (`Route25_BillsHouse_hns`,
  `ConvertToRegionalForm`). Scelta dell'utente, 28/09/2026.
