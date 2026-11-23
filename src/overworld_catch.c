#include "global.h"
#include "option_menu.h"
#include "overworld_catch.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "bg.h"
#include "bug_contest.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_message_box.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "field_effect.h"
#include "fldeff.h"
#include "gpu_regs.h"
#include "item.h"
#include "map_name_popup.h"
#include "menu.h"
#include "metatile_behavior.h"
#include "nuzlocke.h"
#include "overworld.h"
#include "pokeball.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "safari_zone.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "palette.h"
#include "trainer_hill.h"
#include "window.h"
#include "wild_encounter_ow.h"
#include "constants/abilities.h"
#include "constants/event_objects.h"
#include "constants/game_stat.h"
#include "constants/layouts.h"
#include "constants/party_menu.h"
#include "constants/songs.h"

// Hasep: overworld catching, phases 3 and 4 of docs/plans/overworld-catch.md
// (the decision numbers D.. and P.. below are that plan's). R throws the
// selected Ball, or the selected party Pokémon, straight ahead at a visible
// wild Pokémon (OWE). A throw is a record: the outcome is decided when the
// Ball arrives ("impact", D15) and the Pokémon is given at once; what follows
// (the Pokémon drawn in, the fall, the shakes, the click or the break-out) is
// only animation, driven by the record's state and timer.

#define THROW_RANGE             5   // tiles, straight ahead (P2)
#define THROW_MAX_PENDING       3   // Balls in the air, and catches waiting for their banner (D15)
#define THROW_COOLDOWN          30  // frames between two throws, about half a second (D15)
// Flight time in frames: base + per tile, 16 to 48. Longer than the cooldown
// at full range, so a second Ball can be in the air when the first lands.
#define THROW_FLIGHT_BASE       8
#define THROW_FLIGHT_PER_TILE   8

// The animation, in frames. Heights are pixels above the ground.
#define BALL_HAND_HEIGHT        8   // where the Ball leaves the player, half a tile ahead
#define BALL_HIT_HEIGHT         10  // where it meets the Pokémon, and opens
#define BALL_ARC_BASE           4   // top of the arc above the straight line: base + per tile
#define BALL_ARC_PER_TILE       2
#define BALL_OPEN_FRAMES        16  // frames 1-4 of the Ball sheet, 4 each; the Pokémon goes on frame 4
#define BALL_CLOSE_FRAMES       8
#define BALL_FALL_FRAMES        8
#define BALL_SHAKE_PAUSE        16  // still, before each shake and before the end
#define BALL_SHAKE_FRAMES       16  // one wobble
#define BALL_CAUGHT_FRAMES      40  // after the click; it blinks out in the last 16
#define BALL_BREAKOUT_FRAMES    12
#define BALL_DROPPED_FRAMES     24  // a Ball that hit nothing lies there, then blinks out
#define BALL_BLINK_FRAMES       16

#define BALL_SPRITE_MAGIC       0x4F43

enum ThrowState
{
    THROW_FREE,
    THROW_FLYING,
    THROW_OPENING,      // at the Pokémon: the Ball opens and draws it in
    THROW_CLOSING,
    THROW_FALLING,      // down to the ground (also a Ball that hit nothing)
    THROW_SHAKING,
    THROW_CAUGHT,
    THROW_BREAKOUT,
    THROW_DROPPED,      // a Ball that hit nothing, on the ground
};

enum ThrowBlock
{
    THROW_BLOCK_NONE,
    THROW_BLOCK_AREA,
    THROW_BLOCK_NO_ROOM,
    THROW_BLOCK_DISABLED,
    THROW_BLOCK_ONE_TYPE,
    THROW_BLOCK_NUZLOCKE_ZONE,
    THROW_BLOCK_NUZLOCKE_SPECIES,
    THROW_BLOCK_NUZLOCKE_ALREADY_CAUGHT,
};

enum ThrowTarget
{
    THROW_TARGET_NONE,      // nothing on the line: the Ball flies the whole range and falls
    THROW_TARGET_BLOCKED,   // a wall, an NPC, a sign: the Ball hits it and falls (D16)
    THROW_TARGET_OWE,
};

struct PendingThrow
{
    u8 state;
    u8 timer;           // frames left in this state
    u8 objectEventId;
    u8 localId;
    u8 mapNum;
    u8 mapGroup;
    u16 graphicsId;
    u16 ball;
    u8 flightTime;
    u8 shakes;          // shakes still to show
    u8 spriteId;        // MAX_SPRITES: none (the animation goes on without it)
    u8 fallHeight;
    u8 miss:1;          // hit nothing: the Ball goes back to the bag when it is gone (D16)
    u8 caught:1;
    u8 holdsOWE:1;      // the Pokémon is hidden and frozen inside this Ball
    u8 hadNoDespawn:1;  // its no-despawn flag before the Ball held it
    s16 fromX;          // the Ball's ground position at the throw and at the end of
    s16 fromY;          // the flight, in field sprite coordinates (world-fixed with
    s16 toX;            // coordOffsetEnabled)
    s16 toY;
    s16 x;              // where it is now: ground position, and height above it
    s16 y;
    s16 height;
    s16 targetX;        // the tile the Pokémon was on, or stepping into, at the throw
    s16 targetY;
    s16 landX;          // the tile the Ball comes down on: nobody walks onto it
    s16 landY;          // while it lies there (landBlocks)
    u8 landBlocks:1;
    u8 landMapNum;      // the map landX/Y are on: a seam crossing re-bases coordinates
    u8 landMapGroup;
    u8 direction;       // the player's facing at the throw: a backstrike is a Pokémon facing the same way
    u8 bannerShown:1;
    u8 caughtToParty:1;
    u16 caughtSpecies;  // a catch: what its banner shows
    u32 caughtPersonality;
};

// A catch made with the Nuzlocke's compulsory nickname on: the prompt comes
// once no Ball is out and no banner is up. With the Nuzlocke's naming off
// there is no prompt at all for a thrown Ball (user, 20/11): the Name Rater
// renames.
struct CatchResult
{
    u32 personality;
    u16 species;
    u8 toParty;
    u8 boxId;
    u8 boxPos;
};

// A catch banner: a note in a pile at the top of the screen, gone by itself
// after BANNER_SHOW_FRAMES or when a newer catch needs room (user, 20-21/11).
// The world goes on meanwhile.
struct CatchBanner
{
    u32 personality;
    u16 species;
    u16 timer;
    u8 toParty;
};

#define BANNER_MAX          3
#define BANNER_SHOW_FRAMES  180   // ~3 s

struct OverworldCatch
{
    struct PendingThrow throws[THROW_MAX_PENDING];
    struct CatchResult results[THROW_MAX_PENDING];
    u8 numResults;
    u8 nicknameIndex;
    u8 cooldown;
    u8 monMode:1;   // FALSE: R throws a Ball. TRUE: R sends out a Pokémon (D17)
    u8 lHeld:1;
    u8 lScrolled:1;
    u8 monSlot;
    u8 blockReason;
    struct CatchBanner banners[BANNER_MAX];
    u8 numBanners;      // banners[0] is the oldest, at the bottom of the pile
    u8 battleStarting:1;
    u8 bannerWindowId;  // the note on top (WINDOW_NONE: none drawn)
    u8 bannerIconSpriteId;
    u8 bannerIconSpriteId2;
    u8 bannerIconPaletteNum;
    u8 bannerRow;       // tile row of the top note's window
    u8 bannerEdges;     // edges drawn under it
    u8 pileUp:1;        // the fields above are valid (a zeroed EWRAM is window 0, sprite 0)
    u8 pileHidden:1;    // notes kept but not drawn: a battle, a message or a menu is up
    u8 panelWindowId;   // valid only while panelUp (zeroed EWRAM is window 0)
    u8 panelUp:1;
    u8 panelTimer;      // frames the panel stays up after a change or a throw
    u32 panelKey;       // what the window shows now, to redraw only on a change
};

static EWRAM_DATA struct OverworldCatch sCatch = {0};

#define PANEL_SHOW_FRAMES   120   // ~2 s (D18)

EWRAM_DATA u8 gOverworldThrowLeadSlot = 0; // 0 is the usual lead, so a zeroed EWRAM changes nothing

extern const u8 OverworldCatch_EventScript_Blocked[];
extern const u8 OverworldCatch_EventScript_Battle[];
extern const u8 OverworldCatch_EventScript_Results[];

static const u8 sText_CantThrowHere[] = _("You can't throw anything here!");
static const u8 sText_BoxesFull[] = _("The BOX is full.\nYou can't catch any more!");
static const u8 sText_BallsDisabled[] = _("POKé BALLS cannot be used\nright now!");
static const u8 sText_NuzlockeZone[] = _("You already used your\nencounter for this area!");
static const u8 sText_NuzlockeSpecies[] = _("Species Clause: a POKéMON in\nthis evolution line was caught!");
static const u8 sText_NuzlockeAlreadyCaught[] = _("You have already caught\nthis POKéMON!");
static const u8 sText_OneType[] = _("This POKéMON doesn't match\nyour type challenge!");

static const u8 *const sBlockTexts[] =
{
    [THROW_BLOCK_NONE] = sText_CantThrowHere,
    [THROW_BLOCK_AREA] = sText_CantThrowHere,
    [THROW_BLOCK_NO_ROOM] = sText_BoxesFull,
    [THROW_BLOCK_DISABLED] = sText_BallsDisabled,
    [THROW_BLOCK_ONE_TYPE] = sText_OneType,
    [THROW_BLOCK_NUZLOCKE_ZONE] = sText_NuzlockeZone,
    [THROW_BLOCK_NUZLOCKE_SPECIES] = sText_NuzlockeSpecies,
    [THROW_BLOCK_NUZLOCKE_ALREADY_CAUGHT] = sText_NuzlockeAlreadyCaught,
};

static const u8 sText_YouCaught[] = _("You caught {STR_VAR_1}");
static const u8 sText_AddedToParty[] = _("It was added to your party.");
static const u8 sText_SentToPC[] = _("It was sent to the PC.");
static const u8 sText_Exclamation[] = _("!");

// ---------------------------------------------------------------------------
// Selection: which Ball, which Pokémon
// ---------------------------------------------------------------------------

