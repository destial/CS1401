#include "cprocessing.h"
#include "main_menu.h"
#include "game.h"
#include "utils.h"
#include "asset_manager.h"
#include "sprite_manager.h"
#include "game_settings.h"
#include "state_manager.h"
#include "particle_manager.h"
#include "level_manager.h"
#include "currency_manager.h"
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
float logo_x, logo_y, logo_start_x, level_select_start, level_select_end, level_select_x, level_select_y;
int window_size_H, window_size_W;
int resolution_number = 0, equipped_index;

Button buttons[30];
Button level_buttons[TOTAL_LEVELS];
CP_Image background, setting_background, shop_background, golf_ball_default_image, golf_ball_gold_image, shop_banner;
CP_Font button_font;

bool is_clicked = false, is_setting_clicked = false, is_overlayed = false, is_shop_clicked = false, is_buy1_clicked = false, is_buy2_clicked = false, is_shadowed = false, is_equip_clicked;
float sfx_limit = 1.0f, music_limit = 1.0f;

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
	sfx_limit = GameSetting_GetFloat("sfx_volume");
	music_limit = GameSetting_GetFloat("music_volume");
	is_shadowed = GameSetting_GetBool("high_quality");
	int c = GameSetting_GetInt("coins");
	if (c >= 0) set_coins(c);

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
	
	//level select back button
	buttons[3].button_image = Asset_GetImage(SMALL_BUTTON_GREY_ASSET);
	buttons[3].pos.x = world_width1 * 1.5f;
	buttons[3].pos.y = world_height1 * .9f;
	buttons[3].ending_pos.x = world_width1 * 0.1f;
	buttons[3].start_pos.x = buttons[3].pos.x;
	buttons[3].func = play_button;
	buttons[3].text = "<";

	//start of resolution buttons
	buttons[4].button_image = Asset_GetImage(BUTTON_GREEN_ASSET);
	buttons[4].pos.x = world_width1 * 0.35f;
	buttons[4].pos.y = world_height1 * 0.28f;
	buttons[4].start_pos.x = buttons[4].pos.x;
	buttons[4].text = "1280 X 720";

	buttons[5].button_image = Asset_GetImage(BUTTON_GREEN_ASSET);
	buttons[5].pos.x = world_width1 * 0.5f;
	buttons[5].pos.y = world_height1 * 0.28f;
	buttons[5].start_pos.x = buttons[5].pos.x;
	buttons[5].text = "1600 X 900";

	buttons[6].button_image = Asset_GetImage(BUTTON_GREEN_ASSET);
	buttons[6].pos.x = world_width1 * 0.65f;
	buttons[6].pos.y = world_height1 * 0.28f;
	buttons[6].start_pos.x = buttons[6].pos.x;
	buttons[6].text = "1920 X 1080";
	//end of resolution button

	//settings back button
	buttons[7].button_image = Asset_GetImage(SMALL_BUTTON_GREY_ASSET);
	buttons[7].pos.x = world_width1 * 0.25f;
	buttons[7].pos.y = world_height1 * 0.7f;
	buttons[7].start_pos.x = buttons[7].pos.x;
	buttons[7].text = "X";

	buttons[8].button_image = is_shadowed ? Asset_GetImage(BUTTON_GREEN_ASSET) : Asset_GetImage(BUTTON_GREY_ASSET);
	buttons[8].pos.x = world_width1 * 0.75f;
	buttons[8].pos.y = world_height1 * 0.7f;
	buttons[8].start_pos.x = buttons[8].pos.x;
	buttons[8].text = "High Details";
	buttons[8].func = is_shadow;

	//Shop Button
	buttons[9].button_image = Asset_GetImage(BUTTON_BLUE_ASSET);
	buttons[9].pos.x = world_width1 * 0.7f;
	buttons[9].pos.y = world_height1 * 0.35f;
	buttons[9].start_pos.x = buttons[9].pos.x;
	buttons[9].func = shop_button;
	buttons[9].text = "SHOP";

	//Shop Back Button
	buttons[10].button_image = Asset_GetImage(SMALL_BUTTON_GREY_ASSET);
	buttons[10].pos.x = world_width1 * 0.25f;
	buttons[10].pos.y = world_height1 * 0.7f;
	buttons[10].start_pos.x = buttons[10].pos.x;
	buttons[10].text = "X";

	//Shop Buy Button
	buttons[11].button_image = is_equip_clicked ? Asset_GetImage(BUTTON_GREY_ASSET) : Asset_GetImage(BUTTON_YELLOW_ASSET);
	buttons[11].pos.x = world_width1 * 0.3f;
	buttons[11].pos.y = world_height1 * 0.32f;
	buttons[11].start_pos.x = buttons[11].pos.x;
	buttons[11].text = is_equip_clicked ? "EQUIPPED" : "EQUIP";

	buttons[12].button_image = is_equip_clicked ? Asset_GetImage(BUTTON_GREY_ASSET) : is_buy1_clicked ? Asset_GetImage(BUTTON_YELLOW_ASSET) : Asset_GetImage(BUTTON_GREEN_ASSET);
	buttons[12].pos.x = world_width1 * 0.5f;
	buttons[12].pos.y = world_height1 * 0.32f;
	buttons[12].start_pos.x = buttons[12].pos.x;
	buttons[12].text = is_equip_clicked ? "EQUIPPED" : is_buy1_clicked ? "EQUIP" : "BUY: 10 Coins";

	buttons[13].button_image = is_equip_clicked ? Asset_GetImage(BUTTON_GREY_ASSET) : is_buy2_clicked ? Asset_GetImage(BUTTON_YELLOW_ASSET) : Asset_GetImage(BUTTON_GREEN_ASSET);
	buttons[13].pos.x = world_width1 * 0.7f;
	buttons[13].pos.y = world_height1 * 0.32f;
	buttons[13].start_pos.x = buttons[13].pos.x;
	buttons[13].text = is_equip_clicked ? "EQUIPPED" : is_buy2_clicked ? "EQUIP" : "BUY: 20 Coins";

	//start of level select button
	level_buttons[0].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[0].pos.x = world_width1 * 1.1f;
	level_buttons[0].pos.y = world_height1 * .7f;
	level_buttons[0].ending_pos.x = world_width1 * 0.1f;
	level_buttons[0].start_pos.x = level_buttons[0].pos.x;
	level_buttons[0].func = change_screen;
	level_buttons[0].text = "1";

	level_buttons[1].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[1].pos.x = world_width1 * 1.2f;
	level_buttons[1].pos.y = world_height1 * .7f;
	level_buttons[1].ending_pos.x = world_width1 * 0.2f;
	level_buttons[1].start_pos.x = level_buttons[1].pos.x;
	level_buttons[1].func = change_screen;
	level_buttons[1].text = "2";

	level_buttons[2].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[2].pos.x = world_width1 * 1.3f;
	level_buttons[2].pos.y = world_height1 * .7f;
	level_buttons[2].ending_pos.x = world_width1 * 0.3f;
	level_buttons[2].start_pos.x = level_buttons[2].pos.x;
	level_buttons[2].func = change_screen;
	level_buttons[2].text = "3";

	level_buttons[3].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[3].pos.x = world_width1 * 1.4f;
	level_buttons[3].pos.y = world_height1 * .7f;
	level_buttons[3].ending_pos.x = world_width1 * 0.4f;
	level_buttons[3].start_pos.x = level_buttons[3].pos.x;
	level_buttons[3].func = change_screen;
	level_buttons[3].text = "4";

	level_buttons[4].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[4].pos.x = world_width1 * 1.5f;
	level_buttons[4].pos.y = world_height1 * .7f;
	level_buttons[4].ending_pos.x = world_width1 * 0.5f;
	level_buttons[4].start_pos.x = level_buttons[4].pos.x;
	level_buttons[4].func = change_screen;
	level_buttons[4].text = "5";

	level_buttons[5].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[5].pos.x = world_width1 * 1.6f;
	level_buttons[5].pos.y = world_height1 * .7f;
	level_buttons[5].ending_pos.x = world_width1 * 0.6f;
	level_buttons[5].start_pos.x = level_buttons[5].pos.x;
	level_buttons[5].func = change_screen;
	level_buttons[5].text = "6";

	level_buttons[6].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[6].pos.x = world_width1 * 1.7f;
	level_buttons[6].pos.y = world_height1 * .7f;
	level_buttons[6].ending_pos.x = world_width1 * 0.7f;
	level_buttons[6].start_pos.x = level_buttons[6].pos.x;
	level_buttons[6].func = change_screen;
	level_buttons[6].text = "7";

	level_buttons[7].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[7].pos.x = world_width1 * 1.8f;
	level_buttons[7].pos.y = world_height1 * .7f;
	level_buttons[7].ending_pos.x = world_width1 * 0.8f;
	level_buttons[7].start_pos.x = level_buttons[7].pos.x;
	level_buttons[7].func = change_screen;
	level_buttons[7].text = "8";

	level_buttons[8].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[8].pos.x = world_width1 * 1.9f;
	level_buttons[8].pos.y = world_height1 * .7f;
	level_buttons[8].ending_pos.x = world_width1 * 0.9f;
	level_buttons[8].start_pos.x = level_buttons[8].pos.x;
	level_buttons[8].func = change_screen;
	level_buttons[8].text = "9";

	level_buttons[9].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	level_buttons[9].pos.x = world_width1 * 1.1f;
	level_buttons[9].pos.y = world_height1 * .5f;
	level_buttons[9].ending_pos.x = world_width1 * 0.1f;
	level_buttons[9].start_pos.x = level_buttons[9].pos.x;
	level_buttons[9].func = change_screen;
	level_buttons[9].text = "10";
	//end of level select button

	background = Asset_GetImage(GAME_BACKGROUND_ASSET);
	setting_background = Asset_GetImage(SETTING_BACKGROUND_ASSET);
	shop_background = Asset_GetImage(SETTING_BACKGROUND_ASSET);
	golf_ball_default_image = Asset_GetImage(GOLF_BALL_ASSET);
	golf_ball_gold_image = Asset_GetImage(GOLF_BALL_GOLD_ASSET);
	background = Asset_GetImage(GAME_BACKGROUND_ASSET);
	setting_background = Asset_GetImage(SETTING_BACKGROUND_ASSET);

	//setting the button anchor to the center of the world
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);
	CP_System_SetWindowSize(window_size_W, window_size_H);

	//title logo screen position
	logo_start_x = CP_System_GetWindowWidth() * 0.5f;
	logo_x = CP_System_GetWindowWidth() * 0.5f;
	logo_y = CP_System_GetWindowHeight() * 0.3f;

	level_select_start = CP_System_GetWindowWidth() * 1.5f;
	level_select_end = CP_System_GetWindowWidth() * 0.55f;
	level_select_x = level_select_start;
	level_select_y = CP_System_GetWindowHeight() * 0.1f;

	CP_Sound_StopAll();
	CP_Sound_PlayMusic(Asset_GetAudio(BACKGROUND_MUSIC_ASSET));
}

