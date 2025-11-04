#include "cprocessing.h"
#include "main_menu.h"
#include "game.h"
#include "utils.h"
#include "asset_manager.h"
#include "sprite_manager.h"
#include "data_manager.h"
#include "state_manager.h"
#include "particle_manager.h"
#include "level_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*!
@author Nathan Ng (n.ng@digipen.edu)
@date 8 September 2025
@course CSD1401
@brief this file contains functions and code that handles the UI
*/

//setting button
typedef struct ButtonStr {
	CP_Vector pos;
	CP_Vector ending_pos, start_pos;
	CP_Vector scale;
	CP_Image button_image;
	char* text;
	FunctionPtr func;
	float text_size;
} Button;

float world_width1 = 48, world_height1 = 27;
int window_size_H, window_size_W;
int resolution_number = 0;

Button buttons[20];
CP_Image background, setting_background;
CP_Font button_font;

bool is_clicked = false, is_setting_clicked = false, is_overlayed = false, is_shadowed = false;
float sfx_limit = 1.0f, music_limit = 1.0f;
Map* settings_map = NULL;

void exit_main_menu(void) {
	Set_Next_State(STATE_EXIT);
}

void main_menu_init(void) 
{
	Set_Current_State(STATE_MAINMENU);
	printf("Init main menu\n");
	button_font = Asset_GetFont(ARIAL_FONT_ASSET);
	window_size_H = CP_System_GetWindowHeight();
	window_size_W = CP_System_GetWindowWidth();

	CP_Font_Set(button_font);

	settings_map = Map_Load("user.data");
	if (settings_map) {
		printf("Loading previous user data!\n");
		sfx_limit = Map_GetFloat(settings_map, "sfx_volume", sfx_limit);
		CP_Sound_SetGroupVolume(CP_SOUND_GROUP_SFX, sfx_limit);
		music_limit = Map_GetFloat(settings_map, "music_volume", music_limit);
		CP_Sound_SetGroupVolume(CP_SOUND_GROUP_MUSIC, music_limit);
		is_shadowed = Map_GetInt(settings_map, "is_shadowed", is_shadowed);

	}
	else {
		printf("User data file not found! Creating new one!\n");
		settings_map = Map_AddFloat(settings_map, "sfx_volume", sfx_limit);
		settings_map = Map_AddFloat(settings_map, "music_volume", music_limit);
		settings_map = Map_AddInt(settings_map, "is_shadowed", is_shadowed);
	}

	//play button
	buttons[0].button_image = Asset_GetImage(BUTTON_YELLOW_ASSET);
	buttons[0].pos.x = world_width1 * 0.5f;
	buttons[0].pos.y = world_height1 * 0.5f;
	buttons[0].start_pos.x = buttons[0].pos.x;
	buttons[0].func = play_button;
	buttons[0].text = "PLAY";

	//setting button
	buttons[1].button_image = Asset_GetImage(BUTTON_BLUE_ASSET);
	buttons[1].pos.x = world_width1 * 0.5f;
	buttons[1].pos.y = world_height1 * 0.35f;
	buttons[1].start_pos.x = buttons[1].pos.x;
	buttons[1].func = setting_button;
	buttons[1].text = "SETTING";

	//quit button
	buttons[2].button_image = Asset_GetImage(BUTTON_GREY_ASSET);
	buttons[2].pos.x = world_width1 * 0.5f;
	buttons[2].pos.y = world_height1 * 0.2f;
	buttons[2].start_pos.x = buttons[2].pos.x;
	buttons[2].func = exit_main_menu;
	buttons[2].text = "QUIT";

	//start of level select button
	buttons[3].button_image = Asset_GetImage(LEVEL_BUTTON_ASSET);
	buttons[3].pos.x = world_width1 * 1.1f;
	buttons[3].pos.y = world_height1 * .8f;
	buttons[3].ending_pos.x = world_width1 * 0.1f;
	buttons[3].start_pos.x = buttons[3].pos.x;
	buttons[3].func = change_screen;
	buttons[3].text = "1";

	buttons[4].button_image = Asset_GetImage(LEVEL_BUTTON_ASSET);
	buttons[4].pos.x = world_width1 * 1.2f;
	buttons[4].pos.y = world_height1 * .8f;
	buttons[4].ending_pos.x = world_width1 * 0.2f;
	buttons[4].start_pos.x = buttons[4].pos.x;
	buttons[4].func = change_screen;
	buttons[4].text = "2";

	buttons[5].button_image = Asset_GetImage(LEVEL_BUTTON_ASSET);
	buttons[5].pos.x = world_width1 * 1.3f;
	buttons[5].pos.y = world_height1 * .8f;
	buttons[5].ending_pos.x = world_width1 * 0.3f;
	buttons[5].start_pos.x = buttons[5].pos.x;
	buttons[5].func = change_screen;
	buttons[5].text = "3";

	buttons[6].button_image = Asset_GetImage(LEVEL_BUTTON_ASSET);
	buttons[6].pos.x = world_width1 * 1.4f;
	buttons[6].pos.y = world_height1 * .8f;
	buttons[6].ending_pos.x = world_width1 * 0.4f;
	buttons[6].start_pos.x = buttons[6].pos.x;
	buttons[6].func = change_screen;
	buttons[6].text = "4";

	buttons[7].button_image = Asset_GetImage(LEVEL_BUTTON_ASSET);
	buttons[7].pos.x = world_width1 * 1.5f;
	buttons[7].pos.y = world_height1 * .8f;
	buttons[7].ending_pos.x = world_width1 * 0.5f;
	buttons[7].start_pos.x = buttons[7].pos.x;
	buttons[7].func = change_screen;
	buttons[7].text = "5";
	//end of level select button
	
	//level select back button
	buttons[8].button_image = Asset_GetImage(LEVEL_BUTTON_ASSET);
	buttons[8].pos.x = world_width1 * 1.5f;
	buttons[8].pos.y = world_height1 * .15f;
	buttons[8].ending_pos.x = world_width1 * 0.1f;
	buttons[8].start_pos.x = buttons[8].pos.x;
	buttons[8].func = play_button;
	buttons[8].text = "<";

	//start of resolution buttons
	buttons[9].button_image = Asset_GetImage(LEVEL_BUTTON_ASSET);
	buttons[9].pos.x = world_width1 * 0.35f;
	buttons[9].pos.y = world_height1 * 0.28f;
	buttons[9].start_pos.x = buttons[9].pos.x;
	buttons[9].text = "1280 X 720";

	buttons[10].button_image = Asset_GetImage(LEVEL_BUTTON_ASSET);
	buttons[10].pos.x = world_width1 * 0.5f;
	buttons[10].pos.y = world_height1 * 0.28f;
	buttons[10].start_pos.x = buttons[10].pos.x;
	buttons[10].text = "1600 X 900";

	buttons[11].button_image = Asset_GetImage(LEVEL_BUTTON_ASSET);
	buttons[11].pos.x = world_width1 * 0.65f;
	buttons[11].pos.y = world_height1 * 0.28f;
	buttons[11].start_pos.x = buttons[11].pos.x;
	buttons[11].text = "1920 X 1080";
	//end of resolution button

	//settings back button
	buttons[12].button_image = Asset_GetImage(BUTTON_GREY_ASSET);
	buttons[12].pos.x = world_width1 * 0.25f;
	buttons[12].pos.y = world_height1 * 0.7f;
	buttons[12].start_pos.x = buttons[12].pos.x;
	buttons[12].text = "X";

	buttons[13].button_image = is_shadowed ? Asset_GetImage(LEVEL_BUTTON_ASSET) : Asset_GetImage(BUTTON_GREY_ASSET);
	buttons[13].pos.x = world_width1 * 0.75f;
	buttons[13].pos.y = world_height1 * 0.7f;
	buttons[13].start_pos.x = buttons[13].pos.x;
	buttons[13].text = "High Details";
	buttons[13].func = is_shadow;

	background = Asset_GetImage(GAME_BACKGROUND_ASSET);
	setting_background = Asset_GetImage(SETTING_BACKGROUND_ASSET);

	//setting the button anchor to the center of the world
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_System_SetWindowSize(window_size_W, window_size_H);

	CP_Sound_StopAll();
	CP_Sound_PlayMusic(Asset_GetAudio(BACKGROUND_MUSIC_ASSET));
}

