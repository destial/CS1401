#include "splash_screen.h"
#include "cprocessing.h"
#include "game.h"
#include "main_menu.h"
#include "asset_manager.h"

CP_Image logo;
float splashScreenAlpha = 0.f;
float duration = 2.f;
int revert = 0;
void splash_screen_init(void)
{
	logo = get_image("Assets/Images/DigiPen_Singapore_WEB_RED.png");
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_Settings_AntiAlias(TRUE);
	CP_System_SetWindowSize(1600, 900);
	//CP_System_SetWindowSize(CP_System_GetDisplayWidth(), CP_System_GetDisplayHeight());
	CP_System_SetFrameRate(CP_System_GetDisplayRefreshRate());
}

void splash_screen_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));
	int width = CP_Image_GetWidth(logo);
	int height = CP_Image_GetHeight(logo);
	if (revert == 0) {
		splashScreenAlpha += CP_System_GetDt() / duration;
		if (splashScreenAlpha >= 1.f) {
			revert = 1;
		}
	}
	else {
		splashScreenAlpha -= CP_System_GetDt() / duration;
	}
	splashScreenAlpha = CP_Math_ClampFloat(splashScreenAlpha, 0, 1.f);
	float alpha = CP_Math_LerpFloat(0, 255, splashScreenAlpha);

	CP_Image_Draw(logo, (CP_System_GetWindowWidth() - width) / 2.f, (CP_System_GetWindowHeight() - height) / 2.f, width, height, alpha);

	if (revert && splashScreenAlpha == 0) {
		
		CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
	}
}

void splash_screen_exit(void)
{
}