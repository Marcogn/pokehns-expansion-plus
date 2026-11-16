#ifndef GUARD_WILD_ENCOUNTER_OW_SPECIES_BEHAVIOR_H
#define GUARD_WILD_ENCOUNTER_OW_SPECIES_BEHAVIOR_H

#include "wild_encounter_ow.h"
#include "constants/event_object_movement.h"

// Hasep (overworld catch plan, phase 2): sight is a cone (CanAwareOWESeePlayer)
// 1 tile wide at distance 1, 3 at 2-3 and 5 from 4 on, capped by viewWidth.
// Skittish behaviours (flee, despawn) and the chaser look 5 tiles ahead and
// see the full cone; the watcher sees 4; the approacher, curious and not
// wary, 4 and only 3 wide. They all used to see a 4x3 rectangle.
// viewDistance must not exceed activeDistance: with returnToIdle on
// PLAYER_OUTSIDE_ACTIVE_RANGE a Pokémon that saw the player beyond it would go
// back to idle on its next step and notice again, in a loop (the cone's far
// corner, 5 ahead and 2 aside, is 7 = the active range's diagonal).
// activeDistance is also how far running and cycling are heard.
const struct BehaviorOWE gOWESpeciesBehavior[OWE_SPECIES_BEHAVIOR_COUNT] =
{
    [OWE_IGNORE_PLAYER] =
    {
        .movementType = MOVEMENT_TYPE_WANDER_AROUND_OWE,
        .viewDistance = 4,
        .viewWidth = 3,
        .activeDistance = 5,
        .idleSpeed = OWE_SPEED_NORMAL,
        .activeSpeed = OWE_SPEED_SLOW,
        .returnToIdle = PLAYER_OUTSIDE_ACTIVE_RANGE,
    },

    [OWE_CHASE_PLAYER_SLOW] =
    {
        .movementType = MOVEMENT_TYPE_CHASE_PLAYER_OWE,
        .viewDistance = 5,
        .viewWidth = 5,
        .activeDistance = 5,
        .idleSpeed = OWE_SPEED_SLOW,
        .activeSpeed = OWE_SPEED_SLOW,
        .returnToIdle = PLAYER_OUTSIDE_ACTIVE_RANGE,
    },

    [OWE_FLEE_PLAYER_NORMAL] =
    {
        .movementType = MOVEMENT_TYPE_FLEE_PLAYER_OWE,
        .viewDistance = 5,
        .viewWidth = 5,
        .activeDistance = 5,
        .idleSpeed = OWE_SPEED_NORMAL,
        .activeSpeed = OWE_SPEED_NORMAL,
        .returnToIdle = PLAYER_OUTSIDE_ACTIVE_RANGE,
    },

    [OWE_WATCH_PLAYER_NORMAL] =
    {
        .movementType = MOVEMENT_TYPE_WATCH_PLAYER_OWE,
        .viewDistance = 4,
        .viewWidth = 5,
        .activeDistance = 5,
        .idleSpeed = OWE_SPEED_NORMAL,
        .returnToIdle = PLAYER_OUTSIDE_ACTIVE_RANGE,
    },

    [OWE_APPROACH_PLAYER_SLOW] =
    {
        .movementType = MOVEMENT_TYPE_APPROACH_PLAYER_OWE,
        .viewDistance = 4,
        .viewWidth = 3,
        .activeDistance = 5,
        .idleSpeed = OWE_SPEED_NORMAL,
        .activeSpeed = OWE_SPEED_SLOW,
        .returnToIdle = PLAYER_OUTSIDE_ACTIVE_RANGE,
    },

    [OWE_DESPAWN_ON_NOTICE] =
    {
        .movementType = MOVEMENT_TYPE_DESPAWN_OWE,
        .viewDistance = 5,
        .viewWidth = 5,
        .activeDistance = 5,
        .idleSpeed = OWE_SPEED_NORMAL,
        .returnToIdle = PLAYER_OUTSIDE_ACTIVE_RANGE,
    }
};

#endif // GUARD_WILD_ENCOUNTER_OW_SPECIES_BEHAVIOR_H