void main_menu_update(void) 
{
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_Scale(1.f, 1.f);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	CP_Image_Draw(background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, CP_System_GetWindowWidth() * 1.f, CP_System_GetWindowHeight() * 1.f, 255);
	Asset_SpriteStartFrame();

	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	main_menu_button(mouse_x, mouse_y);
	draw_logo_image();
	level_select_button(mouse_x, mouse_y);

	int counter_textsize = 40;
	CP_Settings_TextSize(counter_textsize);
	char coin_count[50] = { 0 };
	sprintf_s(coin_count, 50, "COINS: %i", coins);
	CP_Font_DrawText(coin_count, CP_System_GetWindowWidth() * 0.9f, CP_System_GetWindowHeight() * 0.9f);

	//SETTING PAGE
	if (is_setting_clicked) {
		//drawing setting background
		CP_Image_Draw(setting_background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, CP_System_GetWindowWidth() - 500.f, CP_System_GetWindowHeight() - 300.f, 255);

		volume_bar(mouse_x, mouse_y);
	}

	//SHOP PAGE
	if (is_shop_clicked) {
		//drawing shop background
		CP_Image_Draw(shop_background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, CP_System_GetWindowWidth() - 500.f, CP_System_GetWindowHeight() - 300.f, 255);
		CP_Image shop_banner = Asset_GetImage(BANNER);
		CP_Image_DrawAdvanced(shop_banner, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.2f, 700.0f, 200.0f, 255, 0.0f);
		//shop items would go here
		CP_Settings_TextSize(1.5f * CP_System_GetWindowHeight() / world_height1);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("GOLF SHOP", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.2f);
		CP_Image_Draw(golf_ball_default_image, CP_System_GetWindowWidth() * 0.3f, CP_System_GetWindowHeight() * 0.5f,
			CP_System_GetWindowWidth() / 10.f,
			CP_System_GetWindowWidth() / 10.f,
			255);
		CP_Image_Draw(golf_ball_gold_image, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f,
			CP_System_GetWindowWidth() / 10.f,
			CP_System_GetWindowWidth() / 10.f,
			255);
		
	}

	settings_button(mouse_x, mouse_y);
	shops_button(mouse_x, mouse_y);
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

	//SHOP BUTTON
	CP_Vector coords = World_To_Screen(buttons[9].pos.x, buttons[9].pos.y, world_width1, world_height1);
	CP_Vector local_scale = {
		buttons[9].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[9].scale.y * CP_System_GetWindowHeight() / world_height1
	};
	CP_Image_DrawAdvanced(buttons[9].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);

	//Drawing Text
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(buttons[9].text_size * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buttons[9].text, coords.x, coords.y);
	

	if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5 && !is_overlayed)
	{
		buttons[9].scale.x = CP_Math_LerpFloat(buttons[9].scale.x, 9.0f, CP_System_GetDt() * 5);
		buttons[9].scale.y = CP_Math_LerpFloat(buttons[9].scale.y, 4.0f, CP_System_GetDt() * 5);
		buttons[9].text_size = CP_Math_LerpFloat(buttons[9].text_size, 2.0f, CP_System_GetDt() * 5);

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
		{
			buttons[9].func();
		}
	}
	else {
		buttons[9].scale.x = CP_Math_LerpFloat(buttons[9].scale.x, 7.0f, CP_System_GetDt() * 5);
		buttons[9].scale.y = CP_Math_LerpFloat(buttons[9].scale.y, 3.0f, CP_System_GetDt() * 5);
		buttons[9].text_size = CP_Math_LerpFloat(buttons[9].text_size, 1.3f, CP_System_GetDt() * 5);
	}

	if (is_clicked == 1) {
		buttons[9].pos.x = CP_Math_LerpFloat(buttons[9].pos.x, -15.0f, CP_System_GetDt() * 4);
	}
	else {
		buttons[9].pos.x = CP_Math_LerpFloat(buttons[9].pos.x, buttons[9].start_pos.x, CP_System_GetDt() * 4);
	}  
}

