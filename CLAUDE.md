# Note per Claude — `pokehns-expansion-plus`

Fork personale di `pokemonHnS-expansion` (base: pokeemerald-expansion 1.15.2) in cui
si portano feature da **Soulgold** (`/home/user/soulgold`, se presente).

**Come sono organizzate queste note.** Qui solo ciò che serve in ogni sessione.
Il dettaglio di ogni area sta in `.claude/rules/` (campo `paths:`, caricata da
sola quando apri un file corrispondente; `grep` non la fa scattare: indice al
§7). Le regole tengono **trappole e fatti**; il racconto delle verifiche sta
nell'appendice del piano (`docs/plans/`) o in `docs/claude/*-history.md`. Una
nota nuova va nella regola dell'area, non qui; una regola sopra gli 8 KB si sfoltisce.

---

## 1. Build e consegna

```sh
make hns -j$(nproc) -O          # NON `make modern`: quello è l'altro repo
zip -9 pokemonHnS_<descrizione>_<sha>.zip pokehns.gba
```

**Consegna sempre la ROM completa da 32 MB compressa** (~18 MB), niente ROM
tagliate. **ROM in chat, patch nel repo**: si pubblica solo la patch BPS
(`tools/make_patch.sh`) sulle release di GitHub, mai nel git. Release: Actions →
Release → Run workflow (versione `2.0.6.PLUS.2`, tag `v2.0.6.PLUS.2`) o un tag
`v*`. **Ogni port o feature va annotato in `CHANGELOG.md`** sotto la prossima
versione: senza la sezione `## <versione>` la release si ferma prima della
build (`release.md`, `title_screen.md`).

Uno `#if` non chiuso si manifesta come `'X' used but never defined` su funzioni
che non c'entrano: dopo aver toccato direttive, controlla `#if`/`#endif`.

---

## 2. Regola numero uno: non copiare i valori di Soulgold

Le palette di SG sono tarate sulla **sua** grafica: su una grafica diversa un
valore copiato finisce quasi sempre su una entry che lì significa un'altra cosa.
**Deriva sempre dai file di questo repo**, verificando *prima* di scrivere codice:

- conta gli indici usati davvero dai `.4bpp` (finiscono in ROM; i `.png` possono
  avere palette di authoring diverse: `hns/healthbox_*.png`);
- renderizza offline tile + tilemap + `.gbapal` veri e **guarda l'immagine**;
- una entry serve spesso a due disegni: prima di scurirla controlla **tutti**
  quelli che la usano (`battle_ui.md`).

---

## 3. Emulatore (verifica vera)

mGBA gira headless: è così che sono stati trovati i bug che l'analisi statica
non vedeva. **Guida completa (installazione, tasti, debug R+START, GDB, ora
finta, nuova partita) in `docs/claude/emulator.md`**: leggila prima della prima
verifica. Dopo **ogni** passaggio di menu fai uno screenshot e guardalo;
**misura i pixel invece di fidarti dell'occhio**; se l'analisi statica non
conclude, una **ROM strumentata** usa-e-getta chiude in due build. Il `.srm`
dell'utente si copia come `pokehns.sav` accanto alla ROM, mai nel git.

---

## 4. Salvataggi (sintesi)

Layout, polarità, migrazioni: `.claude/rules/save.md`.

- `ChallengeSettings` è fissata a 32 byte con **1 bit libero**; `soundAndFieldOptions`
  è pieno. Una nuova opzione: un flag, o un campo in coda a SaveBlock3.
- **Polarità**: un salvataggio vecchio legge zero, quindi zero = comportamento
  di prima; l'etichetta del valore zero viene prima nella lista.
- Campo nuovo in SaveBlock3: in **fondo**, `STATIC_ASSERT` sull'offset in
  `save.c`, passo `saveVersion < N` in `LoadGameSave()`; se è un'opzione del
  titolo, anche in `oak_speech_hns.c` **e** in `NewGameInitData`.
- **`NewGameInitData` azzera SaveBlock1 e 3, non SaveBlock2**: un campo di gioco
  in SaveBlock2 va azzerato a mano (come in `ResetPokedex`).

## 5. Tema scuro

Accessore `IsDarkUiEnabled()`; mappa in `battle_ui.md` (lotta, borsa) e
`dark_ui.md` (dialoghi, menu, summary, squadra). In SG l'opzione copre solo
lotta e borsa: il resto non è un port.

---

