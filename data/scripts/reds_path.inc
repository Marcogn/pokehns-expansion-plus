@ Red's Path (docs/plans/reds-path.md): the eight Kanto Leaders as Red met
@ them, fought with Red's team of that point of his journey.
@
@ RedsPath_EventScript_Battle runs challenge VAR_0x8004 (0 Brock .. 7
@ Giovanni): it stashes the player's party, loads Red's in its place, fights
@ the Leader and puts the player's party back. Winning sets the Leader's
@ Trainer flag, which is the "challenge won" state. The caller does the
@ lock/release and everything around the battle (session B of the plan).
@ FLAG_REDS_PATH_BATTLE is on for the whole of it, Red's team included: it
@ keeps the randomizer and the challenge menu's rules out (IsRedsPathBattle),
@ and it is also B_FLAG_NO_WHITEOUT. So a loss costs no money and comes back
@ here like a win, with the Leader's flag still clear; and in the battle Red
@ is drawn as Red, cannot run, gains no experience and does not evolve.
RedsPath_EventScript_Battle::
	setflag FLAG_REDS_PATH_BATTLE
	special SavePlayerParty
	special RedsPath_LoadRedsParty
	switch VAR_0x8004
	case 0, RedsPath_EventScript_BattleBrock
	case 1, RedsPath_EventScript_BattleMisty
	case 2, RedsPath_EventScript_BattleSurge
	case 3, RedsPath_EventScript_BattleErika
	case 4, RedsPath_EventScript_BattleKoga
	case 5, RedsPath_EventScript_BattleSabrina
	case 6, RedsPath_EventScript_BattleBlaine
	case 7, RedsPath_EventScript_BattleGiovanni
	goto RedsPath_EventScript_BattleEnd

RedsPath_EventScript_BattleBrock:
	trainerbattle_no_intro TRAINER_REDS_PATH_BROCK_HNS, RedsPath_Text_BrockDefeat
	goto RedsPath_EventScript_BattleEnd

RedsPath_EventScript_BattleMisty:
	trainerbattle_no_intro TRAINER_REDS_PATH_MISTY_HNS, RedsPath_Text_MistyDefeat
	goto RedsPath_EventScript_BattleEnd

RedsPath_EventScript_BattleSurge:
	trainerbattle_no_intro TRAINER_REDS_PATH_SURGE_HNS, RedsPath_Text_SurgeDefeat
	goto RedsPath_EventScript_BattleEnd

RedsPath_EventScript_BattleErika:
	trainerbattle_no_intro TRAINER_REDS_PATH_ERIKA_HNS, RedsPath_Text_ErikaDefeat
	goto RedsPath_EventScript_BattleEnd

RedsPath_EventScript_BattleKoga:
	trainerbattle_no_intro TRAINER_REDS_PATH_KOGA_HNS, RedsPath_Text_KogaDefeat
	goto RedsPath_EventScript_BattleEnd

RedsPath_EventScript_BattleSabrina:
	trainerbattle_no_intro TRAINER_REDS_PATH_SABRINA_HNS, RedsPath_Text_SabrinaDefeat
	goto RedsPath_EventScript_BattleEnd

RedsPath_EventScript_BattleBlaine:
	trainerbattle_no_intro TRAINER_REDS_PATH_BLAINE_HNS, RedsPath_Text_BlaineDefeat
	goto RedsPath_EventScript_BattleEnd

RedsPath_EventScript_BattleGiovanni:
	trainerbattle_no_intro TRAINER_REDS_PATH_GIOVANNI_HNS, RedsPath_Text_GiovanniDefeat
	goto RedsPath_EventScript_BattleEnd

RedsPath_EventScript_BattleEnd:
	special RedsPath_RestorePlayerParty
	clearflag FLAG_REDS_PATH_BATTLE
	return

@ The Gyms (session B of the plan). Each Gym's aide calls
@ RedsPath_EventScript_Aide, and the row in front of each door
@ RedsPath_EventScript_IntroTrigger, with VAR_0x8004 = the Gym's challenge.
@ The frame is an interactive VR headset run by the POKéMON LEAGUE: no text
@ may suggest travelling back in time (plan, section 4).

