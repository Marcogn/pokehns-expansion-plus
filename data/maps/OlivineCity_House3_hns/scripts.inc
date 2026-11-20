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

@ Tutor Move Master: same idea as Blackthorn's EGG MOVE MASTER, for tutor moves
@ (FLAG_TUTOR_MOVES_UNLOCKED is 0 in every older save).
OlivineCity_TutorMoveMaster_EventScript_Master::
	lock
	faceplayer
	goto_if_set FLAG_TUTOR_MOVES_UNLOCKED, OlivineCity_TutorMoveMaster_EventScript_AlreadyPaid
	showmoneybox 0, 0
	msgbox OlivineCity_TutorMoveMaster_Text_Offer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OlivineCity_TutorMoveMaster_EventScript_Declined
	checkmoney 44444
	goto_if_eq VAR_RESULT, FALSE, OlivineCity_TutorMoveMaster_EventScript_NotEnoughMoney
	removemoney 44444
	playse SE_SHOP
	updatemoneybox
	setflag FLAG_TUTOR_MOVES_UNLOCKED
	msgbox OlivineCity_TutorMoveMaster_Text_Paid, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

OlivineCity_TutorMoveMaster_EventScript_AlreadyPaid::
	msgbox OlivineCity_TutorMoveMaster_Text_AlreadyPaid, MSGBOX_DEFAULT
	release
	end

OlivineCity_TutorMoveMaster_EventScript_Declined::
	msgbox OlivineCity_TutorMoveMaster_Text_Declined, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

OlivineCity_TutorMoveMaster_EventScript_NotEnoughMoney::
	msgbox OlivineCity_TutorMoveMaster_Text_NotEnoughMoney, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

OlivineCity_TutorMoveMaster_Text_Offer:
	.string "I am the TUTOR MOVE MASTER.\p"
	.string "For ¥44,444, once and for all, I will\n"
	.string "show you how POKéMON recall the\l"
	.string "moves that move tutors teach.\p"
	.string "Do you want to learn it?$"

OlivineCity_TutorMoveMaster_Text_Paid:
	.string "Very good!\p"
	.string "Open a POKéMON's BATTLE MOVES page\n"
	.string "and press START to recall its\l"
	.string "TUTOR MOVES.$"

OlivineCity_TutorMoveMaster_Text_AlreadyPaid:
	.string "Your POKéMON can already recall\n"
	.string "TUTOR MOVES. Press START on the\l"
	.string "BATTLE MOVES page.$"

OlivineCity_TutorMoveMaster_Text_Declined:
	.string "Come back when you are ready.$"

OlivineCity_TutorMoveMaster_Text_NotEnoughMoney:
	.string "You do not have enough money.\n"
	.string "It costs ¥44,444.$"
