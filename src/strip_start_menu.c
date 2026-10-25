// Pause menu as a strip of icons drawn over the map, in the style of Pokémon
// Unbound. Port of pollythadon's unbound_start_menu.c (pokeemerald-expansion,
// branch shattered-emerald, 4c4df1aa), cut down to the decisions in
// docs/plans/quest-log.md §2: one page of six fixed icons (no paging, no
// reordering, nothing saved), Start saves, L opens the trainer card and R the
// Pokégear. The classic list stays wherever start_menu.c decides the strip is
// not usable (Safari, Bug-Catching Contest, Battle Pike and Pyramid, Union
// Room, link).
//
// Every screen opened from here returns with CB2_ReturnToFieldWithOpenMenu,
// like from the classic list: FieldCB_ReturnToFieldStartMenu sends it back here.
#include "global.h"
#include "bg.h"
#include "decompress.h"
#include "dexnav.h"
#include "event_data.h"
#include "event_object_lock.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "option_menu.h"
#include "overworld.h"
#include "palette.h"
#include "pokedex.h"
#include "quest_book.h"
#include "rtc.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "start_menu.h"
#include "string_util.h"
#include "strip_start_menu.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "constants/characters.h"
#include "constants/songs.h"

#define STRIP_MAX_ICONS     6
#define STRIP_ICON_WIDTH    32
#define STRIP_BANNER_WIDTH  224
#define STRIP_ICON_Y        128
#define STRIP_ICON_LIFT     6   // how far up an icon being moved sits

// Tiles of the strip background, loaded at the start of BG0's char block
// (field windows start at baseBlock 1, the strip's own at 48).
#define STRIP_BG_TILE_COUNT 36
#define STRIP_BG_PAL        14

enum StripIcon
{
    STRIP_ICON_POKEDEX,
    STRIP_ICON_DEXNAV,
    STRIP_ICON_PARTY,
    STRIP_ICON_BAG,
    STRIP_ICON_QUESTS,
    STRIP_ICON_OPTIONS,
    STRIP_ICON_COUNT,
};

enum StripTileTag
{
    STRIP_TILETAG_POKEDEX = 0x1300,
    STRIP_TILETAG_DEXNAV,
    STRIP_TILETAG_PARTY,
    STRIP_TILETAG_BAG,
    STRIP_TILETAG_QUESTS,
    STRIP_TILETAG_OPTIONS,
};

#define STRIP_PALTAG_ICONS (0x1300 | BLEND_IMMUNE_FLAG)

enum StripWindow
{
    STRIP_WIN_NAME,
    STRIP_WIN_CLOCK,
    STRIP_WIN_HINTS,
    STRIP_WIN_COUNT,
};

struct StripMenu
{
    u8 icons[STRIP_MAX_ICONS];
    u8 iconCount;
    u8 cursor;
    u8 spriteIds[STRIP_MAX_ICONS];
    u8 twinIds[STRIP_MAX_ICONS];
    u8 windowIds[STRIP_WIN_COUNT];
    u8 clockMinute;
    bool8 objWinWasOn;
    bool8 moving;                       // Select lifted the icon under the cursor
    u8 iconsBeforeMove[STRIP_MAX_ICONS]; // what B puts back
    u16 savedWinOut;
};

static const u32 sPokedexIconGfx[] = INCBIN_U32("graphics/strip_start_menu/pokedex.4bpp.smol");
static const u32 sDexNavIconGfx[] = INCBIN_U32("graphics/strip_start_menu/dexnav.4bpp.smol");
static const u32 sPartyIconGfx[] = INCBIN_U32("graphics/strip_start_menu/party.4bpp.smol");
static const u32 sBagIconGfx[] = INCBIN_U32("graphics/strip_start_menu/bag.4bpp.smol");
static const u32 sQuestsIconGfx[] = INCBIN_U32("graphics/strip_start_menu/quests.4bpp.smol");
static const u32 sOptionsIconGfx[] = INCBIN_U32("graphics/strip_start_menu/options.4bpp.smol");
static const u16 sIconsPal[] = INCBIN_U16("graphics/strip_start_menu/icons.gbapal");