void level_select_button(float mouse_x, float mouse_y) {

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Image_DrawAdvanced(Asset_GetImage(HANGING_BANNER), level_select_x, level_select_y, 0.3 * CP_System_GetWindowWidth(), 0.15 * CP_System_GetWindowHeight(), 255, 0.f);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(2 * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("Level Select", level_select_x, level_select_y);
	

	//LEVEL SELECT BUTTONS
	for (int i = 0; i < TOTAL_LEVELS; i++) {
		//defining the position of the button
		CP_Vector coords = World_To_Screen(level_buttons[i].pos.x, level_buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			level_buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			level_buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		CP_Settings_ImageMode(CP_POSITION_CENTER);
		//Drawing the button
		CP_Image_DrawAdvanced(level_buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);

		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Settings_TextSize(level_buttons[i].text_size * CP_System_GetWindowHeight() / world_height1);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText(level_buttons[i].text, coords.x, coords.y);


		if (is_clicked == 1) {
			level_buttons[i].pos.x = CP_Math_LerpFloat(level_buttons[i].pos.x, level_buttons[i].ending_pos.x, CP_System_GetDt() * 4);
			level_select_x = CP_Math_LerpFloat(level_select_x, level_select_end, CP_System_GetDt());
		}
		else {
			level_buttons[i].pos.x = CP_Math_LerpFloat(level_buttons[i].pos.x, level_buttons[i].start_pos.x, CP_System_GetDt() * 4);
			level_select_x = CP_Math_LerpFloat(level_select_x, level_select_start, CP_System_GetDt());
		}

		//telling what the button should do
		if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5)
		{
			level_buttons[i].scale.x = CP_Math_LerpFloat(level_buttons[i].scale.x, 4.0f, CP_System_GetDt() * 5);
			level_buttons[i].scale.y = CP_Math_LerpFloat(level_buttons[i].scale.y, 4.0f, CP_System_GetDt() * 5);
			level_buttons[i].text_size = CP_Math_LerpFloat(level_buttons[i].text_size, 3.f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
			{
				Game_SetLevel(i + 1);
				level_buttons[i].func();
			}
		}
		else {
			level_buttons[i].scale.x = CP_Math_LerpFloat(level_buttons[i].scale.x, 3.0f, CP_System_GetDt() * 5);
			level_buttons[i].scale.y = CP_Math_LerpFloat(level_buttons[i].scale.y, 3.0f, CP_System_GetDt() * 5);
			level_buttons[i].text_size = CP_Math_LerpFloat(level_buttons[i].text_size, 2.f, CP_System_GetDt() * 5);
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
	for (int i = 4; i < 7; i++) {
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
				change_resolution(i - 4);
			}
		}
		else {
			if (is_setting_clicked) {
				buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 6.0f, CP_System_GetDt() * 5);
				buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 2.0f, CP_System_GetDt() * 5);
				buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 0.95f, CP_System_GetDt() * 5);
			}
			else {
				buttons[i].scale.x = 0;
				buttons[i].scale.y = 0;
				buttons[i].text_size = 0;
			}

		}
	}

	//SHADOW BUTTON
	CP_Vector coords = World_To_Screen(buttons[8].pos.x, buttons[8].pos.y, world_width1, world_height1);
	CP_Vector local_scale = {
		buttons[8].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[8].scale.y * CP_System_GetWindowHeight() / world_height1
	};

	CP_Settings_ImageMode(CP_POSITION_CENTER);

	//Drawing the button
	CP_Image_DrawAdvanced(buttons[8].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);

	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(buttons[8].text_size * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buttons[8].text, coords.x, coords.y);

	//telling what the button should do
	if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5)
	{
		buttons[8].scale.x = CP_Math_LerpFloat(buttons[8].scale.x, 7.0f, CP_System_GetDt() * 5);
		buttons[8].scale.y = CP_Math_LerpFloat(buttons[8].scale.y, 3.0f, CP_System_GetDt() * 5);
		buttons[8].text_size = CP_Math_LerpFloat(buttons[8].text_size, 1.3f, CP_System_GetDt() * 5);
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
		{
			buttons[8].func();
		}
	}
	else {
		if (is_setting_clicked) {
			buttons[8].scale.x = CP_Math_LerpFloat(buttons[8].scale.x, 6.0f, CP_System_GetDt() * 5);
			buttons[8].scale.y = CP_Math_LerpFloat(buttons[8].scale.y, 2.0f, CP_System_GetDt() * 5);
			buttons[8].text_size = CP_Math_LerpFloat(buttons[8].text_size, 0.95f, CP_System_GetDt() * 5);
		}
		else {
			buttons[8].scale.x = 0;
			buttons[8].scale.y = 0;
			buttons[8].text_size = 0;
		}

	}
}