@ Walking out of the Gym after beating its own Leader, the first time in the
@ whole game: the aide notices the player, walks up and explains. The rows
@ run while VAR_REDS_PATH_TRIGGER is 0, and it goes to 1 once the
@ introduction has been seen, so after that they cost nothing. Coming in
@ through the door the player faces north: that is not leaving.
@ A trigger script is first run dry (RunScriptImmediatelyUntilEffect) and
@ only started if it does something: the checks up to lockall have no
@ effects (RedsPath_IsGymLeaderBeaten is declared so), so a step that does
@ not show the introduction starts no script. One that only ends, after
@ running for real, leaves the player's step held and the player stuck
@ (seen in the emulator): every real exit goes through releaseall.
RedsPath_EventScript_IntroTrigger::
	goto_if_set FLAG_REDS_PATH_INTRO_SEEN, RedsPath_EventScript_TriggerOff
	specialvar VAR_RESULT, RedsPath_IsGymLeaderBeaten
	goto_if_eq VAR_RESULT, FALSE, RedsPath_EventScript_TriggerEnd
	specialvar VAR_RESULT, GetPlayerFacingDirection
	goto_if_eq VAR_RESULT, DIR_NORTH, RedsPath_EventScript_TriggerEnd
	lockall
	@ The walks are built in RAM, outside the follower-safe movements, so
	@ applymovement recalls the follower: wanted, as it stands behind the
	@ player, often right where the aide walks to.
	special RedsPath_BuildAideApproach
	playse SE_PIN
	applymovement VAR_0x8005, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement VAR_0x8005, Common_Movement_Delay48
	waitmovement 0
	applymovement VAR_0x8005, gRedsPathAideApproach
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, gRedsPathPlayerFaceAide
	waitmovement 0
	msgbox RedsPath_Text_Intro, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_REDS_PATH_INTRO_SEEN
	applymovement VAR_0x8005, gRedsPathAideReturn
	waitmovement 0
	setvar VAR_REDS_PATH_TRIGGER, 1
	releaseall
	end

RedsPath_EventScript_TriggerOff:
	setvar VAR_REDS_PATH_TRIGGER, 1
	releaseall
RedsPath_EventScript_TriggerEnd:
	end

@ The aide, once the Gym's own Leader is beaten.
RedsPath_EventScript_Aide::
	lock
	faceplayer
RedsPath_EventScript_AideLocked::
	goto_if_set FLAG_REDS_PATH_INTRO_SEEN, RedsPath_EventScript_AideState
	msgbox RedsPath_Text_Intro, MSGBOX_DEFAULT
	setflag FLAG_REDS_PATH_INTRO_SEEN
	setvar VAR_REDS_PATH_TRIGGER, 1
RedsPath_EventScript_AideState:
	specialvar VAR_RESULT, RedsPath_GetAideState
	switch VAR_RESULT
	case REDS_PATH_AIDE_OFFER, RedsPath_EventScript_Offer
	case REDS_PATH_AIDE_PRIZE, RedsPath_EventScript_PrizeWaiting
	case REDS_PATH_AIDE_OUT_OF_ORDER, RedsPath_EventScript_OutOfOrder
	case REDS_PATH_AIDE_DONE_HERE, RedsPath_EventScript_DoneHere
	case REDS_PATH_AIDE_PRIZES_LEFT, RedsPath_EventScript_PrizesLeft
	msgbox RedsPath_Text_AllDone, MSGBOX_DEFAULT
	release
	end

RedsPath_EventScript_PrizesLeft:
	msgbox RedsPath_Text_PrizesLeft, MSGBOX_DEFAULT
	release
	end

RedsPath_EventScript_OutOfOrder:
	msgbox RedsPath_Text_OutOfOrder, MSGBOX_DEFAULT
	goto RedsPath_EventScript_NextChallenge

RedsPath_EventScript_DoneHere:
	msgbox RedsPath_Text_DoneHere, MSGBOX_DEFAULT
RedsPath_EventScript_NextChallenge:
	call_if_eq VAR_0x8005, FALSE, RedsPath_EventScript_BeatLeaderFirst
	release
	end

