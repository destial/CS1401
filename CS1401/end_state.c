/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 22 September 2025
@course CSD1401
@brief Definition file for unloading the game's allocation
*/

#include "end_state.h"
#include "cprocessing.h"
#include "asset_manager.h"
#include "utils.h"

CP_Image end_logo;
float end_alpha = 0.f;
float end_duration = 2.f;
char end_revert = 0;

void end_state_init(void) {
	end_logo = get_image(SPLASH_SCREEN_ASSET);
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_Settings_AntiAlias(TRUE);
}

void end_state_update(void) {
	CP_Graphics_ClearBackground(Get_Color(COLOR_BLACK));
	if (end_revert == 0) {
		end_alpha += CP_System_GetDt() / end_duration;
		if (end_alpha >= 1.f) {
			end_revert = 1;
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

void end_state_end(void) {
	free_assets();
}