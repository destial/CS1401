#include <stdio.h>
#include "splash_screen.h"
#include "cprocessing.h"
#include "state_manager.h"
#include "utils.h"
#include "asset_manager.h"
#include "particle_manager.h"

static CP_Image splash_logo;
static float splash_alpha = 0.f;
static float splash_duration = 2.f;
static bool splash_revert = false;

static void render_cursor(void) {
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	CP_Image_DrawAdvanced(Asset_GetImage(CURSOR_ASSET), CP_Input_GetMouseX(), CP_Input_GetMouseY(), 24.f, 24.f, 255, 0.f);
}

void splash_screen_init(void)
{
	GameState_SetCurrent(STATE_SPLASH);
	printf("Init splash\n");
	splash_logo = Asset_GetImage(SPLASH_SCREEN_ASSET);
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	CP_Settings_AntiAlias(true);
	CP_System_ShowCursor(false);
	CP_System_SetWindowSize(1600, 900); // requirement
	CP_System_SetFrameRate(CP_System_GetDisplayRefreshRate() * 1.f);
	CP_System_SetWindowTitle("GGCode - GGGolf Game");
	CP_Engine_SetPostUpdateFunction(render_cursor);
}

void splash_screen_update(void)
{
	CP_Graphics_ClearBackground(Get_Color(COLOR_BLACK));
	if (!splash_revert) {
		splash_alpha += 1.5f * CP_System_GetDt() / splash_duration;
		if (splash_alpha >= 1.f) {
			splash_revert = true;
		}
	}
	else {
		splash_alpha -= CP_System_GetDt() / splash_duration;
	}
	splash_alpha = CP_Math_ClampFloat(splash_alpha, 0.f, 1.f);

	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Image_Draw(splash_logo, 0.f, 0.f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight(), (int)(splash_alpha * 255.f));

	if (splash_revert && splash_alpha == 0.f) {
		GameState_SetNext(STATE_MAINMENU);
	}
}

void splash_screen_exit(void)
{
	Assets_Free();
	Particles_Free();

	printf("Exit splash\n");
}