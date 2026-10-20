---
paths:
  - "include/config/species_enabled.h"
  - "include/config/pokemon.h"
---

# Megaevoluzioni: misurate, non stimate

`P_MEGA_EVOLUTIONS` (`include/config/species_enabled.h`) è **FALSE** qui e
**TRUE** in Soulgold. Messo a TRUE la build `hns` **compila**, ma:

| | ROM | % di 32 MB |
| --- | --- | --- |
| com'è oggi | 31 799 860 | 94,77% |
| con le mega | 33 374 804 | **99,46%** |

Costa **1 574 944 byte** e lascia **179 628 byte liberi**, cioè lo 0,54%. Parte
di quel costo è `P_MODIFIED_MEGA_CRIES`, che nell'expansion è definito uguale a
`P_MEGA_EVOLUTIONS` e da solo dichiara ~3% di ROM: si può spegnere a mano per
recuperare spazio, e va provato prima di dire che non ci stanno.

**Compilare non vuol dire funzionare**: questa misura dice solo che la ROM sta
nei 32 MB. Non è stata provata in emulatore, e comunque la feature non è finita
finché non ci sono Pietraiuto/Megapietre ottenibili da qualche parte e il
giocatore ha modo di megaevolvere. Quello è lavoro di script e negozi, non un
flag.

Per il seguito: dopo le mega qualunque altra aggiunta grossa non entra più.