static bool32 IsThrowableBall(u32 item)
{
    return item != ITEM_NONE
        && GetItemPocket(item) == POCKET_POKE_BALLS
        && item != ITEM_SAFARI_BALL
        && CheckBagHasItem(item, 1);
}

// The last Ball thrown, in battle or here, if there is still one; otherwise
// the first of the Balls pocket (P1). ITEM_NONE with no Balls.
static u32 GetSelectedBall(void)
{
    if (IsThrowableBall(gLastThrownBall))
        return gLastThrownBall;

    for (u32 i = 0; i < gBagPockets[POCKET_POKE_BALLS].capacity; i++)
    {
        u32 item = GetBagItemId(POCKET_POKE_BALLS, i);
        if (IsThrowableBall(item))
            return item;
    }
    return ITEM_NONE;
}

// The Balls scroll in the order of the bag's pocket.
static void ScrollBall(s32 delta)
{
    u32 capacity = gBagPockets[POCKET_POKE_BALLS].capacity;
    u32 current = GetSelectedBall();
    u32 pos;

    if (current == ITEM_NONE)
        return;

    for (pos = 0; pos < capacity; pos++)
    {
        if (GetBagItemId(POCKET_POKE_BALLS, pos) == current)
            break;
    }
    for (u32 i = 0; i < capacity; i++)
    {
        pos = (pos + capacity + delta) % capacity;
        u32 item = GetBagItemId(POCKET_POKE_BALLS, pos);
        if (IsThrowableBall(item))
        {
            if (item != current)
                PlaySE(SE_SELECT);
            gLastThrownBall = item;
            sCatch.panelTimer = PANEL_SHOW_FRAMES;
            return;
        }
    }
}

static bool32 CanMonBeThrown(u32 slot)
{
    struct Pokemon *mon = &gPlayerParty[slot];

    return slot < PARTY_SIZE
        && GetMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE
        && !GetMonData(mon, MON_DATA_IS_EGG)
        && GetMonData(mon, MON_DATA_HP) != 0;
}

// The chosen party slot if it can still fight, otherwise the first that can.
// PARTY_SIZE with nobody.
static u32 GetSelectedMonSlot(void)
{
    if (CanMonBeThrown(sCatch.monSlot))
        return sCatch.monSlot;

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (CanMonBeThrown(i))
            return i;
    }
    return PARTY_SIZE;
}

static void ScrollMon(s32 delta)
{
    u32 current = GetSelectedMonSlot();
    u32 slot = current;

    if (current == PARTY_SIZE)
        return;

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        slot = (slot + PARTY_SIZE + delta) % PARTY_SIZE;
        if (CanMonBeThrown(slot))
        {
            if (slot != current)
                PlaySE(SE_SELECT);
            sCatch.monSlot = slot;
            sCatch.panelTimer = PANEL_SHOW_FRAMES;
            return;
        }
    }
}

// L pressed and released switches between Ball and Pokémon; L held with
// Left/Right scrolls the Balls or the Pokémon (D1). Returns TRUE while L is
// held: the caller then keeps the player still. With BUTTON MODE on L=A the
// L button is an A press (main.c) and none of this exists (P21): R still
// throws whatever is selected.
bool32 OverworldCatch_HandleLButton(u16 newKeys, u16 heldKeys)
{
    // OVERWORLD CATCH off (plan D20): L does nothing on the map, as before.
    if (gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A
     || !FlagGet(FLAG_SYS_POKEMON_GET)
     || !IsOverworldCatchEnabled())
    {
        sCatch.lHeld = FALSE;
        return FALSE;
    }

    if (heldKeys & L_BUTTON)
    {
        if (!sCatch.lHeld)
        {
            sCatch.lHeld = TRUE;
            sCatch.lScrolled = FALSE;
        }
        if (newKeys & (DPAD_LEFT | DPAD_RIGHT))
        {
            s32 delta = (newKeys & DPAD_RIGHT) ? 1 : -1;

            sCatch.lScrolled = TRUE;
            if (sCatch.monMode)
                ScrollMon(delta);
            else
                ScrollBall(delta);
        }
        return TRUE;
    }

    if (sCatch.lHeld)
    {
        sCatch.lHeld = FALSE;
        if (!sCatch.lScrolled)
        {
            // Pokémon mode needs somebody to send.
            if (sCatch.monMode || GetSelectedMonSlot() != PARTY_SIZE)
            {
                sCatch.monMode ^= 1;
                sCatch.panelTimer = PANEL_SHOW_FRAMES;
                PlaySE(SE_SWITCH);
            }
        }
    }
    return FALSE;
}

// ---------------------------------------------------------------------------
// Target
// ---------------------------------------------------------------------------

static void FinishThrow(struct PendingThrow *throw);
static void ShowCatchBanner(struct PendingThrow *throw);
static void DismissCatchBanners(void);
static void HideCatchBanners(void);
static void UpdateBanners(void);

// Water tiles are elevation 1 and the ground around them 3: for a throw the
// two are one level, or every throw from the shore at a Pokémon in the water
// (and from Surf at one on land) would stop at the first tile.
#define ELEVATION_WATER     1
#define ELEVATION_GROUND    3

static u32 GetThrowLevel(u32 elevation)
{
    return (elevation == ELEVATION_WATER) ? ELEVATION_GROUND : elevation;
}

// Two heights that do not meet: neither is a transition (0) nor the
// multi-level one (15), as IsElevationMismatchAt reads a tile, and they
// differ beyond water and ground. Under a multi-level bridge the two levels
// meet, as they do for the engine (CanOWEReachPlayer).
bool32 OverworldCatch_AreLevelsApart(u32 a, u32 b)
{
    return a != ELEVATION_TRANSITION && b != ELEVATION_TRANSITION
        && a != ELEVATION_MULTI_LEVEL && b != ELEVATION_MULTI_LEVEL
        && GetThrowLevel(a) != GetThrowLevel(b);
}

// The first thing on the line in front of the player, up to THROW_RANGE
// tiles. A tile that blocks walking stops the Ball, and so does any visible
// object that is not a generated wild Pokémon: NPCs, signs, the map's
// decorative Pokémon (trainerType 0, not OWEs), map-placed OWEs with a script
// of their own (D16). The follower is not thrown at at all. Water does not
// stop it: it has no collision bit, so a Pokémon swimming next to the shore
// can be hit. *distance is where the Ball ends: the Pokémon's tile, the tile
// it hits, or the full range.
// Set by FindThrowTarget: the follower stands on the line of the throw.
static bool8 sFollowerOnLine;

static enum ThrowTarget FindThrowTarget(u32 *objectEventId, u32 *distance)
{
    sFollowerOnLine = FALSE;
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    s16 x = player->currentCoords.x;
    s16 y = player->currentCoords.y;
    enum Direction direction = player->facingDirection;

    for (u32 d = 1; d <= THROW_RANGE; d++)
    {
        enum ThrowTarget found = THROW_TARGET_NONE;

        *distance = d;
        MoveCoords(direction, &x, &y);
        // A change of level stops the Ball as a wall does (P2): a raised
        // platform with no collision of its own. A ledge is not one (same
        // elevation on both sides): the Ball flies over it.
        if (MapGridGetCollisionAt(x, y) || OverworldCatch_AreLevelsApart(player->currentElevation, MapGridGetElevationAt(x, y)))
            return THROW_TARGET_BLOCKED;

        // Not GetObjectEventIdByXY: an invisible trigger object may share
        // the tile with a Pokémon. A Pokémon inside a Ball is invisible from
        // the beam on, and passed over.
        for (u32 i = 0; i < OBJECT_EVENTS_COUNT; i++)
        {
            struct ObjectEvent *object = &gObjectEvents[i];

            if (!object->active || object->invisible || i == gPlayerAvatar.objectEventId
             || object->currentCoords.x != x || object->currentCoords.y != y)
                continue;
            // Under or over the line (a Pokémon surfing under a bridge):
            // out of reach, as for a touch.
            if (OverworldCatch_AreLevelsApart(player->currentElevation, object->currentElevation))
                continue;

            // The following Pokémon goes back into its Ball when a Ball is
            // thrown (user, 21/11; it used to stop the throw): the line goes
            // on past it.
            if (object->localId == OBJ_EVENT_ID_FOLLOWER)
            {
                sFollowerOnLine = TRUE;
                continue;
            }
            if (!IsOverworldWildEncounter(object, OWE_GENERATED))
                return THROW_TARGET_BLOCKED;

            // Between the impact and the beam it is still there to the eye,
            // but already decided: the Ball hits it and drops, as on a wall.
            if (OverworldCatch_IsOWEInBall(object))
                return THROW_TARGET_BLOCKED;

            *objectEventId = i;
            found = THROW_TARGET_OWE;
        }
        if (found != THROW_TARGET_NONE)
            return found;
    }
    *distance = THROW_RANGE;
    return THROW_TARGET_NONE;
}

// ---------------------------------------------------------------------------
// What stops a catch
// ---------------------------------------------------------------------------

// Places with rules of their own for wild Pokémon (P5): counted Balls and a
// flee meter in the Safari Zone, the Contest's score, the Frontier's rented
// rooms. Here R does nothing but say so.
static bool32 IsThrowDisabledHere(void)
{
    return GetSafariZoneFlag()
        || GetBugContestFlag()
        || InBattlePike()
        || InBattlePyramid_()
        || InTrainerHillChallenge()
        || gMapHeader.mapLayoutId == LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS
        || gMapHeader.mapLayoutId == LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR;
}

// The reasons a battle refuses a Ball (GetBallThrowableState, item_use.c),
// minus the ones that need a battle. Asked when R is pressed and again at the
// impact: in a volley the first catch can use up the area's Nuzlocke
// encounter, or the last free slot, while the next Ball is in the air (P8).
static enum ThrowBlock GetCatchBlock(const struct ObjectEvent *owe)
{
    struct CaptureChallengeChecks checks;

    if (IsPlayerPartyAndPokemonStorageFull())
        return THROW_BLOCK_NO_ROOM;
    if (FlagGet(B_FLAG_NO_CATCHING))
        return THROW_BLOCK_DISABLED;

