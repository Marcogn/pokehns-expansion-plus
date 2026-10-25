#include "global.h"
#include "bg.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "gpu_regs.h"
#include "item_icon.h"
#include "list_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "option_menu.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon_icon.h"
#include "quest_book.h"
#include "quest_log.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// The quest book screen (docs/plans/quest-log.md, phase 2). Ported from
// PokemonSanFran's complex_quests branch (src/quests.c, itself the Unbound
// quest menu by ghoulslash and others). What changed:
//  - state comes from quest_log.c (a var, a function or the book's own bits)
//    instead of five bits per quest in SaveBlock2;
//  - every row string is bounded: PSF appended to 32-byte buffers unchecked;
//  - one allocation for the rows, made once: PSF reallocated the list items
//    on every filter change and never freed the old ones;
//  - the list is rebuilt in place, with no blend fade between lists;
//  - no change to ListMenu_ProcessInput: PSF made every list in the game wrap
//    around, here the book's list simply stops at its ends.

#define ROW_NAME_LEN        48
#define ROWS_SHOWN          4
#define SPRITE_X            20
#define SPRITE_Y            132
#define ITEM_ICON_TAG       102
#define SCROLL_ARROW_TAG    110
#define FOOTER_WIN_HEIGHT   8

enum
{
    FILTER_ALL,
    FILTER_UNLOCKED, // PSF's "inactive": known, nothing done yet
    FILTER_ACTIVE,
    FILTER_REWARD,
    FILTER_COMPLETED,
    FILTER_COUNT,
};

enum
{
    WIN_LIST,
    WIN_FOOTER,
    WIN_HEADER,
};

enum
{
    COLOR_HEADER,
    COLOR_HEADER_DARK,
    COLOR_REWARD,
    COLOR_DONE,
    COLOR_ACTIVE,
    COLOR_FOOTER,
};

struct QuestBook
{
    MainCallback savedCallback;
    u8 *tilemap;
    struct ListMenuItem *items;
    u8 *rowNames;
    u8 *order;              // the quests the list shows, in order
    u8 maxRows;             // capacity of items, rowNames and order
    u8 numRows;             // rows in the list, "Close" included
    u8 filter;              // FILTER_*
    bool8 alpha;
    bool8 inSubquests;
    u8 parentQuest;
    u8 listTaskId;
    u8 arrowTaskId;
    u8 spriteSlot;
    u8 spriteId[2];
    u8 spriteType[2];
    u16 spriteGfx[2];
    u16 scroll;
    u16 row;
    u16 parentScroll;       // where the main list was left on entering a quest
    u16 parentRow;
    u8 states[QUEST_COUNT]; // QUEST_STATE_*, see ReadQuestStates
};

static EWRAM_DATA struct QuestBook *sBook = NULL;

static void QuestBook_Setup(void);
static void CB2_QuestBook(void);
static void VBlankCB_QuestBook(void);
static void Task_QuestBookWaitFadeIn(u8 taskId);
static void Task_QuestBookMain(u8 taskId);
static void Task_QuestBookClose(u8 taskId);
static void Task_OpenQuestBookFromScript(u8 taskId);
static void MoveCursorFunc(s32 id, bool8 onInit, struct ListMenu *list);
static void PrintStateTag(u8 windowId, u32 id, u8 y);
static void RebuildList(bool32 restoreCursor);
static void RebuildListOn(bool32 restoreCursor, s32 focusId);
static void ShowDetails(s32 id);
static void SetPictureBoxVisible(bool32 visible);
static const u8 *GetHeaderColors(void);

static const u32 sQuestBookTiles[] = INCBIN_U32("graphics/quest_book/menu.4bpp.smol");
static const u32 sQuestBookTilemap[] = INCBIN_U32("graphics/quest_book/menu.bin.smolTM");
static const u16 sQuestBookPalette[] = INCBIN_U16("graphics/quest_book/menu.gbapal");
static const u16 sQuestBookPaletteDark[] = INCBIN_U16("graphics/quest_book/menu_dark.gbapal");

// Entries of the book palette that only the picture box uses (fill and border,
// counted on menu.4bpp): with no picture they take the footer's colour, which
// makes the box disappear without touching the tilemap.
#define PICTURE_BOX_FILL    1
#define PICTURE_BOX_BORDER  6
#define FOOTER_FILL         10

