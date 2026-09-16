#ifndef GUARD_BATTLE_BG_H
#define GUARD_BATTLE_BG_H

// BG palettes 12..15 are unused during a battle (0/1 textbox, 2..4 environment,
// 5 window text, 6 VS frame and level-up banner, 7 arena referee, 8..11 the
// battlers' BG effect palettes), so the dark UI takes 13 for the redrawn
// selection cursor.
#define BATTLE_COMMAND_PAL_NUM 13

// Colour slots the dark UI uses inside gBattleWindowTextPalette (BG palette 5).
// 8 is one of the three black, unreferenced entries of that palette; 14 is
// already the white text foreground and 13 the dark text shadow.
#define BATTLE_WINDOW_DARK_BG_PAL_INDEX 8
#define BATTLE_WINDOW_DARK_FG_PAL_INDEX 14
#define BATTLE_WINDOW_DARK_SHADOW_PAL_INDEX 13

// Every one of the 20 window frames in graphics/text_window/ fills its interior
// with entry 14, and in every one of them that entry is pure white.
#define USER_WINDOW_FRAME_FILL_PAL_INDEX 14

struct BattleBackground
{
    const void *tileset;
    const void *tilemap;
};

struct BattleBackgroundEntry
{
    const void *tileset;
    const void *tilemap;
};

void BattleInitBgsAndWindows(void);
void InitBattleBgsVideo(void);
void LoadBattleMenuWindowGfx(void);
void DrawMainBattleBackground(void);
void LoadBattleTextboxAndBackground(void);
void InitLinkBattleVsScreen(u8 taskId);
void DrawBattleEntryBackground(void);
bool8 LoadChosenBattleElement(u8 caseId);
void DrawTerrainTypeBattleBackground(void);
void LoadDarkBattleStdWindowPalette(void);

#endif // GUARD_BATTLE_BG_H
