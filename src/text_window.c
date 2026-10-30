#include "global.h"
#include "text.h"
#include "text_window.h"
#include "window.h"
#include "palette.h"
#include "bg.h"
#include "graphics.h"
#include "menu.h"
#include "option_menu.h"
#include "constants/rgb.h"

static const u16 sStdTextWindow_Gfx[]  = INCBIN_U16("graphics/text_window/std.4bpp");

const u8 gTextWindowFrame1_Gfx[] = INCBIN_U8("graphics/text_window/1.4bpp");
static const u8 sTextWindowFrame2_Gfx[] = INCBIN_U8("graphics/text_window/2.4bpp");
static const u8 sTextWindowFrame3_Gfx[] = INCBIN_U8("graphics/text_window/3.4bpp");
static const u8 sTextWindowFrame4_Gfx[] = INCBIN_U8("graphics/text_window/4.4bpp");
static const u8 sTextWindowFrame5_Gfx[] = INCBIN_U8("graphics/text_window/5.4bpp");
static const u8 sTextWindowFrame6_Gfx[] = INCBIN_U8("graphics/text_window/6.4bpp");
static const u8 sTextWindowFrame7_Gfx[] = INCBIN_U8("graphics/text_window/7.4bpp");
static const u8 sTextWindowFrame8_Gfx[] = INCBIN_U8("graphics/text_window/8.4bpp");
static const u8 sTextWindowFrame9_Gfx[] = INCBIN_U8("graphics/text_window/9.4bpp");
static const u8 sTextWindowFrame10_Gfx[] = INCBIN_U8("graphics/text_window/10.4bpp");
static const u8 sTextWindowFrame11_Gfx[] = INCBIN_U8("graphics/text_window/11.4bpp");
static const u8 sTextWindowFrame12_Gfx[] = INCBIN_U8("graphics/text_window/12.4bpp");
static const u8 sTextWindowFrame13_Gfx[] = INCBIN_U8("graphics/text_window/13.4bpp");
static const u8 sTextWindowFrame14_Gfx[] = INCBIN_U8("graphics/text_window/14.4bpp");
static const u8 sTextWindowFrame15_Gfx[] = INCBIN_U8("graphics/text_window/15.4bpp");
static const u8 sTextWindowFrame16_Gfx[] = INCBIN_U8("graphics/text_window/16.4bpp");
static const u8 sTextWindowFrame17_Gfx[] = INCBIN_U8("graphics/text_window/17.4bpp");
static const u8 sTextWindowFrame18_Gfx[] = INCBIN_U8("graphics/text_window/18.4bpp");
static const u8 sTextWindowFrame19_Gfx[] = INCBIN_U8("graphics/text_window/19.4bpp");
static const u8 sTextWindowFrame20_Gfx[] = INCBIN_U8("graphics/text_window/20.4bpp");

const u16 gTextWindowFrame1_Pal[] = INCBIN_U16("graphics/text_window/1.gbapal");
static const u16 sTextWindowFrame2_Pal[] = INCBIN_U16("graphics/text_window/2.gbapal");
static const u16 sTextWindowFrame3_Pal[] = INCBIN_U16("graphics/text_window/3.gbapal");
static const u16 sTextWindowFrame4_Pal[] = INCBIN_U16("graphics/text_window/4.gbapal");
static const u16 sTextWindowFrame5_Pal[] = INCBIN_U16("graphics/text_window/5.gbapal");
static const u16 sTextWindowFrame6_Pal[] = INCBIN_U16("graphics/text_window/6.gbapal");
static const u16 sTextWindowFrame7_Pal[] = INCBIN_U16("graphics/text_window/7.gbapal");
static const u16 sTextWindowFrame8_Pal[] = INCBIN_U16("graphics/text_window/8.gbapal");
static const u16 sTextWindowFrame9_Pal[] = INCBIN_U16("graphics/text_window/9.gbapal");
static const u16 sTextWindowFrame10_Pal[] = INCBIN_U16("graphics/text_window/10.gbapal");
static const u16 sTextWindowFrame11_Pal[] = INCBIN_U16("graphics/text_window/11.gbapal");
static const u16 sTextWindowFrame12_Pal[] = INCBIN_U16("graphics/text_window/12.gbapal");
static const u16 sTextWindowFrame13_Pal[] = INCBIN_U16("graphics/text_window/13.gbapal");
static const u16 sTextWindowFrame14_Pal[] = INCBIN_U16("graphics/text_window/14.gbapal");
static const u16 sTextWindowFrame15_Pal[] = INCBIN_U16("graphics/text_window/15.gbapal");
static const u16 sTextWindowFrame16_Pal[] = INCBIN_U16("graphics/text_window/16.gbapal");
static const u16 sTextWindowFrame17_Pal[] = INCBIN_U16("graphics/text_window/17.gbapal");
static const u16 sTextWindowFrame18_Pal[] = INCBIN_U16("graphics/text_window/18.gbapal");
static const u16 sTextWindowFrame19_Pal[] = INCBIN_U16("graphics/text_window/19.gbapal");
static const u16 sTextWindowFrame20_Pal[] = INCBIN_U16("graphics/text_window/20.gbapal");

