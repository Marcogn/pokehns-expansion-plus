#include "global.h"
#include "battle_setup.h"
#include "challenge_menu.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_camera.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/battle_partner.h"
#include "constants/characters.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/flags.h"
#include "constants/opponents.h"
#include "constants/pokeball.h"
#include "constants/reds_path.h"
#include "constants/species.h"

// Red's Path in the Gyms (docs/plans/reds-path.md, session B): what the aide
// says, the scene around the battle and the prizes. The battle itself is
// RedsPath_EventScript_Battle (src/battle_partner.c and the battle hooks).
// Every special takes the challenge, which is also the Gym, in VAR_0x8004.

#define PRIZE_LEVEL 70
#define KEEP_HNS_SPRITE 0 // sceneGfx: the Leader is the same person in the scene

struct RedsPathGym
{
    const u8 *leaderName;  // as the aide says it
    const u8 *otName;      // the prize's Original Trainer, PLAYER_NAME_LENGTH at most
    const u8 *placeName;
    u16 hnsLeaderFlag;     // the HnS Leader of this Gym is beaten
    u16 prizeFlag;
    u16 aceSpecies;
    u16 sceneGfx;          // the Leader as Red met them; or KEEP_HNS_SPRITE
    u8 otGender;
    u8 leaderLocalId;
    u8 aideLocalId;
    u8 sceneX, sceneY;     // where Red stands in the scene: below the Leader, facing up
};

// Local ids are the objects' positions in each Gym's map.json. Janine and
// Blue are played by Koga and Giovanni in the scene: it is Red's battle.
static const struct RedsPathGym sGyms[REDS_PATH_CHALLENGE_COUNT] =
{
    {
        .leaderName = COMPOUND_STRING("BROCK"), .otName = COMPOUND_STRING("BROCK"),
        .placeName = COMPOUND_STRING("PEWTER CITY"),
        .hnsLeaderFlag = FLAG_DEFEATED_PEWTER_GYM, .prizeFlag = FLAG_REDS_PATH_PRIZE_BROCK,
        .aceSpecies = SPECIES_ONIX, .sceneGfx = KEEP_HNS_SPRITE, .otGender = MALE,
        .leaderLocalId = 3, .aideLocalId = 1, .sceneX = 6, .sceneY = 6,
    },
    {
        .leaderName = COMPOUND_STRING("MISTY"), .otName = COMPOUND_STRING("MISTY"),
        .placeName = COMPOUND_STRING("CERULEAN CITY"),
        .hnsLeaderFlag = FLAG_DEFEATED_CERULEAN_GYM, .prizeFlag = FLAG_REDS_PATH_PRIZE_MISTY,
        .aceSpecies = SPECIES_STARMIE, .sceneGfx = KEEP_HNS_SPRITE, .otGender = FEMALE,
        .leaderLocalId = 1, .aideLocalId = 6, .sceneX = 7, .sceneY = 7,
    },
    {
        // "LT. SURGE" is one character too long for an OT name.
        .leaderName = COMPOUND_STRING("LT. SURGE"), .otName = COMPOUND_STRING("SURGE"),
        .placeName = COMPOUND_STRING("VERMILION CITY"),
        .hnsLeaderFlag = FLAG_DEFEATED_VERMILION_GYM, .prizeFlag = FLAG_REDS_PATH_PRIZE_SURGE,
        .aceSpecies = SPECIES_RAICHU, .sceneGfx = KEEP_HNS_SPRITE, .otGender = MALE,
        .leaderLocalId = 1, .aideLocalId = 5, .sceneX = 5, .sceneY = 3,
    },
    {
        .leaderName = COMPOUND_STRING("ERIKA"), .otName = COMPOUND_STRING("ERIKA"),
        .placeName = COMPOUND_STRING("CELADON CITY"),
        .hnsLeaderFlag = FLAG_DEFEATED_CELADON_GYM, .prizeFlag = FLAG_REDS_PATH_PRIZE_ERIKA,
        .aceSpecies = SPECIES_VILEPLUME, .sceneGfx = KEEP_HNS_SPRITE, .otGender = FEMALE,
        .leaderLocalId = 1, .aideLocalId = 10, .sceneX = 6, .sceneY = 5,
    },
    {
        .leaderName = COMPOUND_STRING("KOGA"), .otName = COMPOUND_STRING("KOGA"),
        .placeName = COMPOUND_STRING("FUCHSIA CITY"),
        .hnsLeaderFlag = FLAG_DEFEATED_FUCHSIA_GYM, .prizeFlag = FLAG_REDS_PATH_PRIZE_KOGA,
        .aceSpecies = SPECIES_WEEZING, .sceneGfx = OBJ_EVENT_GFX_KOGA_HNS, .otGender = MALE,
        .leaderLocalId = 5, .aideLocalId = 6, .sceneX = 2, .sceneY = 12,
    },
    {
        .leaderName = COMPOUND_STRING("SABRINA"), .otName = COMPOUND_STRING("SABRINA"),
        .placeName = COMPOUND_STRING("SAFFRON CITY"),
        .hnsLeaderFlag = FLAG_DEFEATED_SAFFRON_GYM, .prizeFlag = FLAG_REDS_PATH_PRIZE_SABRINA,
        .aceSpecies = SPECIES_ALAKAZAM, .sceneGfx = KEEP_HNS_SPRITE, .otGender = FEMALE,
        .leaderLocalId = 5, .aideLocalId = 6, .sceneX = 14, .sceneY = 12,
    },
    {
        // Blaine's Gym is on the Seafoam Islands in HnS: Cinnabar's is gone.
        .leaderName = COMPOUND_STRING("BLAINE"), .otName = COMPOUND_STRING("BLAINE"),
        .placeName = COMPOUND_STRING("the SEAFOAM ISLANDS"),
        .hnsLeaderFlag = FLAG_DEFEATED_CINNABAR_ISLAND_GYM, .prizeFlag = FLAG_REDS_PATH_PRIZE_BLAINE,
        .aceSpecies = SPECIES_ARCANINE, .sceneGfx = KEEP_HNS_SPRITE, .otGender = MALE,
        .leaderLocalId = 1, .aideLocalId = 11, .sceneX = 18, .sceneY = 11,
    },
    {
        // "GIOVANNI" is one character too long for an OT name.
        .leaderName = COMPOUND_STRING("GIOVANNI"), .otName = COMPOUND_STRING("GIOVANN"),
        .placeName = COMPOUND_STRING("VIRIDIAN CITY"),
        .hnsLeaderFlag = FLAG_DEFEATED_VIRIDIAN_GYM, .prizeFlag = FLAG_REDS_PATH_PRIZE_GIOVANNI,
        .aceSpecies = SPECIES_RHYDON, .sceneGfx = OBJ_EVENT_GFX_GIOVANNI_HNS, .otGender = MALE,
        .leaderLocalId = 2, .aideLocalId = 1, .sceneX = 5, .sceneY = 3,
    },
};

