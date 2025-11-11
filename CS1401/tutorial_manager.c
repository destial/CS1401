#include <stdio.h>
#include <stdlib.h>
#include "cprocessing.h"
#include "asset_manager.h"
#include "sprite_manager.h"
#include "tutorial_manager.h"

typedef struct TutorialStruct {
	SpriteSheet* gif;
	char instructions[256];
} Tutorial;

bool tutorial_phases_loaded = false;
Tutorial tutorial_phases[TUT_PHASE_LAST];
TutorialPhase curr_tutorial_phase = TUT_PHASE_DRAG_BALL;
bool tutorial_mode = false;

void Tutorial_Load(bool tut_mode) {
	tutorial_mode = tut_mode;
	if (tut_mode) {
		switch (curr_tutorial_phase) {
		case TUT_PHASE_DRAG_BALL:
			tutorial_phases[TUT_PHASE_DRAG_BALL].gif = Asset_GetSprite(TUTORIAL_PHASE_1_ASSET, 14, 5, 5.f);
			sprintf_s(tutorial_phases[TUT_PHASE_DRAG_BALL].instructions, 256,
				"Use the mouse left button to hold "
				"and drag any golf ball to charge up "
				"the swing. Release the mouse left to "
				"hit the golf ball. Click anywhere "
				"to start playing!");
			break;
		case TUT_PHASE_ITEM:
			tutorial_phases[TUT_PHASE_ITEM].gif = Asset_GetSprite(TUTORIAL_PHASE_2_ASSET, 8, 6, 5.f);
			sprintf_s(tutorial_phases[TUT_PHASE_ITEM].instructions, 256,
				"Roll over the powerups in the level "
				"to activate the ability. [placeholder] "
				"means to speed up, [mushroom up] means "
				"to grow in size, [mushroom down] means "
				"to shrink in size!");
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
	if (curr_tutorial_phase == TUT_NO_PHASE)
		return;
	Tutorial* tut = &tutorial_phases[curr_tutorial_phase];
	Asset_Free_Node(tut->gif);
	if (curr_tutorial_phase + 1 >= TUT_PHASE_LAST) {
		curr_tutorial_phase = TUT_NO_PHASE;
		return;
	}
	++curr_tutorial_phase;
}

void Tutorial_Update(void) {
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
		Tutorial_NextPhase();
		tutorial_mode = false;
	}
}

void Tutorial_Render(void) {
	if (!tutorial_mode || curr_tutorial_phase == TUT_NO_PHASE)
		return;

	Tutorial* tut = &tutorial_phases[curr_tutorial_phase];
	if (!tut)
		return;

	CP_Settings_Save();

	CP_Color fade = Get_ColorAlpha(COLOR_BLACK, 192);
	CP_Settings_Stroke(fade);
	CP_Settings_Fill(fade);
	CP_Settings_RectMode(CP_POSITION_CORNER);
	CP_Graphics_DrawRect(0.f, 0.f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());
	Sprite_Draw(tut->gif, CP_System_GetWindowWidth() * 0.25f, CP_System_GetWindowHeight() * 0.5f, tut->gif->sub_w, tut->gif->sub_h, 255);

	CP_Settings_Stroke(Get_Color(COLOR_WHITE));
	CP_Settings_Fill(Get_Color(COLOR_WHITE));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawTextBox(tut->instructions, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, tut->gif->sub_w);

	CP_Settings_Restore();
}
