#include "cprocessing.h"

CP_Image logo;
float timeDelta = 1 / 60.f;
float splashScreenAlpha = 0.f;
int revert = 0;

void game_init(void)
{

}

void game_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		CP_Engine_Terminate();
	}
}

void game_exit(void)
{

}

void splash_screen_init(void)
{
	logo = CP_Image_Load("Assets/DigiPen_Singapore_WEB_RED.png");
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_System_SetWindowSize(CP_System_GetDisplayWidth(), CP_System_GetDisplayHeight());
}

void splash_screen_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));
	int width = CP_Image_GetWidth(logo);
	int height = CP_Image_GetHeight(logo);
	if (revert == 0) {
		splashScreenAlpha += timeDelta;
		if (splashScreenAlpha >= 1.f) {
			revert = 1;
		}
	}
	else {
		splashScreenAlpha -= timeDelta;
	}
	splashScreenAlpha = CP_Math_ClampFloat(splashScreenAlpha, 0, 1.f);
	float alpha = CP_Math_LerpFloat(0, 255, splashScreenAlpha);
	
	CP_Image_Draw(logo, (CP_System_GetDisplayWidth() - width) / 2.f, (CP_System_GetDisplayHeight() - height) / 2.f, width, height, alpha);

	if (revert && splashScreenAlpha == 0) {
		CP_Engine_SetNextGameState(game_init, game_update, game_exit);
	}
}

void splash_screen_exit(void)
{
	CP_Image_Free(&logo);
}

int main(void)
{
	CP_Engine_SetNextGameState(splash_screen_init, splash_screen_update, splash_screen_exit);
	CP_Engine_Run(0);
	return 0;
}