// Where the player was before the scene, and the Leader's own sprite. RAM only:
// the scene is a single script, so the game cannot be saved in the middle of
// it, and a reset reloads the player where the save left them.
static EWRAM_DATA struct
{
    s16 x, y;
    u16 leaderGfx;
    u8 facing;
    u8 challenge;
    bool8 followerWasHidden;
} sScene = {0};

// Movement scripts built at run time for applymovement (the aide's walk to
// the player and back, and the player turning to face them). Longest walk in
// the eight Gyms: under 20 steps.
#define AIDE_MOVEMENT_LENGTH 32
EWRAM_DATA u8 gRedsPathAideApproach[AIDE_MOVEMENT_LENGTH] = {0};
EWRAM_DATA u8 gRedsPathAideReturn[AIDE_MOVEMENT_LENGTH] = {0};
EWRAM_DATA u8 gRedsPathPlayerFaceAide[2] = {0};

static u32 GetChallenge(void)
{
    if (gSpecialVar_0x8004 >= REDS_PATH_CHALLENGE_COUNT)
        return 0;
    return gSpecialVar_0x8004;
}

static bool32 IsChallengeWon(u32 challenge)
{
    return HasTrainerBeenFought(TRAINER_REDS_PATH_BROCK_HNS + challenge);
}

// The first challenge not won yet, REDS_PATH_CHALLENGE_COUNT once all are.
// The quest book reads it too (src/data/quests.h).
u32 RedsPath_GetNextChallenge(void)
{
    u32 i;

    for (i = 0; i < REDS_PATH_CHALLENGE_COUNT; i++)
    {
        if (!IsChallengeWon(i))
            break;
    }
    return i;
}

