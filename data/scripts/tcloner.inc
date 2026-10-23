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

@ ---------------------------------------------------------------------------
@ The scientist (session B of docs/plans/tcloner.md). Elm's former assistant
@ shows up in five places, in story order: Azalea (stage 1), the Mahogany
@ hideout or, once it is cleared, outside the Mahogany shop (2), outside the
@ Radio Tower (3), outside the Cerulean Pokemon Center (4), Oak's lab (5).
@ VAR_TCLONER_STAGE is the last stage met and only goes up: meeting him at
@ stage s hides every earlier one, so no line can arrive out of order. The
@ first meeting, wherever it is, hands over the TcloneR with the introduction;
@ the stage dialogues pick their variant from VAR_TCLONER_STAGE, so a line
@ never repeats or answers something never said ("Sequenza rotta" in the plan).

@ Called from the ON_TRANSITION script of every map that has one of him.
@ Visibility is recomputed on every entry, so no save ever needs migrating.
TCloner_EventScript_UpdateNpcs::
	setflag FLAG_HIDE_TCLONER_AZALEA
	setflag FLAG_HIDE_TCLONER_HIDEOUT
	setflag FLAG_HIDE_TCLONER_MAHOGANY
	setflag FLAG_HIDE_TCLONER_GOLDENROD
	setflag FLAG_HIDE_TCLONER_CERULEAN
	setflag FLAG_HIDE_TCLONER_PALLET
	call_if_lt VAR_TCLONER_STAGE, 1, TCloner_EventScript_ShowAzalea
	goto_if_ge VAR_TCLONER_STAGE, 2, TCloner_EventScript_UpdateGoldenrod
	@ Inside the hideout while it is still Team Rocket's, outside the shop after.
	call_if_lt VAR_MAHOGANY_TOWN_STATE, 14, TCloner_EventScript_ShowHideout
	call_if_ge VAR_MAHOGANY_TOWN_STATE, 14, TCloner_EventScript_ShowMahogany
TCloner_EventScript_UpdateGoldenrod:
	goto_if_ge VAR_TCLONER_STAGE, 3, TCloner_EventScript_UpdateCerulean
	@ From the Radio Tower takeover on (VAR_GOLDENROD_CITY_STATE 6).
	call_if_ge VAR_GOLDENROD_CITY_STATE, 6, TCloner_EventScript_ShowGoldenrod
TCloner_EventScript_UpdateCerulean:
	call_if_lt VAR_TCLONER_STAGE, 4, TCloner_EventScript_ShowCerulean
	@ He stays at Pallet for good once Cerulean is behind him.
	call_if_ge VAR_TCLONER_STAGE, 4, TCloner_EventScript_ShowPallet
	return

TCloner_EventScript_ShowAzalea:
	clearflag FLAG_HIDE_TCLONER_AZALEA
	return

TCloner_EventScript_ShowHideout:
	clearflag FLAG_HIDE_TCLONER_HIDEOUT
	return

TCloner_EventScript_ShowMahogany:
	clearflag FLAG_HIDE_TCLONER_MAHOGANY
	return

TCloner_EventScript_ShowGoldenrod:
	clearflag FLAG_HIDE_TCLONER_GOLDENROD
	return

TCloner_EventScript_ShowCerulean:
	clearflag FLAG_HIDE_TCLONER_CERULEAN
	return

TCloner_EventScript_ShowPallet:
	clearflag FLAG_HIDE_TCLONER_PALLET
	return

@ The introduction after "Psst… Did PROF. ELM send you?": the item and how it
@ works. VAR_0x8005 TRUE keeps the promise to explain next time, which only
@ makes sense when the explanation is still to come (first met at Azalea or
@ Mahogany); FALSE is the short version, for Goldenrod and Cerulean, where
@ the explanation follows in the same dialogue.
TCloner_EventScript_GiveAndExplain:
	msgbox TCloner_Text_TakeThis, MSGBOX_DEFAULT
	@ Already in the Bag only through the debug menu; skip the hand-over then.
	checkitem ITEM_TCLONER
	goto_if_eq VAR_RESULT, TRUE, TCloner_EventScript_Explain
	giveitem ITEM_TCLONER
	goto_if_eq VAR_RESULT, FALSE, TCloner_EventScript_NoRoom