    // An OWE is never a fateful encounter.
    GetCaptureChallengeChecks(OW_SPECIES(owe), OW_SHINY(owe) ? TRUE : FALSE, FALSE, &checks);
    if (checks.oneTypeBlocked)
        return THROW_BLOCK_ONE_TYPE;
    if (checks.zoneBlocked)
        return THROW_BLOCK_NUZLOCKE_ZONE;
    if (checks.speciesClause == 2)
        return THROW_BLOCK_NUZLOCKE_ALREADY_CAUGHT;
    if (checks.speciesClause)
        return THROW_BLOCK_NUZLOCKE_SPECIES;
    return THROW_BLOCK_NONE;
}

void OverworldCatch_ShowBlockedMessage(struct ScriptContext *ctx)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_HARDWARE);
    ShowFieldMessage(sBlockTexts[sCatch.blockReason]);
}

static bool32 StartBlockedScript(enum ThrowBlock reason)
{
    sCatch.blockReason = reason;
    ScriptContext_SetupScript(OverworldCatch_EventScript_Blocked);
    return TRUE;
}

// ---------------------------------------------------------------------------
// The throw
// ---------------------------------------------------------------------------

static u32 CountThrows(enum ThrowState state)
{
    u32 count = 0;

    for (u32 i = 0; i < THROW_MAX_PENDING; i++)
    {
        if (sCatch.throws[i].state == state)
            count++;
    }
    return count;
}

// Balls of this kind in the air: still in the bag, but spoken for.
static u32 CountFlyingBalls(u32 ball)
{
    u32 count = 0;

    for (u32 i = 0; i < THROW_MAX_PENDING; i++)
    {
        if (sCatch.throws[i].state == THROW_FLYING && sCatch.throws[i].ball == ball)
            count++;
    }
    return count;
}

// The battle the player would get by walking into this Pokémon, started from
// where the two stand. No approach: over five tiles the scripted walk could
// drag a swimming Pokémon ashore.
static bool32 StartBattleWithTarget(struct ObjectEvent *owe, u32 objectEventId)
{
    gSpecialVar_LastTalked = owe->localId;
    gSelectedObjectEvent = objectEventId;

    // Stop the bobbing animation, as TryTriggerOverworldWildEncounter does.
    if (owe->movementActionId >= MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_DOWN && owe->movementActionId <= MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_RIGHT)
        ClearObjectEventMovement(owe, &gSprites[owe->spriteId]);

    ScriptContext_SetupScript(OverworldCatch_EventScript_Battle);
    return TRUE;
}

// ---------------------------------------------------------------------------
// The Ball's sprite
// ---------------------------------------------------------------------------

#if OW_FOLLOWERS_POKEBALLS
extern const struct ObjectEventGraphicsInfo gPokeballGraphics[POKEBALL_COUNT];
#endif

// The sheet of a follower's Ball (ObjectEventSetPokeballGfx): frame 0 closed,
// 1-4 opening, with the beam rising on 4. The Poké Ball and any Ball without
// a sheet of its own use the plain Poké Ball.
static const struct ObjectEventGraphicsInfo *GetBallGraphicsInfo(u32 ballItem)
{
#if OW_FOLLOWERS_POKEBALLS
    enum PokeBall ballId = ItemIdToBallId(ballItem);

    if (ballId != BALL_POKE && ballId < POKEBALL_COUNT
     && gPokeballGraphics[ballId].tileTag == TAG_NONE && gPokeballGraphics[ballId].images != NULL)
        return &gPokeballGraphics[ballId];
#endif
    return GetObjectEventGraphicsInfo(OBJ_EVENT_GFX_POKE_BALL);
}

static const union AnimCmd sAnim_Ball0[] = {ANIMCMD_FRAME(0, 1), ANIMCMD_END};
static const union AnimCmd sAnim_Ball1[] = {ANIMCMD_FRAME(1, 1), ANIMCMD_END};
static const union AnimCmd sAnim_Ball2[] = {ANIMCMD_FRAME(2, 1), ANIMCMD_END};
static const union AnimCmd sAnim_Ball3[] = {ANIMCMD_FRAME(3, 1), ANIMCMD_END};
static const union AnimCmd sAnim_Ball4[] = {ANIMCMD_FRAME(4, 1), ANIMCMD_END};

// One animation per frame of the sheet: the record picks the frame.
static const union AnimCmd *const sAnims_ThrownBall[] =
{
    sAnim_Ball0, sAnim_Ball1, sAnim_Ball2, sAnim_Ball3, sAnim_Ball4,
};

#define sThrowIndex data[0]
#define sMagic      data[1]

// Positions are set by OverworldCatch_Update.
static void SpriteCB_ThrownBall(struct Sprite *sprite)
{
}

static bool32 IsBallSpriteValid(const struct PendingThrow *throw)
{
    const struct Sprite *sprite = &gSprites[throw->spriteId];

    return throw->spriteId < MAX_SPRITES
        && sprite->inUse
        && sprite->callback == SpriteCB_ThrownBall
        && sprite->sThrowIndex == throw - sCatch.throws
        && sprite->sMagic == BALL_SPRITE_MAGIC;
}

// Never an assert: VRAM and the 16 sprite palettes are full on busy maps
// (overworld_encounters.md). Without a sprite the record goes on unseen, and
// a step that needs one finishes the throw at once (FinishThrow): nothing
// ever waits on a sprite.
static bool32 CreateBallSprite(struct PendingThrow *throw)
{
    const struct ObjectEventGraphicsInfo *info = GetBallGraphicsInfo(throw->ball);
    struct SpriteTemplate template =
    {
        .tileTag = TAG_NONE,
        .paletteTag = info->paletteTag,
        .oam = info->oam,
        .anims = sAnims_ThrownBall,
        .images = info->images,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_ThrownBall,
    };
    u32 paletteNum;
    u8 spriteId;
    struct Sprite *sprite;

    throw->spriteId = MAX_SPRITES;
    paletteNum = LoadObjectEventPalette(info->paletteTag);
    if (paletteNum == 0xFF)
        return FALSE;
    spriteId = CreateSprite(&template, throw->x, throw->y, 1);
    if (spriteId == MAX_SPRITES)
    {
        FieldEffectFreePaletteIfUnused(paletteNum);
        return FALSE;
    }

    sprite = &gSprites[spriteId];
    sprite->coordOffsetEnabled = TRUE;
    sprite->oam.priority = gSprites[gObjectEvents[gPlayerAvatar.objectEventId].spriteId].oam.priority;
    sprite->sThrowIndex = throw - sCatch.throws;
    sprite->sMagic = BALL_SPRITE_MAGIC;
    throw->spriteId = spriteId;
    return TRUE;
}

static void DestroyBallSprite(struct PendingThrow *throw)
{
    if (IsBallSpriteValid(throw))
    {
        u32 paletteNum = gSprites[throw->spriteId].oam.paletteNum;

        DestroySprite(&gSprites[throw->spriteId]);
        FieldEffectFreePaletteIfUnused(paletteNum);
    }
    throw->spriteId = MAX_SPRITES;
}

static void DrawBall(struct PendingThrow *throw, u32 frame, s32 wobble, bool32 visible)
{
    struct Sprite *sprite;

    if (!IsBallSpriteValid(throw))
        return;
    sprite = &gSprites[throw->spriteId];
    sprite->x = throw->x;
    sprite->y = throw->y;
    sprite->x2 = wobble;
    sprite->y2 = -throw->height;
    sprite->invisible = !visible;
    StartSpriteAnimIfDifferent(sprite, frame);
}

#undef sThrowIndex
#undef sMagic

// Where a Ball (a 16x32 frame, the Ball at its bottom) lies on the ground
// under an object, from the object's sprite: right while it walks, and the
// same coordinate space as the Ball's sprite.
static void GetObjectGroundPos(const struct ObjectEvent *object, s16 *x, s16 *y)
{
    const struct Sprite *sprite = &gSprites[object->spriteId];

    // Without y2: a hop in place (approaching Pokémon jump) lifts the
    // sprite, not the ground under it.
    *x = sprite->x + sprite->x2;
    *y = sprite->y - sprite->centerToCornerVecY - 16;
}

// ---------------------------------------------------------------------------
// The throw
// ---------------------------------------------------------------------------

// The player holds out an arm: the first frame of the running step, which
// the sheet already has (no new graphics). On foot only. Standing still, the
// input code gives the player its facing animation again every frame
// (PlayerFaceDirection), unless a movement action is running: a 16-frame
// delay, which leaves the animation alone, holds the pose and keeps the
// player still for that long (about a quarter of a second). Once it ends the
// facing animation comes back by itself. The last facing action is done but
// still held (PlayerStep clears it later in the frame), and a held action
// counts as busy: it is cleared first, as PlayerStep does.
static void StartThrowPose(void)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct Sprite *sprite = &gSprites[player->spriteId];

    if (!TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_ON_FOOT)
     || player->graphicsId != GetPlayerAvatarGraphicsIdByStateId(PLAYER_AVATAR_STATE_NORMAL)
     || gPlayerAvatar.tileTransitionState != T_NOT_MOVING
     || !ObjectEventClearHeldMovementIfFinished(player)
     || ObjectEventSetHeldMovement(player, MOVEMENT_ACTION_DELAY_16))
        return;

    StartSpriteAnim(sprite, GetRunningDirectionAnimNum(player->facingDirection));
    sprite->animPaused = TRUE;
}