// Text colours for the dark theme, on palette 15. The standard menu palette
// has dark grey text for a light page; the page here is near black. The light
// text goes on its own entry and 2 stays dark grey: the A and R button glyphs
// are a white pill with a letter in entry 2, and a light letter on the white
// pill can't be read. The red/green/blue state tags get brighter versions.
#define DARK_THEME_TEXT 10

static const u16 sDarkTextPalette[PLTT_SIZE_4BPP / sizeof(u16)] =
{
    [0] = RGB_WHITE,
    [1] = RGB_WHITE,
    [2] = RGB(12, 12, 12),
    [3] = RGB(26, 26, 25),
    [4] = RGB(31, 12, 12),  // red: NEW, reward
    [5] = RGB(31, 23, 14),
    [6] = RGB(11, 27, 10),  // green: done, favourites
    [7] = RGB(18, 30, 18),
    [8] = RGB(14, 21, 31),  // blue: active
    [9] = RGB(20, 24, 30),
    [DARK_THEME_TEXT] = RGB(28, 27, 22),
};

static const u8 sText_AllHeader[] = _("All Quests");
static const u8 sText_UnlockedHeader[] = _("Not Started");
static const u8 sText_ActiveHeader[] = _("Active Quests");
static const u8 sText_RewardHeader[] = _("Reward Waiting");
static const u8 sText_CompletedHeader[] = _("Completed Quests");
static const u8 sText_AlphaSuffix[] = _(" A-Z");
static const u8 sText_Count[] = _("{STR_VAR_1}/{STR_VAR_2}");
static const u8 sText_Locked[] = _("??????");
static const u8 sText_Active[] = _("Active");
static const u8 sText_Reward[] = _("Reward");
static const u8 sText_Done[] = _("Done");
static const u8 sText_Location[] = _("Location: {STR_VAR_2}");
static const u8 sText_NothingYet[] = _("Nothing to do yet.");
static const u8 sText_ReturnForReward[] = _("Your reward is waiting.\nReturn to {STR_VAR_2}!");
static const u8 sText_TypeButton[] = _("{R_BUTTON}Type");
static const u8 sText_Close[] = _("Close");
static const u8 sText_Back[] = _("Back");
static const u8 sText_ColorGreen[] = _("{COLOR GREEN}");
static const u8 sText_ColorRed[] = _("{COLOR RED} NEW");
static const u8 sText_SubquestButton[] = _(" {A_BUTTON}");
static const u8 sText_DotSpace[] = _(". ");

static const struct BgTemplate sBgTemplates[] =
{
    {
        // all text
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 1
    },
    {
        // the panel behind it
        .bg = 1,
        .charBaseIndex = 3,
        .mapBaseIndex = 30,
        .priority = 2
    },
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_LIST] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 2,
        .width = 30,
        .height = 8,
        .paletteNum = 15,
        .baseBlock = 1
    },
    [WIN_FOOTER] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 12,
        .width = 30,
        .height = FOOTER_WIN_HEIGHT,
        .paletteNum = 15,
        .baseBlock = 241
    },
    [WIN_HEADER] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 481
    },
    DUMMY_WIN_TEMPLATE
};

// Background, foreground, shadow.
static const u8 sFontColors[][3] =
{
    [COLOR_HEADER] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_TRANSPARENT },
    [COLOR_HEADER_DARK] = { TEXT_COLOR_TRANSPARENT, DARK_THEME_TEXT, TEXT_COLOR_TRANSPARENT },
    [COLOR_REWARD] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED,       TEXT_COLOR_TRANSPARENT },
    [COLOR_DONE]   = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_GREEN,     TEXT_COLOR_TRANSPARENT },
    [COLOR_ACTIVE] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE,      TEXT_COLOR_TRANSPARENT },
    [COLOR_FOOTER] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,     TEXT_COLOR_TRANSPARENT },
};

static const u8 *const sFilterHeaders[FILTER_COUNT] =
{
    [FILTER_ALL] = sText_AllHeader,
    [FILTER_UNLOCKED] = sText_UnlockedHeader,
    [FILTER_ACTIVE] = sText_ActiveHeader,
    [FILTER_REWARD] = sText_RewardHeader,
    [FILTER_COMPLETED] = sText_CompletedHeader,
};

// The quest state each filter keeps; FILTER_ALL keeps all of them.
static const u8 sFilterStates[FILTER_COUNT] =
{
    [FILTER_UNLOCKED] = QUEST_STATE_UNLOCKED,
    [FILTER_ACTIVE] = QUEST_STATE_ACTIVE,
    [FILTER_REWARD] = QUEST_STATE_REWARD,
    [FILTER_COMPLETED] = QUEST_STATE_COMPLETED,
};

