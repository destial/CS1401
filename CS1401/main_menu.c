#include "cprocessing.h"
#include "main_menu.h"
#include "physics.h"
#include "game.h"


typedef struct Button {
	CP_Vector pos;
	CP_Vector scale;
	CP_Image button_image;
	FunctionPtr func;
} Button;

float world_width1 = 48, world_height1 = 27;
float scale_counter = 0;
Button buttons[3];
CP_Image background;

void main_menu_init(void) 
{
	
	buttons[0].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[0].scale.x = 5.0f;
	buttons[0].scale.y = 2.0f;
	buttons[0].pos.x = world_width1 * 0.5;
	buttons[0].pos.y = world_height1 * 0.5;
	buttons[0].func = change_screen;

	buttons[1].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[1].scale.x = 5.0f;
	buttons[1].scale.y = 2.0f;
	buttons[1].pos.x = world_width1 * 0.5;
	buttons[1].pos.y = world_height1 * 0.4;
	buttons[1].func = change_screen;

	buttons[2].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[2].scale.x = 5.0f;
	buttons[2].scale.y = 2.0f;
	buttons[2].pos.x = world_width1 * 0.5;
	buttons[2].pos.y = world_height1 * 0.3;
	buttons[2].func = CP_Engine_Terminate;

	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_System_SetWindowSize(1600, 900);

}

void main_menu_update(void) 
{
	CP_Graphics_ClearBackground(CP_Color_Create(0, 255, 0, 255));
	
	int mouse_x = CP_Input_GetMouseX();
	int mouse_y = CP_Input_GetMouseY();

	scale_counter += CP_System_GetDt();
	
	for (int i = 0; i < 3; i++) {
		CP_Vector coords = world_to_screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x - local_scale.x * 0.5f, coords.y - local_scale.y * 0.5f, local_scale.x, local_scale.y, 255, 0);
		if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5) 
		{
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 6.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 3.0f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
				buttons[i].func();
		}
		else {
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 5.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 2.0f, CP_System_GetDt() * 5);
		}
		// if mouse in button and click
	}
}

void change_screen(void) {
	CP_Engine_SetNextGameState(game_init, game_update, game_exit);
}

void main_menu_exit(void) 
{
	CP_Image_Free(&background);
	for (int n = 0; n < 3; n++) {
		CP_Image_Free(buttons[n].button_image);
	}
}