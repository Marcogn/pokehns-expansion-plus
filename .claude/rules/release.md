---
paths:
  - "tools/make_patch.sh"
  - "tools/changelog_section.py"
  - ".github/workflows/**"
  - "CHANGELOG.md"
---

# Release e patch BPS

**Le note della release vengono da `CHANGELOG.md`** (in radice, ora il
changelog del fork; quello di pokeemerald-expansion è in fondo allo stesso
file). Il workflow pubblica la sezione `## <versione>` (senza la `v` del tag)
estratta da `tools/changelog_section.py`, seguita da come applicare la patch e
dall'elenco delle PR di GitHub. **Senza quella sezione la release si ferma
subito**, prima della build: va scritta nel commit che si rilascia. Un port o
una feature nuova si annotano lì sotto la prossima versione, a titoletti.

La base è **Emerald USA pulito**, e non la deve fornire nessuno: lo script lo
ricompila da pret/pokeemerald (commit fissati nello script, ~1 min, in cache in
`~/.cache/hasep-patch`) e si ferma se lo SHA-1 non è
`f3ae088181bf583e55daf962a92bb46f4f1d07b7`. Poi riapplica la patch alla base e
la confronta con la ROM: una patch che non riproduce la build non viene
scritta. **BPS e non IPS**: IPS ha offset a 24 bit e si ferma a 16 MB, Hasep
ne occupa 32. Non aspettarti una patch piccola: un hack compilato dal decomp
ridispone quasi tutta la ROM. Misurato sulla build di `668cfc6d`: `.bps` di
**18,4 MB**, 17,2 zippata, cioè quanto la ROM zippata. La patch esce identica
byte per byte rilanciando lo script, ed è stata verificata anche con un
applicatore BPS scritto a parte, non solo con Flips che l'ha creata.