// ---- Entry points ----

void QuestBook_Init(MainCallback callback)
{
    sBook = Alloc(sizeof(*sBook));
    if (sBook == NULL)
    {
        SetMainCallback2(callback);
        return;
    }
    memset(sBook, 0, sizeof(*sBook));
    sBook->savedCallback = callback;
    sBook->spriteId[0] = sBook->spriteId[1] = SPRITE_NONE;
    SetMainCallback2(QuestBook_Setup);
}

void Task_OpenQuestBookFromStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        QuestBook_Init(CB2_ReturnToFieldWithOpenMenu);
        DestroyTask(taskId);
    }
}

// questlog QUEST_LOG_OPEN. The script has to follow it with waitstate: the
// field resumes it through CB2_ReturnToFieldContinueScriptPlayMapMusic.
void QuestBook_OpenFromScript(void)
{
    LockPlayerFieldControls();
    FadeScreen(FADE_TO_BLACK, 0);
    CreateTask(Task_OpenQuestBookFromScript, 10);
}

static void Task_OpenQuestBookFromScript(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        QuestBook_Init(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        DestroyTask(taskId);
    }
}

// ---- Setup ----

static void FreeBook(void)
{
    if (sBook == NULL)
        return;
    Free(sBook->tilemap);
    Free(sBook->items);
    Free(sBook->rowNames);
    Free(sBook->order);
    Free(sBook);
    sBook = NULL;
    FreeAllWindowBuffers();
}

static u32 GetMaxRows(void)
{
    u32 i, rows = QUEST_COUNT;

    for (i = 0; i < QUEST_COUNT; i++)
    {
        if (QuestLog_GetSubquestCount(i) > rows)
            rows = QuestLog_GetSubquestCount(i);
    }
    return rows;
}

static bool32 AllocRows(void)
{
    u32 rows = GetMaxRows();

    // One more for "Close". Never zero-sized, so a book with no quests
    // (QUEST_COUNT 0) still allocates.
    sBook->maxRows = rows + 1;
    sBook->items = Alloc(sizeof(struct ListMenuItem) * sBook->maxRows);
    sBook->rowNames = Alloc(ROW_NAME_LEN * sBook->maxRows);
    sBook->order = Alloc(sBook->maxRows);
    return sBook->items != NULL && sBook->rowNames != NULL && sBook->order != NULL;
}

static void BailOut(void)
{
    // Out of memory: back to where the player came from, as if nothing happened.
    MainCallback callback = sBook->savedCallback;

    FreeBook();
    SetMainCallback2(callback);
}

static void QuestBook_Setup(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        ResetAllBgsCoordinates();
        ResetBgsAndClearDma3BusyFlags(0);
        sBook->tilemap = Alloc(BG_SCREEN_SIZE);
        if (sBook->tilemap == NULL || !AllocRows())
        {
            BailOut();
            return;
        }
        memset(sBook->tilemap, 0, BG_SCREEN_SIZE);
        InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
        SetBgTilemapBuffer(1, sBook->tilemap);
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sQuestBookTiles, 0, 0, 0);
        gMain.state++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible())
            return;
        CopyToBgTilemapBuffer(1, sQuestBookTilemap, 0, 0);
        // The windows print on palette 15; it holds the text colours.
        if (IsDarkUiEnabled())
        {
            LoadPalette(sQuestBookPaletteDark, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
            LoadPalette(sDarkTextPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        }
        else
        {
            LoadPalette(sQuestBookPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
            LoadPalette(gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        }
        SetPictureBoxVisible(FALSE);
        InitWindows(sWindowTemplates);
        DeactivateAllTextPrinters();
        FillWindowPixelBuffer(WIN_LIST, PIXEL_FILL(0));
        FillWindowPixelBuffer(WIN_FOOTER, PIXEL_FILL(0));
        FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(0));
        PutWindowTilemap(WIN_LIST);
        PutWindowTilemap(WIN_FOOTER);
        PutWindowTilemap(WIN_HEADER);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        ShowBg(0);
        ShowBg(1);
        ScheduleBgCopyTilemapToVram(0);
        ScheduleBgCopyTilemapToVram(1);
        gMain.state++;
        break;
    case 2:
        sBook->listTaskId = TASK_NONE;
        sBook->arrowTaskId = TASK_NONE;
        CreateTask(Task_QuestBookWaitFadeIn, 0);
        QuestLog_SyncSeen();
        RebuildList(FALSE);
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB_QuestBook);
        SetMainCallback2(CB2_QuestBook);
        gMain.state = 0;
        break;
    }
}