void shops_button(float mouse_x, float mouse_y)
{
	for (int i = 11; i < 14; i++) {
		//defining the position of the button
		CP_Vector coords = World_To_Screen(buttons[i].pos.x, buttons[i].pos.y, world_width1, world_height1);
		CP_Vector local_scale = {
			buttons[i].scale.x * CP_System_GetWindowWidth() / world_width1,
			buttons[i].scale.y * CP_System_GetWindowHeight() / world_height1
		};

		CP_Settings_ImageMode(CP_POSITION_CENTER);

		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);

		//telling what the button should do
		if (mouse_x >= coords.x - local_scale.x * .5f && mouse_y >= coords.y - local_scale.y * .5f && mouse_x <= coords.x + local_scale.x * .5 && mouse_y <= coords.y + local_scale.y * .5)
		{
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 7.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 3.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 1.3f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
			{
				if (i == 11) {
					equip_button(11);
				}
				
				else {
					buy_button(i);
				}
			}
		}
		else {
			if (is_shop_clicked) {
				buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 6.0f, CP_System_GetDt() * 5);
				buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 2.0f, CP_System_GetDt() * 5);
				buttons[16].text_size = CP_Math_LerpFloat(buttons[i].text_size, 0.95f, CP_System_GetDt() * 5);
			}
			else {
				buttons[i].scale.x = 0;
				buttons[i].scale.y = 0;
				buttons[i].text_size = 0;
			}

		}

		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Settings_TextSize(buttons[i].text_size * CP_System_GetWindowHeight() / world_height1);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText(buttons[i].text, coords.x, coords.y);
	}
}