RedsPath_EventScript_BeatLeaderFirst:
	msgbox RedsPath_Text_BeatLeaderFirst, MSGBOX_DEFAULT
	return

RedsPath_EventScript_Offer:
	msgbox RedsPath_Text_WantToRelive, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, RedsPath_EventScript_Declined
	msgbox RedsPath_Text_HereWeGo, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	special RedsPath_EnterScene
	fadescreen FADE_FROM_BLACK
	msgbox RedsPath_Text_RedVs, MSGBOX_DEFAULT
	closemessage
	call RedsPath_EventScript_Battle
	specialvar VAR_RESULT, RedsPath_WonSceneBattle
	goto_if_eq VAR_RESULT, FALSE, RedsPath_EventScript_Lost
	playfanfare MUS_OBTAIN_BADGE
	message RedsPath_Text_RedWins
	waitmessage
	waitfanfare
	waitbuttonpress
	closemessage
	call RedsPath_EventScript_LeaveScene
	@ The battle used the string buffers: fill them again.
	specialvar VAR_RESULT, RedsPath_GetAideState
	msgbox RedsPath_Text_Congratulations, MSGBOX_DEFAULT
	goto RedsPath_EventScript_HandOverPrize

RedsPath_EventScript_Lost:
	call RedsPath_EventScript_LeaveScene
	msgbox RedsPath_Text_TryAgain, MSGBOX_DEFAULT
	release
	end

RedsPath_EventScript_Declined:
	msgbox RedsPath_Text_Declined, MSGBOX_DEFAULT
	release
	end

@ The headset comes off: back where the player was, facing the aide.
RedsPath_EventScript_LeaveScene:
	fadescreen FADE_TO_BLACK
	special RedsPath_ExitScene
	fadescreen FADE_FROM_BLACK
	return

RedsPath_EventScript_PrizeWaiting:
	msgbox RedsPath_Text_WelcomeBackPrize, MSGBOX_DEFAULT
@ Always into the party, never to the PC (plan, section 3): with no room
@ the prize waits here, and whoever hunts a shiny saves in front of the aide.
RedsPath_EventScript_HandOverPrize:
	special RedsPath_GivePrize
	goto_if_eq VAR_RESULT, MON_CANT_GIVE, RedsPath_EventScript_NoRoom
	@ The message's waitbuttonpress writes VAR_RESULT, and GiftMon reads it.
	copyvar VAR_0x8006, VAR_RESULT
	msgbox RedsPath_Text_HeresYourPrize, MSGBOX_DEFAULT
	copyvar VAR_RESULT, VAR_0x8006
	call Common_EventScript_GiftMon
	release
	end

RedsPath_EventScript_NoRoom:
	msgbox RedsPath_Text_NoRoom, MSGBOX_DEFAULT
	release
	end

RedsPath_Text_Intro:
	.string "What a battle!\p"
	.string "It's been three years since a TRAINER\n"
	.string "from PALLET TOWN saved the whole\l"
	.string "region from TEAM ROCKET.\p"
	.string "Every year the POKéMON LEAGUE holds\n"
	.string "RED'S PATH: with this interactive VR\l"
	.string "headset you relive RED's battles\l"
	.string "against the GYM LEADERS.\p"
	.string "You get his team, the POKéMON he had\n"
	.string "back then, and you fight in his place,\l"
	.string "in first person!\p"
	.string "There's a great prize for every\n"
	.string "LEADER you beat.\p"
	.string "Just like RED, you have to take them\n"
	.string "on in order, starting with BROCK in\l"
	.string "PEWTER CITY.\p"
	.string "Talk to me whenever you want to try!$"

RedsPath_Text_OutOfOrder:
	.string "RED'S PATH has to be followed in\n"
	.string "order!\p"
	.string "Your next challenge is {STR_VAR_1},\n"
	.string "in {STR_VAR_2}.$"

RedsPath_Text_DoneHere:
	.string "You've already relived RED's battle\n"
	.string "with {STR_VAR_3}!\p"
	.string "Your next challenge is {STR_VAR_1},\n"
	.string "in {STR_VAR_2}.$"