static const u32 sStripBgTiles[] = INCBIN_U32("graphics/strip_start_menu/tiles.4bpp.smol");
static const u32 sStripBgTilemap[] = INCBIN_U32("graphics/strip_start_menu/map.bin.smolTM");
static const u16 sStripBgPal[] = INCBIN_U16("graphics/strip_start_menu/bg.gbapal");
static const u16 sStripBgPalDark[] = INCBIN_U16("graphics/strip_start_menu/bg_dark.gbapal");

// Each window sits on a part of the strip background and is filled with the
// colour under it: dark tab on the left, light tab on the right, dark bar at
// the bottom (entries 11, 15 and 10 of bg.pal).
static const struct WindowTemplate sStripWindowTemplates[STRIP_WIN_COUNT] =
{
    [STRIP_WIN_NAME]  = {.bg = 0, .tilemapLeft = 0,  .tilemapTop = 11, .width = 7,  .height = 2, .paletteNum = STRIP_BG_PAL, .baseBlock = 48},
    [STRIP_WIN_CLOCK] = {.bg = 0, .tilemapLeft = 20, .tilemapTop = 11, .width = 10, .height = 2, .paletteNum = STRIP_BG_PAL, .baseBlock = 62},
    // 28 tiles wide since Select joined the hints: the longest line (with the
    // Pokégear) is 188 px, and 24 tiles left it 1 px of margin.
    [STRIP_WIN_HINTS] = {.bg = 0, .tilemapLeft = 2,  .tilemapTop = 18, .width = 28, .height = 2, .paletteNum = STRIP_BG_PAL, .baseBlock = 82},
};

static const u8 sStripWindowFill[STRIP_WIN_COUNT] =
{
    [STRIP_WIN_NAME]  = 11,
    [STRIP_WIN_CLOCK] = 15,
    [STRIP_WIN_HINTS] = 10,
};

static const u8 sTextColorLight[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};
static const u8 sTextColorDark[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};

static const struct OamData sIconOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 0,
};

// Frame 0 is the grey icon, frame 1 the coloured one of the selected entry.
static const union AnimCmd sAnim_IconIdle[] = {ANIMCMD_FRAME(0, 30), ANIMCMD_END};
static const union AnimCmd sAnim_IconSelected[] = {ANIMCMD_FRAME(16, 30), ANIMCMD_END};
static const union AnimCmd *const sIconAnims[] = {sAnim_IconIdle, sAnim_IconSelected};

static const union AffineAnimCmd sAffineAnim_IconWiggle[] =
{
    AFFINEANIMCMD_FRAME(10, 10, 0, 5),
    AFFINEANIMCMD_FRAME(-5, -5, 0, 10),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),

    AFFINEANIMCMD_FRAME(0, 0, -1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),

    AFFINEANIMCMD_FRAME(0, 0, 1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),

    AFFINEANIMCMD_JUMP(3),
};

static const union AffineAnimCmd *const sIconAffineAnims[] = {sAffineAnim_IconWiggle};

static void StripMenu_OpenQuestBook(void);

// What an icon opens, once the screen is black and the strip is gone. The
// classic list's entries open the same screens through the same functions
// (StartMenu_Open*, start_menu.c), so a fix to one reaches the other.
struct StripMenuItem
{
    const u8 *label;
    struct CompressedSpriteSheet sheet;
    void (*open)(void);
};

#define ICON_SHEET(gfx, tileTag) {.data = gfx, .size = 0x400, .tag = tileTag}

