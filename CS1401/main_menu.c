#include "cprocessing.h"
#include "main_menu.h"
#include "physics.h"
#include "game.h"

//setting button
typedef struct Button {
	CP_Vector pos;
	CP_Vector ending_pos;
	CP_Vector scale;
	CP_Image button_image;
	FunctionPtr func;
} Button;

float world_width1 = 48, world_height1 = 27;
Button buttons[10];
CP_Image background;

char is_clicked = 0;

void main_menu_init(void) 
{
	//defining the individual buttons
	buttons[0].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[0].scale.x = 6.0f;
	buttons[0].scale.y = 2.0f;
	buttons[0].pos.x = world_width1 * 0.5;
	buttons[0].pos.y = world_height1 * 0.4;
	buttons[0].func = change_screen;

	buttons[1].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[1].scale.x = 6.0f;
	buttons[1].scale.y = 2.0f;
	buttons[1].pos.x = world_width1 * 0.5;
	buttons[1].pos.y = world_height1 * 0.3;
	buttons[1].func = change_screen;

	buttons[2].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[2].scale.x = 6.0f;
	buttons[2].scale.y = 2.0f;
	buttons[2].pos.x = world_width1 * 0.5;
	buttons[2].pos.y = world_height1 * 0.2;
	buttons[2].func = CP_Engine_Terminate;

	buttons[3].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[3].scale.x = 3.0f;
	buttons[3].scale.y = 4.0f;
	buttons[3].pos.x = world_width1 * 1.1;
	buttons[3].pos.y = world_height1 * .8;
	buttons[3].ending_pos.x = world_width1 * 0.1;
	buttons[3].func = change_screen;

	buttons[4].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[4].scale.x = 3.0f;
	buttons[4].scale.y = 4.0f;
	buttons[4].pos.x = world_width1 * 1.2;
	buttons[4].pos.y = world_height1 * .8;
	buttons[4].ending_pos.x = world_width1 * 0.2;
	buttons[4].func = change_screen;

	buttons[5].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[5].scale.x = 3.0f;
	buttons[5].scale.y = 4.0f;
	buttons[5].pos.x = world_width1 * 1.3;
	buttons[5].pos.y = world_height1 * .8;
	buttons[5].ending_pos.x = world_width1 * 0.3;
	buttons[5].func = change_screen;

	buttons[6].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[6].scale.x = 3.0f;
	buttons[6].scale.y = 4.0f;
	buttons[6].pos.x = world_width1 * 1.4;
	buttons[6].pos.y = world_height1 * .8;
	buttons[6].ending_pos.x = world_width1 * 0.4;
	buttons[6].func = change_screen;

	buttons[7].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	buttons[7].scale.x = 3.0f;
	buttons[7].scale.y = 4.0f;
	buttons[7].pos.x = world_width1 * 1.5;
	buttons[7].pos.y = world_height1 * .8;
	buttons[7].ending_pos.x = world_width1 * 0.5;
	buttons[7].func = change_screen;

	//setting the button anchor to the corner of the world
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_System_SetWindowSize(1600, 900);

}

void main_menu_update(void) 
{
	CP_Graphics_ClearBackground(CP_Color_Create(0, 255, 0, 255));
	
	int mouse_x = CP_Input_GetMouseX();
	int mouse_y = CP_Input_GetMouseY();

	//render all each buttons and defining its use
	for (int i = 0; i < 3; i++) {
		//defining the position of the button
		CP_Vector coords = world_to_screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x - local_scale.x * 0.5f, coords.y - local_scale.y * 0.5f, local_scale.x, local_scale.y, 255, 0);

		//telling what the button should do
		if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5) 
		{
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 7.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 3.0f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) 
			{
				is_clicked = 1;
			}
		}
		else {
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 6.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 2.0f, CP_System_GetDt() * 5);
		}

		if (is_clicked == 1) {
			buttons[i].pos.x = CP_Math_LerpFloat(buttons[i].pos.x, -5.0f, CP_System_GetDt() * 4);
		}
	}

	for (int i = 3; i < 10; i++) {
		//defining the position of the button
		CP_Vector coords = world_to_screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x - local_scale.x * 0.1f, coords.y - local_scale.y * 0.5f, local_scale.x, local_scale.y, 255, 0);

		if (is_clicked == 1) {
			buttons[i].pos.x = CP_Math_LerpFloat(buttons[i].pos.x, buttons[i].ending_pos.x, CP_System_GetDt() * 4);
		}
		
		//telling what the button should do
		if (mouse_x >= coords.x * 1.5f - local_scale.x && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * 0.9f && mouse_y <= coords.y + local_scale.y * .5)
		{
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 4.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 5.0f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
			{
				buttons[i].func();
			}
		}
		else {
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 3.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 4.0f, CP_System_GetDt() * 5);
		}
		
	}
}

//changes the Main menu screen to the game start state
void change_screen(void) {
	CP_Engine_SetNextGameState(game_init, game_update, game_exit);
}

//frees the image when exiting the application
void main_menu_exit(void) 
{
	CP_Image_Free(&background);
	for (int n = 0; n < 3; n++) {
		CP_Image_Free(buttons[n].button_image);
	}
}