// Quest book (src/data/quests.h).
bool32 RedsPath_IsIntroSeen(void)
{
    return FlagGet(FLAG_REDS_PATH_INTRO_SEEN);
}

bool32 RedsPath_IsChallengeWon(u32 challenge)
{
    return challenge < REDS_PATH_CHALLENGE_COUNT && IsChallengeWon(challenge);
}

// The first challenge won whose prize is not collected, REDS_PATH_CHALLENGE_COUNT
// if there is none.
u32 RedsPath_GetPrizeWaiting(void)
{
    u32 i;

    for (i = 0; i < REDS_PATH_CHALLENGE_COUNT; i++)
    {
        if (IsChallengeWon(i) && !FlagGet(sGyms[i].prizeFlag))
            break;
    }
    return i;
}

// specialvar: is this Gym's own (HnS) Leader beaten? The trigger in front of
// the door only shows the introduction after that.
bool8 RedsPath_IsGymLeaderBeaten(void)
{
    return FlagGet(sGyms[GetChallenge()].hnsLeaderFlag);
}

// specialvar: what the aide of this Gym has to say (REDS_PATH_AIDE_*). Also
// buffers the next challenge for the texts that name it, see
// constants/reds_path.h.
u16 RedsPath_GetAideState(void)
{
    u32 i, challenge = GetChallenge();
    u32 next = RedsPath_GetNextChallenge();

    StringCopy(gStringVar3, sGyms[challenge].leaderName);
    if (next < REDS_PATH_CHALLENGE_COUNT)
    {
        StringCopy(gStringVar1, sGyms[next].leaderName);
        StringCopy(gStringVar2, sGyms[next].placeName);
        gSpecialVar_0x8005 = FlagGet(sGyms[next].hnsLeaderFlag);
    }

    if (IsChallengeWon(challenge) && !FlagGet(sGyms[challenge].prizeFlag))
        return REDS_PATH_AIDE_PRIZE;
    if (next == challenge)
        return REDS_PATH_AIDE_OFFER;
    if (next < challenge)
        return REDS_PATH_AIDE_OUT_OF_ORDER;
    if (next < REDS_PATH_CHALLENGE_COUNT)
        return REDS_PATH_AIDE_DONE_HERE;
    for (i = 0; i < REDS_PATH_CHALLENGE_COUNT; i++)
    {
        if (!FlagGet(sGyms[i].prizeFlag))
            return REDS_PATH_AIDE_PRIZES_LEFT;
    }
    return REDS_PATH_AIDE_ALL_DONE;
}

// specialvar, after RedsPath_EventScript_Battle: did Red win? The battle
// does not keep VAR_0x8004, so this reads the challenge the scene saved.
bool8 RedsPath_WonSceneBattle(void)
{
    return IsChallengeWon(sScene.challenge);
}

// Moves the player, and the camera with them, to (x, y) in one step. Used
// while the screen is black, so the jump is not seen.
static void TeleportPlayer(s16 x, s16 y)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];

    MoveCameraAndRedrawMap(x - player->currentCoords.x, y - player->currentCoords.y);
    MoveObjectEventToMapCoords(player, x, y);
}

static struct ObjectEvent *GetLeader(u32 challenge)
{
    u32 objectEventId = GetObjectEventIdByLocalId(sGyms[challenge].leaderLocalId);

    if (objectEventId >= OBJECT_EVENTS_COUNT)
        return NULL;
    return &gObjectEvents[objectEventId];
}

