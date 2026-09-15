#ifndef GUARD_OPTION_MENU_H
#define GUARD_OPTION_MENU_H

void CB2_InitOptionMenu(void);

// Single accessor for the dark theme, so no screen reads the save bit itself.
bool8 IsDarkUiEnabled(void);

// Single accessor for the EASY CATCH option.
bool8 IsGuaranteedCatchEnabled(void);

// Single accessor for the NICKNAMES option.
bool8 ShouldSkipNicknamePrompt(void);

#endif // GUARD_OPTION_MENU_H