void main_menu_update(void) 
{
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_Scale(1.f, 1.f);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	CP_Image_Draw(background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, CP_System_GetWindowWidth() * 1.f, CP_System_GetWindowHeight() * 1.f, 255);
	
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	main_menu_button(mouse_x, mouse_y);

	if (!is_clicked) {
		CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		Sprite_Draw(Asset_GetSprite(TITLE_SPRITE_ASSET, 12, 5, 2.f),
			CP_System_GetWindowWidth() * 0.5f,
			CP_System_GetWindowHeight() * 0.3f,
			CP_System_GetWindowWidth() * 0.5f,
			CP_System_GetWindowHeight() * 0.5f,
			255);
	}

	level_select_button(mouse_x, mouse_y);

	//SETTING PAGE
	if (is_setting_clicked) {
		//drawing setting background
		CP_Image_Draw(setting_background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, CP_System_GetWindowWidth() - 500.f, CP_System_GetWindowHeight() - 300.f, 255);

		volume_bar(mouse_x, mouse_y);
	}

	settings_button(mouse_x, mouse_y);
	back_buttons(mouse_x, mouse_y);
}

void main_menu_button(float mouse_x, float mouse_y) {
	//MAIN MENU SCREEN BUTTON
	for (int i = 0; i < 3; i++) {
		//defining the position of the button
		CP_Vector coords = World_To_Screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);

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
}