TCloner_EventScript_Explain:
	msgbox TCloner_Text_HowItWorks, MSGBOX_DEFAULT
	call_if_eq VAR_0x8005, TRUE, TCloner_EventScript_ExplainLater
	call_if_eq VAR_0x8005, FALSE, TCloner_EventScript_NobodyGetsHurt
	msgbox TCloner_Text_NiceReward, MSGBOX_DEFAULT
	return

@ Key Items pocket full: he stays, and the stage does not move.
TCloner_EventScript_NoRoom:
	msgbox TCloner_Text_NoRoom, MSGBOX_DEFAULT
	release
	end

TCloner_EventScript_ExplainLater:
	msgbox TCloner_Text_ExplainLater, MSGBOX_DEFAULT
	return

TCloner_EventScript_NobodyGetsHurt:
	msgbox TCloner_Text_NobodyGetsHurt, MSGBOX_DEFAULT
	return

@ He leaves once the dialogue is over, rewards pending or not. removeobject
@ sets the object's hide flag; TCloner_EventScript_UpdateNpcs keeps it set.
TCloner_EventScript_Leave:
	closemessage
	fadescreen FADE_TO_BLACK
	removeobject VAR_LAST_TALKED
	fadescreen FADE_FROM_BLACK
	release
	end

@ Stage 1. Always the first meeting: the Azalea scientist hides with any
@ later stage. After the Slowpoke Well (FLAG_HIDE_AZALEA_TOWN_ROCKETS) he
@ remarks on it and points to the Lake of Rage.
TCloner_EventScript_Azalea::
	lock
	faceplayer
	msgbox TCloner_Text_Psst, MSGBOX_DEFAULT
	call_if_set FLAG_HIDE_AZALEA_TOWN_ROCKETS, TCloner_EventScript_Commotion
	setvar VAR_0x8005, TRUE
	call TCloner_EventScript_GiveAndExplain
	call_if_set FLAG_HIDE_AZALEA_TOWN_ROCKETS, TCloner_EventScript_AzaleaGoFindOthers
	call_if_unset FLAG_HIDE_AZALEA_TOWN_ROCKETS, TCloner_EventScript_AzaleaHaveToGo
	setvar VAR_TCLONER_STAGE, 1
	goto TCloner_EventScript_Leave

TCloner_EventScript_Commotion:
	msgbox TCloner_Text_Commotion, MSGBOX_DEFAULT
	return

TCloner_EventScript_AzaleaGoFindOthers:
	msgbox TCloner_Text_AzaleaGoFindOthers, MSGBOX_DEFAULT
	return

TCloner_EventScript_AzaleaHaveToGo:
	msgbox TCloner_Text_HaveToGoNow, MSGBOX_DEFAULT
	return

@ Stage 2: the hideout entrance, or outside the shop after it.
TCloner_EventScript_Mahogany::
	lock
	faceplayer
	goto_if_eq VAR_TCLONER_STAGE, 0, TCloner_EventScript_MahoganyFirst
	msgbox TCloner_Text_MahoganyGreeting, MSGBOX_DEFAULT
	call EventScript_TClonerRewards
	msgbox TCloner_Text_MahoganyWhoAmI, MSGBOX_DEFAULT
	goto TCloner_EventScript_MahoganyEnd

TCloner_EventScript_MahoganyFirst:
	msgbox TCloner_Text_Psst, MSGBOX_DEFAULT
	setvar VAR_0x8005, TRUE
	call TCloner_EventScript_GiveAndExplain
	msgbox TCloner_Text_MahoganyOneMoreThing, MSGBOX_DEFAULT
TCloner_EventScript_MahoganyEnd:
	msgbox TCloner_Text_MahoganyShadowOfATower, MSGBOX_DEFAULT
	setvar VAR_TCLONER_STAGE, 2
	goto TCloner_EventScript_Leave

