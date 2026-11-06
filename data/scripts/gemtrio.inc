@ ─────────────────────────────────────────────────────────────────────────────
@ GemTrio shared QoL scripts
@ Included from data/event_scripts.s
@ ─────────────────────────────────────────────────────────────────────────────

@ Announces the new party Level Cap after a Gym Badge is earned.
@ The cap itself is computed in src/battle_setup.c (GemTrio_RefreshLevelCap)
@ and returned in VAR_0x8006; 0 means "do not announce".
GemTrio_EventScript_AnnounceLevelCap::
	special GemTrio_RefreshLevelCap
	goto_if_eq VAR_0x8006, 0, GemTrio_EventScript_AnnounceLevelCap_End
	buffernumberstring STR_VAR_1, VAR_0x8006
	msgbox GemTrio_Text_LevelCapRaised, MSGBOX_DEFAULT
GemTrio_EventScript_AnnounceLevelCap_End::
	return

@ A gym guide / signboard helper could call this to report the current cap
@ at any time, without earning a Badge.
GemTrio_EventScript_ShowCurrentLevelCap::
	lockall
	special GemTrio_RefreshLevelCap
	goto_if_eq VAR_0x8006, 0, GemTrio_EventScript_ShowCurrentLevelCap_End
	buffernumberstring STR_VAR_1, VAR_0x8006
	msgbox GemTrio_Text_CurrentLevelCap, MSGBOX_DEFAULT
GemTrio_EventScript_ShowCurrentLevelCap_End::
	releaseall
	end

GemTrio_Text_LevelCapRaised:
	.string "The party's LEVEL CAP is now {STR_VAR_1}!\p"
	.string "POKéMON stop gaining EXP at that level.\n"
	.string "Earn another BADGE to raise the cap.$"

GemTrio_Text_CurrentLevelCap:
	.string "The party's current LEVEL CAP is\n"
	.string "{STR_VAR_1}.\p"
	.string "POKéMON stop gaining EXP at that level.\n"
	.string "Earn another BADGE to raise the cap.$"

@ ── Inter-region Ferry Travel: Hoenn <-> Kanto ──────────────────────────────────
GemTrio_EventScript_FerryToKanto::
	lockall
	faceplayer
	msgbox GemTrio_Text_FerryToKantoPrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, GemTrio_EventScript_FerryToKanto_Decline
	msgbox GemTrio_Text_FerryToKantoDepart, MSGBOX_DEFAULT
	closemessage
	warp MAP_VERMILION_CITY, 15, 20
	waitstate
	releaseall
	end

GemTrio_EventScript_FerryToKanto_Decline::
	msgbox GemTrio_Text_FerryToKantoDeclined, MSGBOX_DEFAULT
	releaseall
	end

GemTrio_EventScript_FerryToHoenn::
	lockall
	faceplayer
	msgbox GemTrio_Text_FerryToHoennPrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, GemTrio_EventScript_FerryToHoenn_Decline
	msgbox GemTrio_Text_FerryToHoennDepart, MSGBOX_DEFAULT
	closemessage
	warp MAP_LILYCOVE_CITY_HARBOR, 8, 11
	waitstate
	releaseall
	end

GemTrio_EventScript_FerryToHoenn_Decline::
	msgbox GemTrio_Text_FerryToHoennDeclined, MSGBOX_DEFAULT
	releaseall
	end

GemTrio_Text_FerryToKantoPrompt:
	.string "Ahoy! The S.S. TIDAL is preparing to\n"
	.string "sail across the sea to KANTO.\p"
	.string "Would you like to sail to VERMILION CITY?$"

GemTrio_Text_FerryToKantoDepart:
	.string "All aboard! Setting sail for KANTO!$"

GemTrio_Text_FerryToKantoDeclined:
	.string "Come back whenever you're ready to\n"
	.string "challenge the KANTO GYM LEADERS!$"

GemTrio_Text_FerryToHoennPrompt:
	.string "Welcome to the KANTO seaport!\p"
	.string "Would you like to board the S.S. TIDAL\n"
	.string "back to HOENN (LILYCOVE CITY)?$"

GemTrio_Text_FerryToHoennDepart:
	.string "Aye aye! Setting sail for HOENN!$"

GemTrio_Text_FerryToHoennDeclined:
	.string "Safe travels in KANTO!$"