static void CB2_QuestBook(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCB_QuestBook(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_QuestBookWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_QuestBookMain;
}

// ---- The list ----

// Every quest's state, read once per list rebuild: a function quest runs its
// progress function on each read, and the sort, the rows, the header and the
// state tags all ask.
static void ReadQuestStates(void)
{
    u32 quest;

    for (quest = 0; quest < QUEST_COUNT; quest++)
        sBook->states[quest] = QuestLog_GetState(quest);
}

static bool32 HasSubquestList(u32 quest)
{
    return QuestLog_GetSubquestCount(quest) != 0 && sBook->states[quest] >= QUEST_STATE_ACTIVE;
}

// Favourites first; then A-Z (quests still locked last, so the order does not
// give their names away) or in id order.
static bool32 QuestComesBefore(u32 a, u32 b)
{
    bool32 favA = QuestLog_IsFavorite(a);
    bool32 favB = QuestLog_IsFavorite(b);

    if (favA != favB)
        return favA;
    if (sBook->alpha)
    {
        bool32 lockedA = sBook->states[a] == QUEST_STATE_LOCKED;
        bool32 lockedB = sBook->states[b] == QUEST_STATE_LOCKED;

        if (lockedA != lockedB)
            return lockedB;
        if (!lockedA)
        {
            s32 compare = StringCompare(QuestLog_GetName(a), QuestLog_GetName(b));

            if (compare != 0)
                return compare < 0;
        }
    }
    return a < b;
}

static u32 CollectQuests(void)
{
    u32 quest, i, count = 0;

    for (quest = 0; quest < QUEST_COUNT; quest++)
    {
        if (sBook->filter == FILTER_ALL || sBook->states[quest] == sFilterStates[sBook->filter])
            sBook->order[count++] = quest;
    }
    // Insertion sort: QUEST_COUNT is small.
    for (quest = 1; quest < count; quest++)
    {
        u8 value = sBook->order[quest];

        for (i = quest; i > 0 && QuestComesBefore(value, sBook->order[i - 1]); i--)
            sBook->order[i] = sBook->order[i - 1];
        sBook->order[i] = value;
    }
    return count;
}

static void AppendBounded(u8 *dst, const u8 *src)
{
    u32 length = StringLength(dst);

    if (length < ROW_NAME_LEN - 1)
        StringAppendN(dst, src, ROW_NAME_LEN - 1 - length);
}

static u8 *GetRowName(u32 row)
{
    return &sBook->rowNames[row * ROW_NAME_LEN];
}

static void BuildQuestRowName(u8 *dst, u32 quest)
{
    u32 state = sBook->states[quest];

    dst[0] = EOS;
    if (state == QUEST_STATE_LOCKED)
    {
        AppendBounded(dst, sText_Locked);
        return;
    }
    if (QuestLog_IsFavorite(quest))
        AppendBounded(dst, sText_ColorGreen);
    AppendBounded(dst, QuestLog_GetName(quest));
    if (HasSubquestList(quest))
        AppendBounded(dst, sText_SubquestButton);
    if (QuestLog_IsNew(quest, state))
        AppendBounded(dst, sText_ColorRed);
}

static void BuildSubquestRowName(u8 *dst, u32 quest, u32 subquest)
{
    const struct QuestInfo *info = QuestLog_GetInfo(quest);

    ConvertIntToDecimalStringN(dst, subquest + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
    AppendBounded(dst, sText_DotSpace);
    if (QuestLog_IsSubquestDone(quest, subquest) && info->subquestInfo != NULL)
        AppendBounded(dst, info->subquestInfo[subquest].name);
    else
        AppendBounded(dst, sText_Locked);
}

static void BuildItems(void)
{
    u32 i, count;

    if (sBook->inSubquests)
    {
        count = QuestLog_GetSubquestCount(sBook->parentQuest);
        for (i = 0; i < count; i++)
        {
            BuildSubquestRowName(GetRowName(i), sBook->parentQuest, i);
            sBook->items[i].name = GetRowName(i);
            sBook->items[i].id = i;
        }
    }
    else
    {
        count = CollectQuests();
        for (i = 0; i < count; i++)
        {
            BuildQuestRowName(GetRowName(i), sBook->order[i]);
            sBook->items[i].name = GetRowName(i);
            sBook->items[i].id = sBook->order[i];
        }
    }
    sBook->items[count].name = sBook->inSubquests ? sText_Back : sText_Close;
    sBook->items[count].id = LIST_CANCEL;
    sBook->numRows = count + 1;
}

static void PrintHeader(void)
{
    u32 total, shown, i;
    const u8 *context;

    if (sBook->inSubquests)
    {
        total = QuestLog_GetSubquestCount(sBook->parentQuest);
        shown = QuestLog_CountSubquestsDone(sBook->parentQuest);
        context = QuestLog_GetName(sBook->parentQuest);
    }
    else
    {
        total = QUEST_COUNT;
        shown = 0;
        for (i = 0; i < QUEST_COUNT; i++)
        {
            u32 state = sBook->states[i];

            if (sBook->filter == FILTER_ALL ? state != QUEST_STATE_LOCKED : state == sFilterStates[sBook->filter])
                shown++;
        }
        context = sFilterHeaders[sBook->filter];
    }

    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(0));
    StringCopy(gStringVar3, context);
    if (!sBook->inSubquests && sBook->alpha)
        StringAppend(gStringVar3, sText_AlphaSuffix);
    AddTextPrinterParameterized4(WIN_HEADER, FONT_SMALL, 10, 1, 0, 1, GetHeaderColors(), 0, gStringVar3);

    ConvertIntToDecimalStringN(gStringVar1, shown, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, total, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, sText_Count);
    AddTextPrinterParameterized4(WIN_HEADER, FONT_SMALL, 167, 1, 0, 1, GetHeaderColors(), 0, gStringVar4);

    if (!sBook->inSubquests)
        AddTextPrinterParameterized4(WIN_HEADER, FONT_SMALL, 198, 1, 0, 1, GetHeaderColors(), 0, sText_TypeButton);
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
}

static void RemoveArrows(void)
{
    if (sBook->arrowTaskId != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sBook->arrowTaskId);
        sBook->arrowTaskId = TASK_NONE;
    }
}