// R pressed on foot or surfing. Returns TRUE when a script was started (the
// caller locks the controls); a Ball thrown leaves the player free.
bool32 OverworldCatch_TryThrow(void)
{
    u32 objectEventId = OBJECT_EVENTS_COUNT;
    u32 distance = 0;
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct ObjectEvent *owe = NULL;
    enum ThrowTarget target;

    // The caller starts a DexNav search instead while OVERWORLD CATCH is
    // off; this is the guard for any other caller.
    if (!FlagGet(FLAG_SYS_POKEMON_GET) || !IsOverworldCatchEnabled())
        return FALSE;

    target = FindThrowTarget(&objectEventId, &distance);
    if (target == THROW_TARGET_OWE)
        owe = &gObjectEvents[objectEventId];

    if (IsThrowDisabledHere())
    {
        // Said only when there is something to catch.
        if (owe != NULL)
            return StartBlockedScript(THROW_BLOCK_AREA);
        return FALSE;
    }

    if (sCatch.monMode)
    {
        u32 slot = GetSelectedMonSlot();

        // Nothing to hit: nothing happens (D17).
        if (owe == NULL || slot == PARTY_SIZE)
            return FALSE;
        gOverworldThrowLeadSlot = slot;
        return StartBattleWithTarget(owe, objectEventId);
    }

    u32 ball = GetSelectedBall();
    if (ball == ITEM_NONE)
        return FALSE;

    // Roamers and outbreak Pokémon cannot be built outside a battle (P4):
    // the Ball is not thrown and the battle starts.
    if (owe != NULL && IsOWEBattleOnly(owe))
        return StartBattleWithTarget(owe, objectEventId);

    // Every Ball in the air may become a catch, and the queue of catches
    // holds THROW_MAX_PENDING: a Ball that has landed is in that queue
    // already or was a miss. A record also stays busy while its Ball is on
    // the ground (up to about three seconds): with none free the loop below
    // throws nothing and takes no Ball.
    if (sCatch.cooldown != 0 || CountThrows(THROW_FLYING) + sCatch.numResults >= THROW_MAX_PENDING)
        return FALSE;

    // The Ball stays in the bag until it holds a Pokémon (user, 20/11): the
    // ones in the air are spoken for, so three in the air are three real
    // Balls.
    if (CountTotalItemQuantityInBag(ball) <= CountFlyingBalls(ball))
        return FALSE;

    if (owe != NULL)
    {
        enum ThrowBlock block = GetCatchBlock(owe);
        if (block != THROW_BLOCK_NONE)
            return StartBlockedScript(block);
    }

    for (u32 i = 0; i < THROW_MAX_PENDING; i++)
    {
        struct PendingThrow *throw = &sCatch.throws[i];

        if (throw->state != THROW_FREE)
            continue;

        gLastThrownBall = ball;
        throw->state = THROW_FLYING;
        throw->flightTime = throw->timer = THROW_FLIGHT_BASE + THROW_FLIGHT_PER_TILE * distance;
        throw->objectEventId = objectEventId;
        throw->ball = ball;
        throw->miss = (owe == NULL);
        throw->caught = FALSE;
        throw->holdsOWE = FALSE;
        throw->bannerShown = FALSE;
        throw->landBlocks = TRUE;
        throw->direction = player->facingDirection;
        throw->landMapNum = gSaveBlock1Ptr->location.mapNum;
        throw->landMapGroup = gSaveBlock1Ptr->location.mapGroup;
        if (owe != NULL)
        {
            throw->localId = owe->localId;
            throw->mapNum = owe->mapNum;
            throw->mapGroup = owe->mapGroup;
            throw->graphicsId = owe->graphicsId;
        }

        GetObjectGroundPos(player, &throw->fromX, &throw->fromY);
        // From the hand, not from the middle of the player.
        throw->fromX += (s32)gDirectionToVectors[player->facingDirection].x * 8;
        throw->fromY += (s32)gDirectionToVectors[player->facingDirection].y * 8;
        if (owe != NULL)
        {
            GetObjectGroundPos(owe, &throw->toX, &throw->toY);
            throw->targetX = throw->landX = owe->currentCoords.x;
            throw->targetY = throw->landY = owe->currentCoords.y;
        }
        else
        {
            // The full range, or the tile before whatever it hits; none
            // when that is the player's own (a wall right in front: the
            // Ball drops at the player's feet).
            u32 tiles = (target == THROW_TARGET_BLOCKED) ? distance - 1 : distance;

            throw->landX = player->currentCoords.x + gDirectionToVectors[player->facingDirection].x * tiles;
            throw->landY = player->currentCoords.y + gDirectionToVectors[player->facingDirection].y * tiles;
            throw->landBlocks = (tiles != 0);

            // The full range, or the near side of whatever it hits,
            // counted from the player (the Ball starts half a tile ahead).
            s32 pixels = 16 * distance - 8 - (target == THROW_TARGET_BLOCKED ? 8 : 0);

            throw->toX = throw->fromX + (s32)gDirectionToVectors[player->facingDirection].x * pixels;
            throw->toY = throw->fromY + (s32)gDirectionToVectors[player->facingDirection].y * pixels;
        }
        throw->x = throw->fromX;
        throw->y = throw->fromY;
        throw->height = BALL_HAND_HEIGHT;
        // A miss with nothing to show is over at once: the Ball is back.
        if (!CreateBallSprite(throw) && throw->miss)
            FinishThrow(throw);
        else
            DrawBall(throw, 0, 0, TRUE);

        sCatch.cooldown = THROW_COOLDOWN;
        sCatch.panelTimer = PANEL_SHOW_FRAMES;
        // The Ball flies over the follower while it goes back into its own.
        if (sFollowerOnLine)
            SendFollowerIntoPokeball();
        StartThrowPose();
        PlaySE(SE_BALL_THROW);
        break;
    }
    return FALSE;
}

// The Pokémon the Ball was thrown at, if it is still that one: it may have
// fled, been replaced in its slot, or been left behind on another map.
static struct ObjectEvent *GetThrowTarget(const struct PendingThrow *throw)
{
    struct ObjectEvent *owe = &gObjectEvents[throw->objectEventId];

    if (throw->miss
     || throw->objectEventId >= OBJECT_EVENTS_COUNT
     || !IsOverworldWildEncounter(owe, OWE_GENERATED)
     || owe->localId != throw->localId
     || owe->mapNum != throw->mapNum
     || owe->mapGroup != throw->mapGroup
     || owe->graphicsId != throw->graphicsId)
        return NULL;
    return owe;
}

// While the Pokémon is in the Ball it does not move, and from the beam on
// (frame 4 of the opening) it does not show. Asked every frame: a script's
// release unfreezes every object (UnfreezeObjectEvents), and the swim sprite
// reads the flag too (SpriteCB_OWESwim).
static void KeepOWEInBall(struct PendingThrow *throw, bool32 hide)
{
    struct ObjectEvent *owe = GetThrowTarget(throw);

    if (owe == NULL)
    {
        throw->holdsOWE = FALSE;
        return;
    }
    FreezeObjectEvent(owe);
    if (hide)
    {
        owe->invisible = TRUE;
        gSprites[owe->spriteId].invisible = TRUE;
    }
}

// It breaks out, and now knows the player is there: it flees or attacks as
// its behaviour says (D2), with its "!" when it notices.
static void ReleaseOWEFromBall(struct PendingThrow *throw)
{
    struct ObjectEvent *owe = throw->holdsOWE ? GetThrowTarget(throw) : NULL;

    throw->holdsOWE = FALSE;
    if (owe == NULL)
        return;
    ReleaseOWEFromThrowHold(owe, throw->hadNoDespawn);
    owe->invisible = FALSE;
    UnfreezeObjectEvent(owe);
    ForceOWEToNoticePlayer(owe);
}

// The Pokémon goes into the Ball for good. Any other Ball still flying at it
// becomes a miss now: the freed slot can get a new Pokémon of the same
// species at once (with a Lure the spawn countdown is 0), with the same
// localId and graphics, which GetThrowTarget would take for this one.
static void RemoveOWEInBall(struct PendingThrow *throw)
{
    struct ObjectEvent *owe = throw->holdsOWE ? GetThrowTarget(throw) : NULL;

    throw->holdsOWE = FALSE;
    if (owe == NULL)
        return;
    for (u32 i = 0; i < THROW_MAX_PENDING; i++)
    {
        struct PendingThrow *other = &sCatch.throws[i];

        if (other != throw && other->state == THROW_FLYING && GetThrowTarget(other) == owe)
            other->miss = TRUE;
    }
    RemoveCaughtOWE(owe);
}

// Skips the rest of the animation, with its outcome: when a sprite is
// missing, and when a battle starts.
static void FinishThrow(struct PendingThrow *throw)
{
    switch (throw->state)
    {
    case THROW_FREE:
        return;
    case THROW_FLYING:
        // Not landed: nothing decided yet.
        if (!throw->miss)
            return;
        break;
    default:
        if (throw->caught)
            RemoveOWEInBall(throw);
        else
            ReleaseOWEFromBall(throw);
        break;
    }
    // A catch whose banner the animation did not get to (no sprite, the
    // field rebuilt, a battle starting during the shakes): its note now,
    // hidden by ShowCatchBanner until the player is free again.
    if (throw->caught && !throw->bannerShown)
        ShowCatchBanner(throw);
    DestroyBallSprite(throw);
    throw->state = THROW_FREE;
}

// A battle is about to start: the Balls on the ground are done with before
// the transition. A Ball still in the air keeps flying after the battle
// (unseen until it lands); its target may be the Pokémon of this battle,
// which makes it a miss.
void OverworldCatch_OnBattleStart(void)
{
    HideCatchBanners();
    sCatch.battleStarting = TRUE;
    for (u32 i = 0; i < THROW_MAX_PENDING; i++)
    {
        struct PendingThrow *throw = &sCatch.throws[i];

        if (throw->state == THROW_FLYING && !throw->miss)
            DestroyBallSprite(throw);
        else
            FinishThrow(throw);
    }
    sCatch.battleStarting = FALSE;
}

// A Pokémon inside a Ball cannot be walked into: there is nothing to fight
// until it breaks out (TryTriggerOverworldWildEncounter).
bool32 OverworldCatch_IsOWEInBall(const struct ObjectEvent *owe)
{
    for (u32 i = 0; i < THROW_MAX_PENDING; i++)
    {
        struct PendingThrow *throw = &sCatch.throws[i];

        if (throw->state != THROW_FREE && throw->holdsOWE && GetThrowTarget(throw) == owe)
            return TRUE;
    }
    return FALSE;
}

// ---------------------------------------------------------------------------
// Impact: the catch is decided and, if it holds, made
// ---------------------------------------------------------------------------

static struct Pokemon *GetFirstMonAbleToBattle(void)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (CanMonBeThrown(i))
            return &gPlayerParty[i];
    }
    return &gPlayerParty[0];
}

