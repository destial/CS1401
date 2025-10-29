/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 September 2025
@course CSD1401
@brief Definition file for the splash screen when the game starts up
*/

#include "splash_screen.h"
#include "cprocessing.h"
#include "main_menu.h"
#include "utils.h"
#include "asset_manager.h"

CP_Image splash_logo;
float splash_alpha = 0.f;
float splash_duration = 2.f;
char splash_revert = 0;

void splash_screen_init(void)
{
	splash_logo = get_image(SPLASH_SCREEN_ASSET);
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_Settings_AntiAlias(TRUE);
	CP_System_SetWindowSize(1600, 900); // requirement
	CP_System_SetFrameRate(CP_System_GetDisplayRefreshRate() * 1.f);
	CP_System_SetWindowTitle("GGCode - GGGolf Game");
}

void splash_screen_update(void)
{
	CP_Graphics_ClearBackground(Get_Color(COLOR_BLACK));
	if (splash_revert == 0) {
		splash_alpha += CP_System_GetDt() / splash_duration;
		if (splash_alpha >= 1.f) {
			splash_revert = 1;
		}
	}
	else {
		splash_alpha -= CP_System_GetDt() / splash_duration;
	}
	splash_alpha = CP_Math_ClampFloat(splash_alpha, 0.f, 1.f);

	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Image_Draw(splash_logo, 0.f, 0.f, CP_System_GetWindowWidth() * 1.f, CP_System_GetWindowHeight() * 1.f, (int)(splash_alpha * 255.f));

	if (splash_revert && splash_alpha == 0.f) {
		CP_Engine_SetNextGameStateForced(main_menu_init, main_menu_update, main_menu_exit);
	}
}

void splash_screen_exit(void)
{
	free_assets();
}