static const struct StripMenuItem sStripMenuItems[STRIP_ICON_COUNT] =
{
    [STRIP_ICON_POKEDEX] = {COMPOUND_STRING("Pokédex"), ICON_SHEET(sPokedexIconGfx, STRIP_TILETAG_POKEDEX), StartMenu_OpenPokedex},
    [STRIP_ICON_DEXNAV]  = {COMPOUND_STRING("DexNav"),  ICON_SHEET(sDexNavIconGfx,  STRIP_TILETAG_DEXNAV),  StartMenu_OpenDexNav},
    [STRIP_ICON_PARTY]   = {COMPOUND_STRING("Party"),   ICON_SHEET(sPartyIconGfx,   STRIP_TILETAG_PARTY),   StartMenu_OpenParty},
    [STRIP_ICON_BAG]     = {COMPOUND_STRING("Bag"),     ICON_SHEET(sBagIconGfx,     STRIP_TILETAG_BAG),     StartMenu_OpenBag},
    [STRIP_ICON_QUESTS]  = {COMPOUND_STRING("Quests"),  ICON_SHEET(sQuestsIconGfx,  STRIP_TILETAG_QUESTS),  StripMenu_OpenQuestBook},
    [STRIP_ICON_OPTIONS] = {COMPOUND_STRING("Options"), ICON_SHEET(sOptionsIconGfx, STRIP_TILETAG_OPTIONS), StartMenu_OpenOptions},
};

static const struct SpritePalette sIconsSpritePalette = {sIconsPal, STRIP_PALTAG_ICONS};

static EWRAM_DATA struct StripMenu *sStrip = NULL;
// The entry under the cursor, kept across openings like the classic list's.
static EWRAM_DATA u8 sStripLastIcon = 0;
// The screen chosen, kept past the strip's own data, freed before it opens.
static EWRAM_DATA void (*sStripOpen)(void) = NULL;

static void Task_StripMenu_HandleInput(u8 taskId);
static void Task_StripMenu_WaitFadeIn(u8 taskId);
static void Task_StripMenu_FadeAndRun(u8 taskId);
static bool32 StripMenu_Init(void);
static void StripMenu_Destroy(void);
static void StripMenu_Close(void);
static void StripMenu_PrintLabel(void);
static void StripMenu_PrintClock(void);
static void StripMenu_PrintHints(void);
static void StripMenu_SetIconSelected(u32 index, bool32 selected);

static bool32 IsIconAvailable(enum StripIcon icon)
{
    switch (icon)
    {
    case STRIP_ICON_POKEDEX:
        return FlagGet(FLAG_SYS_POKEDEX_GET);
    case STRIP_ICON_DEXNAV:
        return DN_FLAG_DEXNAV_GET != 0 && FlagGet(DN_FLAG_DEXNAV_GET);
    case STRIP_ICON_PARTY:
        return FlagGet(FLAG_SYS_POKEMON_GET);
    default:
        return TRUE;
    }
}

static bool32 IsPokegearAvailable(void)
{
    return FlagGet(FLAG_SYS_POKENAV_GET);
}

// The player's order of the icons (gSaveBlock3Ptr->stripMenuOrder): slot i
// holds icon id + 1 in nibble i. Anything but a full permutation - all zero
// in a new game or a migrated save - is the default order.
static void GetIconOrder(u8 *order)
{
    u32 i, icon, seen = 0;

    for (i = 0; i < STRIP_ICON_COUNT; i++)
    {
        icon = (gSaveBlock3Ptr->stripMenuOrder[i / 2] >> ((i % 2) * 4)) & 0xF;
        if (icon == 0 || icon > STRIP_ICON_COUNT || (seen & (1 << icon)))
            break;
        seen |= 1 << icon;
        order[i] = icon - 1;
    }
    if (i < STRIP_ICON_COUNT)
    {
        for (i = 0; i < STRIP_ICON_COUNT; i++)
            order[i] = i;
    }
}

static void SetIconOrder(const u8 *order)
{
    u32 i;

    memset(gSaveBlock3Ptr->stripMenuOrder, 0, sizeof(gSaveBlock3Ptr->stripMenuOrder));
    for (i = 0; i < STRIP_ICON_COUNT; i++)
        gSaveBlock3Ptr->stripMenuOrder[i / 2] |= (order[i] + 1) << ((i % 2) * 4);
}

static void FreezeFieldForMenu(void)
{
    if (!IsOverworldLinkActive())
    {
        FreezeObjectEvents();
        PlayerFreeze();
        StopPlayerAvatar();
    }
    LockPlayerFieldControls();
}

void StripMenu_Open(void)
{
    FreezeFieldForMenu();
    if (!StripMenu_Init())
    {
        // Out of memory: fall back to a closed menu rather than a stuck player.
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        return;
    }
    // Wait one frame: the Start press that opened the menu is still new.
    CreateTask(Task_StripMenu_HandleInput, 80);
}

