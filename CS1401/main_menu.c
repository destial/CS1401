#include "cprocessing.h"
#include "main_menu.h"
#include "utils.h"
#include "game.h"
#include "asset_manager.h"
#include "sprite_manager.h"
#include "end_state.h"

//setting button
typedef struct Button {
	CP_Vector pos;
	CP_Vector ending_pos, start_pos;
	CP_Vector scale;
	CP_Image* button_image;
	char* text;
	float text_size;
	FunctionPtr func;
} Button;

float world_width1 = 48, world_height1 = 27;
int resolution_number = 0;

Button buttons[20];
CP_Image* background, *setting_background;
CP_Font* button_font;

char is_clicked = 0, is_setting_clicked = 0, is_overlayed = 0;

void exit_main_menu(void) {
	CP_Engine_SetNextGameState(end_state_init, end_state_update, end_state_end);
}

void main_menu_init(void) 
{
	button_font = get_font(ARIAL_FONT_ASSET);

	CP_Font_Set((CP_Font) button_font);

	//defining the individual buttons
	buttons[0].button_image = get_image(BUTTON_YELLOW_ASSET);
	buttons[0].pos.x = world_width1 * 0.5f;
	buttons[0].pos.y = world_height1 * 0.5f;
	buttons[0].start_pos.x = buttons[0].pos.x;
	buttons[0].func = play_button;
	buttons[0].text = "PLAY";

	buttons[1].button_image = get_image(BUTTON_BLUE_ASSET);
	buttons[1].pos.x = world_width1 * 0.5f;
	buttons[1].pos.y = world_height1 * 0.35f;
	buttons[1].start_pos.x = buttons[1].pos.x;
	buttons[1].func = setting_button;
	buttons[1].text = "SETTING";

	buttons[2].button_image = get_image(BUTTON_GREY_ASSET);
	buttons[2].pos.x = world_width1 * 0.5f;
	buttons[2].pos.y = world_height1 * 0.2f;
	buttons[2].start_pos.x = buttons[2].pos.x;
	buttons[2].func = exit_main_menu;
	buttons[2].text = "QUIT";

	buttons[3].button_image = get_image(LEVEL_BUTTON_ASSET);
	buttons[3].pos.x = world_width1 * 1.1f;
	buttons[3].pos.y = world_height1 * .8f;
	buttons[3].ending_pos.x = world_width1 * 0.1f;
	buttons[3].start_pos.x = buttons[3].pos.x;
	buttons[3].func = change_screen;
	buttons[3].text = "1";

	buttons[4].button_image = get_image(LEVEL_BUTTON_ASSET);
	buttons[4].pos.x = world_width1 * 1.2f;
	buttons[4].pos.y = world_height1 * .8f;
	buttons[4].ending_pos.x = world_width1 * 0.2f;
	buttons[4].start_pos.x = buttons[4].pos.x;
	buttons[4].func = change_screen;
	buttons[4].text = "2";

	buttons[5].button_image = get_image(LEVEL_BUTTON_ASSET);
	buttons[5].pos.x = world_width1 * 1.3f;
	buttons[5].pos.y = world_height1 * .8f;
	buttons[5].ending_pos.x = world_width1 * 0.3f;
	buttons[5].start_pos.x = buttons[5].pos.x;
	buttons[5].func = change_screen;
	buttons[5].text = "3";

	buttons[6].button_image = get_image(LEVEL_BUTTON_ASSET);
	buttons[6].pos.x = world_width1 * 1.4f;
	buttons[6].pos.y = world_height1 * .8f;
	buttons[6].ending_pos.x = world_width1 * 0.4f;
	buttons[6].start_pos.x = buttons[6].pos.x;
	buttons[6].func = change_screen;
	buttons[6].text = "4";

	buttons[7].button_image = get_image(LEVEL_BUTTON_ASSET);
	buttons[7].pos.x = world_width1 * 1.5f;
	buttons[7].pos.y = world_height1 * .8f;
	buttons[7].ending_pos.x = world_width1 * 0.5f;
	buttons[7].start_pos.x = buttons[7].pos.x;
	buttons[7].func = change_screen;
	buttons[7].text = "5";

	buttons[8].button_image = get_image(LEVEL_BUTTON_ASSET);
	buttons[8].pos.x = world_width1 * 1.5f;
	buttons[8].pos.y = world_height1 * .15f;
	buttons[8].ending_pos.x = world_width1 * 0.1f;
	buttons[8].start_pos.x = buttons[8].pos.x;
	buttons[8].func = play_button;
	buttons[8].text = "<";

	buttons[9].button_image = get_image(LEVEL_BUTTON_ASSET);
	buttons[9].pos.x = world_width1 * 0.5f;
	buttons[9].pos.y = world_height1 * 0.5f;
	buttons[9].start_pos.x = buttons[9].pos.x;
	buttons[9].text = "1280 X 720";

	buttons[10].button_image = get_image(LEVEL_BUTTON_ASSET);
	buttons[10].pos.x = world_width1 * 0.5f;
	buttons[10].pos.y = world_height1 * 0.4f;
	buttons[10].start_pos.x = buttons[10].pos.x;
	buttons[10].text = "1600 X 900";

	buttons[11].button_image = get_image(LEVEL_BUTTON_ASSET);
	buttons[11].pos.x = world_width1 * 0.5f;
	buttons[11].pos.y = world_height1 * 0.3f;
	buttons[11].start_pos.x = buttons[11].pos.x;
	buttons[11].text = "1920 X 1080";

	buttons[12].button_image = get_image(BUTTON_GREY_ASSET);
	buttons[12].pos.x = world_width1 * 0.25f;
	buttons[12].pos.y = world_height1 * 0.7f;
	buttons[12].start_pos.x = buttons[12].pos.x;
	buttons[12].text = "X";

	background = get_image(GAME_BACKGROUND_ASSET);
	setting_background = get_image(SETTING_BACKGROUND_ASSET);

	//setting the button anchor to the center of the world
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_System_SetWindowSize(1600, 900);
}