void level_select_button(float mouse_x, float mouse_y) {
	//LEVEL SELECT BUTTONS
	for (int i = 3; i < 3 + TOTAL_LEVELS; i++) {
		//defining the position of the button
		CP_Vector coords = World_To_Screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		CP_Settings_ImageMode(CP_POSITION_CENTER);
		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);

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
				Game_SetLevel(i - 3 + 1);
				buttons[i].func();
			}
		}
		else {
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 3.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 5.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 3.f, CP_System_GetDt() * 5);
		}

	}
}

void volume_bar(float mouse_x, float mouse_y) {
	CP_Color sound_bar = Get_Color(COLOR_WHITE);
	CP_Color sound_bar_background = CP_Color_Create(150, 150, 150, 255);

	CP_Settings_RectMode(CP_POSITION_CORNER);

	//START DRAWING OF VOLUME BARS
	//drawing of SFX volume button
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Settings_Stroke(Get_Color(COLOR_BLACK));
	CP_Settings_TextSize(1.5f * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("SFX Volume", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.35f);

	//drawing of Music volume button
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Settings_TextSize(1.5f * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("Music Volume", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f);

	//drawing of SFX volume bar
	CP_Settings_Fill(sound_bar_background);
	CP_Settings_Stroke(Get_Color(COLOR_BLACK));
	CP_Graphics_DrawRect(CP_System_GetWindowWidth() * 0.22f, CP_System_GetWindowHeight() * 0.4f, 27.0f * CP_System_GetWindowWidth() / world_width1, 1.3f * CP_System_GetWindowHeight() / world_height1);
	//drawing of SFX volume bar background
	CP_Settings_Fill(sound_bar);
	CP_Graphics_DrawRect(CP_System_GetWindowWidth() * 0.22f, CP_System_GetWindowHeight() * 0.4f, (sfx_limit * 27.0f) * CP_System_GetWindowWidth() / world_width1, 1.3f * CP_System_GetWindowHeight() / world_height1);

	//drawing of Music volume bar
	CP_Settings_Fill(sound_bar_background);
	CP_Graphics_DrawRect(CP_System_GetWindowWidth() * 0.22f, CP_System_GetWindowHeight() * 0.55f, 27.0f * CP_System_GetWindowWidth() / world_width1, 1.3f * CP_System_GetWindowHeight() / world_height1);
	//drawing of Music volume bar background
	CP_Settings_Fill(sound_bar);
	CP_Graphics_DrawRect(CP_System_GetWindowWidth() * 0.22f, CP_System_GetWindowHeight() * 0.55f, (music_limit * 27.0f) * CP_System_GetWindowWidth() / world_width1, 1.3f * CP_System_GetWindowHeight() / world_height1);

	//check if mouse is within the SFX volume bar and if the left mouse is held down
	if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
		if (mouse_x >= 0.22f * CP_System_GetWindowWidth() && mouse_x <= 0.22f * CP_System_GetWindowWidth() + (27.0f * CP_System_GetWindowWidth() / world_width1)
			&& mouse_y >= 0.4f * CP_System_GetWindowHeight() && mouse_y <= 0.4f * CP_System_GetWindowHeight() + 1.3f * CP_System_GetWindowHeight() / world_height1) {

			sfx_limit = (mouse_x - CP_System_GetWindowWidth() * 0.22f) / (27.0f * CP_System_GetWindowWidth() / world_width1);
			CP_Sound_SetGroupVolume(CP_SOUND_GROUP_SFX, sfx_limit);
		}
	}

	//check if mouse is within the Music volume bar and if the left mouse is held down
	if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
		if (mouse_x >= 0.22f * CP_System_GetWindowWidth() && mouse_x <= 0.22f * CP_System_GetWindowWidth() + (27.0f * CP_System_GetWindowWidth() / world_width1)
			&& mouse_y >= 0.55f * CP_System_GetWindowHeight() && mouse_y <= 0.55f * CP_System_GetWindowHeight() + 1.3f * CP_System_GetWindowHeight() / world_height1) {

			music_limit = (mouse_x - CP_System_GetWindowWidth() * 0.22f) / (27.0f * CP_System_GetWindowWidth() / world_width1);
			CP_Sound_SetGroupVolume(CP_SOUND_GROUP_MUSIC, music_limit);
		}
	}

	//drawing of volume percentage base on the mouse position
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(1.0f * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	char p[50]; sprintf_s(p, 50, "%.1f", sfx_limit * 100);
	CP_Font_DrawText(p, CP_System_GetWindowWidth() * 0.25f, CP_System_GetWindowHeight() * 0.425f);

	//drawing of volume percentage base on the mouse position
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(1.0f * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	char o[50]; sprintf_s(o, 50, "%.1f", music_limit * 100);
	CP_Font_DrawText(o, CP_System_GetWindowWidth() * 0.25f, CP_System_GetWindowHeight() * 0.575f);
	//ENDING DRAWING OF VOLUME BARS
}

void settings_button(float mouse_x, float mouse_y) {
	//RESOLUTION BUTTONS
	for (int i = 9; i < 12; i++) {
		//defining the position of the button
		CP_Vector coords = World_To_Screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		CP_Settings_ImageMode(CP_POSITION_CENTER);

		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);

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

	//SHADOW BUTTON
	CP_Vector coords = World_To_Screen(buttons[13].pos.x, buttons[13].pos.y, world_width1, world_height1);
	CP_Vector local_scale = {
		buttons[13].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[13].scale.y * CP_System_GetWindowHeight() / world_height1
	};

	CP_Settings_ImageMode(CP_POSITION_CENTER);

	//Drawing the button
	CP_Image_DrawAdvanced(buttons[13].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);

	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(buttons[13].text_size * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buttons[13].text, coords.x, coords.y);

	//telling what the button should do
	if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5)
	{
		buttons[13].scale.x = CP_Math_LerpFloat(buttons[13].scale.x, 7.0f, CP_System_GetDt() * 5);
		buttons[13].scale.y = CP_Math_LerpFloat(buttons[13].scale.y, 3.0f, CP_System_GetDt() * 5);
		buttons[13].text_size = CP_Math_LerpFloat(buttons[13].text_size, 1.3f, CP_System_GetDt() * 5);
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
		{
			buttons[13].func();
		}
	}
	else {
		if (is_setting_clicked) {
			buttons[13].scale.x = CP_Math_LerpFloat(buttons[13].scale.x, 6.0f, CP_System_GetDt() * 5);
			buttons[13].scale.y = CP_Math_LerpFloat(buttons[13].scale.y, 2.0f, CP_System_GetDt() * 5);
			buttons[13].text_size = CP_Math_LerpFloat(buttons[13].text_size, 0.95f, CP_System_GetDt() * 5);
		}
		else {
			buttons[13].scale.x = CP_Math_LerpFloat(buttons[13].scale.x, 0.0f, CP_System_GetDt() * 5);
			buttons[13].scale.y = CP_Math_LerpFloat(buttons[13].scale.y, 0.0f, CP_System_GetDt() * 5);
			buttons[13].text_size = CP_Math_LerpFloat(buttons[13].text_size, 0.0f, CP_System_GetDt() * 5);
		}

	}
}

void back_buttons(float mouse_x, float mouse_y) {
	//BACK BUTTON FOR LEVEL SELECT
	CP_Vector coords_back_level = World_To_Screen(buttons[8].pos.x, buttons[8].pos.y, world_width1, world_height1);
	CP_Vector local_scale_level = {
		buttons[8].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[8].scale.y * CP_System_GetWindowHeight() / world_height1
	};

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced(buttons[8].button_image, coords_back_level.x, coords_back_level.y, local_scale_level.x, local_scale_level.y, 255, 0.f);

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
	CP_Vector coords_back_setting = World_To_Screen(buttons[12].pos.x, buttons[12].pos.y, world_width1, world_height1);
	CP_Vector local_scale_setting = {
		buttons[12].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[12].scale.y * CP_System_GetWindowHeight() / world_height1
	};

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced(buttons[12].button_image, coords_back_setting.x, coords_back_setting.y, local_scale_setting.x, local_scale_setting.y, 255, 0.f);


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
	Set_Next_State(STATE_GAME);
}

void change_resolution(num) {
	switch (num) {
	case 0:
		CP_System_SetWindowSize(1280, 720);
		break;
	case 1:
		CP_System_SetWindowSize(1600, 900);
		break;
	case 2:
		CP_System_SetWindowSize(1920, 1080);
		break;
	}
}

void is_shadow(void) {
	is_shadowed = !is_shadowed;
	buttons[13].button_image = is_shadowed ? Asset_GetImage(LEVEL_BUTTON_ASSET): Asset_GetImage(BUTTON_GREY_ASSET);
}

bool get_shadow_setting(void) {
	return is_shadowed;
}

//frees the image when exiting the application
void main_menu_exit(void) 
{
	if (Get_Next_State() == STATE_MAINMENU) {
		Assets_Free();
		Particles_Free();
	}
	if (settings_map) {
		printf("Saving user data!\n");
		Map_EditFloat(settings_map, "sfx_volume", sfx_limit);
		Map_EditFloat(settings_map, "music_volume", music_limit);
		Map_EditFloat(settings_map, "is_shadowed", is_shadowed);

		Map_Save("user.data", settings_map);
		Map_Free(settings_map);
		settings_map = NULL;
	}
	printf("Exit main menu\n");
}