static void DestroyList(void)
{
    RemoveArrows();
    if (sBook->listTaskId != TASK_NONE)
    {
        DestroyListMenuTask(sBook->listTaskId, &sBook->scroll, &sBook->row);
        sBook->listTaskId = TASK_NONE;
    }
}

// The cursor of a list that has shrunk, or of a different list, has to land
// on a row that exists.
static void ClampCursor(void)
{
    u32 maxShowed = min(sBook->numRows, ROWS_SHOWN);

    if (sBook->scroll + maxShowed > sBook->numRows)
        sBook->scroll = sBook->numRows - maxShowed;
    if (sBook->row >= maxShowed)
        sBook->row = maxShowed - 1;
}

static void RebuildList(bool32 restoreCursor)
{
    RebuildListOn(restoreCursor, LIST_NOTHING_CHOSEN);
}

// With a focus id, the cursor lands on that row (after Select the quest has
// moved, favourites go first), scrolling as little as possible.
static void RebuildListOn(bool32 restoreCursor, s32 focusId)
{
    u32 i, maxShowed;

    DestroyList();
    ReadQuestStates();
    BuildItems();
    maxShowed = min(sBook->numRows, ROWS_SHOWN);

    if (restoreCursor)
    {
        sBook->scroll = sBook->parentScroll;
        sBook->row = sBook->parentRow;
    }
    else
    {
        sBook->scroll = 0;
        sBook->row = 0;
    }
    for (i = 0; focusId != LIST_NOTHING_CHOSEN && i < sBook->numRows; i++)
    {
        if (sBook->items[i].id == focusId)
        {
            sBook->scroll = i < maxShowed ? 0 : i - (maxShowed - 1);
            sBook->row = i - sBook->scroll;
            break;
        }
    }
    ClampCursor();

    gMultiuseListMenuTemplate.items = sBook->items;
    gMultiuseListMenuTemplate.totalItems = sBook->numRows;
    gMultiuseListMenuTemplate.maxShowed = maxShowed;
    gMultiuseListMenuTemplate.windowId = WIN_LIST;
    gMultiuseListMenuTemplate.header_X = 0;
    gMultiuseListMenuTemplate.item_X = 23;
    gMultiuseListMenuTemplate.cursor_X = 15;
    gMultiuseListMenuTemplate.upText_Y = 1;
    gMultiuseListMenuTemplate.cursorPal = GetHeaderColors()[1];
    gMultiuseListMenuTemplate.fillValue = 0;
    gMultiuseListMenuTemplate.cursorShadowPal = TEXT_COLOR_TRANSPARENT;
    gMultiuseListMenuTemplate.lettersSpacing = 1;
    gMultiuseListMenuTemplate.itemVerticalPadding = 0;
    gMultiuseListMenuTemplate.scrollMultiple = LIST_MULTIPLE_SCROLL_DPAD;
    gMultiuseListMenuTemplate.fontId = FONT_NARROW;
    gMultiuseListMenuTemplate.cursorKind = CURSOR_BLACK_ARROW;
    gMultiuseListMenuTemplate.moveCursorFunc = MoveCursorFunc;
    gMultiuseListMenuTemplate.itemPrintFunc = PrintStateTag;

    FillWindowPixelBuffer(WIN_LIST, PIXEL_FILL(0));
    PrintHeader();
    sBook->listTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sBook->scroll, sBook->row);
    sBook->arrowTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 228, 19, 77,
                                                                  sBook->numRows - maxShowed,
                                                                  SCROLL_ARROW_TAG, SCROLL_ARROW_TAG,
                                                                  &sBook->scroll);
}