static const u16 sTextWindowPalettes[][16] =
{
    INCBIN_U16("graphics/text_window/message_box.gbapal"),
    INCBIN_U16("graphics/text_window/text_pal1.gbapal"),
    INCBIN_U16("graphics/text_window/text_pal2.gbapal"),
    INCBIN_U16("graphics/text_window/text_pal3.gbapal"),
    INCBIN_U16("graphics/text_window/text_pal4.gbapal")
};

static const struct TilesPal sWindowFrames[WINDOW_FRAMES_COUNT] =
{
    {gTextWindowFrame1_Gfx, gTextWindowFrame1_Pal},
    {sTextWindowFrame2_Gfx, sTextWindowFrame2_Pal},
    {sTextWindowFrame3_Gfx, sTextWindowFrame3_Pal},
    {sTextWindowFrame4_Gfx, sTextWindowFrame4_Pal},
    {sTextWindowFrame5_Gfx, sTextWindowFrame5_Pal},
    {sTextWindowFrame6_Gfx, sTextWindowFrame6_Pal},
    {sTextWindowFrame7_Gfx, sTextWindowFrame7_Pal},
    {sTextWindowFrame8_Gfx, sTextWindowFrame8_Pal},
    {sTextWindowFrame9_Gfx, sTextWindowFrame9_Pal},
    {sTextWindowFrame10_Gfx, sTextWindowFrame10_Pal},
    {sTextWindowFrame11_Gfx, sTextWindowFrame11_Pal},
    {sTextWindowFrame12_Gfx, sTextWindowFrame12_Pal},
    {sTextWindowFrame13_Gfx, sTextWindowFrame13_Pal},
    {sTextWindowFrame14_Gfx, sTextWindowFrame14_Pal},
    {sTextWindowFrame15_Gfx, sTextWindowFrame15_Pal},
    {sTextWindowFrame16_Gfx, sTextWindowFrame16_Pal},
    {sTextWindowFrame17_Gfx, sTextWindowFrame17_Pal},
    {sTextWindowFrame18_Gfx, sTextWindowFrame18_Pal},
    {sTextWindowFrame19_Gfx, sTextWindowFrame19_Pal},
    {sTextWindowFrame20_Gfx, sTextWindowFrame20_Pal}
};

static const u16 sTextWindowDexNavFrame[] = INCBIN_U16("graphics/text_window/dexnav_pal.gbapal");
static const struct TilesPal sDexNavWindowFrame = {gTextWindowFrame1_Gfx, sTextWindowDexNavFrame};

// code
const struct TilesPal *GetWindowFrameTilesPal(u8 id)
{
    if (id >= WINDOW_FRAMES_COUNT)
        return &sWindowFrames[0];
    else
        return &sWindowFrames[id];
}

// ---------------------------------------------------------------------------
// Dark UI: dialogue boxes, signposts and menu windows
//
// Soulgold has nothing to take here: its dark option only covers battle and
// the Bag, and its message box stays white either way. The values are derived
// from HnS's own files instead, with the background shared with the dark
// battle UI so the two look like one theme.
//
//  - Text is printed with entries 1 (background), 2 (glyph) and 3 (shadow);
//    the coloured placeholders use the glyph/shadow pairs 4/5, 6/7 and 8/9.
//    On a dark background the glyph has to be the light half of each pair and
//    the shadow the dark half, so the pairs are re-picked, not swapped.
//  - hns/message_box.4bpp paints the tile behind the text (tile 9) with entry
//    12 and its inner bevel with entry 11; 13 and 14 are the outer border,
//    dark already, and stay as they are.
//  - signpost.4bpp, drawn with text_pal1, uses entry 1 for its interior, 11
//    for the inner bevel and 15 for the outline.
//  - The 20 user frames all fill their interior from entry 14 (see
//    USER_WINDOW_FRAME_FILL_PAL_INDEX).
//
// None of this is applied by the plain loaders above: a box is the frame of
// one palette around a window printed with another, and the two only go dark
// together. Darkening every frame load left screens such as the Bag with a
// dark ring around a white menu, so each caller that loads both halves applies
// both (see LoadMessageBoxAndBorderGfx in menu.c).
// ---------------------------------------------------------------------------

