#include "splash_screen.h"
#include "cprocessing.h"
#include "main_menu.h"
#include "game.h"
#include "asset_manager.h"

CP_Image* logo;
float splashScreenAlpha = 0.f;
float duration = 2.f;
char revert = 0;
void splash_screen_init(void)
{
	logo = get_image(SPLASH_SCREEN_ASSET);
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_Settings_AntiAlias(TRUE);
	CP_System_SetWindowSize(1600, 900); // requirement
	CP_System_SetFrameRate(CP_System_GetDisplayRefreshRate() * 1.f);
}

void splash_screen_update(void)
{
	CP_Graphics_ClearBackground(get_color(BLACK));
	if (revert == 0) {
		splashScreenAlpha += CP_System_GetDt() / duration;
		if (splashScreenAlpha >= 1.f) {
			revert = 1;
		}
	}
	else {
		splashScreenAlpha -= CP_System_GetDt() / duration;
	}
	splashScreenAlpha = CP_Math_ClampFloat(splashScreenAlpha, 0.f, 1.f);
	int alpha = CP_Math_LerpInt(0, 255, splashScreenAlpha);

	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Image_Draw((CP_Image) logo, 0.f, 0.f, CP_System_GetWindowWidth() * 1.f, CP_System_GetWindowHeight() * 1.f, alpha);

	if (revert && splashScreenAlpha == 0.f) {
		CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
	}
}

void splash_screen_exit(void)
{
	// dont need to clean up logo image since its handled by asset manager already :D
}