// The right-hand tag of a row: the state of a quest, what a done subquest was.
static void PrintStateTag(u8 windowId, u32 id, u8 y)
{
    const u8 *text = NULL;
    u32 color = IsDarkUiEnabled() ? COLOR_HEADER_DARK : COLOR_HEADER;

    if (id == (u32)LIST_CANCEL)
        return;

    if (sBook->inSubquests)
    {
        const struct QuestInfo *info = QuestLog_GetInfo(sBook->parentQuest);

        if (QuestLog_IsSubquestDone(sBook->parentQuest, id) && info->subquestInfo != NULL)
            text = info->subquestInfo[id].type;
        color = COLOR_DONE;
    }
    else
    {
        switch (sBook->states[id])
        {
        case QUEST_STATE_COMPLETED:
            text = sText_Done;
            color = COLOR_DONE;
            break;
        case QUEST_STATE_REWARD:
            text = sText_Reward;
            color = COLOR_REWARD;
            break;
        case QUEST_STATE_ACTIVE:
            text = sText_Active;
            color = COLOR_ACTIVE;
            break;
        }
    }
    if (text != NULL)
        AddTextPrinterParameterized4(windowId, FONT_SMALL, 200, y, 0, 0, sFontColors[color], TEXT_SKIP_DRAW, text);
}

// ---- The picture and the text under the list ----

static void DestroyPicture(u32 slot)
{
    u8 spriteId = sBook->spriteId[slot];

    if (spriteId == SPRITE_NONE)
        return;
    switch (sBook->spriteType[slot])
    {
    case QUEST_SPRITE_OBJECT:
        // Frees the sheet and palette the sprite loaded: with OW_GFX_COMPRESS
        // they stay in VRAM otherwise, one more for every quest browsed.
        DestroySpriteAndFreeResources(&gSprites[spriteId]);
        break;
    case QUEST_SPRITE_ITEM:
        DestroySprite(&gSprites[spriteId]);
        FreeSpriteTilesByTag(ITEM_ICON_TAG + slot);
        FreeSpritePaletteByTag(ITEM_ICON_TAG + slot);
        break;
    case QUEST_SPRITE_MON:
        FreeAndDestroyMonIconSprite(&gSprites[spriteId]);
        break;
    }
    sBook->spriteId[slot] = SPRITE_NONE;
}

