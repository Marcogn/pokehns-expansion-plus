@ Red's Path (docs/plans/reds-path.md): the eight Kanto Leaders as Red met
@ them, fought with Red's team of that point of his journey.
@
@ RedsPath_EventScript_Battle runs challenge VAR_0x8004 (0 Brock .. 7
@ Giovanni): it stashes the player's party, loads Red's in its place, fights
@ the Leader and puts the player's party back. Winning sets the Leader's
@ Trainer flag, which is the "challenge won" state. The caller does the
@ lock/release and everything around the battle (session B of the plan).
@
@ Until the battle hooks of session A2 are in, a loss whites out as in any
@ Trainer battle, and the script never gets to restore the party: the real
@ team is still in the save, so a soft reset brings it back.
RedsPath_EventScript_Battle::
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
	special LoadPlayerParty
	return

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
	special LoadPlayerParty
	return

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
