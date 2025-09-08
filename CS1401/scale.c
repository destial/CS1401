#include "cprocessing.h"

CP_Image logo;
float mouseX, mouseY;

void scale_init(void)
{
	logo = CP_Image_Load("Assets/DigiPen_BLACK.png");
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);

	int imageWidth = CP_Image_GetWidth(logo);
	int imageHeight = CP_Image_GetHeight(logo);

	CP_System_SetWindowSize(imageWidth, imageHeight);
	CP_System_SetFrameRate(CP_System_GetDisplayRefreshRate());
}

void scale_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
	mouseX = CP_Input_GetMouseX();
	mouseY = CP_Input_GetMouseY();

	CP_Image_Draw(logo, 0, 0, mouseX, mouseY, 255);

	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		CP_Engine_Terminate();
	}
}

void scale_exit(void)
{
	CP_Image_Free(&logo);
}