// Built in the other slot and the old one dropped after, so that the item
// icon's tile and palette tags never collide.
static void ShowPicture(u32 sprite, u32 type)
{
    u32 slot = sBook->spriteSlot ^ 1;
    u8 spriteId = SPRITE_NONE;

    // The same picture as the one shown stays: destroying it after creating
    // the "new" one would free the sheet and palette they share.
    if (sBook->spriteId[sBook->spriteSlot] != SPRITE_NONE
     && sBook->spriteType[sBook->spriteSlot] == type
     && sBook->spriteGfx[sBook->spriteSlot] == sprite)
        return;
    if (sBook->spriteId[sBook->spriteSlot] == SPRITE_NONE && type == QUEST_SPRITE_NONE)
        return;
    DestroyPicture(slot);
    // Two object pictures can share a palette (Pryce and Falkner do):
    // destroying the old one after creating the new one would free the
    // palette out from under it and draw it magenta. Objects go one at a time.
    if (type == QUEST_SPRITE_OBJECT && sBook->spriteType[sBook->spriteSlot] == QUEST_SPRITE_OBJECT)
    {
        DestroyPicture(sBook->spriteSlot);
        slot = sBook->spriteSlot;
    }
    switch (type)
    {
    case QUEST_SPRITE_OBJECT:
        spriteId = CreateObjectGraphicsSprite(sprite, SpriteCallbackDummy, SPRITE_X, SPRITE_Y, 0);
        break;
    case QUEST_SPRITE_ITEM:
        spriteId = AddItemIconSprite(ITEM_ICON_TAG + slot, ITEM_ICON_TAG + slot, sprite);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].x = SPRITE_X + 4;
            gSprites[spriteId].y = SPRITE_Y + 8;
        }
        break;
    case QUEST_SPRITE_MON:
        LoadMonIconPalettes();
        spriteId = CreateMonIcon(sprite, SpriteCallbackDummy, SPRITE_X, SPRITE_Y, 0, 0);
        break;
    }
    // A picture that could not be made is just not shown: the text is the point.
    if (spriteId == MAX_SPRITES)
        spriteId = SPRITE_NONE;
    else if (spriteId != SPRITE_NONE)
        gSprites[spriteId].oam.priority = 0;
    sBook->spriteId[slot] = spriteId;
    sBook->spriteType[slot] = type;
    sBook->spriteGfx[slot] = sprite;
    if (slot != sBook->spriteSlot)
        DestroyPicture(sBook->spriteSlot);
    sBook->spriteSlot = slot;
    SetPictureBoxVisible(sBook->spriteId[slot] != SPRITE_NONE);
}

static const u8 *GetHeaderColors(void)
{
    return sFontColors[IsDarkUiEnabled() ? COLOR_HEADER_DARK : COLOR_HEADER];
}

static void SetPictureBoxVisible(bool32 visible)
{
    const u16 *pal = IsDarkUiEnabled() ? sQuestBookPaletteDark : sQuestBookPalette;
    u16 colors[2];

    colors[0] = pal[visible ? PICTURE_BOX_FILL : FOOTER_FILL];
    colors[1] = pal[visible ? PICTURE_BOX_BORDER : FOOTER_FILL];
    // Two entries written one by one: they are not next to each other.
    LoadPalette(&colors[0], BG_PLTT_ID(0) + PICTURE_BOX_FILL, sizeof(u16));
    LoadPalette(&colors[1], BG_PLTT_ID(0) + PICTURE_BOX_BORDER, sizeof(u16));
}

static void PrintFooter(const u8 *map, const u8 *desc, void (*bufferText)(void))
{
    FillWindowPixelBuffer(WIN_FOOTER, PIXEL_FILL(0));
    if (bufferText != NULL)
        bufferText();
    if (map != NULL)
    {
        StringCopy(gStringVar2, map);
        StringExpandPlaceholders(gStringVar4, sText_Location);
        AddTextPrinterParameterized4(WIN_FOOTER, FONT_NARROW, 2, 3, 2, 0, sFontColors[COLOR_FOOTER], TEXT_SKIP_DRAW, gStringVar4);
    }
    if (desc != NULL)
    {
        StringCopy(gStringVar2, map != NULL ? map : gText_EmptyString2);
        StringExpandPlaceholders(gStringVar4, desc);
        AddTextPrinterParameterized4(WIN_FOOTER, FONT_NARROW, 40, 19, 1, 0, sFontColors[COLOR_FOOTER], TEXT_SKIP_DRAW, gStringVar4);
    }
    CopyWindowToVram(WIN_FOOTER, COPYWIN_GFX);
}

static void ShowSubquestDetails(u32 subquest)
{
    const struct QuestInfo *info = QuestLog_GetInfo(sBook->parentQuest);

    if (QuestLog_IsSubquestDone(sBook->parentQuest, subquest) && info->subquestInfo != NULL)
    {
        const struct QuestSubquestInfo *sub = &info->subquestInfo[subquest];

        PrintFooter(sub->map, sub->desc, info->bufferText);
        ShowPicture(sub->sprite, sub->spriteType);
    }
    else
    {
        PrintFooter(NULL, NULL, NULL);
        ShowPicture(0, QUEST_SPRITE_NONE);
    }
}

