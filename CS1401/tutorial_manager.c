/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 November 2025
@course CSD1401
@brief Definition file for controlling the tutorial stages
*/

#include <stdio.h>
#include <stdlib.h>
#include "cprocessing.h"
#include "asset_manager.h"
#include "sprite_manager.h"
#include "tutorial_manager.h"
#include "ui_manager.h"

typedef struct TutorialStruct {
	SpriteSheet gif;
	char instructions[256]; // fixed size
} Tutorial;

static Tutorial tutorial_phases[TUT_PHASE_LAST];
static TutorialPhase curr_tutorial_phase = TUT_PHASE_DRAG_BALL;
static bool tutorial_mode = false;

void Tutorial_Load(bool tut_mode) {
	tutorial_mode = tut_mode;
	if (tut_mode) {
		switch (curr_tutorial_phase) {
		case TUT_PHASE_DRAG_BALL:
			tutorial_phases[curr_tutorial_phase].gif = Asset_GetSprite(TUTORIAL_PHASE_1_ASSET, 8, 6, 5.f);
			sprintf_s(tutorial_phases[curr_tutorial_phase].instructions, 256,
				"Click and drag a golf ball to aim your shot.\n"
				"Release to swing.\n"
				"Scroll to adjust power.\n"
				"Click anywhere to start playing!\n");
			break;
		case TUT_PHASE_ITEM:
			tutorial_phases[curr_tutorial_phase].gif = Asset_GetSprite(TUTORIAL_PHASE_2_ASSET, 9, 5, 5.f);
			sprintf_s(tutorial_phases[curr_tutorial_phase].instructions, 256,
				"Roll over power-ups to activate them:\n"
				"Bolt = Speed Boost,\n"
				"Red Mushroom = Grow,\n"
				"Blue Mushroom = Shrink,\n"
				"Arrows = Reverse Direction,\n"
				"Coins = To buy in the shop.\n"
				"Click anywhere to start playing!\n");
			break;
		case TUT_PHASE_TERRAIN:
			tutorial_phases[curr_tutorial_phase].gif = Asset_GetSprite(TUTORIAL_PHASE_3_ASSET, 11, 4, 5.f);
			sprintf_s(tutorial_phases[curr_tutorial_phase].instructions, 256,
				"Different terrains affect your ball:\n"
				"Sand slows down,\n"
				"Ice makes you slide,\n"
				"Red pits reset your ball to its previous spot.\n"
				"Click anywhere to start playing!\n");
			break;
		case TUT_PHASE_PORTAL:
			tutorial_phases[curr_tutorial_phase].gif = Asset_GetSprite(TUTORIAL_PHASE_4_ASSET, 6, 6, 5.f);
			sprintf_s(tutorial_phases[curr_tutorial_phase].instructions, 256,
				"Sometimes there are portals:\n"
				"Teleport the ball from one\n"
				"portal to another.\n"
				"Click anywhere to start playing!\n");
			break;
		default: break;
		}
	}
}

void Tutorial_SetPhase(TutorialPhase phase) {
	curr_tutorial_phase = phase;
}

TutorialPhase Tutorial_GetPhase(void) {
	return curr_tutorial_phase;
}

bool Tutorial_GetMode(void) {
	return tutorial_mode;
}

void Tutorial_NextPhase(void) {
	// no phase detected, return
	if (curr_tutorial_phase == TUT_NO_PHASE)
		return;

	// clear the previous loaded tutorial phase
	Tutorial* tut = &tutorial_phases[curr_tutorial_phase];
	Asset_Free_Node(tut->gif);
	
	// if last in the phase, no more phases left
	if (curr_tutorial_phase + 1 >= TUT_PHASE_LAST) {
		curr_tutorial_phase = TUT_NO_PHASE;
		return;
	}
	++curr_tutorial_phase;
}

void Tutorial_Update(void) {
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) { // click anywhere on the screen to play the game
		Tutorial_NextPhase();
		tutorial_mode = false;
	}
}

void Tutorial_Render(void) {
	// not in tutorial mode
	if (!tutorial_mode || curr_tutorial_phase == TUT_NO_PHASE)
		return;

	// if for some reason the tutorial phase isnt loaded
	Tutorial* tut = &tutorial_phases[curr_tutorial_phase];
	if (!tut)
		return;

	UI_SetPriority(1);
	// tinted background
	UI_Quad(CP_System_GetWindowWidth() * 0.5f, (float)CP_System_GetWindowHeight() * 0.5f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight(), Get_ColorAlpha(COLOR_BLACK, 192));
	UI_SetPriority(2);
	// the gif
	UI_Video(CP_System_GetWindowWidth() * 0.25f, CP_System_GetWindowHeight() * 0.5f, (float)Sprite_GetWidth(tut->gif), (float)Sprite_GetHeight(tut->gif), tut->gif);

	// instructions border
	UI_Quad(CP_System_GetWindowWidth() * 0.75f, CP_System_GetWindowHeight() * 0.5f, (float)Sprite_GetWidth(tut->gif), (float)Sprite_GetHeight(tut->gif), Get_Color(COLOR_GRAY));

	// the instructions
	UI_SetPriority(3);
	UI_Text(CP_System_GetWindowWidth() * 0.75f, CP_System_GetWindowHeight() * 0.35f, (float)Sprite_GetWidth(tut->gif), (float)Sprite_GetHeight(tut->gif), Get_Color(COLOR_WHITE), tut->instructions, 50.f);
	UI_SetPriority(0);
}
