#include "cprocessing.h"
#include <stdio.h>

/*
plans:
- oop paradigm
- move all of this to application class
- gameobject will have an init / update / render function
- clear up memory afterwards

*/

CP_Image logo;
float timeDelta = 1 / 60.f;
float splashScreenAlpha = 0.f;
int revert = 0;
CP_Font font;
float mouseX, mouseY;
int cookies = 0;
int clicked = 0;

void to_string(int f, const char* buffer)
{
	sprintf_s(buffer, 50, "%d", f);
}

void debug_draw(void)
{
	CP_Settings_Scale(1.f, 1.f);
	char r[256];
	to_string(1 / CP_System_GetDt(), r);
	CP_Font_DrawText(r, 0, 20);
	
	to_string(mouseX, r);
	CP_Font_DrawText(r, 0, 40);

	to_string(mouseY, r);
	CP_Font_DrawText(r, 0, 60);

	char b[256];
	to_string(cookies, b);
	CP_Font_DrawTextBox(b, 80, 80, 50);
}

void game_init(void)
{
	font = CP_Font_Load("Assets/arial.ttf");
	CP_Font_Set(font);
}

void game_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
	mouseX = CP_Input_GetMouseX();
	mouseY = CP_Input_GetMouseY();
	float circleDiameter = CP_System_GetDisplayWidth() / 4.f;
	float fixedCircleDiameter = circleDiameter;
	float x = (CP_System_GetDisplayWidth() - circleDiameter) / 2;
	float y = (CP_System_GetDisplayHeight() - circleDiameter) / 2;

	if (CP_Input_MouseDown(MOUSE_BUTTON_1)) {
		if (CP_Math_Square(mouseX - x) <= CP_Math_Square(circleDiameter / 2) && CP_Math_Square(mouseY - y) < CP_Math_Square(circleDiameter / 2) && clicked == 0) {
			circleDiameter *= 1.1f;
			cookies++;
			clicked = 1;
		}
	}
	else {
		clicked = 0;
	}
	
	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		CP_Engine_Terminate();
	}

	CP_Graphics_DrawCircle(x, y, circleDiameter);
	CP_Font_DrawText("Cookie Clicker", x, y + fixedCircleDiameter);
	CP_Settings_Stroke(CP_Color_Create(255, 0, 0, 255));
	CP_Graphics_DrawCircle(mouseX, mouseY, 5);

	debug_draw();
}

void game_exit(void)
{
	CP_Font_Free(&font);
}

void splash_screen_init(void)
{
	logo = CP_Image_Load("Assets/DigiPen_Singapore_WEB_RED.png");
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_System_SetWindowSize(CP_System_GetDisplayWidth(), CP_System_GetDisplayHeight());
	CP_System_SetFrameRate(CP_System_GetDisplayRefreshRate());
}

void splash_screen_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));
	int width = CP_Image_GetWidth(logo);
	int height = CP_Image_GetHeight(logo);
	if (revert == 0) {
		splashScreenAlpha += CP_System_GetDt();
		if (splashScreenAlpha >= 1.f) {
			revert = 1;
		}
	}
	else {
		splashScreenAlpha -= CP_System_GetDt();
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