RedsPath_Text_BeatLeaderFirst:
	.string "Beat the LEADER there first!$"

RedsPath_Text_WantToRelive:
	.string "Want to relive RED's battle\n"
	.string "with {STR_VAR_3}?$"

RedsPath_Text_HereWeGo:
	.string "Put on the headset…\n"
	.string "Here we go!$"

RedsPath_Text_Declined:
	.string "OK! Remember, there are great\n"
	.string "prizes!$"

RedsPath_Text_RedVs:
	.string "RED VS {STR_VAR_3}$"

RedsPath_Text_RedWins:
	.string "RED WINS!$"

RedsPath_Text_TryAgain:
	.string "Try again!$"

RedsPath_Text_Congratulations:
	.string "Congratulations! You beat {STR_VAR_3}\n"
	.string "just like RED did!$"

RedsPath_Text_WelcomeBackPrize:
	.string "There you are! Your prize for the\n"
	.string "battle with {STR_VAR_3} is waiting.$"

RedsPath_Text_HeresYourPrize:
	.string "Here's your prize!$"

RedsPath_Text_NoRoom:
	.string "Come back with room in your party!$"

RedsPath_Text_PrizesLeft:
	.string "You've relived every one of RED's\n"
	.string "GYM battles!\p"
	.string "Some of your prizes are still waiting\n"
	.string "at their GYMS, though.$"

RedsPath_Text_AllDone:
	.string "You've relived every one of RED's\n"
	.string "GYM battles, from BROCK to GIOVANNI!\p"
	.string "Nobody has walked RED'S PATH like\n"
	.string "that in all these years.\p"
	.string "RED himself would be proud of you!$"

@ Debug menu, "Red's Path…": challenge N with no map script around it.
Debug_EventScript_RedsPath_Brock::
	setvar VAR_0x8004, 0
	goto Debug_EventScript_RedsPath
Debug_EventScript_RedsPath_Misty::
	setvar VAR_0x8004, 1
	goto Debug_EventScript_RedsPath
Debug_EventScript_RedsPath_Surge::
	setvar VAR_0x8004, 2
	goto Debug_EventScript_RedsPath
Debug_EventScript_RedsPath_Erika::
	setvar VAR_0x8004, 3
	goto Debug_EventScript_RedsPath
Debug_EventScript_RedsPath_Koga::
	setvar VAR_0x8004, 4
	goto Debug_EventScript_RedsPath
Debug_EventScript_RedsPath_Sabrina::
	setvar VAR_0x8004, 5
	goto Debug_EventScript_RedsPath
Debug_EventScript_RedsPath_Blaine::
	setvar VAR_0x8004, 6
	goto Debug_EventScript_RedsPath
Debug_EventScript_RedsPath_Giovanni::
	setvar VAR_0x8004, 7
Debug_EventScript_RedsPath:
	lockall
	call RedsPath_EventScript_Battle
	releaseall
	end

RedsPath_Text_BrockDefeat:
	.string "I took you for granted, RED.\n"
	.string "Your POKéMON hit harder than rock!$"

RedsPath_Text_MistyDefeat:
	.string "Wow! You're too much!\n"
	.string "My STARMIE didn't stand a chance…$"

RedsPath_Text_SurgeDefeat:
	.string "Whoa! You're the real deal, kid!\n"
	.string "My electric POKéMON got zapped!$"

RedsPath_Text_ErikaDefeat:
	.string "Oh! I concede defeat…\n"
	.string "You are remarkably strong.$"

RedsPath_Text_KogaDefeat:
	.string "Humph! You have proven your worth!\n"
	.string "Even my poison could not stop you.$"

RedsPath_Text_SabrinaDefeat:
	.string "I saw this coming…\n"
	.string "and still I could not change it.$"

RedsPath_Text_BlaineDefeat:
	.string "I've burnt out!\n"
	.string "Your fire burns brighter than mine!$"

RedsPath_Text_GiovanniDefeat:
	.string "Ha! That was a truly intense fight.\n"
	.string "You have won, RED!$"