// The battle formula at full HP (D2), with the party's first able Pokémon in
// the place of the battler. Twice the odds if the Pokémon has not noticed the
// player: our own balance choice, not a Legends: Arceus number. With ARCEUS
// FORMULAS on, Arceus' own formula instead (phase 10): the noticed/unnoticed
// behavior and a hit from behind take the place of the ×2.
static u32 ComputeOverworldCaptureOdds(struct Pokemon *mon, const struct ObjectEvent *owe, u32 ball, bool32 aware, u32 throwDirection)
{
    struct CaptureParams params = {0};
    struct Pokemon *lead = GetFirstMonAbleToBattle();
    u32 odds;

    params.species = GetMonData(mon, MON_DATA_SPECIES);
    params.level = GetMonData(mon, MON_DATA_LEVEL);
    params.hp = params.maxHP = GetMonData(mon, MON_DATA_MAX_HP);
    params.gender = GetMonGender(mon);
    params.types[0] = GetSpeciesType(params.species, 0);
    params.types[1] = GetSpeciesType(params.species, 1);
    params.types[2] = TYPE_MYSTERY;
    params.comatose = (GetMonAbility(mon) == ABILITY_COMATOSE);
    params.playerSpecies = GetMonData(lead, MON_DATA_SPECIES);
    params.playerLevel = GetMonData(lead, MON_DATA_LEVEL);
    params.playerGender = GetMonGender(lead);
    params.ballItem = ball;
    // There are no turns here. The Quick Ball's "first turn" is a Pokémon
    // that has not noticed the player yet; the Timer Ball never warms up.
    params.turnCounter = 0;
    params.quickBallSpent = aware;
    // The Dive Ball's bonus for a Pokémon met on the water.
    params.surfing = MetatileBehavior_IsSurfableWaterOrUnderwater(owe->currentMetatileBehavior);

    if (AreArceusFormulasEnabled())
        return ComputeArceusCaptureOddsFromParams(&params, aware, owe->facingDirection == throwDirection);

    odds = ComputeCaptureOddsFromParams(&params);
    if (odds != (u32)CAPTURE_GUARANTEED && !aware)
        odds *= 2;
    return odds;
}

// The same roll as Cmd_handleballthrow, critical captures aside
// (B_CRITICAL_CAPTURE is off). Returns the number of shakes: BALL_3_SHAKES_SUCCESS
// is a catch, anything less is where the Pokémon breaks out (phase 4 animates it).
static u32 RollShakes(u32 odds)
{
    u32 shakes;

    if (odds == (u32)CAPTURE_GUARANTEED || odds > 254)
        return BALL_3_SHAKES_SUCCESS;

    odds = ComputeBallShakeOdds(odds);
    for (shakes = 0; shakes < BALL_3_SHAKES_SUCCESS; shakes++)
    {
        if (Random() >= odds)
            break;
    }
    return shakes;
}

// Everything a battle does once the Ball has held (FinalizeCapture,
// BattleScript_SuccessBallThrow, Cmd_givecaughtmon and the Nuzlocke bookkeeping
// at the end of the battle), minus the screens: no experience (P6), no
// Pokédex page (P18), no swap-into-party prompt (P8: a full party sends it to
// the PC). Returns FALSE if the Pokémon could not be stored.
static bool32 GiveCaughtMon(struct PendingThrow *throw, struct Pokemon *mon)
{
    u32 ball = throw->ball;
    u32 species = GetMonData(mon, MON_DATA_SPECIES);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    enum NationalDexOrder dexNum = SpeciesToNationalPokedexNum(species);
    u32 ballId = ItemIdToBallId(ball);
    struct CaptureChallengeChecks checks;
    struct CatchResult *result;
    u32 givenTo;

    SetMonData(mon, MON_DATA_POKEBALL, &ballId);
    // FinalizeCapture's Friend Ball half; its Heal Ball half has nothing to
    // heal on a Pokémon that was never hit.
    if (ballId == BALL_FRIEND)
    {
        u32 friendship = (B_FRIEND_BALL_MODIFIER >= GEN_8 ? 150 : 200);
        SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
    }
    GiveWildMonHeldItem(mon);

    // Read before the Pokédex is written: the Species Clause would see the
    // Pokémon just caught.
    GetCaptureChallengeChecks(species, IsMonShiny(mon), FALSE, &checks);

    givenTo = GiveCapturedMonToPlayer(mon);
    if (givenTo == MON_CANT_GIVE)
        return FALSE;

    HandleSetPokedexFlag(dexNum, FLAG_SET_SEEN, personality);
    HandleSetPokedexFlag(dexNum, FLAG_SET_CAUGHT, personality);
    IncrementGameStat(GAME_STAT_POKEMON_CAPTURES);

    // As at the end of a wild battle (battle_main.c): the area's encounter
    // is used, unless the catch was one the rules do not count.
    if (IsNuzlockeActive() && !checks.speciesClause && !checks.oneTypeBlocked && !IsNuzlockeCaptureSuspended())
        NuzlockeFlagSet(NuzlockeGetCurrentRegionMapSectionId());

    throw->caughtSpecies = species;
    throw->caughtPersonality = personality;
    throw->caughtToParty = (givenTo == MON_GIVEN_TO_PARTY);

    // The Ball leaves the bag now that it holds the Pokémon.
    RemoveBagItem(ball, 1);

    if (IsNuzlockeNicknamingActive() && sCatch.numResults < THROW_MAX_PENDING)
    {
        result = &sCatch.results[sCatch.numResults++];
        result->species = species;
        result->personality = personality;
        result->toParty = (givenTo == MON_GIVEN_TO_PARTY);
        result->boxId = gSpecialVar_MonBoxId;
        result->boxPos = gSpecialVar_MonBoxPos;
    }
    return TRUE;
}

// The Ball comes down from where it is: a miss, a Pokémon gone from under it,
// a Ball that bounced off.
static void StartBallFall(struct PendingThrow *throw)
{
    throw->miss = TRUE;
    throw->holdsOWE = FALSE;
    throw->fallHeight = throw->height;
    throw->state = THROW_FALLING;
    throw->timer = BALL_FALL_FRAMES;
    // Nothing to show: the Ball is back at once.
    if (!IsBallSpriteValid(throw))
        FinishThrow(throw);
}

// By tile, not by pixels: currentCoords is where the Pokémon stands, or the
// tile it is stepping into. One finishing a step it had begun at the throw
// is hit, one that has set off elsewhere is missed, and a hop in place
// (approaching Pokémon jump) changes nothing.
static bool32 IsOWEAtLanding(const struct PendingThrow *throw, const struct ObjectEvent *owe)
{
    return owe->currentCoords.x == throw->targetX && owe->currentCoords.y == throw->targetY;
}

static void ResolveImpact(struct PendingThrow *throw, struct ObjectEvent *owe)
{
    // The stack, not gEnemyParty: the spawn code builds its Pokémon there
    // and the controls are free while a Ball flies.
    struct Pokemon mon;
    bool32 aware = IsOWEAwareOfPlayer(owe);
    u32 shakes;

    // The Ball is still in the bag until it catches: one used up meanwhile
    // (thrown in a battle that started while this one flew) is gone, and
    // this one becomes a miss, or the catch would be free.
    if (!CheckBagHasItem(throw->ball, 1))
    {
        StartBallFall(throw);
        return;
    }

    // The Ball bounces off and comes back (P8).
    if (GetCatchBlock(owe) != THROW_BLOCK_NONE)
    {
        PlaySE(SE_BALL_BOUNCE_1);
        StartBallFall(throw);
        return;
    }

    CreateMonFromOWE(&mon, owe);
    shakes = RollShakes(ComputeOverworldCaptureOdds(&mon, owe, throw->ball, aware, throw->direction));
    throw->caught = (shakes == BALL_3_SHAKES_SUCCESS && GiveCaughtMon(throw, &mon));
    // BALL_3_SHAKES_FAIL breaks out after the third.
    throw->shakes = min(shakes, BALL_3_SHAKES_FAIL);

    // The Pokémon stops where it is, and goes into the Ball on the beam.
    throw->holdsOWE = TRUE;
    throw->hadNoDespawn = HoldOWEForThrow(owe);
    FreezeObjectEvent(owe);
    GetObjectGroundPos(owe, &throw->x, &throw->y);
    throw->height = BALL_HIT_HEIGHT;
    throw->state = THROW_OPENING;
    throw->timer = BALL_OPEN_FRAMES;
    PlaySE(SE_BALL_TRADE);

    if (!IsBallSpriteValid(throw) && !CreateBallSprite(throw))
        FinishThrow(throw);
}

static const s8 sShakeWobble[BALL_SHAKE_FRAMES] = {0, 1, 1, 2, 2, 1, 1, 0, 0, -1, -1, -2, -2, -1, -1, 0};