// Called from gFieldCallback2 while the screen is still black, after a screen
// opened from the strip closes.
void StripMenu_ReturnToField(void)
{
    LockPlayerFieldControls();
    StripMenu_Init();
    FadeInFromBlack();
    CreateTask(Task_StripMenu_WaitFadeIn, 80);
}

// The save dialog was cancelled: the field is visible, objects still frozen.
void StripMenu_Reopen(void)
{
    if (!StripMenu_Init())
    {
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        return;
    }
    CreateTask(Task_StripMenu_HandleInput, 80);
}

static void Task_StripMenu_WaitFadeIn(u8 taskId)
{
    if (IsWeatherNotFadingIn())
    {
        if (sStrip == NULL)
        {
            ScriptUnfreezeObjectEvents();
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
            return;
        }
        gTasks[taskId].func = Task_StripMenu_HandleInput;
    }
}

static void StripMenu_LoadBg(void)
{
    void *tilemap = GetBgTilemapBuffer(0);

    CpuFastFill(0, tilemap, BG_SCREEN_SIZE);
    DecompressAndLoadBgGfxUsingHeap(0, sStripBgTiles, 0, 0, 0);
    DecompressDataWithHeaderWram(sStripBgTilemap, tilemap);
    LoadPalette(IsDarkUiEnabled() ? sStripBgPalDark : sStripBgPal, BG_PLTT_ID(STRIP_BG_PAL), PLTT_SIZE_4BPP);
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    ScheduleBgCopyTilemapToVram(0);
}

static void StripMenu_CreateIcons(void)
{
    u32 i;
    s16 x = 24 + (STRIP_BANNER_WIDTH - sStrip->iconCount * STRIP_ICON_WIDTH) / 2;
    u8 palSlot;

    palSlot = LoadSpritePalette(&sIconsSpritePalette);
    if (palSlot != 0xFF)
        PreservePaletteInWeather(palSlot + 16);

    for (i = 0; i < sStrip->iconCount; i++)
    {
        const struct StripMenuItem *item = &sStripMenuItems[sStrip->icons[i]];
        struct SpriteTemplate template =
        {
            .tileTag = item->sheet.tag,
            .paletteTag = STRIP_PALTAG_ICONS,
            .oam = &sIconOam,
            .anims = sIconAnims,
            .images = NULL,
            .affineAnims = sIconAffineAnims,
            .callback = SpriteCallbackDummy,
        };

        sStrip->spriteIds[i] = MAX_SPRITES;
        sStrip->twinIds[i] = MAX_SPRITES;
        // A map full of object events can leave too little OBJ VRAM: the entry
        // then has no icon, but its label and the menu still work.
        if (palSlot == 0xFF || LoadCompressedSpriteSheet(&item->sheet) == TAG_NONE)
            continue;
        sStrip->spriteIds[i] = CreateSprite(&template, x + i * STRIP_ICON_WIDTH, STRIP_ICON_Y, 1);
        if (sStrip->spriteIds[i] != MAX_SPRITES && GetFlashLevel() > 0)
        {
            sStrip->twinIds[i] = CreateSprite(&template, x + i * STRIP_ICON_WIDTH, STRIP_ICON_Y, 2);
            if (sStrip->twinIds[i] != MAX_SPRITES)
            {
                gSprites[sStrip->twinIds[i]].oam.objMode = ST_OAM_OBJ_WINDOW;
                StartSpriteAnim(&gSprites[sStrip->twinIds[i]], 1);
            }
        }
    }
    StripMenu_SetIconSelected(sStrip->cursor, TRUE);
}

