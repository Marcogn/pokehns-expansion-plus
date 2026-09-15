#ifndef GUARD_OPTION_MENU_H
#define GUARD_OPTION_MENU_H

void CB2_InitOptionMenu(void);

// Single accessor for the dark theme, so no screen reads the save bit itself.
bool8 IsDarkUiEnabled(void);

#endif // GUARD_OPTION_MENU_H