static void ShowQuestDetails(u32 quest)
{
    const struct QuestInfo *info = QuestLog_GetInfo(quest);
    u32 state = sBook->states[quest];
    const struct QuestStep *step = NULL;
    const u8 *map, *desc;

    if (state == QUEST_STATE_LOCKED)
    {
        PrintFooter(NULL, NULL, NULL);
        ShowPicture(0, QUEST_SPRITE_NONE);
        return;
    }
    // A quest with no texts yet is shown, just empty.
    if (state == QUEST_STATE_COMPLETED && info->done != NULL)
        step = info->done;
    else if (info->steps != NULL && info->numSteps != 0)
        step = &info->steps[QuestLog_GetStep(quest)];
    map = step != NULL ? step->map : NULL;
    if (map == NULL && step != info->done && info->steps != NULL)
        map = info->steps[0].map;

    switch (state)
    {
    case QUEST_STATE_COMPLETED:
        desc = step != NULL ? step->desc : NULL;
        break;
    case QUEST_STATE_REWARD:
        // The step's own text if it has one: where to collect may not be a
        // place ("any TcloneR scientist").
        desc = step != NULL && step->desc != NULL ? step->desc : sText_ReturnForReward;
        break;
    default:
        desc = step != NULL ? step->desc : sText_NothingYet;
        break;
    }
    PrintFooter(map, desc, info->bufferText);
    if (step != NULL)
        ShowPicture(step->sprite, step->spriteType);
    else
        ShowPicture(0, QUEST_SPRITE_NONE);
}

static void ShowDetails(s32 id)
{
    if (id == LIST_CANCEL)
    {
        PrintFooter(NULL, NULL, NULL);
        ShowPicture(0, QUEST_SPRITE_NONE);
    }
    else if (sBook->inSubquests)
    {
        ShowSubquestDetails(id);
    }
    else
    {
        QuestLog_MarkSeen(id);
        ShowQuestDetails(id);
    }
}

static void MoveCursorFunc(s32 id, bool8 onInit, struct ListMenu *list)
{
    if (!onInit)
        PlaySE(SE_RG_BAG_CURSOR);
    ShowDetails(id);
}

// ---- Input ----

static s32 GetSelectedId(void)
{
    ListMenuGetScrollAndRow(sBook->listTaskId, &sBook->scroll, &sBook->row);
    return sBook->items[sBook->scroll + sBook->row].id;
}

static void Task_QuestBookMain(u8 taskId)
{
    s32 input;

    if (gPaletteFade.active)
        return;

    input = ListMenu_ProcessInput(sBook->listTaskId);
    ListMenuGetScrollAndRow(sBook->listTaskId, &sBook->scroll, &sBook->row);

    switch (input)
    {
    case LIST_NOTHING_CHOSEN:
        if (sBook->inSubquests)
            break;
        if (JOY_NEW(R_BUTTON))
        {
            PlaySE(SE_SELECT);
            sBook->filter = (sBook->filter + 1) % FILTER_COUNT;
            RebuildList(FALSE);
        }
        else if (JOY_NEW(START_BUTTON))
        {
            PlaySE(SE_SELECT);
            sBook->alpha ^= TRUE;
            RebuildList(FALSE);
        }
        else if (JOY_NEW(SELECT_BUTTON))
        {
            s32 id = GetSelectedId();

            if (id != LIST_CANCEL)
            {
                PlaySE(SE_SELECT);
                QuestLog_SetFavorite(id, !QuestLog_IsFavorite(id));
                RebuildListOn(FALSE, id);
            }
        }
        break;
    case LIST_CANCEL:
        PlaySE(SE_SELECT);
        if (sBook->inSubquests)
        {
            sBook->inSubquests = FALSE;
            RebuildList(TRUE);
        }
        else
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_QuestBookClose;
        }
        break;
    default:
        // A on a quest with a subquest list opens it.
        if (!sBook->inSubquests && HasSubquestList(input))
        {
            PlaySE(SE_SELECT);
            sBook->parentScroll = sBook->scroll;
            sBook->parentRow = sBook->row;
            sBook->parentQuest = input;
            sBook->inSubquests = TRUE;
            RebuildList(FALSE);
        }
        break;
    }
}

static void Task_QuestBookClose(u8 taskId)
{
    MainCallback callback;

    if (gPaletteFade.active)
        return;

    DestroyList();
    DestroyPicture(0);
    DestroyPicture(1);
    callback = sBook->savedCallback;
    FreeBook();
    SetMainCallback2(callback);
    DestroyTask(taskId);
}