static void UpdateThrow(struct PendingThrow *throw)
{
    u32 frame = 0;
    s32 wobble = 0;
    bool32 visible = TRUE;

    // The field was rebuilt (a screen opened from it resets every sprite):
    // the slot is free or someone else's now. In the air the Ball just goes
    // unseen until it lands.
    if (throw->spriteId < MAX_SPRITES && !IsBallSpriteValid(throw))
    {
        throw->spriteId = MAX_SPRITES;
        if (throw->state != THROW_FLYING)
        {
            FinishThrow(throw);
            return;
        }
    }

    if (throw->holdsOWE)
        KeepOWEInBall(throw, throw->state != THROW_OPENING || throw->timer <= BALL_OPEN_FRAMES / 4);

    switch (throw->state)
    {
    case THROW_FREE:
        return;
    case THROW_FLYING:
    {
        struct ObjectEvent *owe = GetThrowTarget(throw);
        s32 elapsed, total = throw->flightTime;

        // Another Ball got there first (a volley at one Pokémon): this one
        // has nothing left to hit, or the Pokémon would be given twice.
        if (owe != NULL && OverworldCatch_IsOWEInBall(owe))
            owe = NULL;

        // The Ball flies to where the Pokémon stood at the throw and does
        // not follow it (user, 18/11, phase 6: it used to, more generous
        // than Legends: Arceus). Gone meanwhile, the Ball goes on and lands
        // there empty.
        elapsed = total - --throw->timer;
        throw->x = throw->fromX + (throw->toX - throw->fromX) * elapsed / total;
        throw->y = throw->fromY + (throw->toY - throw->fromY) * elapsed / total;
        // A straight line from the hand to the Pokémon, plus a parabola.
        throw->height = BALL_HAND_HEIGHT + (BALL_HIT_HEIGHT - BALL_HAND_HEIGHT) * elapsed / total
                      + 4 * (BALL_ARC_BASE + BALL_ARC_PER_TILE * total / THROW_FLIGHT_PER_TILE) * elapsed * (total - elapsed) / (total * total);

        if (throw->timer == 0)
        {
            // Hit if it is still on its tile, or has just stepped into it.
            // A miss comes back (D16).
            if (owe != NULL && IsOWEAtLanding(throw, owe))
                ResolveImpact(throw, owe);
            else
                StartBallFall(throw);
        }
        break;
    }
    case THROW_OPENING:
        frame = 1 + (BALL_OPEN_FRAMES - throw->timer) / (BALL_OPEN_FRAMES / 4);
        if (--throw->timer == 0)
        {
            throw->state = THROW_CLOSING;
            throw->timer = BALL_CLOSE_FRAMES;
            if (throw->caught)
                RemoveOWEInBall(throw);
        }
        break;
    case THROW_CLOSING:
        frame = (throw->timer + 1) / 2;
        if (--throw->timer == 0)
        {
            throw->fallHeight = throw->height;
            throw->state = THROW_FALLING;
            throw->timer = BALL_FALL_FRAMES;
        }
        break;
    case THROW_FALLING:
    {
        s32 left = --throw->timer;

        // Faster as it falls.
        throw->height = throw->fallHeight * left * left / (BALL_FALL_FRAMES * BALL_FALL_FRAMES);
        if (left == 0)
        {
            PlaySE(SE_BALL_BOUNCE_1);
            if (throw->miss)
            {
                throw->state = THROW_DROPPED;
                throw->timer = BALL_DROPPED_FRAMES;
            }
            else
            {
                throw->state = THROW_SHAKING;
                throw->timer = BALL_SHAKE_PAUSE;
            }
        }
        break;
    }
    case THROW_SHAKING:
        if (throw->timer > BALL_SHAKE_PAUSE)
            wobble = sShakeWobble[BALL_SHAKE_PAUSE + BALL_SHAKE_FRAMES - throw->timer];
        if (--throw->timer == 0)
        {
            if (throw->shakes != 0)
            {
                throw->shakes--;
                throw->timer = BALL_SHAKE_FRAMES + BALL_SHAKE_PAUSE;
                PlaySE(SE_BALL);
            }
            else if (throw->caught)
            {
                throw->state = THROW_CAUGHT;
                throw->timer = BALL_CAUGHT_FRAMES;
                PlaySE(SE_RG_BALL_CLICK);
                ShowCatchBanner(throw);
            }
            else
            {
                throw->state = THROW_BREAKOUT;
                throw->timer = BALL_BREAKOUT_FRAMES;
                ReleaseOWEFromBall(throw);
                PlaySE(SE_BALL_OPEN);
            }
        }
        break;
    case THROW_CAUGHT:
    case THROW_DROPPED:
        visible = (throw->timer > BALL_BLINK_FRAMES || (throw->timer & 2));
        if (--throw->timer == 0)
        {
            FinishThrow(throw);
            return;
        }
        break;
    case THROW_BREAKOUT:
        frame = min(4, 1 + (BALL_BREAKOUT_FRAMES - throw->timer) / 2);
        if (--throw->timer == 0)
        {
            FinishThrow(throw);
            return;
        }
        break;
    }

    DrawBall(throw, frame, wobble, visible);
}


// ---------------------------------------------------------------------------
// Selection panel (D18): bottom left, while L is held and ~2 s after a change
// or a throw. Same frame and text palettes as the catch banner (and so the
// same dark theme), no sprites.
// ---------------------------------------------------------------------------

static const u8 sText_PanelBall[] = _("Ball ×");
static const u8 sText_PanelLevel[] = _("Lv. ");
static const u8 sText_PanelNoBalls[] = _("No Balls");
static const u8 sText_PanelNoMon[] = _("Nobody can fight");
static const u8 sText_PanelBallLine[] = _("Ball");
static const u8 sText_PanelMonLine[] = _("Pokémon");

#define BANNER_FRAME_TILE   0x214
#define BANNER_FRAME_PAL    14

// The frame of the panel and of the catch notes: the player's choice, its
// palette in 14, darkened with the theme. Loaded here, not taken as found:
// after a save is loaded palette 14 holds whatever the title screen left
// until a message or a menu loads the frame, and the panel and the notes
// came out with a white border (user, 21/11). Not during a DexNav search,
// whose window puts its own palette in 14 (LoadDexNavWindowGfx).
static void LoadCatchFrameGfx(void)
{
    if (FlagGet(DN_FLAG_SEARCHING))
        return;
    LoadUserWindowBorderGfxOnBg(0, BANNER_FRAME_TILE, BG_PLTT_ID(BANNER_FRAME_PAL));
    ApplyDarkUiWindowFramePalette(BG_PLTT_ID(BANNER_FRAME_PAL));
}

static void HideCatchBanners(void);

static void HidePanel(void)
{
    if (!sCatch.panelUp)
        return;
    sCatch.panelUp = FALSE;
    ClearStdWindowAndFrameToTransparent(sCatch.panelWindowId, FALSE);
    CopyWindowToVram(sCatch.panelWindowId, COPYWIN_FULL);
    RemoveWindow(sCatch.panelWindowId);
}

// Two lines: the name of the Ball or the Pokémon, and "Ball ×12" or "Lv. 30".
static void DrawPanel(u32 windowId, u32 key)
{
    u8 *line1 = gStringVar4;
    u8 *line2 = gStringVar3;

    // Colour 1 is the box's white: SetStandardWindowBorderStyle fills the
    // window with it, and a transparent fill would show the map through.
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    if (sCatch.monMode)
    {
        u32 slot = GetSelectedMonSlot();

        if (slot == PARTY_SIZE)
        {
            StringCopy(line1, sText_PanelMonLine);
            StringCopy(line2, sText_PanelNoMon);
        }
        else
        {
            GetMonData(&gPlayerParty[slot], MON_DATA_NICKNAME, line1);
            StringGet_Nickname(line1);
            StringCopy(line2, sText_PanelLevel);
            ConvertIntToDecimalStringN(gStringVar1, GetMonData(&gPlayerParty[slot], MON_DATA_LEVEL), STR_CONV_MODE_LEFT_ALIGN, 3);
            StringAppend(line2, gStringVar1);
        }
    }
    else
    {
        u32 ball = GetSelectedBall();

        if (ball == ITEM_NONE)
        {
            StringCopy(line1, sText_PanelBallLine);
            StringCopy(line2, sText_PanelNoBalls);
        }
        else
        {
            StringCopy(line1, GetItemName(ball));
            StringCopy(line2, sText_PanelBall);
            ConvertIntToDecimalStringN(gStringVar1, CountTotalItemQuantityInBag(ball), STR_CONV_MODE_LEFT_ALIGN, 4);
            StringAppend(line2, gStringVar1);
        }
    }
    AddTextPrinterParameterized(windowId, FONT_SMALL, line1, 2, 0, 0, NULL);
    AddTextPrinterParameterized(windowId, FONT_SMALL, line2, 2, 12, 0, NULL);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    sCatch.panelKey = key;
}

static void UpdatePanel(void)
{
    bool32 show = FlagGet(FLAG_SYS_POKEMON_GET) && (sCatch.lHeld || sCatch.panelTimer != 0);

    // A message, a menu or a script owns the bottom of the screen, and a
    // DexNav search has its own window there (dexnav.c, AddSearchWindow:
    // BG0 at (1,16), tiles from 8, the same as the panel's).
    if (ArePlayerFieldControlsLocked() || gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A
     || FlagGet(DN_FLAG_SEARCHING) || !IsOverworldCatchEnabled())
        show = FALSE;
    if (sCatch.panelTimer != 0 && !sCatch.lHeld)
        sCatch.panelTimer--;

    if (!show)
    {
        HidePanel();
        return;
    }

    {
        // Mode, selection, quantity or level: a change redraws.
        u32 slot = GetSelectedMonSlot();
        u32 ball = GetSelectedBall();
        u32 key = sCatch.monMode ? (1 | slot << 1 | (slot < PARTY_SIZE ? GetMonData(&gPlayerParty[slot], MON_DATA_LEVEL) : 0) << 8)
                                 : (ball << 1 | CountTotalItemQuantityInBag(ball) << 16);

        if (!sCatch.panelUp)
        {
            struct WindowTemplate template;

            SetWindowTemplateFields(&template, 0, 2, 16, 12, 3, 15, 8);
            sCatch.panelWindowId = AddWindow(&template);
            if (sCatch.panelWindowId == WINDOW_NONE)
                return;
            sCatch.panelUp = TRUE;
            LoadCatchFrameGfx();
            PutWindowTilemap(sCatch.panelWindowId);
            SetStandardWindowBorderStyle(sCatch.panelWindowId, FALSE);
            DrawStdFrameWithCustomTileAndPalette(sCatch.panelWindowId, FALSE, 0x214, 14);
            DrawPanel(sCatch.panelWindowId, key);
        }
        else if (key != sCatch.panelKey)
        {
            DrawPanel(sCatch.panelWindowId, key);
        }
    }
}

// Every frame of the overworld, first thing in CB2: before the script and
// the tasks of the frame run. The panel shares BG0 tiles from 8 with the
// catch banner, the item description box and the Start menu's windows, and
// its tilemap rows with the message box. The controls are locked in CB1
// when any of them is about to open, so hiding here clears the panel before
// they draw. At the end of the frame (where it was) the hiding came after
// anything a script drew in the frame of the lock, a sign's message box for
// one, and would erase it where the two overlap (by reading, not seen).
void OverworldCatch_UpdatePanel(void)
{
    UpdatePanel();
    UpdateBanners();
}

// Right before the Start menu or the debug menu opens. They open from
// ProcessPlayerFieldInput, in CB1, and draw at once: hiding the panel and
// the notes in CB2, as for everything else, came after they had drawn and
// cut holes into them where the two overlapped (user, 21/11). The notes are
// kept and come back when the menu closes.
void OverworldCatch_ClearScreenForMenu(void)
{
    HidePanel();
    if (sCatch.numBanners != 0 && !sCatch.pileHidden)
        HideCatchBanners();
}

