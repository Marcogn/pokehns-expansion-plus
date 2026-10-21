#ifndef GUARD_WILD_ENCOUNTER_OW_H
#define GUARD_WILD_ENCOUNTER_OW_H

#define OWE_SPAWNS_MAX                  6   // Hasep: 6 (decision D4), was 4. Also the land cap, see below.
// Hasep: per-area caps (decision D3). OWE_SPAWNS_MAX stays the hard limit and
// sizes the slot arrays; these lower it while surfing or in caves/indoors.
#define OWE_SPAWNS_MAX_LAND             OWE_SPAWNS_MAX
#define OWE_SPAWNS_MAX_WATER            3
#define OWE_SPAWNS_MAX_CAVE             3
// Hasep: the user asked for 4 in the Safari Zone (land only; its water and
// cave keep the caps above). With 6 there, many of them AGGRESSIVE (Tauros,
// Rhyhorn), a test run had 18 battles in about 8 minutes.
#define OWE_SPAWNS_MAX_SAFARI           4
// Hasep: random tiles tried per spawn attempt (upstream: 1); see
// TrySelectTilesForOWE. Game time to fill 6 on Route 31 from a warp, 3 runs:
// 4 tries 17-31 s, 6 tries 11-29 s, 8 tries 9-13 s (the spawn countdowns
// alone take about 10 s). The user chose 6 for now, 8 to try later.
#define OWE_SPAWN_TILE_TRIES            6
// Hasep: sprite tiles a spawn must leave free for icons and field effects
// ("!", "?", follower emotes, grass, splashes). With OW_GFX_COMPRESS every
// object keeps its whole sheet in VRAM, and in the Johto Safari Zone the OWEs
// filled it down to 10 free tiles of 1024. 32 = room for several 16x16 icons;
// one OWE sheet is 112 tiles (32x32, 7 frames) or more.
#define OWE_SPRITE_TILE_HEADROOM        32
#define OWE_APPROACH_DISTANCE           2
#define OWE_APPROACH_JUMP_TIMER_MIN     16
#define OWE_APPROACH_JUMP_TIMER_MAX     64
#define OWE_FLEE_COLLISION_TIME         6   // If a fleeing mon is unable to take a step for this many tries it will despawn. (Multiply this value by 16 to get number of frames.)
#define OWE_DESPAWN_FRAMES              30  // Number of frames before a mon despawns after noticing the player (OWE_BEHAVIOR_DESPAWN)

enum SpawnDespawnTypeOWE
{
    OWE_SPAWN_ANIM_GRASS,
    OWE_SPAWN_ANIM_LONG_GRASS,
    OWE_SPAWN_ANIM_WATER,
    OWE_SPAWN_ANIM_UNDERWATER,
    OWE_SPAWN_ANIM_CAVE,
    OWE_SPAWN_ANIM_SHINY
};

enum TypeOWE
{
    OWE_ANY,
    OWE_GENERATED,
    OWE_MANUAL,
    OWE_NONE
};

// OWE_SPEED_FASTER seems to visually bug out sometimes.
enum SpeedOWE
{
    OWE_SPEED_NORMAL,
    OWE_SPEED_SLOW,
    OWE_SPEED_FAST,
    OWE_SPEED_FASTER
};

enum ReturnToIdleOWE
{
    NEVER_RETURN,
    PLAYER_OUTSIDE_ACTIVE_RANGE,
    PLAYER_CANT_BE_SEEN
};

struct BehaviorOWE
{
    u32 movementType:8;
    u32 viewDistance:4;
    u32 viewWidth:4;
    u32 activeDistance:4;
    enum SpeedOWE idleSpeed:2;
    enum SpeedOWE activeSpeed:2;
    enum ReturnToIdleOWE returnToIdle:2;
    u32 padding:6;
};

enum __attribute__((packed)) OverworldWildEncounterBehaviors
{
    OWE_IGNORE_PLAYER,
    OWE_CHASE_PLAYER_SLOW,
    OWE_FLEE_PLAYER_NORMAL,
    OWE_WATCH_PLAYER_NORMAL,
    OWE_APPROACH_PLAYER_SLOW,
    OWE_DESPAWN_ON_NOTICE,
    OWE_SPECIES_BEHAVIOR_COUNT
};

// Hasep: each OWE rolls its behaviour when it spawns, weighted by its
// species' temperament (docs/plans/overworld-encounters.md, phase 4; data
// from tools/owe_temperaments.py). NEUTRO must stay 0: it is the default of
// every species the table does not list.
enum OverworldWildEncounterTemperament
{
    OWE_TEMPERAMENT_NEUTRO,
    OWE_TEMPERAMENT_TIMIDO,
    OWE_TEMPERAMENT_AGGRESSIVO,
    OWE_TEMPERAMENT_CURIOSO,
    OWE_TEMPERAMENT_PLACIDO,
    OWE_TEMPERAMENT_SFUGGENTE,
    OWE_TEMPERAMENT_COUNT
};

