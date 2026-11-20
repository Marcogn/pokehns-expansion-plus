@ Hasep: overworld catching (src/overworld_catch.c, docs/plans/overworld-catch.md).

@ R pressed where a Ball cannot be thrown, or at a Pokemon that cannot be caught.
OverworldCatch_EventScript_Blocked::
	lock
	callnative OverworldCatch_ShowBlockedMessage, requests_effects=1
	waitmessage
	waitbuttonpress
	closemessage
	release
	end

@ A party Pokemon sent at a wild one, or a Ball thrown at a roamer: the battle
@ of InteractWithOverworldWildEncounter without the approach, from where the
@ two stand. The controls stay locked until the battle starts (field.md: a
@ spawn in between would zero gEnemyParty).
OverworldCatch_EventScript_Battle::
	lock
	applymovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	facetogether
	getoverworldwildencounterspecies VAR_0x8005
	playmoncry VAR_0x8005, CRY_MODE_DOUBLES
	waitmoncry
	tryoverworldwildencounter
	waitstate
	end

@ After a volley, with the Nuzlocke's compulsory nickname on: one prompt per
@ catch. The banners do not stop the game (overworld_catch.c), and without the
@ Nuzlocke's naming a thrown Ball asks for no nickname at all.
OverworldCatch_EventScript_Results::
	lock
OverworldCatch_EventScript_NextNickname:
	callnative OverworldCatch_SelectNextNickname, requests_effects=1
	goto_if_eq VAR_RESULT, FALSE, OverworldCatch_EventScript_ResultsEnd
	specialvar VAR_RESULT, IsNuzlockeNicknamingActive
	goto_if_eq VAR_RESULT, TRUE, OverworldCatch_EventScript_Nickname
	asknickname OverworldCatch_Text_GiveNickname
	closemessage
	goto_if_eq VAR_RESULT, NO, OverworldCatch_EventScript_NextNickname
OverworldCatch_EventScript_Nickname:
	fadescreen FADE_TO_BLACK
	special ChangePokemonNickname
	waitstate
	goto OverworldCatch_EventScript_NextNickname
OverworldCatch_EventScript_ResultsEnd:
	release
	end

OverworldCatch_Text_GiveNickname:
	.string "Give a nickname to the\n"
	.string "captured {STR_VAR_1}?$"
