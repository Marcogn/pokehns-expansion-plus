---
paths:
  - "sound/**"
  - "src/m4a.c"
  - "src/sound.c"
  - "include/constants/songs.h"
---

# Musica

- **Il riverbero di m4a è globale.** `MPlayStart` (`src/m4a.c`) scrive il
  riverbero dell'header del brano in `SoundInfo` solo se il bit `0x80` è acceso,
  e il valore resta finché un altro brano non lo riscrive. `mid2agb -R<n>` mette
  sempre `0x80 | n`: tutti i brani di `midi.cfg` hanno un `-R`, quindi ognuno
  imposta il suo (HG `-R0`, RSE/FRLG e i 4 DP nuovi `-R50`). Misurato: Centro di
  notte (DP) riverbero 50, uscendo su Route 29 torna 0. Un brano nuovo **senza**
  `-R` erediterebbe quello del brano precedente.

- Come leggere il brano in riproduzione e l'ora per mGBA: `docs/claude/emulator.md`.