struct DarkTextboxEntry
{
    u8 index;
    u16 color;
};

static const struct DarkTextboxEntry sDarkTextColors[] =
{
    { TEXT_COLOR_WHITE,       DARK_UI_BG_COLOR },  // was RGB(31, 31, 31)
    { TEXT_COLOR_DARK_GRAY,   RGB(28, 28, 28)  },  // was RGB(12, 12, 12)
    { TEXT_COLOR_LIGHT_GRAY,  RGB(1, 1, 1)     },  // was RGB(26, 26, 25)
    { TEXT_COLOR_RED,         RGB(31, 12, 12)  },  // was RGB(28, 1, 1)
    { TEXT_COLOR_LIGHT_RED,   RGB(10, 2, 2)    },  // was RGB(31, 23, 14)
    { TEXT_COLOR_GREEN,       RGB(14, 29, 10)  },  // was RGB(4, 19, 1)
    { TEXT_COLOR_LIGHT_GREEN, RGB(2, 9, 1)     },  // was RGB(18, 30, 18)
    { TEXT_COLOR_BLUE,        RGB(15, 20, 31)  },  // was RGB(6, 10, 25)
    { TEXT_COLOR_LIGHT_BLUE,  RGB(3, 5, 14)    },  // was RGB(20, 24, 30)
};

static const struct DarkTextboxEntry sDarkMessageBoxColors[] =
{
    { 11, RGB(10, 10, 11)   },  // inner bevel, was RGB(28, 29, 28)
    { 12, DARK_UI_BG_COLOR  },  // behind the text, was RGB(31, 31, 31)
};

static const struct DarkTextboxEntry sDarkSignpostColors[] =
{
    { 11, RGB(10, 10, 12)   },  // inner bevel, was RGB(25, 25, 27)
};

static const u16 sDarkUiBgColor = DARK_UI_BG_COLOR;

static void LoadDarkEntries(const struct DarkTextboxEntry *entries, u32 count, u16 palOffset)
{
    u32 i;

    for (i = 0; i < count; i++)
        LoadPalette(&entries[i].color, palOffset + entries[i].index, PLTT_SIZEOF(1));
}

void ApplyDarkUiTextPalette(u16 palOffset)
{
    if (IsDarkUiEnabled())
        LoadDarkEntries(sDarkTextColors, ARRAY_COUNT(sDarkTextColors), palOffset);
}

void ApplyDarkUiMessageBoxPalette(u16 palOffset)
{
    if (IsDarkUiEnabled())
    {
        LoadDarkEntries(sDarkTextColors, ARRAY_COUNT(sDarkTextColors), palOffset);
        LoadDarkEntries(sDarkMessageBoxColors, ARRAY_COUNT(sDarkMessageBoxColors), palOffset);
    }
}

void ApplyDarkUiSignpostPalette(u16 palOffset)
{
    if (IsDarkUiEnabled())
    {
        LoadDarkEntries(sDarkTextColors, ARRAY_COUNT(sDarkTextColors), palOffset);
        LoadDarkEntries(sDarkSignpostColors, ARRAY_COUNT(sDarkSignpostColors), palOffset);
    }
}

void ApplyDarkUiWindowFramePalette(u16 palOffset)
{
    if (IsDarkUiEnabled())
        LoadPalette(&sDarkUiBgColor, palOffset + USER_WINDOW_FRAME_FILL_PAL_INDEX, PLTT_SIZEOF(1));
}

void LoadMessageBoxGfx(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), gMessageBox_Gfx, 0x1C0, destOffset);
    LoadPalette(GetOverworldTextboxPalettePtr(), palOffset, PLTT_SIZE_4BPP);
}

void LoadStdWindowGfx(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), sStdTextWindow_Gfx, 0x120, destOffset);
    LoadPalette(GetTextWindowPalette(3), palOffset, PLTT_SIZE_4BPP);
}

