OlivineCity_House3_hns_MapScripts::
	.byte 0

OlivineCityHouse3_EventScriptFishingGuru::
	lock
	faceplayer
	checkitem ITEM_GOOD_ROD
	goto_if_eq VAR_RESULT, TRUE, OlivineCityHouse3_EventScriptFishingGuru2
	msgbox OlivineCity_Text_OfferGoodRod, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, FALSE, OlivineCityHouse3_EventScriptFishingGuruRefused
	msgbox OlivineCity_Text_GiveGoodRod, MSGBOX_DEFAULT
	giveitem ITEM_GOOD_ROD
	msgbox OlivineCity_Text_GaveGoodRod, MSGBOX_DEFAULT
	closemessage
	release
	end

OlivineCityHouse3_EventScriptFishingGuru2::
	msgbox OlivineCity_Text_HaveGoodRod, MSGBOX_DEFAULT
	closemessage
	release
	end

OlivineCityHouse3_EventScriptFishingGuruRefused::
	msgbox OlivineCity_Text_DontWantGoodRod, MSGBOX_DEFAULT
	closemessage
	release
	end

OlivineCity_Text_OfferGoodRod:
	.string "OLIVINE is on the sea!\p"
	.string "And if it's on the sea, there\n"
	.string "are bound to be fish!\p"
	.string "I've fished here for 30 years.\p"
	.string "Would you like to face the sea\n"
	.string "and try your hand at fishing?$"

OlivineCity_Text_GiveGoodRod:
	.string "Ah, hahah! We have ourselves\n"
	.string "a new angler in the making!$"

OlivineCity_Text_GaveGoodRod:
	.string "Fish aren't only found in the\n"
	.string "sea--they're wherever there's\l"
	.string "water.\p"
	.string "By the way, your POKéDEX shows\n"
	.string "fishing groups for POKéMON.\p"
	.string "Different spots may contain\n"
	.string "similar fish types.$"

OlivineCity_Text_DontWantGoodRod:
	.string "Whaaat? You don't like to fish!?\n"
	.string "That's incomprehensible!$"

OlivineCity_Text_HaveGoodRod:
	.string "How are things?\n"
	.string "Did you land the big one?$"

@ Tutor Move Master, ported from Soulgold (same flow and texts). The fee is
@ half of Soulgold's (22,222 instead of 44,444), halved again once the player is Johto
@ Champion (FLAG_IS_CHAMPION). FLAG_TUTOR_MOVES_UNLOCKED is 0 in old saves, so the
@ moves start closed (overworld-catch plan, D10).
OlivineCity_TutorMoveMaster_EventScript_Master::
	lock
	faceplayer
	goto_if_set FLAG_TUTOR_MOVES_UNLOCKED, OlivineCity_TutorMoveMaster_EventScript_Master_Known
	call OlivineCity_TutorMoveMaster_EventScript_Master_BufferFee
	msgbox OlivineCity_TutorMoveMaster_EventScript_Master_Text_Intro
	showmoneybox 0, 0
	msgbox OlivineCity_TutorMoveMaster_EventScript_Master_Text_Ask, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, OlivineCity_TutorMoveMaster_EventScript_Master_TryPay
	hidemoneybox
	msgbox OlivineCity_TutorMoveMaster_EventScript_Master_Text_Declined
	release
	end

OlivineCity_TutorMoveMaster_EventScript_Master_Known:
	msgbox OlivineCity_TutorMoveMaster_EventScript_Master_Text_HowTo
	release
	end

OlivineCity_TutorMoveMaster_EventScript_Master_BufferFee:
	bufferstring STR_VAR_1, OlivineCity_TutorMoveMaster_EventScript_Master_Text_Fee
	goto_if_unset FLAG_IS_CHAMPION, OlivineCity_TutorMoveMaster_EventScript_Master_BufferFeeEnd
	bufferstring STR_VAR_1, OlivineCity_TutorMoveMaster_EventScript_Master_Text_FeeChampion
OlivineCity_TutorMoveMaster_EventScript_Master_BufferFeeEnd:
	return

OlivineCity_TutorMoveMaster_EventScript_Master_TryPay:
	goto_if_set FLAG_IS_CHAMPION, OlivineCity_TutorMoveMaster_EventScript_Master_TryPayChampion
	checkmoney 22222
	goto_if_eq VAR_RESULT, FALSE, OlivineCity_TutorMoveMaster_EventScript_Master_NoMoney
	removemoney 22222
	goto OlivineCity_TutorMoveMaster_EventScript_Master_Paid

OlivineCity_TutorMoveMaster_EventScript_Master_TryPayChampion:
	checkmoney 11111
	goto_if_eq VAR_RESULT, FALSE, OlivineCity_TutorMoveMaster_EventScript_Master_NoMoney
	removemoney 11111
OlivineCity_TutorMoveMaster_EventScript_Master_Paid:
	updatemoneybox
	playse SE_SHOP
	waitse
	setflag FLAG_TUTOR_MOVES_UNLOCKED
	msgbox OlivineCity_TutorMoveMaster_EventScript_Master_Text_Thanks
	hidemoneybox
	closemessage
	fadescreen FADE_TO_BLACK
	delay 60
	fadescreen FADE_FROM_BLACK
	delay 20
	playfanfare MUS_OBTAIN_ITEM
	msgbox OlivineCity_TutorMoveMaster_EventScript_Master_Text_Learned
	waitfanfare
	msgbox OlivineCity_TutorMoveMaster_EventScript_Master_Text_HowTo
	release
	end

OlivineCity_TutorMoveMaster_EventScript_Master_NoMoney:
	msgbox OlivineCity_TutorMoveMaster_EventScript_Master_Text_NoMoney
	hidemoneybox
	release
	end

OlivineCity_TutorMoveMaster_EventScript_Master_Text_Fee:
	.string "22,222$"

OlivineCity_TutorMoveMaster_EventScript_Master_Text_FeeChampion:
	.string "11,111$"

OlivineCity_TutorMoveMaster_EventScript_Master_Text_HowTo:
	.string "You can teach Tutor Moves by\n"
	.string "pressing Start on the Battle Moves\l"
	.string "page of the Summary Screen and\l"
	.string "switching to the Tutor Moves page.$"

OlivineCity_TutorMoveMaster_EventScript_Master_Text_Intro:
	.string "I'm the Tutor Move Master.\p"
	.string "I know all there is to know about Tutor\n"
	.string "Moves.\p"
	.string "If you wish, I can teach you all that I \n"
	.string "know for a one-time fee of ¥{STR_VAR_1}.$"

OlivineCity_TutorMoveMaster_EventScript_Master_Text_Ask:
	.string "What do you say? Do you wish to learn\n"
	.string "everything about Tutor Moves?$"

OlivineCity_TutorMoveMaster_EventScript_Master_Text_NoMoney:
	.string "It seems you do not have the money\n"
	.string "to learn. Shame.$"

OlivineCity_TutorMoveMaster_EventScript_Master_Text_Thanks:
	.string "Thank you for the business!\p"
	.string "Now let me teach you the grand\n"
	.string "secrets of the Tutor Moves.$"

OlivineCity_TutorMoveMaster_EventScript_Master_Text_Learned:
	.string "You can now teach Tutor Moves to\n"
	.string "your Pokémon!$"

OlivineCity_TutorMoveMaster_EventScript_Master_Text_Declined:
	.string "A pity.$"