void back_buttons(float mouse_x, float mouse_y) {
	//BACK BUTTON FOR LEVEL SELECT
	CP_Vector coords_back_level = World_To_Screen(buttons[3].pos.x, buttons[3].pos.y, world_width1, world_height1);
	CP_Vector local_scale_level = {
		buttons[3].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[3].scale.y * CP_System_GetWindowHeight() / world_height1
	};

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced(buttons[3].button_image, coords_back_level.x, coords_back_level.y, local_scale_level.x, local_scale_level.y, 255, 0.f);

	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(buttons[3].text_size * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buttons[3].text, coords_back_level.x, coords_back_level.y);

	if (is_clicked == 1) {
		buttons[3].pos.x = CP_Math_LerpFloat(buttons[3].pos.x, buttons[3].ending_pos.x, CP_System_GetDt() * 4);
	}
	else {
		buttons[3].pos.x = CP_Math_LerpFloat(buttons[3].pos.x, buttons[3].start_pos.x, CP_System_GetDt() * 4);
	}

	//telling what the button should do
	if (mouse_x >= coords_back_level.x - local_scale_level.x * .5f && mouse_y >= coords_back_level.y - local_scale_level.y * .5f && mouse_x <= coords_back_level.x + local_scale_level.x * .5 && mouse_y <= coords_back_level.y + local_scale_level.y * .5)
	{
		buttons[3].scale.x = CP_Math_LerpFloat(buttons[3].scale.x, 3.0f, CP_System_GetDt() * 5);
		buttons[3].scale.y = CP_Math_LerpFloat(buttons[3].scale.y, 3.0f, CP_System_GetDt() * 5);
		buttons[3].text_size = CP_Math_LerpFloat(buttons[3].text_size, 3.f, CP_System_GetDt() * 5);
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
		{
			buttons[3].func();
		}
	}
	else {
		buttons[3].scale.x = CP_Math_LerpFloat(buttons[3].scale.x, 2.0f, CP_System_GetDt() * 5);
		buttons[3].scale.y = CP_Math_LerpFloat(buttons[3].scale.y, 2.0f, CP_System_GetDt() * 5);
		buttons[3].text_size = CP_Math_LerpFloat(buttons[3].text_size, 2.0f, CP_System_GetDt() * 5);
	}

	//BACK BUTTON FOR SETTINGS PAGE
	CP_Vector coords_back_setting = World_To_Screen(buttons[7].pos.x, buttons[7].pos.y, world_width1, world_height1);
	CP_Vector local_scale_setting = {
		buttons[7].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[7].scale.y * CP_System_GetWindowHeight() / world_height1
	};

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced(buttons[7].button_image, coords_back_setting.x, coords_back_setting.y, local_scale_setting.x, local_scale_setting.y, 255, 0.f);


	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(buttons[7].text_size * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buttons[7].text, coords_back_setting.x, coords_back_setting.y);

	//telling what the button should do
	if (mouse_x >= coords_back_setting.x - local_scale_setting.x * .5f && mouse_y >= coords_back_setting.y - local_scale_setting.y * .5f && mouse_x <= coords_back_setting.x + local_scale_setting.x * .5 && mouse_y <= coords_back_setting.y + local_scale_setting.y * .5)
	{
		if (is_setting_clicked == 1) {
			buttons[7].scale.x = CP_Math_LerpFloat(buttons[7].scale.x, 3.0f, CP_System_GetDt() * 5);
			buttons[7].scale.y = CP_Math_LerpFloat(buttons[7].scale.y, 3.0f, CP_System_GetDt() * 5);
			buttons[7].text_size = CP_Math_LerpFloat(buttons[7].text_size, 2.0f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
			{
				setting_button();
			}
		}
	}
	if (is_setting_clicked) {
		buttons[7].scale.x = CP_Math_LerpFloat(buttons[7].scale.x, 2.0f, CP_System_GetDt() * 5);
		buttons[7].scale.y = CP_Math_LerpFloat(buttons[7].scale.y, 2.0f, CP_System_GetDt() * 5);
		buttons[7].text_size = CP_Math_LerpFloat(buttons[7].text_size, 1.3f, CP_System_GetDt() * 5);
	}
	else {
		buttons[7].scale.x = 0;
		buttons[7].scale.y = 0;
		buttons[7].text_size = 0;
	}

	//BACK BUTTON FOR SHOP PAGE
	CP_Vector coords_back_shop = World_To_Screen(buttons[10].pos.x, buttons[10].pos.y, world_width1, world_height1);
	CP_Vector local_scale_shop = {
		buttons[10].scale.x * CP_System_GetWindowWidth() / world_width1,
		buttons[10].scale.y * CP_System_GetWindowHeight() / world_height1
	};

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced(buttons[10].button_image, coords_back_shop.x, coords_back_shop.y, local_scale_shop.x, local_scale_shop.y, 255, 0.f);


	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(buttons[10].text_size * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buttons[10].text, coords_back_shop.x, coords_back_shop.y);

	//telling what the button should do
	if (mouse_x >= coords_back_shop.x - local_scale_shop.x * .5f && mouse_y >= coords_back_shop.y - local_scale_shop.y * .5f && mouse_x <= coords_back_shop.x + local_scale_shop.x * .5 && mouse_y <= coords_back_shop.y + local_scale_shop.y * .5)
	{
		if (is_shop_clicked == 1) {
			buttons[10].scale.x = CP_Math_LerpFloat(buttons[10].scale.x, 3.0f, CP_System_GetDt() * 5);
			buttons[10].scale.y = CP_Math_LerpFloat(buttons[10].scale.y, 3.0f, CP_System_GetDt() * 5);
			buttons[10].text_size = CP_Math_LerpFloat(buttons[10].text_size, 2.0f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
			{
				shop_button();
			}
		}
	}
	if (is_shop_clicked) {
		buttons[10].scale.x = CP_Math_LerpFloat(buttons[10].scale.x, 2.0f, CP_System_GetDt() * 5);
		buttons[10].scale.y = CP_Math_LerpFloat(buttons[10].scale.y, 2.0f, CP_System_GetDt() * 5);
		buttons[10].text_size = CP_Math_LerpFloat(buttons[10].text_size, 1.3f, CP_System_GetDt() * 5);
	}
	else {
		buttons[10].scale.x = 0;
		buttons[10].scale.y = 0;
		buttons[10].text_size = 0;
	}
	
}

void play_button(void) {
	is_clicked = !is_clicked;
}

void setting_button(void) {
	is_setting_clicked = !is_setting_clicked;
	is_overlayed = !is_overlayed;
}

void shop_button(void) {
	is_shop_clicked = !is_shop_clicked;
	is_overlayed = !is_overlayed;
}

void buy_button(num) {
		switch (num) {
		case 12:
			if (coins >= 10) {
				if (!is_buy1_clicked) {
					deduct_coins(10);
				}
				is_buy1_clicked = true;
				equip_button(num);
			}
			break;
		case 13:
			if (coins >= 20) {
				if (!is_buy2_clicked) {
					deduct_coins(20);
				}
				is_buy2_clicked = true;
				equip_button(num);
			}
			break;
		}
}

void equip_button(num) {

	int index = num - 11;

	if (num == 12 && !is_buy1_clicked) return; 
	if (num == 13 && !is_buy2_clicked) return;  

	equipped_index = index;

	if (equipped_index == 0) {
		buttons[11].button_image = Asset_GetImage(BUTTON_GREY_ASSET);
		buttons[11].text = "EQUIPPED";
	}
	else {
		buttons[11].button_image = Asset_GetImage(BUTTON_YELLOW_ASSET);
		buttons[11].text = "EQUIP";
	}

	if (!is_buy1_clicked) {
		buttons[12].button_image = Asset_GetImage(BUTTON_GREEN_ASSET);
		buttons[12].text = "BUY: 10 Coins";
	}
	else if (equipped_index == 1) {
		buttons[12].button_image = Asset_GetImage(BUTTON_GREY_ASSET);
		buttons[12].text = "EQUIPPED";
	}
	else {
		buttons[12].button_image = Asset_GetImage(BUTTON_YELLOW_ASSET);
		buttons[12].text = "EQUIP";
	}

	if (!is_buy2_clicked) {
		buttons[13].button_image = Asset_GetImage(BUTTON_GREEN_ASSET);
		buttons[13].text = "BUY: 20 Coins";
	}
	else if (equipped_index == 2) {
		buttons[13].button_image = Asset_GetImage(BUTTON_GREY_ASSET);
		buttons[13].text = "EQUIPPED";
	}
	else {
		buttons[13].button_image = Asset_GetImage(BUTTON_YELLOW_ASSET);
		buttons[13].text = "EQUIP";
	}
}


void draw_logo_image(void) {

	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	Sprite_Draw(Asset_GetSprite(TITLE_SPRITE_ASSET, 12, 5, 2.f), logo_x, logo_y,
		CP_System_GetWindowWidth() * 0.5f,
		CP_System_GetWindowHeight() * 0.5f, 255);

	if (!is_clicked) {
		logo_x = CP_Math_LerpFloat(logo_x, logo_start_x, CP_System_GetDt() * 4);
	}
	else {
		logo_x = CP_Math_LerpFloat(logo_x, -500.0f, CP_System_GetDt() * 4);
	}
}

//changes the Main menu screen to the game start state
void change_screen(void) {
	is_clicked = 0;
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
	buttons[8].button_image = is_shadowed ? Asset_GetImage(BUTTON_GREEN_ASSET): Asset_GetImage(BUTTON_GREY_ASSET);
}

bool get_shadow_setting(void) {
	return is_shadowed;
}

//frees the image when exiting the application
void main_menu_exit(void) 
{
	Assets_Free();
	Particles_Free();
	GameSetting_SetFloat("sfx_volume", sfx_limit);
	GameSetting_SetFloat("music_volume", music_limit);
	GameSetting_SetBool("high_quality", is_shadowed);
	GameSetting_SetInt("coins", getCoins());
	GameSetting_Save();
	printf("Exit main menu\n");
}