struct FieldEffectInfoOWE
{
    s16 xOffset;
    s16 yOffset;
    u32 visual;
};

extern const struct FieldEffectInfoOWE gOverworldWildEncounterFieldEffectInfo[];

void UpdateOverworldWildEncounter(void);
bool32 IsOverworldWildEncounter(struct ObjectEvent *owe, enum TypeOWE oweType);
void SetInstantOWESpawnTimer(void);
void SetMinimumOWESpawnTimer(void);
void TryTriggerOverworldWildEncounter(struct ObjectEvent *obstacle, struct ObjectEvent *collider);
const u8 *GetOverworlWildEncounterScript(u32 objectEventId);
void OnOverworldWildEncounterSpawn(struct ObjectEvent *owe);
void OnOverworldWildEncounterDespawn(struct ObjectEvent *owe);
bool32 IsOWEDespawnExempt(struct ObjectEvent *owe);
bool32 DespawnOWEDueToNPCCollision(struct ObjectEvent *obstacle, struct ObjectEvent *activeObject);
void DespawnAllOverworldWildEncounters(enum TypeOWE oweType, u32 flags);
bool32 TryAndDespawnOldestGeneratedOWE_ToFreeObject(u8 *objectEventId);
void DespawnOWEOnBattleStart(void);
void TryDespawnOWEsCrossingMapConnection(void);
void RestoreSavedOWEBehaviorState(struct ObjectEvent *owe, struct Sprite *sprite);
void SetSavedOWEMovementState(struct ObjectEvent *owe);
void ClearSavedOWEMovementState(struct ObjectEvent *owe);
bool32 CheckRestrictedOWEMovement(struct ObjectEvent *owe, enum Direction direction);
bool32 CanAwareOWESeePlayer(struct ObjectEvent *owe);
bool32 IsPlayerInsideOWEActiveDistance(struct ObjectEvent *owe);
bool32 IsOWENextToPlayer(struct ObjectEvent *owe);
enum Direction DirectionOfOWEToPlayerFromCollision(struct ObjectEvent *owe);
u32 GetApproachingOWEDistanceToPlayer(struct ObjectEvent *owe, bool32 *equalDistances);
u32 GetOWEWalkMovementActionInDirectionWithSpeed(enum Direction direction, enum SpeedOWE speed);
bool32 TryPlayAmbientCryOWE(void);
u32 GetNumberOfActiveOWEs(enum TypeOWE oweType);
const struct ObjectEventTemplate TryGetObjectEventTemplateForOWE(const struct ObjectEventTemplate *template);
struct SpritePalette GetOWESpawnDespawnAnimFldEffPalette(enum SpawnDespawnTypeOWE spawnAnim);
u32 RemoveOldestGeneratedOWE(void);
bool32 CanRemoveObjectForOWEMovement(struct ObjectEvent *objectEvent);
bool32 AreOverworldMonsEnabled(void);
void GetOverworldMonsOption(void);

// Hasep: upstream reads these from the species (OWE_Get*FromSpecies). With the
// behaviour rolled per OWE they come from the object, through its movement
// type: each behaviour has its own MOVEMENT_TYPE_*_OWE.
extern const struct BehaviorOWE gOWESpeciesBehavior[OWE_SPECIES_BEHAVIOR_COUNT];
const struct BehaviorOWE *OWE_GetBehaviorInfo(const struct ObjectEvent *owe);
static inline u32 OWE_GetViewDistance(const struct ObjectEvent *owe)
{
    return OWE_GetBehaviorInfo(owe)->viewDistance;
}

static inline u32 OWE_GetViewWidth(const struct ObjectEvent *owe)
{
    return OWE_GetBehaviorInfo(owe)->viewWidth;
}

static inline u32 OWE_GetViewActiveDistance(const struct ObjectEvent *owe)
{
    return OWE_GetBehaviorInfo(owe)->activeDistance;
}

static inline enum SpeedOWE OWE_GetIdleSpeed(const struct ObjectEvent *owe)
{
    return OWE_GetBehaviorInfo(owe)->idleSpeed;
}

static inline enum SpeedOWE OWE_GetActiveSpeed(const struct ObjectEvent *owe)
{
    return OWE_GetBehaviorInfo(owe)->activeSpeed;
}

static inline enum ReturnToIdleOWE OWE_GetReturnToIdle(const struct ObjectEvent *owe)
{
    return OWE_GetBehaviorInfo(owe)->returnToIdle;
}

extern const u8 InteractWithOverworldWildEncounter[];

#endif // GUARD_WILD_ENCOUNTER_OW_H