// special, with the screen black: the player puts on the headset. Red stands
// in front of the Leader as they were then. The follower is not part of the
// simulation. RedsPath_ExitScene undoes all of it.
// Red is an object of his own over the hidden player, not the player with
// Red's sprite: back from the battle, SetPlayerAvatarObjectEventIdAndObjectId
// works out the avatar's state and gender from the player's sprite and puts
// the player's own sprite back (and a female player would come back male).
void RedsPath_EnterScene(void)
{
    u32 challenge = GetChallenge();
    const struct RedsPathGym *gym = &sGyms[challenge];
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct ObjectEvent *leader;
    u8 redId;

    sScene.challenge = challenge;
    sScene.x = player->currentCoords.x;
    sScene.y = player->currentCoords.y;
    sScene.facing = player->facingDirection;
    sScene.followerWasHidden = FlagGet(FLAG_TEMP_HIDE_FOLLOWER);

    FlagSet(FLAG_TEMP_HIDE_FOLLOWER);
    UpdateFollowingPokemon();

    TeleportPlayer(gym->sceneX + MAP_OFFSET, gym->sceneY + MAP_OFFSET);
    ObjectEventTurn(player, DIR_NORTH);
    player->invisible = TRUE;
    redId = SpawnSpecialObjectEventParameterized(OBJ_EVENT_GFX_RED_NORMAL_HNS, MOVEMENT_TYPE_FACE_UP,
                                                 LOCALID_REDS_PATH_RED,
                                                 player->currentCoords.x, player->currentCoords.y,
                                                 player->currentElevation);
    if (redId < OBJECT_EVENTS_COUNT)
        ObjectEventTurn(&gObjectEvents[redId], DIR_NORTH);

    // Only now: the Leader can be out of view from the door (Janine is), and
    // objects only exist while in view.
    leader = GetLeader(challenge);
    if (leader != NULL)
    {
        sScene.leaderGfx = leader->graphicsId;
        if (gym->sceneGfx != KEEP_HNS_SPRITE)
            ObjectEventSetGraphicsId(leader, gym->sceneGfx);
        ObjectEventTurn(leader, DIR_SOUTH);
    }
    // What came into view with the jump missed the script's lockall, and
    // Janine's spot would have Koga looking around during the scene.
    FreezeObjectEvents();
}

// special, with the screen black: the headset comes off, and the player is
// back where they stood, facing the aide.
void RedsPath_ExitScene(void)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct ObjectEvent *leader = GetLeader(sScene.challenge);

    if (leader != NULL)
    {
        if (leader->graphicsId != sScene.leaderGfx)
            ObjectEventSetGraphicsId(leader, sScene.leaderGfx);
        ObjectEventTurn(leader, DIR_SOUTH);
    }

    RemoveObjectEventByLocalIdAndMap(LOCALID_REDS_PATH_RED, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    player->invisible = FALSE;
    TeleportPlayer(sScene.x, sScene.y);
    ObjectEventTurn(player, sScene.facing);

    if (!sScene.followerWasHidden)
        FlagClear(FLAG_TEMP_HIDE_FOLLOWER);
    UpdateFollowingPokemon();
    gSpecialVar_0x8004 = sScene.challenge;
}

static u32 AppendSteps(u8 *movement, u32 n, u32 direction, s32 steps)
{
    while (steps-- > 0 && n < AIDE_MOVEMENT_LENGTH - 2)
        movement[n++] = GetWalkNormalMovementAction(direction);
    return n;
}

// special: builds gRedsPathAideApproach, the aide's walk to the tile next to
// the player on the aide's side (down or up to the player's row first, then
// across), gRedsPathAideReturn, the same walk back, and
// gRedsPathPlayerFaceAide. VAR_0x8005 = the aide's local id, for
// applymovement. The trigger rows are open floor, and so is the aide's
// column down to them: checked for each Gym.
void RedsPath_BuildAideApproach(void)
{
    const struct RedsPathGym *gym = &sGyms[GetChallenge()];
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct ObjectEvent *aide;
    u32 objectEventId = GetObjectEventIdByLocalId(gym->aideLocalId);
    s32 dx, dy, vertical, across;
    u32 verticalDir, acrossDir, faceDir, n;

    gSpecialVar_0x8005 = gym->aideLocalId;
    gRedsPathAideApproach[0] = gRedsPathAideReturn[0] = MOVEMENT_ACTION_STEP_END;
    gRedsPathPlayerFaceAide[0] = MOVEMENT_ACTION_STEP_END;
    if (objectEventId >= OBJECT_EVENTS_COUNT)
        return;
    aide = &gObjectEvents[objectEventId];

    dx = player->currentCoords.x - aide->currentCoords.x;
    dy = player->currentCoords.y - aide->currentCoords.y;
    verticalDir = dy > 0 ? DIR_SOUTH : DIR_NORTH;
    acrossDir = dx > 0 ? DIR_EAST : DIR_WEST;
    if (dx == 0)
    {
        vertical = (dy > 0 ? dy : -dy) - 1;
        across = 0;
        faceDir = verticalDir;
    }
    else
    {
        vertical = dy > 0 ? dy : -dy;
        across = (dx > 0 ? dx : -dx) - 1;
        faceDir = acrossDir;
    }

    n = AppendSteps(gRedsPathAideApproach, 0, verticalDir, vertical);
    n = AppendSteps(gRedsPathAideApproach, n, acrossDir, across);
    gRedsPathAideApproach[n++] = GetFaceDirectionMovementAction(faceDir);
    gRedsPathAideApproach[n] = MOVEMENT_ACTION_STEP_END;

    n = AppendSteps(gRedsPathAideReturn, 0, GetOppositeDirection(acrossDir), across);
    n = AppendSteps(gRedsPathAideReturn, n, GetOppositeDirection(verticalDir), vertical);
    gRedsPathAideReturn[n++] = GetFaceDirectionMovementAction(aide->facingDirection);
    gRedsPathAideReturn[n] = MOVEMENT_ACTION_STEP_END;

    gRedsPathPlayerFaceAide[0] = GetFaceDirectionMovementAction(GetOppositeDirection(faceDir));
    gRedsPathPlayerFaceAide[1] = MOVEMENT_ACTION_STEP_END;
}