void LoadSignBoxGfx(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), gSignpostWindow_Gfx, 0x1C0, destOffset);
    LoadPalette(GetTextWindowPalette(1), palOffset, PLTT_SIZE_4BPP);
}

void LoadUserWindowBorderGfx_(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadUserWindowBorderGfx(windowId, destOffset, palOffset);
}

void LoadWindowGfx(u8 windowId, u8 frameId, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), sWindowFrames[frameId].tiles, 0x120, destOffset);
    LoadPalette(sWindowFrames[frameId].pal, palOffset, PLTT_SIZE_4BPP);
}

void LoadUserWindowBorderGfx(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadWindowGfx(windowId, gSaveBlock2Ptr->optionsWindowFrameType, destOffset, palOffset);
}

void DrawTextBorderOuter(u8 windowId, u16 tileNum, u8 palNum)
{
    u8 bgLayer = GetWindowAttribute(windowId, WINDOW_BG);
    u16 tilemapLeft = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    u16 tilemapTop = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    u16 width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    u16 height = GetWindowAttribute(windowId, WINDOW_HEIGHT);

    FillBgTilemapBufferRect(bgLayer, tileNum + 0, tilemapLeft - 1,      tilemapTop - 1,         1,      1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 1, tilemapLeft,          tilemapTop - 1,         width,  1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 2, tilemapLeft + width,  tilemapTop - 1,         1,      1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 3, tilemapLeft - 1,      tilemapTop,             1,      height, palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 5, tilemapLeft + width,  tilemapTop,             1,      height, palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 6, tilemapLeft - 1,      tilemapTop + height,    1,      1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 7, tilemapLeft,          tilemapTop + height,    width,  1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 8, tilemapLeft + width,  tilemapTop + height,    1,      1,      palNum);
}

void DrawTextBorderInner(u8 windowId, u16 tileNum, u8 palNum)
{
    u8 bgLayer = GetWindowAttribute(windowId, WINDOW_BG);
    u16 tilemapLeft = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    u16 tilemapTop = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    u16 width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    u16 height = GetWindowAttribute(windowId, WINDOW_HEIGHT);

    FillBgTilemapBufferRect(bgLayer, tileNum + 0, tilemapLeft,              tilemapTop,                 1,          1,          palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 1, tilemapLeft + 1,          tilemapTop,                 width - 2,  1,          palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 2, tilemapLeft + width - 1,  tilemapTop,                 1,          1,          palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 3, tilemapLeft,              tilemapTop + 1,             1,          height - 2, palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 5, tilemapLeft + width - 1,  tilemapTop + 1,             1,          height - 2, palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 6, tilemapLeft,              tilemapTop + height - 1,    1,          1,          palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 7, tilemapLeft + 1,          tilemapTop + height - 1,    width -     2,  1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 8, tilemapLeft + width - 1,  tilemapTop + height - 1,    1,          1,          palNum);
}

void rbox_fill_rectangle(u8 windowId)
{
    u8 bgLayer = GetWindowAttribute(windowId, WINDOW_BG);
    u16 tilemapLeft = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    u16 tilemapTop = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    u16 width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    u16 height = GetWindowAttribute(windowId, WINDOW_HEIGHT);

    FillBgTilemapBufferRect(bgLayer, 0, tilemapLeft - 1, tilemapTop - 1, width + 2, height + 2, 0x11);
}

const u16 *GetTextWindowPalette(u8 id)
{
    switch (id)
    {
    case 0:
        id = 0x00;
        break;
    case 1:
        id = 0x10;
        break;
    case 2:
        id = 0x20;
        break;
    case 3:
        id = 0x30;
        break;
    case 4:
    default:
        id = 0x40;
        break;
    }

    return (const u16 *)(sTextWindowPalettes) + id;
}

const u16 *GetOverworldTextboxPalettePtr(void)
{
    return gMessageBox_Pal;
}

// Effectively LoadUserWindowBorderGfx but specifying the bg directly instead of a window from that bg
void LoadUserWindowBorderGfxOnBg(u8 bg, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(bg, sWindowFrames[gSaveBlock2Ptr->optionsWindowFrameType].tiles, 0x120, destOffset);
    LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, palOffset, PLTT_SIZE_4BPP);
}

void LoadDexNavWindowGfx(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), sDexNavWindowFrame.tiles, 0x120, destOffset);
    LoadPalette(sDexNavWindowFrame.pal, palOffset, 32);
}
