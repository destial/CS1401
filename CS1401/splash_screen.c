#include "splash_screen.h"
#include "cprocessing.h"
#include "game.h"

CP_Image logo;
float splashScreenAlpha = 0.f;
float duration = 2.f;
int revert = 0;
void splash_screen_init(void)
{
	logo = CP_Image_Load("Assets/Images/DigiPen_Singapore_WEB_RED.png");
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
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
		splashScreenAlpha += CP_System_GetDt() * (1 / duration);
		if (splashScreenAlpha >= 1.f) {
			revert = 1;
		}
	}
	else {
		splashScreenAlpha -= CP_System_GetDt() * (1 / duration);
	}
	splashScreenAlpha = CP_Math_ClampFloat(splashScreenAlpha, 0, 1.f);
	float alpha = CP_Math_LerpFloat(0, 255, splashScreenAlpha);

	CP_Image_Draw(logo, (CP_System_GetWindowWidth() - width) / 2.f, (CP_System_GetWindowHeight() - height) / 2.f, width, height, alpha);

	if (revert && splashScreenAlpha == 0) {
		// @todo: set this to menu screen instead of game screen
		CP_Engine_SetNextGameState(game_init, game_update, game_exit);
	}
}

void splash_screen_exit(void)
{
	CP_Image_Free(&logo);
}