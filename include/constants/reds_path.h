#ifndef GUARD_CONSTANTS_REDS_PATH_H
#define GUARD_CONSTANTS_REDS_PATH_H

// Red's Path (docs/plans/reds-path.md). Shared by src/reds_path.c and
// data/scripts/reds_path.inc. A challenge number (0 Brock .. 7 Giovanni) is
// also the Gym that offers it, in VAR_0x8004.

// Result of RedsPath_GetAideState, in VAR_RESULT. STR_VAR_3 holds this
// Gym's Leader (as Red met them: KOGA, GIOVANNI), STR_VAR_1 and STR_VAR_2 the
// next challenge's Leader and city, and VAR_0x8005 is TRUE when that Gym's
// own Leader is already beaten.
#define REDS_PATH_AIDE_OFFER        0 // this Gym's challenge is the next one
#define REDS_PATH_AIDE_PRIZE        1 // won here, prize not collected yet
#define REDS_PATH_AIDE_OUT_OF_ORDER 2 // an earlier challenge is still open
#define REDS_PATH_AIDE_DONE_HERE    3 // won and collected here, more to go
#define REDS_PATH_AIDE_ALL_DONE     4 // all eight won and collected
#define REDS_PATH_AIDE_PRIZES_LEFT  5 // all eight won, a prize waits in another Gym

// The aide's approach (RedsPath_EventScript_IntroTrigger) and the trigger
// rows in front of the Gym doors run while this is 0. It goes to 1 once the
// introduction has been seen, so the rows stop starting scripts.
#define VAR_REDS_PATH_TRIGGER VAR_TEMP_9

#endif // GUARD_CONSTANTS_REDS_PATH_H