## 6. Prima di portare qualcosa: controlla se c'è già

**Cosa è arrivato da Soulgold e cosa no: `docs/SOULGOLD_PORT_REPORT.md`**, da
tenere in pari a ogni port (allineato a SG 1.1.4 più fix del §4 e tre feature di
SG 1.2). Sync HnS/SG: `/upstream-sync`. DexNav, icone dei tipi e ricorda mosse
erano già nel codice, **spenti**: un `#if IS_HNS / #else` è spesso il posto in
cui una feature è spenta senza che nessun flag lo dica.

### Lezioni che valgono ovunque

- **Porti un file da SG? Confronta le firme delle funzioni che chiama** (un
  argomento in meno compila, una feature in meno no).
- **Prima di scrivere un simbolo, guarda se c'è già**: `nm --defined-only
  build/hns/src/<file>.o`. Senza prefisso di variante è una collisione di link.
- **"Rendilo come SG" può cancellare feature**: la divergenza va nei due sensi
  (`dexnav.md`). **Non dire "è identico in SG" senza averlo aperto.**
- **Un diff vuoto fra due estratti vuoti non è una verifica.**
- **Leggi la segnalazione dell'utente come una misura, non come un sintomo.**
- **Verifica con un caso che fallisce senza la modifica**, guardando il
  prodotto (il Pokémon catturato era un Bad Egg: `trainer_catch.md`).
- **Un `out of memory` con un numero preciso va tradotto in un file**: `ls -l`
  sui `.4bpp` (`pokedex.md`). **Strumentare, non dedurre.**
- Una feature sempre spenta ha dati posizionali di default di expansion. Una
  schermata senza il suo `SetVBlankCallback` eredita quello del chiamante.
- **`LoadPalette` a schermo nero lo riaccende** (scrive anche il buffer
  sfumato): mai fra una dissolvenza al nero e la schermata dopo (`strip_menu.md`).
- **`make hns` non è una build RELEASE: un `assertf` che fallisce mostra una
  schermata blu.** Solo per errori di programma, mai per stati raggiungibili
  (VRAM, tabella oggetti) (`overworld_encounters.md`).
- Con `OW_GFX_COMPRESS` ogni oggetto tiene in VRAM tutto il suo foglio (10 tile
  libere su 1024 nel Safari): uno sprite che non si crea non deve lasciare uno
  script in attesa.

---

## 7. Indice delle regole (`.claude/rules/`)

| Regola | Area |
| --- | --- |
| `release`, `title_screen` | patch BPS, base Emerald, note; "+" e versione |
| `save` | layout, polarità, migrazioni |
| `battle_ui`, `dark_ui` | palette in lotta, icone dei tipi; tema scuro fuori dalla lotta |
| `dexnav`, `relearner`, `candy_jar`, `pc_box`, `bag`, `music`, `megas` | una per feature |
| `party_menu`, `pokedex`, `strip_menu`, `quest_book` | menu e schermate |
| `field` | script MN `_hns`, negozi, nickname, descrizione oggetto, Testata |
| `trainer_catch`, `reds_path`, `level_scaling`, `overworld_encounters` | TcloneR; Red's Path; level scaling; OWE |

**Skill** (`/<nome>`): `upstream-sync`, `next-phase`, `new-plan`, `add-quest`,
`doc-sync`, `new-issue`, `pr-review`. A mano: `docs/claude/emulator.md`,
`docs/claude/*-history.md`. I piani in `docs/plans/` hanno decisioni e verifiche;
**`docs/analysis/` è l'archivio delle ricerche**, non serve per implementare.

---

## 8. Rotture preesistenti (non tue)

`make all/firered/leafgreen` falliscono su `src/mom_savings.c`; `make check`
fallisce (`test/save.c` stale). La CI (`build.yml`) costruisce **solo** `make hns`
con `UNUSED_ERROR=0` (con 1 fallirebbe su 40 funzioni spente dietro `IS_HNS`) e
scrive quanto dei 32 MB è usato: **avviso dal 98%, errore dal 99,5%** (le mega da
sole: 99,46%, `megas.md`).

## 9. Stile

- Commenti e commit **in inglese**, conversazione con l'utente in italiano.
- Commenti sul **perché**; di un valore derivato di' da dove (palette:
  `// was RGB(...)`). Commit: cosa è rotto, perché, come si è verificato.
- Branch di sviluppo: quello assegnato dalla sessione, non pushare altrove.