// Every frame of the overworld, after the spawns.
void OverworldCatch_Update(void)
{
    if (sCatch.cooldown != 0)
        sCatch.cooldown--;

    // While a script runs nothing moves: the script may be the battle with
    // the very Pokémon a Ball is flying at.
    if (ArePlayerFieldControlsLocked())
    {
        // OverworldCatch_HandleLButton does not run meanwhile. An L held now
        // is taken as already used, so letting it go later is not a tap
        // that switches between Ball and Pokémon.
        if (JOY_HELD(L_BUTTON))
            sCatch.lHeld = sCatch.lScrolled = TRUE;
        else
            sCatch.lHeld = FALSE;
        return;
    }

    // A Pokémon sent out is the lead of the battle its script starts, and
    // that script keeps the controls locked until SetBattlePartyIds has read
    // the slot. Controls free again with the slot still set mean the battle
    // never started (the target was gone): it must not lead the next one.
    gOverworldThrowLeadSlot = PARTY_SIZE;

    for (u32 i = 0; i < THROW_MAX_PENDING; i++)
        UpdateThrow(&sCatch.throws[i]);
}

// ---------------------------------------------------------------------------
// After the volley: banners, then nicknames
// ---------------------------------------------------------------------------

// A Ball is out (in the air or on the ground) until its record is free: no
// Start menu meanwhile. The throw lives in EWRAM, and a menu rebuilds the
// field's sprites under it.
bool32 OverworldCatch_IsBallOut(void)
{
    return CountThrows(THROW_FREE) != THROW_MAX_PENDING;
}

// A Ball lying on this tile (or about to: falling onto it): nobody walks
// onto it (GetVanillaCollision). In the air it is no obstacle.
bool32 OverworldCatch_IsBallOnGroundAt(s16 x, s16 y)
{
    for (u32 i = 0; i < THROW_MAX_PENDING; i++)
    {
        const struct PendingThrow *throw = &sCatch.throws[i];

        if (throw->state != THROW_FREE && throw->state != THROW_FLYING
         && throw->landBlocks && throw->landX == x && throw->landY == y
         && throw->landMapNum == gSaveBlock1Ptr->location.mapNum
         && throw->landMapGroup == gSaveBlock1Ptr->location.mapGroup)
            return TRUE;
    }
    return FALSE;
}

// Called when the player could start a script: the Nuzlocke's compulsory
// nicknames, once no Ball is out and the banners are gone. Returns TRUE if
// the script was started.
bool32 OverworldCatch_TryStartResults(void)
{
    if (sCatch.numResults == 0 || sCatch.numBanners != 0
     || CountThrows(THROW_FREE) != THROW_MAX_PENDING)
        return FALSE;

    sCatch.nicknameIndex = 0;
    ScriptContext_SetupScript(OverworldCatch_EventScript_Results);
    return TRUE;
}

// ---------------------------------------------------------------------------
// Catch banners (D14, made non-blocking on 20/11): the box of the "item
// obtained" description (ScriptShowItemDescription, overworld.c: same frame
// and palettes, so it follows the dark theme the same way) with the party
// icon of the Pokémon in the item's place. A pile of notes at the top (user,
// 21/11): the newest one lies on top and is the one shown; each older one
// shows only the bottom edge of its frame, one tile lower and narrower per
// note, and they go from the bottom of the pile up, each ~3 s after it came.
// BG0 tiles from 92, past the DexNav search window (8-91) and the panel
// (8-43); the edges are the frame's own tiles. The map name pop-up scrolls
// all of BG0 while it slides: a banner hides it and it clears the banners
// (ShowMapNamePopup). During a DexNav search its window can be at the top,
// so the pile moves down under it.
// ---------------------------------------------------------------------------

#define BANNER_ICON_X       26
#define BANNER_ICON_Y       16  // was 20: the icon's art sits low in its 32x32 frame (measured, 20/11)
#define BANNER_TEXT_X       40
#define BANNER_TILE_BASE    92
#define PILE_ROWS_PER_NOTE  2   // rows each note under the top one sticks out by
// Three tiles built from the frame (LoadPileJoinTiles), past the map name
// pop-up's frame (0x21D-0x23C) and before the Match Call window (0x270).
#define BANNER_JOIN_TILE    0x23D

static u32 GetBannerTopRow(void)
{
    return FlagGet(DN_FLAG_SEARCHING) ? 6 : 1;
}

static u8 CreateBannerIcon(const struct CatchBanner *banner, u32 row, bool32 objWindow)
{
    u8 spriteId = CreateMonIcon(banner->species, SpriteCB_MonIcon, BANNER_ICON_X,
                                BANNER_ICON_Y + 8 * (row - 1), 0, banner->personality);

    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].oam.priority = 0;
        if (objWindow)
            gSprites[spriteId].oam.objMode = ST_OAM_OBJ_WINDOW;
    }
    return spriteId;
}

// The note on top and the edges under it, off the screen.
static void ErasePile(void)
{
    if (!sCatch.pileUp)
        return;
    sCatch.pileUp = FALSE;
    if (sCatch.bannerIconSpriteId != MAX_SPRITES)
        FreeAndDestroyMonIconSprite(&gSprites[sCatch.bannerIconSpriteId]);
    if (sCatch.bannerIconSpriteId2 != MAX_SPRITES)
        FreeAndDestroyMonIconSprite(&gSprites[sCatch.bannerIconSpriteId2]);
    sCatch.bannerIconSpriteId = sCatch.bannerIconSpriteId2 = MAX_SPRITES;
    // The six mon icon palettes are shared by tag: freed only if no other
    // icon on screen uses it (the DexNav search HUD).
    if (sCatch.bannerIconPaletteNum != 0xFF)
        FieldEffectFreePaletteIfUnused(sCatch.bannerIconPaletteNum);
    sCatch.bannerIconPaletteNum = 0xFF;
    if (sCatch.bannerWindowId != WINDOW_NONE)
    {
        ClearStdWindowAndFrameToTransparent(sCatch.bannerWindowId, FALSE);
        CopyWindowToVram(sCatch.bannerWindowId, COPYWIN_FULL);
        RemoveWindow(sCatch.bannerWindowId);
        sCatch.bannerWindowId = WINDOW_NONE;
    }
    if (sCatch.bannerEdges != 0)
    {
        FillBgTilemapBufferRect(0, 0, 0, sCatch.bannerRow + 4, 30, PILE_ROWS_PER_NOTE * sCatch.bannerEdges, 0);
        CopyBgTilemapBufferToVram(0);
        sCatch.bannerEdges = 0;
    }
}

