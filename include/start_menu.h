#ifndef GUARD_START_MENU_H
#define GUARD_START_MENU_H

extern bool8 (*gMenuCallback)(void);

void ShowReturnToFieldStartMenu(void);
void Task_ShowStartMenu(u8 taskId);
void ShowStartMenu(void);
void ShowBattlePyramidStartMenu(void);
void SaveGame(void);
void CB2_SetUpSaveAfterLinkBattle(void);
void SaveForBattleTowerLink(void);
void HideStartMenu(void);
void StartMenu_OnClose(void);
void StartMenu_SaveFromStrip(void);
void StartMenu_OpenPokedex(void);
void StartMenu_OpenParty(void);
void StartMenu_OpenBag(void);
void StartMenu_OpenPokegear(void);
void StartMenu_OpenTrainerCard(void);
void StartMenu_OpenOptions(void);
void StartMenu_OpenDexNav(void);
void AppendToList(u8 *list, u8 *pos, u8 newEntry);

#endif // GUARD_START_MENU_H