static bool32 StripMenu_Init(void)
{
    u32 i;
    u8 order[STRIP_ICON_COUNT];

    sStrip = AllocZeroed(sizeof(*sStrip));
    if (sStrip == NULL)
        return FALSE;

    GetIconOrder(order);
    for (i = 0; i < STRIP_ICON_COUNT; i++)
    {
        if (IsIconAvailable(order[i]))
        {
            if (order[i] == sStripLastIcon)
                sStrip->cursor = sStrip->iconCount;
            sStrip->icons[sStrip->iconCount++] = order[i];
        }
    }

    // Under Flash, outside the lit circle the field shows BG0 but no sprites
    // (WINOUT in overworld.c): each icon gets a twin drawn as object window,
    // the same trick as ShowItemIconSprite.
    sStrip->objWinWasOn = (GetGpuReg(REG_OFFSET_DISPCNT) & DISPCNT_OBJWIN_ON) != 0;
    sStrip->savedWinOut = GetGpuReg(REG_OFFSET_WINOUT);
    if (GetFlashLevel() > 0)
    {
        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJWIN_ON);
        SetGpuRegBits(REG_OFFSET_WINOUT, WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_BG0);
    }

    StripMenu_LoadBg();
    for (i = 0; i < STRIP_WIN_COUNT; i++)
    {
        sStrip->windowIds[i] = AddWindow(&sStripWindowTemplates[i]);
        FillWindowPixelBuffer(sStrip->windowIds[i], PIXEL_FILL(sStripWindowFill[i]));
        PutWindowTilemap(sStrip->windowIds[i]);
    }
    StripMenu_PrintLabel();
    StripMenu_PrintClock();
    StripMenu_PrintHints();
    StripMenu_CreateIcons();
    return TRUE;
}

static void StripMenu_Destroy(void)
{
    u32 i;

    if (sStrip == NULL)
        return;

    for (i = 0; i < sStrip->iconCount; i++)
    {
        if (sStrip->spriteIds[i] == MAX_SPRITES)
            continue;
        if (sStrip->twinIds[i] != MAX_SPRITES)
        {
            // Shares the icon's matrix: the icon frees it.
            gSprites[sStrip->twinIds[i]].oam.affineMode = ST_OAM_AFFINE_OFF;
            DestroySprite(&gSprites[sStrip->twinIds[i]]);
        }
        FreeSpriteOamMatrix(&gSprites[sStrip->spriteIds[i]]);
        DestroySprite(&gSprites[sStrip->spriteIds[i]]);
    }
    for (i = 0; i < STRIP_ICON_COUNT; i++)
        FreeSpriteTilesByTag(sStripMenuItems[i].sheet.tag);
    if (IndexOfSpritePaletteTag(STRIP_PALTAG_ICONS) != 0xFF)
        ResetPaletteColorMapType(IndexOfSpritePaletteTag(STRIP_PALTAG_ICONS) + 16);
    FreeSpritePaletteByTag(STRIP_PALTAG_ICONS);

    for (i = 0; i < STRIP_WIN_COUNT; i++)
    {
        ClearWindowTilemap(sStrip->windowIds[i]);
        RemoveWindow(sStrip->windowIds[i]);
    }
    CpuFastFill(0, GetBgTilemapBuffer(0), BG_SCREEN_SIZE);
    CpuFastFill(0, (void *)BG_CHAR_ADDR(2), STRIP_BG_TILE_COUNT * TILE_SIZE_4BPP);
    ScheduleBgCopyTilemapToVram(0);
    // The strip took palettes 14 and 15, which are the field's window frame
    // palettes (STD_WINDOW_PALETTE_NUM, DLG_WINDOW_PALETTE_NUM). Nothing
    // reloads them before the next std window: the save info window and the
    // Yes/No box drew their frame in the strip's blue, green and red until the
    // next map load. Put back what the field loaded.
    LoadMessageBoxAndBorderGfx();

    if (!sStrip->objWinWasOn)
        ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJWIN_ON);
    SetGpuReg(REG_OFFSET_WINOUT, sStrip->savedWinOut);

    FREE_AND_SET_NULL(sStrip);
}

// B: back to the field, as HideStartMenu does for the classic list.
static void StripMenu_Close(void)
{
    PlaySE(SE_SELECT);
    StripMenu_Destroy();
    StartMenu_OnClose();
    ScriptUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
}

