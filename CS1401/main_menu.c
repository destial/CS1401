#include "cprocessing.h"
#include "main_menu.h"
#include "physics.h"
#include "game.h"
#include "asset_manager.h"
#include "end_state.h"

//setting button
typedef struct Button {
	CP_Vector pos;
	CP_Vector ending_pos, start_pos;
	CP_Vector scale;
	CP_Image button_image;
	char* text;
	float text_size;
	FunctionPtr func;
} Button;

float world_width1 = 48, world_height1 = 27;
Button buttons[10];
CP_Image* background;
CP_Font button_font;

char is_clicked = 0;

void play_button(void);

void exit_main_menu(void) {
	CP_Engine_SetNextGameState(end_state_init, end_state_update, end_state_end);
}

void main_menu_init(void) 
{
	button_font = get_font("Assets/Fonts/arial.ttf");

	CP_Font_Set(button_font);

	//defining the individual buttons
	buttons[0].button_image = get_image("Assets/Images/button_yellow.png");
	buttons[0].pos.x = world_width1 * 0.5;
	buttons[0].pos.y = world_height1 * 0.5;
	buttons[0].start_pos.x = buttons[0].pos.x;
	buttons[0].func = play_button;
	buttons[0].text = "PLAY";

	buttons[1].button_image = get_image("Assets/Images/button_blue.png");
	buttons[1].pos.x = world_width1 * 0.5;
	buttons[1].pos.y = world_height1 * 0.35;
	buttons[1].start_pos.x = buttons[1].pos.x;
	buttons[1].text = "SETTING";

	buttons[2].button_image = get_image("Assets/Images/button_grey.png");
	buttons[2].pos.x = world_width1 * 0.5;
	buttons[2].pos.y = world_height1 * 0.2;
	buttons[2].start_pos.x = buttons[2].pos.x;
	buttons[2].func = exit_main_menu;
	buttons[2].text = "QUIT";

	buttons[3].button_image = get_image("Assets/Images/level button.png");
	buttons[3].pos.x = world_width1 * 1.1;
	buttons[3].pos.y = world_height1 * .8;
	buttons[3].ending_pos.x = world_width1 * 0.1;
	buttons[3].start_pos.x = buttons[3].pos.x;
	buttons[3].func = change_screen;
	buttons[3].text = "1";

	buttons[4].button_image = get_image("Assets/Images/level button.png");
	buttons[4].pos.x = world_width1 * 1.2;
	buttons[4].pos.y = world_height1 * .8;
	buttons[4].ending_pos.x = world_width1 * 0.2;
	buttons[4].start_pos.x = buttons[3].pos.x;
	buttons[4].func = change_screen;
	buttons[4].text = "2";

	buttons[5].button_image = get_image("Assets/Images/level button.png");
	buttons[5].pos.x = world_width1 * 1.3;
	buttons[5].pos.y = world_height1 * .8;
	buttons[5].ending_pos.x = world_width1 * 0.3;
	buttons[5].start_pos.x = buttons[3].pos.x;
	buttons[5].func = change_screen;
	buttons[5].text = "3";

	buttons[6].button_image = get_image("Assets/Images/level button.png");
	buttons[6].pos.x = world_width1 * 1.4;
	buttons[6].pos.y = world_height1 * .8;
	buttons[6].ending_pos.x = world_width1 * 0.4;
	buttons[6].start_pos.x = buttons[3].pos.x;
	buttons[6].func = change_screen;
	buttons[6].text = "4";

	buttons[7].button_image = get_image("Assets/Images/level button.png");
	buttons[7].pos.x = world_width1 * 1.5;
	buttons[7].pos.y = world_height1 * .8;
	buttons[7].ending_pos.x = world_width1 * 0.5;
	buttons[7].start_pos.x = buttons[3].pos.x;
	buttons[7].func = change_screen;
	buttons[7].text = "5";

	buttons[8].button_image = get_image("Assets/Images/level button.png");
	buttons[8].pos.x = world_width1 * 1.5;
	buttons[8].pos.y = world_height1 * .15;
	buttons[8].ending_pos.x = world_width1 * 0.1;
	buttons[8].start_pos.x = buttons[3].pos.x;
	buttons[8].text = "<";

	background = get_image("Assets/Images/Game Background.png");

	//setting the button anchor to the center of the world
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_System_SetWindowSize(1600, 900);
}

void main_menu_update(void) 
{
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));
	CP_Image_Draw(background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, CP_System_GetWindowWidth(), CP_System_GetWindowHeight(), 255);
	CP_Color textColor = CP_Color_Create(0, 0, 0, 255);

	int mouse_x = CP_Input_GetMouseX();
	int mouse_y = CP_Input_GetMouseY();

	//MAIN MENU SCREEN BUTTON
	for (int i = 0; i < 3; i++) {
		//defining the position of the button
		CP_Vector coords = world_to_screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);

		//Drawing Text
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Settings_TextSize(buttons[i].text_size);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText(buttons[i].text, coords.x, coords.y);

		//telling what the button should do
		if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5) 
		{
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 9.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 4.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 70.0f, CP_System_GetDt() * 5);

			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) 
			{
				if (buttons[i].func) {
					buttons[i].func();
				}
			}
		}
		else {
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 7.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 3.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 50.0f, CP_System_GetDt() * 5);
		}

		if (is_clicked == 1) {
			buttons[i].pos.x = CP_Math_LerpFloat(buttons[i].pos.x, -15.0f, CP_System_GetDt() * 4);
		}
		else {
			buttons[i].pos.x = CP_Math_LerpFloat(buttons[i].pos.x, buttons[i].start_pos.x, CP_System_GetDt() * 4);
		}
	}

	//LEVEL SELECT BUTTONS
	for (int i = 3; i < 9; i++) {
		//defining the position of the button
		CP_Vector coords = world_to_screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		CP_Settings_ImageMode(CP_POSITION_CENTER);
		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);

		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Settings_TextSize(buttons[i].text_size);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText(buttons[i].text, coords.x, coords.y);

		if (is_clicked == 1) {
			buttons[i].pos.x = CP_Math_LerpFloat(buttons[i].pos.x, buttons[i].ending_pos.x, CP_System_GetDt() * 4);
		}
		else {
			buttons[i].pos.x = CP_Math_LerpFloat(buttons[i].pos.x, buttons[i].start_pos.x, CP_System_GetDt() * 4);
		}
		
		//telling what the button should do
		if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5)
		{
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 4.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 7.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 90.0f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
			{
				if (buttons[i].func) {
					set_game_level(i - 3);
					buttons[i].func();
				}
				else {
					is_clicked = 0;
				}
			}
		}
		else {
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 3.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 5.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 70.0f, CP_System_GetDt() * 5);
		}
		
	}

}

void play_button(void) {
	is_clicked = !is_clicked;
}

//changes the Main menu screen to the game start state
void change_screen(void) {
	CP_Engine_SetNextGameState(game_init, game_update, game_exit);
}

//frees the image when exiting the application
void main_menu_exit(void) 
{
}