// A fixed Trainer ID for each Leader, from their name, like the partners'
// (GetPartnerOtId): the same Leader always has the same ID.
static u32 GetLeaderOtId(const u8 *name)
{
    u32 id = 0;

    while (*name != EOS)
        id = id * 31 + *name++;
    return id;
}

// special: the prize of this Gym, into the party. VAR_RESULT = MON_GIVEN_TO_*
// for Common_EventScript_GiftMon, or MON_CANT_GIVE with no room in the party
// (the plan: never to the PC, come back with room), STR_VAR_1 = species.
// The Leader's ace at Lv70 with its level-up moves, IVs 31, random nature,
// and shiny at the normal odds: it is rolled here, so a reset in front of
// the aide gives another try. Everything else is the Leader's: OT, ID, Ball.
void RedsPath_GivePrize(void)
{
    u32 challenge = GetChallenge();
    const struct RedsPathGym *gym = &sGyms[challenge];
    struct Pokemon mon;
    u32 i, value, otId;
    u8 otIdBytes[4];
    u8 otName[PLAYER_NAME_LENGTH + 1];

    gSpecialVar_Result = MON_CANT_GIVE;
    StringCopy(gStringVar1, GetSpeciesName(gym->aceSpecies));
    if (CalculatePlayerPartyCount() >= GetMaxPartySize())
        return;

    CreateMon(&mon, gym->aceSpecies, PRIZE_LEVEL,
              GetMonPersonality(gym->aceSpecies, MON_GENDER_RANDOM, NATURE_RANDOM, RANDOM_UNOWN_LETTER),
              OTID_STRUCT_PLAYER_ID);
    // CreateMon leaves the moves empty: givemon adds them on its own too.
    GiveMonInitialMoveset(&mon);
    value = MAX_PER_STAT_IVS;
    for (i = 0; i < NUM_STATS; i++)
        SetMonData(&mon, MON_DATA_HP_IV + i, &value);
    CalculateMonStats(&mon);
    value = BALL_POKE;
    SetMonData(&mon, MON_DATA_POKEBALL, &value);

    memset(otName, EOS, sizeof(otName));
    StringCopyN(otName, gym->otName, PLAYER_NAME_LENGTH);
    SetMonData(&mon, MON_DATA_OT_NAME, otName);
    value = gym->otGender;
    SetMonData(&mon, MON_DATA_OT_GENDER, &value);
    otId = GetLeaderOtId(gym->leaderName);
    for (i = 0; i < 4; i++)
        otIdBytes[i] = otId >> (8 * i);
    // Keeps the shininess rolled above against the player's ID.
    SetMonOtIdRekeyed(&mon, otIdBytes);

    gSpecialVar_Result = GiveScriptedMonToPlayer(&mon, PARTY_SIZE);
    if (gSpecialVar_Result != MON_CANT_GIVE)
        FlagSet(gym->prizeFlag);
}