static void StripMenu_PrintLabel(void)
{
    u8 windowId = sStrip->windowIds[STRIP_WIN_NAME];
    const u8 *label = sStripMenuItems[sStrip->icons[sStrip->cursor]].label;
    s32 x = GetStringCenterAlignXOffset(FONT_SMALL, label, sStripWindowTemplates[STRIP_WIN_NAME].width * 8);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(sStripWindowFill[STRIP_WIN_NAME]));
    AddTextPrinterParameterized4(windowId, FONT_SMALL, x, 1, 0, 0, sTextColorLight, TEXT_SKIP_DRAW, label);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static const u8 *const sWeekdayNames[WEEKDAY_COUNT] =
{
    [WEEKDAY_SUN] = COMPOUND_STRING("Sun"),
    [WEEKDAY_MON] = COMPOUND_STRING("Mon"),
    [WEEKDAY_TUE] = COMPOUND_STRING("Tue"),
    [WEEKDAY_WED] = COMPOUND_STRING("Wed"),
    [WEEKDAY_THU] = COMPOUND_STRING("Thu"),
    [WEEKDAY_FRI] = COMPOUND_STRING("Fri"),
    [WEEKDAY_SAT] = COMPOUND_STRING("Sat"),
};

// Same 12-hour clock as the classic list's time window, with the weekday the
// Pokégear radio uses.
static void StripMenu_PrintClock(void)
{
    u8 windowId = sStrip->windowIds[STRIP_WIN_CLOCK];
    u32 hours, weekday;
    u8 *end;
    s32 x;

    RtcCalcLocalTime();
    sStrip->clockMinute = gLocalTime.minutes;
    hours = gLocalTime.hours % 12;
    if (hours == 0)
        hours = 12;
    weekday = GetDayOfWeek();

    end = StringCopy(gStringVar4, sWeekdayNames[weekday % WEEKDAY_COUNT]);
    *end++ = CHAR_SPACE;
    end = ConvertIntToDecimalStringN(end, hours, STR_CONV_MODE_LEFT_ALIGN, 2);
    *end++ = CHAR_COLON;
    end = ConvertIntToDecimalStringN(end, gLocalTime.minutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    *end++ = CHAR_SPACE;
    StringCopy(end, gLocalTime.hours < 12 ? COMPOUND_STRING("AM") : COMPOUND_STRING("PM"));

    x = GetStringCenterAlignXOffset(FONT_SMALL, gStringVar4, sStripWindowTemplates[STRIP_WIN_CLOCK].width * 8);
    FillWindowPixelBuffer(windowId, PIXEL_FILL(sStripWindowFill[STRIP_WIN_CLOCK]));
    AddTextPrinterParameterized4(windowId, FONT_SMALL, x, 1, 0, 0, IsDarkUiEnabled() ? sTextColorLight : sTextColorDark, TEXT_SKIP_DRAW, gStringVar4);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

// The buttons are the game's own key glyphs (graphics/fonts/keypad_icons.png).
static const u8 sText_HintsWithPokegear[] = _("{SELECT_BUTTON} Move  {START_BUTTON} Save  {L_BUTTON} Card  {R_BUTTON} Pokégear ");
static const u8 sText_Hints[] = _("{SELECT_BUTTON} Move  {START_BUTTON} Save  {L_BUTTON} Card ");
static const u8 sText_HintsMoving[] = _("{DPAD_LEFTRIGHT} Move  {A_BUTTON} Place  {B_BUTTON} Cancel ");

static void StripMenu_PrintHints(void)
{
    u8 windowId = sStrip->windowIds[STRIP_WIN_HINTS];
    const u8 *hints;
    s32 x;

    if (sStrip->moving)
        hints = sText_HintsMoving;
    else if (IsPokegearAvailable())
        hints = sText_HintsWithPokegear;
    else
        hints = sText_Hints;

    x = GetStringRightAlignXOffset(FONT_SMALL_NARROWER, hints, sStripWindowTemplates[STRIP_WIN_HINTS].width * 8);
    FillWindowPixelBuffer(windowId, PIXEL_FILL(sStripWindowFill[STRIP_WIN_HINTS]));
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROWER, x, 1, 0, 0, sTextColorLight, TEXT_SKIP_DRAW, hints);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void StripMenu_SetIconSelected(u32 index, bool32 selected)
{
    struct Sprite *sprite, *twin = NULL;

    if (index >= sStrip->iconCount || sStrip->spriteIds[index] == MAX_SPRITES)
        return;

    sprite = &gSprites[sStrip->spriteIds[index]];
    if (sStrip->twinIds[index] != MAX_SPRITES)
        twin = &gSprites[sStrip->twinIds[index]];
    if (selected)
    {
        u8 matrixNum = AllocOamMatrix();

        StartSpriteAnim(sprite, 1);
        if (matrixNum == 0xFF)
            return;
        sprite->oam.affineMode = ST_OAM_AFFINE_NORMAL;
        sprite->oam.matrixNum = matrixNum;
        StartSpriteAffineAnim(sprite, 0);
        if (twin != NULL)
        {
            twin->oam.affineMode = ST_OAM_AFFINE_NORMAL;
            twin->oam.matrixNum = matrixNum;
        }
    }
    else
    {
        if (twin != NULL)
            twin->oam.affineMode = ST_OAM_AFFINE_OFF;
        FreeSpriteOamMatrix(sprite);
        sprite->oam.affineMode = ST_OAM_AFFINE_OFF;
        StartSpriteAnim(sprite, 0);
    }
}

static void StripMenu_MoveCursor(s32 delta)
{
    u32 next = (sStrip->cursor + sStrip->iconCount + delta) % sStrip->iconCount;

    PlaySE(SE_SELECT);
    StripMenu_SetIconSelected(sStrip->cursor, FALSE);
    sStrip->cursor = next;
    sStripLastIcon = sStrip->icons[next];
    StripMenu_SetIconSelected(sStrip->cursor, TRUE);
    StripMenu_PrintLabel();
}

// ---- Moving an icon (Select) ----
//
// The icons, their sprites and their Flash twins are permuted together, so
// the sprite at index i is always the one of icons[i]; the cursor follows the
// lifted icon. Nothing is saved until the icon is placed.

static void StripMenu_PlaceIcons(void)
{
    u32 i;
    s16 x = 24 + (STRIP_BANNER_WIDTH - sStrip->iconCount * STRIP_ICON_WIDTH) / 2;

    for (i = 0; i < sStrip->iconCount; i++)
    {
        s16 y = STRIP_ICON_Y - ((sStrip->moving && i == sStrip->cursor) ? STRIP_ICON_LIFT : 0);

        if (sStrip->spriteIds[i] != MAX_SPRITES)
        {
            gSprites[sStrip->spriteIds[i]].x = x + i * STRIP_ICON_WIDTH;
            gSprites[sStrip->spriteIds[i]].y = y;
        }
        if (sStrip->twinIds[i] != MAX_SPRITES)
        {
            gSprites[sStrip->twinIds[i]].x = x + i * STRIP_ICON_WIDTH;
            gSprites[sStrip->twinIds[i]].y = y;
        }
    }
}

static void StripMenu_SwapIcons(u32 a, u32 b)
{
    u8 temp;

    SWAP(sStrip->icons[a], sStrip->icons[b], temp);
    SWAP(sStrip->spriteIds[a], sStrip->spriteIds[b], temp);
    SWAP(sStrip->twinIds[a], sStrip->twinIds[b], temp);
}

static void StripMenu_StartMove(void)
{
    PlaySE(SE_SELECT);
    // The cursor may sit on the first icon only because the remembered one is
    // not available: from here on, the remembered one is the icon in hand.
    sStripLastIcon = sStrip->icons[sStrip->cursor];
    memcpy(sStrip->iconsBeforeMove, sStrip->icons, sizeof(sStrip->icons));
    sStrip->moving = TRUE;
    StripMenu_PlaceIcons();
    StripMenu_PrintHints();
}

static void StripMenu_MoveIcon(s32 delta)
{
    s32 next = sStrip->cursor + delta;

    // No wrapping: past an end there is no neighbour to trade places with.
    if (next < 0 || next >= sStrip->iconCount)
        return;
    PlaySE(SE_SELECT);
    StripMenu_SwapIcons(sStrip->cursor, next);
    sStrip->cursor = next;
    StripMenu_PlaceIcons();
}

static void StripMenu_EndMove(void)
{
    sStrip->moving = FALSE;
    StripMenu_PlaceIcons();
    StripMenu_PrintHints();
}

// A or Select: the visible icons take the order on screen. Icons not
// available yet keep their slots in the saved order, so each one shows up
// where it was once the player gets it.
static void StripMenu_PlaceMovedIcon(void)
{
    u8 order[STRIP_ICON_COUNT];
    u32 i, shown = 0;

    PlaySE(SE_SELECT);
    GetIconOrder(order);
    for (i = 0; i < STRIP_ICON_COUNT; i++)
    {
        if (IsIconAvailable(order[i]))
            order[i] = sStrip->icons[shown++];
    }
    SetIconOrder(order);
    StripMenu_EndMove();
}

// B: back to the order the move started from, cursor still on the same icon.
static void StripMenu_CancelMove(void)
{
    u32 i, j;

    PlaySE(SE_SELECT);
    for (i = 0; i < sStrip->iconCount; i++)
    {
        for (j = i; j < sStrip->iconCount && sStrip->icons[j] != sStrip->iconsBeforeMove[i]; j++)
            ;
        if (j != i && j < sStrip->iconCount)
            StripMenu_SwapIcons(i, j);
        if (sStrip->icons[i] == sStripLastIcon)
            sStrip->cursor = i;
    }
    StripMenu_EndMove();
}

static void StripMenu_HandleMoveInput(void)
{
    if (JOY_NEW(DPAD_LEFT))
        StripMenu_MoveIcon(-1);
    else if (JOY_NEW(DPAD_RIGHT))
        StripMenu_MoveIcon(1);
    else if (JOY_NEW(A_BUTTON | SELECT_BUTTON))
        StripMenu_PlaceMovedIcon();
    else if (JOY_NEW(B_BUTTON))
        StripMenu_CancelMove();
}

static void StripMenu_StartCallback(u8 taskId, void (*open)(void))
{
    PlaySE(SE_SELECT);
    sStripOpen = open;
    FadeScreen(FADE_TO_BLACK, 0);
    gTasks[taskId].func = Task_StripMenu_FadeAndRun;
}

static void Task_StripMenu_HandleInput(u8 taskId)
{
    if (gTasks[taskId].data[0] == 0)
    {
        gTasks[taskId].data[0] = 1;
        return;
    }

    RtcCalcLocalTime();
    if (sStrip->clockMinute != gLocalTime.minutes)
        StripMenu_PrintClock();

    if (sStrip->moving)
    {
        StripMenu_HandleMoveInput();
        return;
    }

    if (JOY_NEW(DPAD_LEFT))
    {
        StripMenu_MoveCursor(-1);
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        StripMenu_MoveCursor(1);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        enum StripIcon icon = sStrip->icons[sStrip->cursor];

        // Same rule as the classic list: an empty Pokédex does not open.
        if (icon == STRIP_ICON_POKEDEX && GetNationalPokedexCount(FLAG_GET_SEEN) == 0)
            return;
        StripMenu_StartCallback(taskId, sStripMenuItems[icon].open);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        StripMenu_Close();
        DestroyTask(taskId);
    }
    else if (JOY_NEW(START_BUTTON))
    {
        PlaySE(SE_SELECT);
        StripMenu_Destroy();
        DestroyTask(taskId);
        StartMenu_SaveFromStrip();
    }
    else if (JOY_NEW(L_BUTTON))
    {
        StripMenu_StartCallback(taskId, StartMenu_OpenTrainerCard);
    }
    else if (JOY_NEW(R_BUTTON) && IsPokegearAvailable())
    {
        StripMenu_StartCallback(taskId, StartMenu_OpenPokegear);
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        StripMenu_StartMove();
    }
}

static void Task_StripMenu_FadeAndRun(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StripMenu_Destroy();
        PlayRainStoppingSoundEffect();
        sStripOpen();
        DestroyTask(taskId);
    }
}

static void StripMenu_OpenQuestBook(void)
{
    CreateTask(Task_OpenQuestBookFromStartMenu, 0);
}