@ Stage 3: outside the Radio Tower. Team Rocket holds it while
@ VAR_GOLDENROD_CITY_STATE is below 10 (Archer beaten).
TCloner_EventScript_Goldenrod::
	lock
	faceplayer
	goto_if_eq VAR_TCLONER_STAGE, 0, TCloner_EventScript_GoldenrodFirst
	call_if_lt VAR_GOLDENROD_CITY_STATE, 10, TCloner_EventScript_GoldenrodGreetingDuring
	call_if_ge VAR_GOLDENROD_CITY_STATE, 10, TCloner_EventScript_GoldenrodGreetingAfter
	call EventScript_TClonerRewards
	@ Mahogany skipped: Team Rocket was never mentioned.
	goto_if_ne VAR_TCLONER_STAGE, 1, TCloner_EventScript_GoldenrodParticles
	call_if_lt VAR_GOLDENROD_CITY_STATE, 10, TCloner_EventScript_GoldenrodByTheWay
	call_if_ge VAR_GOLDENROD_CITY_STATE, 10, TCloner_EventScript_GoldenrodWas
	goto TCloner_EventScript_GoldenrodParticles

TCloner_EventScript_GoldenrodFirst:
	msgbox TCloner_Text_Psst, MSGBOX_DEFAULT
	setvar VAR_0x8005, FALSE
	call TCloner_EventScript_GiveAndExplain
	call_if_lt VAR_GOLDENROD_CITY_STATE, 10, TCloner_EventScript_GoldenrodFirstDuring
	call_if_ge VAR_GOLDENROD_CITY_STATE, 10, TCloner_EventScript_GoldenrodFirstAfter
TCloner_EventScript_GoldenrodParticles:
	msgbox TCloner_Text_GoldenrodParticles, MSGBOX_DEFAULT
	setvar VAR_TCLONER_STAGE, 3
	goto TCloner_EventScript_Leave

TCloner_EventScript_GoldenrodGreetingDuring:
	msgbox TCloner_Text_GoldenrodGreetingDuring, MSGBOX_DEFAULT
	return

TCloner_EventScript_GoldenrodGreetingAfter:
	msgbox TCloner_Text_GoldenrodGreetingAfter, MSGBOX_DEFAULT
	return

TCloner_EventScript_GoldenrodByTheWay:
	msgbox TCloner_Text_GoldenrodByTheWay, MSGBOX_DEFAULT
	return

TCloner_EventScript_GoldenrodWas:
	msgbox TCloner_Text_GoldenrodWas, MSGBOX_DEFAULT
	return

TCloner_EventScript_GoldenrodFirstDuring:
	msgbox TCloner_Text_GoldenrodFirstDuring, MSGBOX_DEFAULT
	return

TCloner_EventScript_GoldenrodFirstAfter:
	msgbox TCloner_Text_GoldenrodFirstAfter, MSGBOX_DEFAULT
	return

@ Stage 4: outside the Cerulean Pokemon Center.
TCloner_EventScript_Cerulean::
	lock
	faceplayer
	goto_if_eq VAR_TCLONER_STAGE, 0, TCloner_EventScript_CeruleanFirst
	msgbox TCloner_Text_CeruleanGreeting, MSGBOX_DEFAULT
	call EventScript_TClonerRewards
	@ Celebi was named at Goldenrod; without it, he starts from the question.
	goto_if_ge VAR_TCLONER_STAGE, 3, TCloner_EventScript_CeruleanCelebiKnown
	call_if_eq VAR_TCLONER_STAGE, 1, TCloner_EventScript_CeruleanWasRocket
	goto TCloner_EventScript_CeruleanHowItWorks

TCloner_EventScript_CeruleanFirst:
	msgbox TCloner_Text_Psst, MSGBOX_DEFAULT
	setvar VAR_0x8005, FALSE
	call TCloner_EventScript_GiveAndExplain
	msgbox TCloner_Text_CeruleanFirstJob, MSGBOX_DEFAULT
