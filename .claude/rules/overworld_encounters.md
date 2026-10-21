---
paths:
  - "src/wild_encounter.c"
  - "src/wild_encounter_ow.c"
  - "include/wild_encounter_ow.h"
  - "src/event_object_movement.c"
  - "src/field_control_avatar.c"
  - "include/config/overworld.h"
  - "docs/plans/overworld-encounters.md"
  - "docs/plans/overworld-encounters/**"
---

# Pokémon visibili nell'overworld: in corso

Piano, stato, fatti verificati e decisioni in
[`docs/plans/overworld-encounters.md`](../../docs/plans/overworld-encounters.md):
port della PR #8434 di expansion (dal tag `expansion/1.16.4`, **non** da
`master`, che dipende da `mass_outbreak.h`) più un tiro pesato per indole,
sul modello del mod Wilds of Kanto. Le indoli le sceglie l'utente su una
pagina artifact con database; il piano dice come leggerle. Non ancora
iniziato nel codice: aggiornare questa regola con trappole e misure quando
il port parte.