void main_menu_update(void) 
{
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_Scale(1.f, 1.f);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	CP_Image_Draw((CP_Image) background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, CP_System_GetWindowWidth() * 1.f, CP_System_GetWindowHeight() * 1.f, 255);
	
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	if (!is_clicked) {
		CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		DrawSprite(get_sprite(TITLE_SPRITE_ASSET, 12, 5, 2.f),
			CP_System_GetWindowWidth() * 0.5f,
			CP_System_GetWindowHeight() * 0.3f,
			CP_System_GetWindowWidth() * 0.5f,
			CP_System_GetWindowHeight() * 0.5f,
			255);
	}

	//MAIN MENU SCREEN BUTTON
	for (int i = 0; i < 3; i++) {
		//defining the position of the button
		CP_Vector coords = world_to_screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		//Drawing the button
		CP_Image_DrawAdvanced((CP_Image) buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);

		//Drawing Text
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Settings_TextSize(buttons[i].text_size * CP_System_GetWindowHeight() / world_height1);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText(buttons[i].text, coords.x, coords.y);

		//telling what the button should do
		if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5 && !is_overlayed) 
		{
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 9.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 4.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 2.0f, CP_System_GetDt() * 5);

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
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 1.3f, CP_System_GetDt() * 5);
		}

		if (is_clicked == 1) {
			buttons[i].pos.x = CP_Math_LerpFloat(buttons[i].pos.x, -15.0f, CP_System_GetDt() * 4);
		}
		else {
			buttons[i].pos.x = CP_Math_LerpFloat(buttons[i].pos.x, buttons[i].start_pos.x, CP_System_GetDt() * 4);
		}
	}

	//LEVEL SELECT BUTTONS
	for (int i = 3; i < 8; i++) {
		//defining the position of the button
		CP_Vector coords = world_to_screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		CP_Settings_ImageMode(CP_POSITION_CENTER);
		//Drawing the button
		CP_Image_DrawAdvanced((CP_Image) buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);

		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Settings_TextSize(buttons[i].text_size * CP_System_GetWindowHeight() / world_height1);
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
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 4.f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
			{
				set_game_level(i - 3);
				buttons[i].func();
			}
		}
		else {
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 3.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 5.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 3.f, CP_System_GetDt() * 5);
		}
		
	}

	if (is_setting_clicked) {
		CP_Image_Draw((CP_Image)setting_background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, CP_System_GetWindowWidth() - 500.f, CP_System_GetWindowHeight() - 300.f, 255);
	}
	else {
		CP_Image_Draw((CP_Image) setting_background, CP_System_GetWindowWidth()* 0.5f, CP_System_GetWindowHeight() * 0.5f, 0.f, 0.f, 255);
	}

	//SETTING BUTTONS
	for (int i = 9; i < 12; i++) {
		//defining the position of the button
		CP_Vector coords = world_to_screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		CP_Settings_ImageMode(CP_POSITION_CENTER);

		//Drawing the button
		CP_Image_DrawAdvanced((CP_Image) buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);

		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Settings_TextSize(buttons[i].text_size * CP_System_GetWindowHeight() / world_height1);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText(buttons[i].text, coords.x, coords.y);

		//telling what the button should do
		if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5)
		{
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 7.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 3.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 1.3f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
			{
				change_resolution(i - 9);
			}
		}
		else {
			if (is_setting_clicked) {
				buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 6.0f, CP_System_GetDt() * 5);
				buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 2.0f, CP_System_GetDt() * 5);
				buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 0.95f, CP_System_GetDt() * 5);
			}
			else {
				buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 0.0f, CP_System_GetDt() * 5);
				buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 0.0f, CP_System_GetDt() * 5);
				buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 0.0f, CP_System_GetDt() * 5);
			}
			
		}

		
	}

	//BACK BUTTON FOR LEVEL SELECT
	CP_Vector coords_back_level = world_to_screen(buttons[8].pos.x, buttons[8].pos.y, world_width1, world_height1);
	CP_Vector local_scale_level = {
		buttons[8].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[8].scale.y * CP_System_GetWindowHeight() / world_height1
	};

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced((CP_Image) buttons[8].button_image, coords_back_level.x, coords_back_level.y, local_scale_level.x, local_scale_level.y, 255, 0.f);

	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(buttons[8].text_size * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buttons[8].text, coords_back_level.x, coords_back_level.y);

	if (is_clicked == 1) {
		buttons[8].pos.x = CP_Math_LerpFloat(buttons[8].pos.x, buttons[8].ending_pos.x, CP_System_GetDt() * 4);
	}
	else {
		buttons[8].pos.x = CP_Math_LerpFloat(buttons[8].pos.x, buttons[8].start_pos.x, CP_System_GetDt() * 4);
	}

	//telling what the button should do
	if (mouse_x >= coords_back_level.x - local_scale_level.x * .5f && mouse_y >= coords_back_level.y - local_scale_level.y * .5f && mouse_x <= coords_back_level.x + local_scale_level.x * .5 && mouse_y <= coords_back_level.y + local_scale_level.y * .5)
	{
		buttons[8].scale.x = CP_Math_LerpFloat(buttons[8].scale.x, 3.0f, CP_System_GetDt() * 5);
		buttons[8].scale.y = CP_Math_LerpFloat(buttons[8].scale.y, 3.0f, CP_System_GetDt() * 5);
		buttons[8].text_size = CP_Math_LerpFloat(buttons[8].text_size, 3.f, CP_System_GetDt() * 5);
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
		{
			buttons[8].func();
		}
	}
	else {
		buttons[8].scale.x = CP_Math_LerpFloat(buttons[8].scale.x, 2.0f, CP_System_GetDt() * 5);
		buttons[8].scale.y = CP_Math_LerpFloat(buttons[8].scale.y, 2.0f, CP_System_GetDt() * 5);
		buttons[8].text_size = CP_Math_LerpFloat(buttons[8].text_size, 2.0f, CP_System_GetDt() * 5);
	}

	//BACK BUTTON FOR SETTINGS PAGE
	CP_Vector coords_back_setting = world_to_screen(buttons[12].pos.x, buttons[12].pos.y, world_width1, world_height1);
	CP_Vector local_scale_setting = {
		buttons[12].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[12].scale.y * CP_System_GetWindowHeight() / world_height1
	};

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced((CP_Image)buttons[12].button_image, coords_back_setting.x, coords_back_setting.y, local_scale_setting.x, local_scale_setting.y, 255, 0.f);

	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(buttons[12].text_size * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buttons[12].text, coords_back_setting.x, coords_back_setting.y);

	//telling what the button should do
	if (mouse_x >= coords_back_setting.x - local_scale_setting.x * .5f && mouse_y >= coords_back_setting.y - local_scale_setting.y * .5f && mouse_x <= coords_back_setting.x + local_scale_setting.x * .5 && mouse_y <= coords_back_setting.y + local_scale_setting.y * .5)
	{
		if (is_setting_clicked == 1) {
			buttons[12].scale.x = CP_Math_LerpFloat(buttons[12].scale.x, 3.0f, CP_System_GetDt() * 5);
			buttons[12].scale.y = CP_Math_LerpFloat(buttons[12].scale.y, 3.0f, CP_System_GetDt() * 5);
			buttons[12].text_size = CP_Math_LerpFloat(buttons[12].text_size, 2.0f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
			{
				setting_button();
			}
		}
	}
	if (is_setting_clicked) {
		buttons[12].scale.x = CP_Math_LerpFloat(buttons[12].scale.x, 2.0f, CP_System_GetDt() * 5);
		buttons[12].scale.y = CP_Math_LerpFloat(buttons[12].scale.y, 2.0f, CP_System_GetDt() * 5);
		buttons[12].text_size = CP_Math_LerpFloat(buttons[12].text_size, 1.3f, CP_System_GetDt() * 5);
	}
	else {
		buttons[12].scale.x = CP_Math_LerpFloat(buttons[12].scale.x, 0.0f, CP_System_GetDt() * 5);
		buttons[12].scale.y = CP_Math_LerpFloat(buttons[12].scale.y, 0.0f, CP_System_GetDt() * 5);
		buttons[12].text_size = CP_Math_LerpFloat(buttons[12].text_size, 0.0f, CP_System_GetDt() * 5);
	}
}

void play_button(void) {
	is_clicked = !is_clicked;
}

void setting_button(void) {
	is_setting_clicked = !is_setting_clicked;
	is_overlayed = !is_overlayed;
}

//changes the Main menu screen to the game start state
void change_screen(void) {
	CP_Engine_SetNextGameState(game_init, game_update, game_exit);
}

void change_resolution(num) {
	if (num == 0) {
		CP_System_SetWindowSize(1280, 720);
	}
	else if (num == 1) {
		CP_System_SetWindowSize(1600, 900);
	}
	else if (num == 2) {
		CP_System_SetWindowSize(1920, 1080);
	}
}

//frees the image when exiting the application
void main_menu_exit(void) 
{
}