TCloner_EventScript_CeruleanHowItWorks:
	msgbox TCloner_Text_CeruleanHowItWorks, MSGBOX_DEFAULT
	goto TCloner_EventScript_CeruleanEnd

TCloner_EventScript_CeruleanCelebiKnown:
	msgbox TCloner_Text_CeruleanCelebi, MSGBOX_DEFAULT
TCloner_EventScript_CeruleanEnd:
	msgbox TCloner_Text_CeruleanParticles, MSGBOX_DEFAULT
	setvar VAR_TCLONER_STAGE, 4
	goto TCloner_EventScript_Leave

TCloner_EventScript_CeruleanWasRocket:
	msgbox TCloner_Text_CeruleanWasRocket, MSGBOX_DEFAULT
	return

@ Stage 5: Oak's lab, for good. Never a first meeting (he only gets here after
@ Cerulean), and he does not leave.
TCloner_EventScript_Pallet::
	lock
	faceplayer
	goto_if_ge VAR_TCLONER_STAGE, 5, TCloner_EventScript_PalletAgain
	msgbox TCloner_Text_PalletGreeting, MSGBOX_DEFAULT
	call EventScript_TClonerRewards
	msgbox TCloner_Text_PalletWhy, MSGBOX_DEFAULT
	setvar VAR_TCLONER_STAGE, 5
	release
	end

TCloner_EventScript_PalletAgain:
	msgbox TCloner_Text_PalletAgain, MSGBOX_DEFAULT
	call EventScript_TClonerRewards
	msgbox TCloner_Text_PalletDropBy, MSGBOX_DEFAULT
	release
	end

TCloner_Text_Psst:
	.string "Psst… Hey, you! Did PROF. ELM\n"
	.string "send you?\p"
	.string "I used to be ELM's assistant.$"

TCloner_Text_Commotion:
	.string "Quite a commotion around here,\n"
	.string "huh?$"

TCloner_Text_TakeThis:
	.string "Here, take this.$"

TCloner_Text_NoRoom:
	.string "Oh, no room in your BAG? Come back\n"
	.string "when you've made some.$"

TCloner_Text_HowItWorks:
	.string "It's the TcloneR. Use it.\p"
	.string "You can stea… ahem, CLONE one of\n"
	.string "your opponent's POKéMON per battle.\p"
	.string "They won't even notice, and they'll\n"
	.string "challenge you again if they see you.\p"
	.string "The POKéMON will be just like one\n"
	.string "you caught yourself.$"

TCloner_Text_ExplainLater:
	.string "It's complicated… I'll explain if\n"
	.string "we meet again.\p"
	.string "Nobody gets hurt… Probably.$"

TCloner_Text_NobodyGetsHurt:
	.string "Nobody gets hurt… Probably.$"

TCloner_Text_NiceReward:
	.string "There's a nice reward waiting if\n"
	.string "you steal… uh, clone a lot of them.$"

TCloner_Text_AzaleaGoFindOthers:
	.string "Now I have to go find the others.\p"
	.string "I heard there's a nice lake near\n"
	.string "MAHOGANY TOWN.$"

TCloner_Text_HaveToGoNow:
	.string "I have to go now.$"

TCloner_Text_MahoganyGreeting:
	.string "Look who it is. How's it going?\n"
	.string "How many have you got?$"

TCloner_Text_MahoganyWhoAmI:
	.string "There are still plenty of rewards\n"
	.string "for you.\p"
	.string "What? Who am I?\p"
	.string "I used to be ELM's assistant.\n"
	.string "Now I'm with TEAM ROCKET.\p"
	.string "Fewer rules, more money. I don't\n"
	.string "like it much, but I can do\l"
	.string "whatever I want.$"

TCloner_Text_MahoganyOneMoreThing:
	.string "Oh, one more thing. I'm with\n"
	.string "TEAM ROCKET now.\p"
	.string "Fewer rules, more money. I don't\n"
	.string "like it much, but I can do\l"
	.string "whatever I want.$"

TCloner_Text_MahoganyShadowOfATower:
	.string "I have to go now, but I think we'll\n"
	.string "meet again soon…\p"
	.string "in the shadow of a tower.\p"
	.string "Clone 'em all!$"

