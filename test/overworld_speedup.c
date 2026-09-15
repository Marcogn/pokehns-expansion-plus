#include "global.h"
#include "main.h"
#include "load_save.h"
#include "new_game.h"
#include "overworld.h"
#include "test/test.h"

// OverworldSpeedup_AdditionalIterations returns *extra* iterations, so the
// effective multiplier is the return value plus the frame's own iteration.
TEST("Overworld speed-up maps each setting to one extra iteration per step")
{
    gMain.heldKeys = 0;

    EXPECT_EQ(OverworldSpeedup_AdditionalIterations(OPTIONS_OVERWORLD_SPEED_1X, TRUE), 0);
    EXPECT_EQ(OverworldSpeedup_AdditionalIterations(OPTIONS_OVERWORLD_SPEED_2X, TRUE), 1);
    EXPECT_EQ(OverworldSpeedup_AdditionalIterations(OPTIONS_OVERWORLD_SPEED_3X, TRUE), 2);
    EXPECT_EQ(OverworldSpeedup_AdditionalIterations(OPTIONS_OVERWORLD_SPEED_4X, TRUE), 3);
}

TEST("Overworld speed-up falls back to 1x for an out-of-range setting")
{
    gMain.heldKeys = 0;

    EXPECT_EQ(OverworldSpeedup_AdditionalIterations(OPTIONS_OVERWORLD_SPEED_COUNT, TRUE), 0);
    EXPECT_EQ(OverworldSpeedup_AdditionalIterations(0xFFFF, TRUE), 0);
}

TEST("Holding R drops the overworld back to 1x")
{
    gMain.heldKeys = R_BUTTON;

    EXPECT_EQ(OverworldSpeedup_AdditionalIterations(OPTIONS_OVERWORLD_SPEED_4X, TRUE), 0);

    // Battle transitions pass overworld = TRUE too, but a caller that opts out
    // of the hold-to-slow behaviour keeps its speed.
    EXPECT_EQ(OverworldSpeedup_AdditionalIterations(OPTIONS_OVERWORLD_SPEED_4X, FALSE), 3);

    gMain.heldKeys = 0;
}

TEST("A fresh new game defaults the overworld to 1x")
{
    Sav2_ClearSetDefault();
    NewGameInitData();

    EXPECT_EQ((u8)gSaveblock3.challengeSettings.overworldSpeed, OPTIONS_OVERWORLD_SPEED_1X);
    EXPECT_EQ(GetOverworldSpeedupSetting(), OPTIONS_OVERWORLD_SPEED_1X);
}

TEST("The overworld speed setting round-trips through ChallengeSettings")
{
    gSaveblock3.challengeSettings.overworldSpeed = OPTIONS_OVERWORLD_SPEED_4X;
    EXPECT_EQ(GetOverworldSpeedupSetting(), OPTIONS_OVERWORLD_SPEED_4X);

    gSaveblock3.challengeSettings.overworldSpeed = OPTIONS_OVERWORLD_SPEED_1X;
    EXPECT_EQ(GetOverworldSpeedupSetting(), OPTIONS_OVERWORLD_SPEED_1X);
}
