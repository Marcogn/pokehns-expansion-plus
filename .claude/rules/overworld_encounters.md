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
port della PR #8434 di expansion (dal diff del merge `10ae7f9`, che usa
ancora i nomi `OW_` come noi, più le sole correzioni arrivate fino a
`expansion/1.16.4`; **non** da `master`, che dipende da `mass_outbreak.h`)
più un tiro pesato per indole,
sul modello del mod Wilds of Kanto. Le indoli scelte dall'utente sono in
`docs/plans/overworld-encounters/temperaments.json`. Non ancora
iniziato nel codice: aggiornare questa regola con trappole e misure quando
il port parte.
