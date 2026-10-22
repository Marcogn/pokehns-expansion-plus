#ifndef GUARD_CONFIG_LEVEL_SCALING_H
#define GUARD_CONFIG_LEVEL_SCALING_H

// Level scaling engine from Soulgold (src/level_scaling.c there, itself from
// fisham-org/pokeemerald-expansion-features, branch feature/level-scaling).
// The per-trainer rules file of Soulgold is not ported: it was never read.
// The decisions behind these numbers are in docs/plans/level-scaling.md.

#define B_LEVEL_SCALING_ENABLED             TRUE

// Reference level read from the player's party (enum LevelScalingMode).
// PARTY_AVG leaves out every mon this many levels or more below the strongest
// one, so a fresh catch or an HM carrier does not drag everything down.
#define B_LEVEL_SCALING_MODE                LEVEL_SCALING_PARTY_AVG
#define B_LEVEL_SCALING_PASSENGER_GAP       7

// Trainers: the ace is the reference plus this, minus 0..VARIATION picked once
// per save and trainer, so a rematch meets the same levels. The others keep
// their authored distance from the ace, at most MAX_AUTHORED_GAP levels.
// No mon ever goes below the level HnS gives it.
#define B_TRAINER_SCALING_LEVEL_AUGMENT     1
#define B_TRAINER_SCALING_LEVEL_VARIATION   1
#define B_TRAINER_SCALING_MAX_AUTHORED_GAP  2
// Evolve through plain level evolutions with a single possible target.
#define B_TRAINER_SCALING_EVOLVE            TRUE

// Wild Pokémon: the reference plus this (negative), minus 0..VARIATION at
// random, never below the level the encounter table rolled. Soulgold uses -8.
#define B_WILD_SCALING_LEVEL_AUGMENT        -2
#define B_WILD_SCALING_LEVEL_VARIATION      2

// Prize money with trainer scaling on: +1% for every this many levels of the
// current level cap (cap 37 -> +7%, 70 -> +14%). A proposal, not a request.
#define B_TRAINER_SCALING_MONEY_CAP_STEP    5

#endif // GUARD_CONFIG_LEVEL_SCALING_H