// "You caught XXX♂!" and "It was added to your party." or "It was sent to
// the PC.", on the note at the given tile row.
static void DrawTopNote(const struct CatchBanner *banner, u32 row)
{
    static const u8 sGenderColors[][3] =
    {
        [MON_MALE] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE, TEXT_COLOR_LIGHT_BLUE},
        [MON_FEMALE] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED, TEXT_COLOR_LIGHT_RED},
    };
    struct WindowTemplate template;
    u32 gender, x;
    u8 windowId;

    sCatch.pileUp = TRUE;
    sCatch.bannerIconSpriteId = sCatch.bannerIconSpriteId2 = MAX_SPRITES;
    sCatch.bannerIconPaletteNum = 0xFF;
    sCatch.bannerEdges = 0;
    SetWindowTemplateFields(&template, 0, 1, row, 28, 3, 15, BANNER_TILE_BASE);
    windowId = sCatch.bannerWindowId = AddWindow(&template);
    if (windowId == WINDOW_NONE)
        return;
    LoadCatchFrameGfx();
    FillWindowPixelBuffer(windowId, PIXEL_FILL(0));
    PutWindowTilemap(windowId);
    SetStandardWindowBorderStyle(windowId, FALSE);
    DrawStdFrameWithCustomTileAndPalette(windowId, FALSE, BANNER_FRAME_TILE, BANNER_FRAME_PAL);

    // The party icon: the 64x64 battle sprite does not fit a box 3 tiles
    // high. In a dark cave a second copy opens a hole in the darkness, as
    // the item icon does.
    LoadMonIconPalettePersonality(banner->species, banner->personality);
    sCatch.bannerIconSpriteId = CreateBannerIcon(banner, row, FALSE);
    if (GetFlashLevel() > 0 || InBattlePyramid_())
    {
        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJWIN_ON);
        SetGpuRegBits(REG_OFFSET_WINOUT, WINOUT_WINOBJ_OBJ);
        sCatch.bannerIconSpriteId2 = CreateBannerIcon(banner, row, TRUE);
    }
    // Remembered from whichever copy exists: with neither (no free sprite at
    // all) the palette stays loaded until the next map load frees them all.
    if (sCatch.bannerIconSpriteId != MAX_SPRITES)
        sCatch.bannerIconPaletteNum = gSprites[sCatch.bannerIconSpriteId].oam.paletteNum;
    else if (sCatch.bannerIconSpriteId2 != MAX_SPRITES)
        sCatch.bannerIconPaletteNum = gSprites[sCatch.bannerIconSpriteId2].oam.paletteNum;

    // FONT_SMALL has no gender glyphs (the symbol comes out as an "8"), so
    // the symbol is printed on its own in FONT_NORMAL, blue or red.
    StringCopy(gStringVar1, GetSpeciesName(banner->species));
    StringExpandPlaceholders(gStringVar4, sText_YouCaught);
    AddTextPrinterParameterized(windowId, FONT_SMALL, gStringVar4, BANNER_TEXT_X, 0, TEXT_SKIP_DRAW, NULL);
    x = BANNER_TEXT_X + GetStringWidth(FONT_SMALL, gStringVar4, 0);
    gender = GetGenderFromSpeciesAndPersonality(banner->species, banner->personality);
    if (gender == MON_MALE || gender == MON_FEMALE)
    {
        const u8 *symbol = (gender == MON_MALE) ? gText_MaleSymbol : gText_FemaleSymbol;

        AddTextPrinterParameterized3(windowId, FONT_NORMAL, x + 1, 0, sGenderColors[gender], TEXT_SKIP_DRAW, symbol);
        x += 1 + GetStringWidth(FONT_NORMAL, symbol, 0);
    }
    AddTextPrinterParameterized(windowId, FONT_SMALL, sText_Exclamation, x, 0, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_SMALL, banner->toParty ? sText_AddedToParty : sText_SentToPC, BANNER_TEXT_X, 12, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

// The bottom edge of most frames (tiles 6, 7, 8) ends in one or two rows of
// transparent pixels, 13 of the 20 frames among them the default one: a note
// drawn right under another showed the map through that strip (user, 22/11).
// The join tiles are the bottom edge (7) with its transparent pixels taken
// from the paper of the note under it: its left side (3), its fill (4) and
// its right side (5). Built from the frame's own data, not read back from
// VRAM: the frame's tiles are queued for the same VBlank. Static, because the
// copy to VRAM is queued too.
static EWRAM_DATA u8 sPileJoinTiles[3][TILE_SIZE_4BPP] = {0};

static void LoadPileJoinTiles(void)
{
    const u8 *frame = GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles;

    for (u32 t = 0; t < 3; t++)
    {
        for (u32 i = 0; i < TILE_SIZE_4BPP; i++)
        {
            u32 edge = frame[7 * TILE_SIZE_4BPP + i];
            u32 paper = frame[(3 + t) * TILE_SIZE_4BPP + i];
            u32 lo = (edge & 0xF) ? (edge & 0xF) : (paper & 0xF);
            u32 hi = (edge & 0xF0) ? (edge & 0xF0) : (paper & 0xF0);

            sPileJoinTiles[t][i] = hi | lo;
        }
    }
    LoadBgTiles(0, sPileJoinTiles, sizeof(sPileJoinTiles), BANNER_JOIN_TILE);
}

// The bottom edge of a note at tile row y, over a note under it that spans
// the columns left..right.
static void DrawPileJoin(u32 y, u32 left, u32 right)
{
    FillBgTilemapBufferRect(0, BANNER_JOIN_TILE + 0, left, y, 1, 1, BANNER_FRAME_PAL);
    FillBgTilemapBufferRect(0, BANNER_JOIN_TILE + 1, left + 1, y, right - left - 1, 1, BANNER_FRAME_PAL);
    FillBgTilemapBufferRect(0, BANNER_JOIN_TILE + 2, right, y, 1, 1, BANNER_FRAME_PAL);
}

// The notes under the top one, each sticking out by two tile rows and one
// tile narrower on each side: a row of paper (the frame's sides 3 and 5 and
// its fill, tile 4, the theme's colour 14) and its bottom edge (6, 7, 8).
// The bottom edge alone showed 2 px of paper over 5 of border: the notes
// below looked see-through (user, 21/11). The edge above each of them is
// redrawn with the join tiles.
static void DrawPileEdges(u32 row, u32 count)
{
    LoadPileJoinTiles();
    for (u32 k = 1; k <= count; k++)
    {
        u32 y = row + 4 + PILE_ROWS_PER_NOTE * (k - 1);

        FillBgTilemapBufferRect(0, BANNER_FRAME_TILE + 3, k, y, 1, 1, BANNER_FRAME_PAL);
        FillBgTilemapBufferRect(0, BANNER_FRAME_TILE + 4, k + 1, y, 28 - 2 * k, 1, BANNER_FRAME_PAL);
        FillBgTilemapBufferRect(0, BANNER_FRAME_TILE + 5, 29 - k, y, 1, 1, BANNER_FRAME_PAL);
        FillBgTilemapBufferRect(0, BANNER_FRAME_TILE + 6, k, y + 1, 1, 1, BANNER_FRAME_PAL);
        FillBgTilemapBufferRect(0, BANNER_FRAME_TILE + 7, k + 1, y + 1, 28 - 2 * k, 1, BANNER_FRAME_PAL);
        FillBgTilemapBufferRect(0, BANNER_FRAME_TILE + 8, 29 - k, y + 1, 1, 1, BANNER_FRAME_PAL);
        // The edge above: the top note's frame (row + 3) or the note above's.
        DrawPileJoin(y - 1, k, 29 - k);
    }
    sCatch.bannerEdges = count;
    CopyBgTilemapBufferToVram(0);
}

// Draws the pile again: the newest note on top, the edges of the others.
// With only the oldest gone the top note stays as it is.
static void RedrawPile(bool32 topChanged)
{
    u32 row = GetBannerTopRow();

    if (topChanged || row != sCatch.bannerRow)
    {
        ErasePile();
        sCatch.bannerRow = row;
        if (sCatch.numBanners != 0)
            DrawTopNote(&sCatch.banners[sCatch.numBanners - 1], row);
    }
    else if (sCatch.bannerEdges != 0)
    {
        FillBgTilemapBufferRect(0, 0, 0, row + 4, 30, PILE_ROWS_PER_NOTE * sCatch.bannerEdges, 0);
        // The top note's own bottom edge, joined to the note that was under it.
        FillBgTilemapBufferRect(0, BANNER_FRAME_TILE + 7, 1, row + 3, 28, 1, BANNER_FRAME_PAL);
        sCatch.bannerEdges = 0;
    }
    if (sCatch.pileUp && sCatch.numBanners > 1)
        DrawPileEdges(row, sCatch.numBanners - 1);
    else
        CopyBgTilemapBufferToVram(0);
}

static void RemoveOldestBanner(void)
{
    sCatch.numBanners--;
    for (u32 i = 0; i < sCatch.numBanners; i++)
        sCatch.banners[i] = sCatch.banners[i + 1];
}

static void DismissCatchBanners(void)
{
    ErasePile();
    sCatch.numBanners = 0;
    sCatch.pileHidden = FALSE;
}

// A battle, a message or a menu: the notes leave the screen but are kept,
// and come back, each with its full time, once the player is free again. A
// battle that starts while a Ball shakes used to drop them, and the player
// never learnt what the Ball had caught (user, 21/11).
static void HideCatchBanners(void)
{
    ErasePile();
    if (sCatch.numBanners != 0)
        sCatch.pileHidden = TRUE;
}

// Called from ShowMapNamePopup: the pop-up scrolls all of BG0 while it
// slides. The notes were seen already: they go.
void OverworldCatch_DismissBanners(void)
{
    if (sCatch.numBanners != 0)
        DismissCatchBanners();
}

// A catch: its note on top of the pile, the oldest dropped if the pile is
// full. A short sound, no fanfare: the music goes on (user, 20/11).
static void ShowCatchBanner(struct PendingThrow *throw)
{
    struct CatchBanner *banner;

    throw->bannerShown = TRUE;
    while (sCatch.numBanners >= BANNER_MAX)
        RemoveOldestBanner();
    banner = &sCatch.banners[sCatch.numBanners++];
    banner->species = throw->caughtSpecies;
    banner->personality = throw->caughtPersonality;
    banner->toParty = throw->caughtToParty;
    banner->timer = BANNER_SHOW_FRAMES;
    // A battle starting (the Balls on the ground finish before it) or a
    // message up: the note waits for the player to be free (UpdateBanners).
    // A fade too: the frame's palette load would light it up early.
    if (sCatch.battleStarting || ArePlayerFieldControlsLocked() || gPaletteFade.active)
    {
        HideCatchBanners();
        return;
    }

    HideMapNamePopUpWindow();
    sCatch.pileHidden = FALSE;
    RedrawPile(TRUE);
    PlaySE(SE_SUCCESS);
}

// Each frame, first thing in CB2 (with the panel): a message, a menu or a
// script owns the screen, so the pile goes; otherwise each note counts down
// from when it came, and the oldest, at the bottom, goes first.
static void UpdateBanners(void)
{
    u32 before = sCatch.numBanners;

    if (sCatch.numBanners == 0)
        return;
    if (ArePlayerFieldControlsLocked() || gPaletteFade.active)
    {
        if (!sCatch.pileHidden)
            HideCatchBanners();
        return;
    }
    // Back from a battle or a menu: the notes come back, the oldest still
    // first to go.
    if (sCatch.pileHidden)
    {
        sCatch.pileHidden = FALSE;
        for (u32 i = 0; i < sCatch.numBanners; i++)
            sCatch.banners[i].timer = BANNER_SHOW_FRAMES + 30 * i;
        HideMapNamePopUpWindow();
        RedrawPile(TRUE);
        PlaySE(SE_SUCCESS);
        return;
    }
    for (u32 i = 0; i < sCatch.numBanners; i++)
        sCatch.banners[i].timer--;
    while (sCatch.numBanners != 0 && sCatch.banners[0].timer == 0)
        RemoveOldestBanner();
    // A search started or ended under the pile: it moves.
    if (sCatch.numBanners != before || GetBannerTopRow() != sCatch.bannerRow)
        RedrawPile(sCatch.numBanners == 0);
}

// The next caught Pokémon to offer a nickname for (asked after the last
// banner, D15). Sets VAR_RESULT to FALSE when the queue is over, otherwise
// selects the Pokémon for ChangePokemonNickname and puts its species in
// STR_VAR_1.
void OverworldCatch_SelectNextNickname(struct ScriptContext *ctx)
{
    Script_RequestEffects(SCREFF_V1);

    while (sCatch.nicknameIndex < sCatch.numResults)
    {
        const struct CatchResult *result = &sCatch.results[sCatch.nicknameIndex++];
        struct BoxPokemon *boxMon = NULL;

        if (result->toParty)
        {
            // By personality, not by slot: a battle fought before the
            // banner may have compacted the party (Nuzlocke deaths).
            for (u32 i = 0; i < PARTY_SIZE; i++)
            {
                if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == result->species
                 && GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY) == result->personality)
                {
                    gSpecialVar_0x8004 = i;
                    boxMon = &gPlayerParty[i].box;
                    break;
                }
            }
        }
        else
        {
            boxMon = GetBoxedMonPtr(result->boxId, result->boxPos);
            if (GetBoxMonData(boxMon, MON_DATA_SPECIES) != result->species
             || GetBoxMonData(boxMon, MON_DATA_PERSONALITY) != result->personality)
                boxMon = NULL;
            gSpecialVar_0x8004 = PC_MON_CHOSEN;
            gSpecialVar_MonBoxId = result->boxId;
            gSpecialVar_MonBoxPos = result->boxPos;
        }

        if (boxMon != NULL)
        {
            StringCopy(gStringVar1, GetSpeciesName(result->species));
            gSpecialVar_Result = TRUE;
            return;
        }
    }

    sCatch.numResults = 0;
    gSpecialVar_Result = FALSE;
}
