#include <stdio.h>
#include "end_state.h"
#include "cprocessing.h"
#include "asset_manager.h"
#include "particle_manager.h"
#include "state_manager.h"
#include "utils.h"

CP_Image end_logo;
float end_alpha = 0.f;
float end_duration = 2.f;
bool end_revert = false;

void end_state_init(void) 
{
	Set_Current_State(STATE_EXIT);
	printf("Init end\n");
	end_logo = Asset_GetImage(SPLASH_SCREEN_ASSET);
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_Settings_AntiAlias(true);
	CP_Sound_StopAll();
}

void end_state_update(void) 
{
	CP_Graphics_ClearBackground(Get_Color(COLOR_BLACK));
	if (end_revert == 0) {
		end_alpha += CP_System_GetDt() / end_duration;
		if (end_alpha >= 1.f) {
			end_revert = true;
		}
	}
	else {
		end_alpha -= CP_System_GetDt() / end_duration;
	}
	end_alpha = CP_Math_ClampFloat(end_alpha, 0.f, 1.f);

	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Image_Draw(end_logo, 0.f, 0.f, CP_System_GetWindowWidth() * 1.f, CP_System_GetWindowHeight() * 1.f, (int) (end_alpha * 255.f));

	if (end_revert && end_alpha == 0.f) {
		CP_Engine_Terminate();
	}
}

void end_state_exit(void) 
{
	Assets_Free();
	Particles_Free();
	printf("Exit end\n");
}