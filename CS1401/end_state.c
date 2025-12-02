#include <stdio.h>
#include "end_state.h"
#include "cprocessing.h"
#include "asset_manager.h"
#include "particle_manager.h"
#include "state_manager.h"
#include "utils.h"
#include "game_settings.h"

static CP_Image end_logo[3];
static float end_alpha = 0.f;
static float end_duration = 1.f;
static bool end_revert = false;
static int end_index = 0;

void end_state_init(void) 
{
	GameSetting_SetCursor(CURSOR_DEFAULT);
	GameState_SetCurrent(STATE_EXIT);
	printf("Init end\n");
	end_logo[0] = Asset_GetImage(END_CREDITS_1_ASSET);
	end_logo[1] = Asset_GetImage(END_CREDITS_2_ASSET);
	end_logo[2] = Asset_GetImage(END_CREDITS_3_ASSET);
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	CP_Settings_AntiAlias(true);
	CP_Sound_StopAll();
}

void end_state_update(void) 
{
	CP_Graphics_ClearBackground(Get_Color(COLOR_BLACK));
	if (!end_revert) {
		end_alpha += CP_System_GetDt() / end_duration;
		if (end_alpha >= 1.f) {
			end_revert = true;
		}
	}
	else {
		end_alpha -= CP_System_GetDt() / end_duration;
	}
	end_alpha = CP_Math_ClampFloat(end_alpha, 0.f, 1.f);
	
	CP_Image_Draw(end_logo[end_index], 0.f, 0.f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight(), (int)(end_alpha * 255.f));

	if (end_revert && end_alpha == 0.f) {
		end_index++;
		end_revert = false;

		if (end_index == 3) {
			CP_Engine_Terminate();
		}
	}
}

void end_state_exit(void) 
{
	Particles_Free();
	GameSetting_Save();
	Assets_Free();
	printf("Exit end\n");
}