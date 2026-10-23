@ TcloneR rewards (src/tcloner.c, docs/plans/tcloner.md). Hands out every
@ reward step that is due, one message each, and closes with one of the
@ common lines. The scientist's scripts call it after "How many have you
@ got?"; the caller does the lock/faceplayer and the rest of the dialogue.
@ A full Bag or full PC Boxes leaves the reward pending for the next meeting.
EventScript_TClonerRewards::
	setvar VAR_0x8004, FALSE  @ whether anything was handed out this time
EventScript_TClonerRewards_Next:
	special TClonerGetNextReward
	goto_if_eq VAR_RESULT, TCLONER_REWARD_ITEM, EventScript_TClonerRewards_Item
	goto_if_eq VAR_RESULT, TCLONER_REWARD_MON, EventScript_TClonerRewards_Mon
	goto_if_eq VAR_RESULT, TCLONER_REWARD_ALL_CLAIMED, EventScript_TClonerRewards_AllClaimed
	goto_if_eq VAR_0x8004, TRUE, EventScript_TClonerRewards_End
	msgbox Text_TClonerNotYet, MSGBOX_DEFAULT
EventScript_TClonerRewards_End:
	return

EventScript_TClonerRewards_Item:
	checkitemspace VAR_0x8000, VAR_0x8001
	goto_if_eq VAR_RESULT, FALSE, EventScript_TClonerRewards_BagFull
	giveitem VAR_0x8000, VAR_0x8001
	@ Checked again: with the randomizer on, giveitem may hand out another item.
	goto_if_eq VAR_RESULT, FALSE, EventScript_TClonerRewards_BagFull
	special TClonerClaimItemReward
	setvar VAR_0x8004, TRUE
	goto EventScript_TClonerRewards_Next

EventScript_TClonerRewards_Mon:
	special TClonerGiveRewardMon
	goto_if_eq VAR_RESULT, TCLONER_MON_BOXES_FULL, EventScript_TClonerRewards_BoxesFull
	playfanfare MUS_OBTAIN_ITEM
	message Text_TClonerMonSentToPC
	waitmessage
	waitfanfare
	waitbuttonpress
	setvar VAR_0x8004, TRUE
	goto EventScript_TClonerRewards_Next

EventScript_TClonerRewards_AllClaimed:
	msgbox Text_TClonerAllClaimed, MSGBOX_DEFAULT
	return

EventScript_TClonerRewards_BagFull:
	msgbox Text_TClonerBagFull, MSGBOX_DEFAULT
	return

EventScript_TClonerRewards_BoxesFull:
	msgbox Text_TClonerBoxesFull, MSGBOX_DEFAULT
	return

Text_TClonerNotYet:
	.string "Hmm, not enough for the next reward\n"
	.string "yet. Keep at it!$"

Text_TClonerAllClaimed:
	.string "Impressive. I have nothing left to\n"
	.string "give you.\p"
	.string "You've been really great. Well done!$"

Text_TClonerBagFull:
	.string "Your BAG is full! I'll keep the rest\n"
	.string "for next time.$"

Text_TClonerBoxesFull:
	.string "Your PC BOXES are full! I'll keep\n"
	.string "this one for next time.$"

Text_TClonerMonSentToPC:
	.string "{STR_VAR_1} was sent to the PC!$"