TCloner_Text_GoldenrodGreetingDuring:
	.string "I knew you'd be the one to show up.\p"
	.string "You're giving the higher-ups a lot\n"
	.string "of trouble. I don't think they'll\l"
	.string "give up easily.\p"
	.string "Anyway, how many have you cloned?$"

TCloner_Text_GoldenrodGreetingAfter:
	.string "I knew it wouldn't last. Now I need\n"
	.string "a new job… I think I'll head to\l"
	.string "KANTO.\p"
	.string "What about you? How many have you\n"
	.string "cloned?$"

TCloner_Text_GoldenrodByTheWay:
	.string "Oh, right: I'm with TEAM ROCKET,\n"
	.string "by the way.$"

TCloner_Text_GoldenrodWas:
	.string "Oh, right. I was with TEAM ROCKET.\n"
	.string "Was.$"

TCloner_Text_GoldenrodFirstDuring:
	.string "Oh, and I'm with TEAM ROCKET.\p"
	.string "You're giving the higher-ups a lot\n"
	.string "of trouble, you know. I don't think\l"
	.string "they'll give up easily.$"

TCloner_Text_GoldenrodFirstAfter:
	.string "I was with TEAM ROCKET. Was.\p"
	.string "I knew it wouldn't last. Now I need\n"
	.string "a new job… I think I'll head to\l"
	.string "KANTO.$"

TCloner_Text_GoldenrodParticles:
	.string "The other TRAINERS don't notice\n"
	.string "because we use tachyon particles\l"
	.string "that…\p"
	.string "Wow, what a blank look…\p"
	.string "Fine, let's just say CELEBI is\n"
	.string "involved. OK?\p"
	.string "I've got to run too. See you!$"

TCloner_Text_CeruleanGreeting:
	.string "Took you long enough to get here.\p"
	.string "Me? Still looking for a job. Maybe\n"
	.string "I'll go back to PROF. OAK. You?$"

TCloner_Text_CeruleanFirstJob:
	.string "I was with TEAM ROCKET, until it\n"
	.string "fell apart.\p"
	.string "Now I'm looking for a job. Maybe\n"
	.string "I'll go back to PROF. OAK.$"

TCloner_Text_CeruleanWasRocket:
	.string "I was with TEAM ROCKET, until it\n"
	.string "fell apart.$"

TCloner_Text_CeruleanHowItWorks:
	.string "How does it work? Let's say we had\n"
	.string "an unpleasant run-in with CELEBI…$"

TCloner_Text_CeruleanCelebi:
	.string "CELEBI? Let's say we had an\n"
	.string "unpleasant run-in with CELEBI.$"

TCloner_Text_CeruleanParticles:
	.string "It lost some hair, and we extracted\n"
	.string "the particles from it.\p"
	.string "It's like time travel, so to speak,\n"
	.string "but very, very limited.\p"
	.string "Imagine what we could have done if\n"
	.string "we'd caught it…\p"
	.string "I have to go. Looks like someone\n"
	.string "still has it in for TEAM ROCKET.\p"
	.string "Clone 'em all!$"

TCloner_Text_PalletGreeting:
	.string "I came to visit an old friend, and\n"
	.string "he asked me to stay. Lucky, huh?\p"
	.string "Better this way. Life on the edge\n"
	.string "isn't for me.\p"
	.string "Our deal still stands, though. You?$"

TCloner_Text_PalletWhy:
	.string "Why did I ask you to do this?\p"
	.string "I thought that if their research\n"
	.string "moved forward, TEAM ROCKET would\l"
	.string "have more ways to make money too.\p"
	.string "But that's pointless now.\p"
	.string "I think I'll stay here for a while.\n"
	.string "Drop by when you're around. Maybe\l"
	.string "I'll have something for you.\p"
	.string "Gotta catch 'em all!$"

TCloner_Text_PalletAgain:
	.string "Our deal still stands. How many\n"
	.string "this time?$"

TCloner_Text_PalletDropBy:
	.string "Drop by again!$"
