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
@author Nathan Ng (n.ng@digipen.edu), Ethan Chua (chua.e@digipen.edu)
@date 8 September 2025
@course CSD1401
@brief this file contains functions and code that handles the main menu UI
*/

//defining the various variable for a button
typedef struct ButtonStr {
	CP_Vector pos;
	CP_Vector ending_pos, start_pos;
	CP_Vector scale;
	CP_Image button_image;
	char* text;
	FunctionPtr func;
	float text_size;
} Button;

static const float world_width1 = 48, world_height1 = 27;
static float logo_x, logo_y, logo_start_x, level_select_start, level_select_end, level_select_x, level_select_y;
int equipped_ball_index, equipped_effect_index, shop_page;

static Button buttons[30];
static Button level_buttons[TOTAL_LEVELS];
static CP_Image background, setting_background, shop_background, golf_ball_default_image, golf_ball_gold_image, golf_ball_pool_image, shop_banner, shop_banner_small;

static bool is_clicked = false, is_setting_clicked = false, is_overlayed = false, is_shop_clicked = false, is_buy1_clicked = false, is_buy2_clicked = false, is_buy3_clicked = false, is_buy4_clicked = false, is_shadowed = false, is_equip_clicked = false, is_inversed = false;
static float sfx_limit = 1.0f, music_limit = 1.0f;

/*
@brief This function is implemented for rendering the background ball that moves around at the main menu screen
*/
static void background_ball_update(void) {
	static CP_Vector ball_pos = { 2.5f, 2.5f };
	static CP_Vector ball_vel = { 20.f, 20.f };
	static float ball_angle = 0.f;
	static float diameter = 3.f;

	// bound the ball at the right border
	if (ball_pos.x > world_width1 - diameter * 0.5f) {
		ball_pos.x = world_width1 - diameter * 0.5f;
		ball_vel.x = -ball_vel.x;
		ball_vel.y = ball_vel.y;
		CP_Sound_Play(Asset_GetAudio(GOLF_HIT_WALL_ASSET));
	}
	// bound the ball at the left border
	if (ball_pos.x < diameter * 0.5f) {
		ball_pos.x = diameter * 0.5f;
		ball_vel.x = -ball_vel.x;
		ball_vel.y = ball_vel.y;
		CP_Sound_Play(Asset_GetAudio(GOLF_HIT_WALL_ASSET));
	}
	// bound the ball at the top border
	if (ball_pos.y > world_height1 - diameter * 0.5f) {
		ball_pos.y = world_height1 - diameter * 0.5f;
		ball_vel.y = -ball_vel.y;
		ball_vel.x = ball_vel.x;
		CP_Sound_Play(Asset_GetAudio(GOLF_HIT_WALL_ASSET));
	}
	// bound the ball at the bottom border
	if (ball_pos.y < 1.f) {
		ball_pos.y = 1.f;
		ball_vel.y = -ball_vel.y ;
		ball_vel.x = ball_vel.x;
		CP_Sound_Play(Asset_GetAudio(GOLF_HIT_WALL_ASSET));
	}
	// rotate ball based on direction
	ball_angle += (CP_Vector_Length(ball_vel) * 0.1f) * (ball_vel.x > 0.f ? 1.f : -1.f);
	if (ball_angle > 360.f) {
		ball_angle -= 360.f;
	}
	else if (ball_angle < 0.f) {
		ball_angle += 360.f;
	}
	// update ball position in delta time
	ball_pos = CP_Vector_Add(ball_pos, CP_Vector_Scale(ball_vel, CP_System_GetDt()));
	CP_Vector coords = World_To_Screen(ball_pos.x, ball_pos.y, world_width1, world_height1);
	// render ball shadow first if enabled
	if (is_shadowed) {
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 64));
		CP_Settings_NoStroke();
		CP_Graphics_DrawEllipse(
			coords.x,
			coords.y + 7.5f,
			diameter * CP_System_GetWindowWidth() / world_width1,
			(diameter * CP_System_GetWindowHeight() / world_height1) + 7.5f);
	}
	// render ball based on equipped texture
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	CP_Image_DrawAdvanced(equipped_ball_index == 0 ? golf_ball_default_image : equipped_ball_index == 1 ? golf_ball_gold_image : golf_ball_pool_image,
		coords.x,
		coords.y,
		diameter * CP_System_GetWindowWidth() / world_width1,
		diameter * CP_System_GetWindowWidth() / world_width1,
		255,
		ball_angle);

	CP_Vector mouse_world = Screen_To_World(CP_Input_GetMouseX(), CP_Input_GetMouseY(), world_width1, world_height1);
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && CP_Vector_Distance(mouse_world, ball_pos) <= diameter * 0.5f) {
		Particles_Spawn(PARTICLE_STAR, mouse_world, 1, 2.f, 0.3f, 1.f, 1.f, 2.f);
		CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
	}
}

/*
@brief sets the enum to STATE_EXIT and exits the game
*/
void exit_main_menu(void) {
	GameState_SetNext(STATE_EXIT);
}

void main_menu_init(void) 
{
	GameState_SetCurrent(STATE_MAINMENU);
	printf("Init main menu\n");
	CP_Font_Set(Asset_GetFont(CARTER_FONT_ASSET));
	is_clicked = 0;

	sfx_limit = GameSetting_GetFloat("sfx_volume");
	music_limit = GameSetting_GetFloat("music_volume");
	is_shadowed = GameSetting_GetBool("high_quality");
	is_inversed = GameSetting_GetBool("inverse_controls");
	int res = GameSetting_GetInt("resolution");
	if (res >= 0) {
		change_resolution(res);
	}
	int c = GameSetting_GetInt("coins");
	if (c >= 0) set_coins(c);

	CP_Sound_SetGroupVolume(CP_SOUND_GROUP_MUSIC, music_limit);
	CP_Sound_SetGroupVolume(CP_SOUND_GROUP_SFX, sfx_limit);

	//play button
	buttons[0].button_image = Asset_GetImage(BUTTON_YELLOW_ASSET);
	buttons[0].pos.x = world_width1 * 0.5f;
	buttons[0].pos.y = world_height1 * 0.5f;
	buttons[0].start_pos.x = buttons[0].pos.x;
	buttons[0].func = play_button;
	buttons[0].text = "PLAY";

	//setting button
	buttons[1].button_image = Asset_GetImage(BUTTON_BLUE_ASSET);
	buttons[1].pos.x = world_width1 * 0.4f;
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
	buttons[8].pos.x = world_width1 * 0.76f;
	buttons[8].pos.y = world_height1 * 0.75f;
	buttons[8].start_pos.x = buttons[8].pos.x;
	buttons[8].text = "High Details";
	buttons[8].func = is_shadow;

	//Shop Button
	buttons[9].button_image = Asset_GetImage(BUTTON_BLUE_ASSET);
	buttons[9].pos.x = world_width1 * 0.6f;
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

	buttons[14].button_image = is_inversed ? Asset_GetImage(BUTTON_GREEN_ASSET) : Asset_GetImage(BUTTON_GREY_ASSET);
	buttons[14].pos.x = world_width1 * 0.76f;
	buttons[14].pos.y = world_height1 * 0.66f;
	buttons[14].start_pos.x = buttons[14].pos.x;
	buttons[14].text = "Inverse Control";
	buttons[14].func = is_inverse;

	//Shop Page Buttons
	buttons[15].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	buttons[15].pos.x = world_width1 * 0.8f;
	buttons[15].pos.y = world_height1 * 0.5f;
	buttons[15].start_pos.x = buttons[15].pos.x;
	buttons[15].text = ">";

	buttons[16].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
	buttons[16].pos.x = world_width1 * 0.2f;
	buttons[16].pos.y = world_height1 * 0.5f;
	buttons[16].start_pos.x = buttons[16].pos.x;
	buttons[16].text = "<";

	buttons[17].button_image = is_equip_clicked ? Asset_GetImage(BUTTON_GREY_ASSET) : Asset_GetImage(BUTTON_YELLOW_ASSET);
	buttons[17].pos.x = world_width1 * 0.3f;
	buttons[17].pos.y = world_height1 * 0.32f;
	buttons[17].start_pos.x = buttons[17].pos.x;
	buttons[17].text = is_equip_clicked ? "EQUIPPED" : "EQUIP";

	buttons[18].button_image = is_equip_clicked ? Asset_GetImage(BUTTON_GREY_ASSET) : is_buy1_clicked ? Asset_GetImage(BUTTON_YELLOW_ASSET) : Asset_GetImage(BUTTON_GREEN_ASSET);
	buttons[18].pos.x = world_width1 * 0.5f;
	buttons[18].pos.y = world_height1 * 0.32f;
	buttons[18].start_pos.x = buttons[18].pos.x;
	buttons[18].text = is_equip_clicked ? "EQUIPPED" : is_buy3_clicked ? "EQUIP" : "BUY: 10 Coins";

	buttons[19].button_image = is_equip_clicked ? Asset_GetImage(BUTTON_GREY_ASSET) : is_buy1_clicked ? Asset_GetImage(BUTTON_YELLOW_ASSET) : Asset_GetImage(BUTTON_GREEN_ASSET);
	buttons[19].pos.x = world_width1 * 0.7f;
	buttons[19].pos.y = world_height1 * 0.32f;
	buttons[19].start_pos.x = buttons[19].pos.x;
	buttons[19].text = is_equip_clicked ? "EQUIPPED" : is_buy4_clicked ? "EQUIP" : "BUY: 20 Coins";

	//start of level select button
	int row = 0;
	int column = 0;
	for (int i = 0; i < TOTAL_LEVELS; ++i, ++column) {
		if (i != 0 && i % 9 == 0) {
			++row;
			column = 0;
		}
		level_buttons[i].button_image = Asset_GetImage(SMALL_BUTTON_YELLOW_ASSET);
		level_buttons[i].pos.x = world_width1 * (1.1f + (column * 0.1f));
		level_buttons[i].pos.y = world_height1 * (.7f - (row * 0.2f));
		level_buttons[i].ending_pos.x = world_width1 * (0.1f + (column * 0.1f));
		level_buttons[i].start_pos.x = level_buttons[i].pos.x;
		level_buttons[i].func = change_screen;
	}
	level_buttons[0].text = "1";
	level_buttons[1].text = "2";
	level_buttons[2].text = "3";
	level_buttons[3].text = "4";
	level_buttons[4].text = "5";
	level_buttons[5].text = "6";
	level_buttons[6].text = "7";
	level_buttons[7].text = "8";
	level_buttons[8].text = "9";
	level_buttons[9].text = "10";
#ifdef _DEBUG
	level_buttons[10].text = "T";
#endif
	//end of level select button

	background = Asset_GetImage(GAME_BACKGROUND_ASSET);
	setting_background = Asset_GetImage(MENU_BACKGROUND_ASSET);
	shop_background = Asset_GetImage(SHOP_BACKGROUND_ASSET);
	golf_ball_default_image = Asset_GetImage(GOLF_BALL_ASSET);
	golf_ball_gold_image = Asset_GetImage(GOLF_BALL_EYE_ASSET);
	golf_ball_pool_image = Asset_GetImage(GOLF_BALL_POOL_ASSET);
	background = Asset_GetImage(GAME_BACKGROUND_ASSET);

	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);

	//title logo screen position
	logo_start_x = CP_System_GetWindowWidth() * 0.5f;
	logo_x = CP_System_GetWindowWidth() * 0.5f;
	logo_y = CP_System_GetWindowHeight() * 0.3f;

	level_select_start = CP_System_GetWindowWidth() * 1.5f;
	level_select_end = CP_System_GetWindowWidth() * 0.5f;
	level_select_x = level_select_start;
	level_select_y = CP_System_GetWindowHeight() * 0.1f;

	CP_Sound_StopAll();
	CP_Sound_PlayMusic(Asset_GetAudio(BACKGROUND_MUSIC_ASSET));

	//Sets the default golf ball
	is_buy1_clicked = GameSetting_GetBool("bought1");
	is_buy2_clicked = GameSetting_GetBool("bought2");
	is_buy3_clicked = GameSetting_GetBool("bought3");
	is_buy4_clicked = GameSetting_GetBool("bought4");
	int index = GameSetting_GetInt("equipped");
	if (index < 0) index = 11;
	equip_ball_button(index);
	//Sets the default golf effect
	int index2 = GameSetting_GetInt("equipped_effect");
	if (index2 < 0) index2 = 17;
	equip_effect_button(index2);

	//Sets default shop page
	shop_page = 0;
	
}

void main_menu_update(void) 
{
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));
	GameSetting_SetCursor(CURSOR_DEFAULT);
	CP_Settings_Scale(1.f, 1.f);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Image_Draw(background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, CP_System_GetWindowWidth() * 1.f, CP_System_GetWindowHeight() * 1.f, 255);
	Asset_SpriteStartFrame();

	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();
	Sprite_SetDt(CP_System_GetDt());
	background_ball_update();
	Particles_Update(CP_System_GetDt());
	CP_Vector world_limits = (CP_Vector){ world_width1, world_height1 };
	CP_Vector world_ratios = (CP_Vector){ CP_System_GetWindowWidth() / world_width1, CP_System_GetWindowHeight() / world_height1 };
	Particles_Render(world_limits, world_ratios);

	main_menu_button(mouse_x, mouse_y);
	draw_logo_image();
	level_select_button(mouse_x, mouse_y);

	int counter_textsize = 50;
	CP_Settings_TextSize((float) counter_textsize);
	CP_TEXT_ALIGN_HORIZONTAL h_align = CP_TEXT_ALIGN_H_RIGHT;
	char coin_count[50] = { 0 };
	sprintf_s(coin_count, 50, "%i", coins);
	CP_Font_DrawText(coin_count, CP_System_GetWindowWidth() * 0.9f, CP_System_GetWindowHeight() * 0.9f);

	SpriteSheet s = Asset_GetSprite(COIN_ITEM_SPRITE_ASSET, 2, 4, 0.25f);
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	Sprite_Draw(s, CP_System_GetWindowWidth() * 0.85f, CP_System_GetWindowHeight() * 0.9f,
		60.0f * (CP_System_GetWindowWidth() / 1600.f), 60.0f * (CP_System_GetWindowHeight() / 900.f),
		255);

	//SETTING PAGE
	if (is_setting_clicked) {
		//drawing setting background
		CP_Image_Draw(setting_background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.48f, CP_System_GetWindowWidth() - 200.f, CP_System_GetWindowHeight() - 240.f, 255);
		CP_Image shop_banner = Asset_GetImage(BANNER);
		CP_Image_DrawAdvanced(shop_banner, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.16f, 800.0f, 170.0f, 255, 0.0f);
		draw_text(1.8f, "SETTINGS", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.155f);

		volume_bar(mouse_x, mouse_y);
	}

	
	//SHOP PAGE
	if (is_shop_clicked) {
		//drawing shop background
		CP_Image_Draw(shop_background, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.48f, CP_System_GetWindowWidth() - 200.f, CP_System_GetWindowHeight() - 240.f, 255);
		CP_Image shop_banner_small = Asset_GetImage(HANGING_BANNER);
		CP_Image_DrawAdvanced(shop_banner_small, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.23f, 270.0f, 200.0f, 255, 0.0f);
		CP_Image shop_banner = Asset_GetImage(BANNER);
		CP_Image_DrawAdvanced(shop_banner, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.16f, 800.0f, 170.0f, 255, 0.0f);

		shop_page_buttons(mouse_x, mouse_y);

		//shop items would go here
		switch (shop_page){
		case 0:
		draw_text(1.8f, "CUSTOMIZATION SHOP", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.155f);
		draw_text(0.9f, "GOLF BALLS", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.25f);
		CP_Image_Draw(golf_ball_default_image, CP_System_GetWindowWidth() * 0.3f, CP_System_GetWindowHeight() * 0.5f,
			CP_System_GetWindowWidth() / 10.f,
			CP_System_GetWindowWidth() / 10.f,
			255);

		CP_Image_Draw(golf_ball_gold_image, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f,
			CP_System_GetWindowWidth() / 10.f,
			CP_System_GetWindowWidth() / 10.f,
			255);

		CP_Image_Draw(golf_ball_pool_image, CP_System_GetWindowWidth() * 0.7f, CP_System_GetWindowHeight() * 0.5f,
			CP_System_GetWindowWidth() / 10.f,
			CP_System_GetWindowWidth() / 10.f,
			255);
		break;

		case 1:
		draw_text(1.8f, "CUSTOMIZATION SHOP", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.155f);
		draw_text(0.9f, "EFFECTS", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.25f);
		CP_Image_Draw(Asset_GetImage(SPARKLE_IMAGE_ASSET), CP_System_GetWindowWidth() * 0.3f, CP_System_GetWindowHeight() * 0.5f, 
			CP_System_GetWindowWidth() / 10.f, 
			CP_System_GetWindowWidth() / 10.f, 255);

		CP_Image_Draw(Asset_GetImage(FLAME_IMAGE_ASSET), CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, 
			CP_System_GetWindowWidth() / 10.f, 
			CP_System_GetWindowWidth() / 10.f, 255);

		CP_Image_Draw(Asset_GetImage(LIGHTNING_IMAGE_ASSET), CP_System_GetWindowWidth() * 0.7f, CP_System_GetWindowHeight() * 0.5f, 
			CP_System_GetWindowWidth() / 10.f, 
			CP_System_GetWindowWidth() / 10.f, 255);
		break;
		}

	}
	shops_button(mouse_x, mouse_y);
	settings_button(mouse_x, mouse_y);
	back_buttons(mouse_x, mouse_y);
	GameState_Update();
}

/*
@brief This function is called when rendering and animating the main menu buttons play button, setting button, quit button
@param takes in the position of the mouse (x and y axis)
*/
void main_menu_button(float mouse_x, float mouse_y) {
	//MAIN MENU SCREEN BUTTON
	for (int i = 0; i < 3; i++) {
		//defining the position of the button
		CP_Vector coords = defining_position(buttons[i].pos.x, buttons[i].pos.y);
		CP_Vector local_scale = defining_coords(buttons[i].scale.x, buttons[i].scale.y);
		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);
		draw_text(buttons[i].text_size, buttons[i].text, coords.x, coords.y);

		//telling what the button should do
		if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y) && !is_overlayed)
		{
			GameSetting_SetCursor(CURSOR_POINT);
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 9.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 4.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 2.0f, CP_System_GetDt() * 5);


			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
			{
				if (buttons[i].func) {
					CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
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
	CP_Vector coords = defining_position(buttons[9].pos.x, buttons[9].pos.y);
	CP_Vector local_scale = defining_coords(buttons[9].scale.x, buttons[9].scale.y);
	//Drawing the button
	CP_Image_DrawAdvanced(buttons[9].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);
	draw_text(buttons[9].text_size, buttons[9].text, coords.x, coords.y);

	if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y) && !is_overlayed)
	{
		GameSetting_SetCursor(CURSOR_POINT);
		buttons[9].scale.x = CP_Math_LerpFloat(buttons[9].scale.x, 9.0f, CP_System_GetDt() * 5);
		buttons[9].scale.y = CP_Math_LerpFloat(buttons[9].scale.y, 4.0f, CP_System_GetDt() * 5);
		buttons[9].text_size = CP_Math_LerpFloat(buttons[9].text_size, 2.0f, CP_System_GetDt() * 5);

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
		{
			if (buttons[9].func) {
				CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
				shop_page = 0;
				buttons[9].func();
			}
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

/*
@brief This function is called when rendering and animating the square level select buttons from 1 - 10
@param takes in the position of the mouse (x and y axis)
*/
void level_select_button(float mouse_x, float mouse_y) {

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Image_DrawAdvanced(Asset_GetImage(HANGING_BANNER), level_select_x, level_select_y, 0.3f * CP_System_GetWindowWidth(), 0.15f * CP_System_GetWindowHeight(), 255, 0.f);
	draw_text(2, "Level Select", level_select_x, level_select_y);
	int level_highlight = 0;
	//LEVEL SELECT BUTTONS
	for (int i = 0; i < TOTAL_LEVELS; i++) {
		//defining the position of the button
		CP_Vector coords = defining_position(level_buttons[i].pos.x, level_buttons[i].pos.y);
		CP_Vector local_scale = defining_coords(level_buttons[i].scale.x, level_buttons[i].scale.y);

		CP_Settings_ImageMode(CP_POSITION_CENTER);
		//Drawing the button
		CP_Image_DrawAdvanced(level_buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);
		draw_text(level_buttons[i].text_size, level_buttons[i].text, coords.x, coords.y);

		if (is_clicked == 1) {
			level_buttons[i].pos.x = CP_Math_LerpFloat(level_buttons[i].pos.x, level_buttons[i].ending_pos.x, CP_System_GetDt() * 4);
			level_select_x = CP_Math_LerpFloat(level_select_x, level_select_end, CP_System_GetDt());
		}
		else {
			level_buttons[i].pos.x = CP_Math_LerpFloat(level_buttons[i].pos.x, level_buttons[i].start_pos.x, CP_System_GetDt() * 4);
			level_select_x = CP_Math_LerpFloat(level_select_x, level_select_start, CP_System_GetDt());
		}

		//telling what the button should do
		if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y))
		{
			level_highlight = i + 1;
			GameSetting_SetCursor(CURSOR_POINT);
			level_buttons[i].scale.x = CP_Math_LerpFloat(level_buttons[i].scale.x, 4.0f, CP_System_GetDt() * 5);
			level_buttons[i].scale.y = CP_Math_LerpFloat(level_buttons[i].scale.y, 4.0f, CP_System_GetDt() * 5);
			level_buttons[i].text_size = CP_Math_LerpFloat(level_buttons[i].text_size, 3.f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
			{
				CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
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

	// render level personal best scoreboard
	static float y = -1.f;
	if (y < 0.f) {
		y = CP_System_GetWindowHeight() * 1.1f;
	}
	if (level_highlight != 0) {
		y = CP_Math_LerpFloat(y, CP_System_GetWindowHeight() * 0.9f, CP_System_GetDt() * 5.f);
	}
	else {
		y = CP_Math_LerpFloat(y, CP_System_GetWindowHeight() * 1.1f, CP_System_GetDt() * 5.f);
	}
	CP_Image_Draw(shop_background, level_select_x, y, 0.2f * CP_System_GetWindowWidth(), 0.15f * CP_System_GetWindowHeight(), 255);
	char pb[16]; sprintf_s(pb, 16, "pb_level_%d", level_highlight);
	CP_Vector personal_best = GameSetting_GetVect(pb);
	char pb_stroke[32]; sprintf_s(pb_stroke, 32, "Personal best: %d", (int)personal_best.x);
	draw_text(1.f, pb_stroke, level_select_x, y - (0.5f * CP_System_GetWindowHeight() / world_height1));

	char pb_time[32]; sprintf_s(pb_time, 32, "Time: %0.2fs", personal_best.y);
	draw_text(1.f, pb_time, level_select_x, y + (0.5f * CP_System_GetWindowHeight() / world_height1));
}

/*
@brief This function is called when rendering and animating the two volume bar in the setting page, music and sfx volume
@param takes in the position of the mouse (x and y axis)
*/
void volume_bar(float mouse_x, float mouse_y) {
	CP_Color sound_bar = Get_Color(COLOR_WHITE);
	CP_Color sound_bar_background = CP_Color_Create(150, 150, 150, 255);

	CP_Settings_RectMode(CP_POSITION_CORNER);

	//START DRAWING OF VOLUME BARS
	//drawing of SFX volume button
	draw_text(1.5f, "SFX Volume", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.35f);

	//drawing of Music volume button
	draw_text(1.5f, "Music Volume", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f);

	//drawing of SFX volume bar
	CP_Settings_Stroke(Get_Color(COLOR_BLACK));
	CP_Settings_Fill(sound_bar_background);
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
	if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT) && !GameState_IsTransitioning()) {
		
		if (check_mouse_pos_volume(mouse_x, mouse_y, 0.22f * CP_System_GetWindowWidth(), 0.4f * CP_System_GetWindowHeight(), 27.0f * CP_System_GetWindowWidth() / world_width1, 1.3f * CP_System_GetWindowHeight() / world_height1)) {
			GameSetting_SetCursor(CURSOR_GRAB);
			sfx_limit = (mouse_x - CP_System_GetWindowWidth() * 0.22f) / (27.0f * CP_System_GetWindowWidth() / world_width1);
			CP_Sound_SetGroupVolume(CP_SOUND_GROUP_SFX, sfx_limit);
		}
	}

	//check if mouse is within the Music volume bar and if the left mouse is held down
	if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT) && !GameState_IsTransitioning()) {
		
		if (check_mouse_pos_volume(mouse_x, mouse_y, 0.22f * CP_System_GetWindowWidth(), 0.55f * CP_System_GetWindowHeight(), 27.0f * CP_System_GetWindowWidth() / world_width1, 1.3f * CP_System_GetWindowHeight() / world_height1)) {
			GameSetting_SetCursor(CURSOR_GRAB);
			music_limit = (mouse_x - CP_System_GetWindowWidth() * 0.22f) / (27.0f * CP_System_GetWindowWidth() / world_width1);
			CP_Sound_SetGroupVolume(CP_SOUND_GROUP_MUSIC, music_limit);
		}
	}

	//drawing of volume percentage base on the mouse position
	draw_volume(1.0f * CP_System_GetWindowHeight() / world_height1, CP_System_GetWindowWidth() * 0.25f, CP_System_GetWindowHeight() * 0.425f, sfx_limit);
	draw_volume(1.0f * CP_System_GetWindowHeight() / world_height1, CP_System_GetWindowWidth() * 0.25f, CP_System_GetWindowHeight() * 0.575f, music_limit);
}

/*
@brief This function is called when rendering and animating the 3 resolution buttons, is_shadow button, and is_inverse button
@param takes in the position of the mouse (x and y axis)
*/
void settings_button(float mouse_x, float mouse_y) {
	//RESOLUTION BUTTONS
	for (int i = 4; i < 7; i++) {
		//defining the position of the button
		CP_Vector coords = defining_position(buttons[i].pos.x, buttons[i].pos.y);
		CP_Vector local_scale = defining_coords(buttons[i].scale.x, buttons[i].scale.y);

		CP_Settings_ImageMode(CP_POSITION_CENTER);

		//Drawing the button
		CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);
		draw_text(buttons[i].text_size, buttons[i].text, coords.x, coords.y);

		//telling what the button should do
		if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y))
		{
			GameSetting_SetCursor(CURSOR_POINT);
			buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 7.0f, CP_System_GetDt() * 5);
			buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 3.0f, CP_System_GetDt() * 5);
			buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 1.3f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
			{
				CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
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
	CP_Vector coords = defining_position(buttons[8].pos.x, buttons[8].pos.y);
	CP_Vector local_scale = defining_coords(buttons[8].scale.x, buttons[8].scale.y);

	CP_Settings_ImageMode(CP_POSITION_CENTER);

	//Drawing the button
	CP_Image_DrawAdvanced(buttons[8].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);
	draw_text(buttons[8].text_size, buttons[8].text, coords.x, coords.y);

	//telling what the button should do
	if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y))
	{
		GameSetting_SetCursor(CURSOR_POINT);
		buttons[8].scale.x = CP_Math_LerpFloat(buttons[8].scale.x, 7.0f, CP_System_GetDt() * 5);
		buttons[8].scale.y = CP_Math_LerpFloat(buttons[8].scale.y, 3.0f, CP_System_GetDt() * 5);
		buttons[8].text_size = CP_Math_LerpFloat(buttons[8].text_size, 1.3f, CP_System_GetDt() * 5);
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
		{
			CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
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

	//INVERSE BUTTON
	coords = defining_position(buttons[14].pos.x, buttons[14].pos.y);
	local_scale = defining_coords(buttons[14].scale.x, buttons[14].scale.y);

	CP_Settings_ImageMode(CP_POSITION_CENTER);

	//Drawing the button
	CP_Image_DrawAdvanced(buttons[14].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);
	draw_text(buttons[14].text_size, buttons[14].text, coords.x, coords.y);

	//telling what the button should do
	if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y))
	{
		GameSetting_SetCursor(CURSOR_POINT);
		buttons[14].scale.x = CP_Math_LerpFloat(buttons[14].scale.x, 7.0f, CP_System_GetDt() * 5);
		buttons[14].scale.y = CP_Math_LerpFloat(buttons[14].scale.y, 3.0f, CP_System_GetDt() * 5);
		buttons[14].text_size = CP_Math_LerpFloat(buttons[14].text_size, 1.1f, CP_System_GetDt() * 5);
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
		{
			CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
			buttons[14].func();
		}
	}
	else {
		if (is_setting_clicked) {
			buttons[14].scale.x = CP_Math_LerpFloat(buttons[14].scale.x, 6.0f, CP_System_GetDt() * 5);
			buttons[14].scale.y = CP_Math_LerpFloat(buttons[14].scale.y, 2.0f, CP_System_GetDt() * 5);
			buttons[14].text_size = CP_Math_LerpFloat(buttons[14].text_size, 0.8f, CP_System_GetDt() * 5);
		}
		else {
			buttons[14].scale.x = 0;
			buttons[14].scale.y = 0;
			buttons[14].text_size = 0;
		}
	}
}

/*
@brief This function is called when rendering and animating the shop button at the main menu screen
@param takes in the position of the mouse (x and y axis)
*/
void shops_button(float mouse_x, float mouse_y)
{
	switch (shop_page)
	{
	case 0:
		for (int i = 11; i < 14; i++) {
			//defining the position of the button
			CP_Vector coords = defining_position(buttons[i].pos.x, buttons[i].pos.y);
			CP_Vector local_scale = defining_coords(buttons[i].scale.x, buttons[i].scale.y);

			CP_Settings_ImageMode(CP_POSITION_CENTER);

			//Drawing the button
			CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);

			//telling what the button should do
			if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y))
			{
				GameSetting_SetCursor(CURSOR_POINT);
				buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 7.0f, CP_System_GetDt() * 5);
				buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 3.0f, CP_System_GetDt() * 5);
				buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 1.3f, CP_System_GetDt() * 5);

				if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
				{
					CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
					if (i == 11) {
						equip_ball_button(11);
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
					buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 0.95f, CP_System_GetDt() * 5);
				}
				else {
					buttons[i].scale.x = 0;
					buttons[i].scale.y = 0;
					buttons[i].text_size = 0;
				}
			}
			draw_text(buttons[i].text_size, buttons[i].text, coords.x, coords.y);
		}
		break;

	case 1:
		for (int i = 17; i < 20; i++) {
			//defining the position of the button
			CP_Vector coords = defining_position(buttons[i].pos.x, buttons[i].pos.y);
			CP_Vector local_scale = defining_coords(buttons[i].scale.x, buttons[i].scale.y);

			CP_Settings_ImageMode(CP_POSITION_CENTER);

			//Drawing the button
			CP_Image_DrawAdvanced(buttons[i].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0);

			//telling what the button should do
			if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y))
			{
				GameSetting_SetCursor(CURSOR_POINT);
				buttons[i].scale.x = CP_Math_LerpFloat(buttons[i].scale.x, 7.0f, CP_System_GetDt() * 5);
				buttons[i].scale.y = CP_Math_LerpFloat(buttons[i].scale.y, 3.0f, CP_System_GetDt() * 5);
				buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 1.3f, CP_System_GetDt() * 5);

				if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
				{
					CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
					if (i == 17) {
						equip_effect_button(17);
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
					buttons[i].text_size = CP_Math_LerpFloat(buttons[i].text_size, 0.95f, CP_System_GetDt() * 5);
				}
				else {
					buttons[i].scale.x = 0;
					buttons[i].scale.y = 0;
					buttons[i].text_size = 0;
				}
			}
			draw_text(buttons[i].text_size, buttons[i].text, coords.x, coords.y);
		}
		break;
	}
}

void shop_page_buttons(float mouse_x, float mouse_y) {
	//SHOP PAGE CHANGE BUTTON
		CP_Vector coords1 = defining_position(buttons[15].pos.x, buttons[15].pos.y);
		CP_Vector local_scale1 = defining_coords(buttons[15].scale.x, buttons[15].scale.y);

		CP_Settings_ImageMode(CP_POSITION_CENTER);

		CP_Vector coords2 = defining_position(buttons[16].pos.x, buttons[16].pos.y);
		CP_Vector local_scale2 = defining_coords(buttons[16].scale.x, buttons[16].scale.y);

		CP_Settings_ImageMode(CP_POSITION_CENTER);

		switch (shop_page) {
		case 0:

			CP_Image_DrawAdvanced(buttons[15].button_image, coords1.x, coords1.y, local_scale1.x, local_scale1.y, 255, 0.f);
			draw_text(buttons[15].text_size, buttons[15].text, coords1.x, coords1.y);
			//telling what the button should do
			if (check_mouse_pos(mouse_x, mouse_y, coords1.x, coords1.y, local_scale1.x, local_scale1.y))
			{
				GameSetting_SetCursor(CURSOR_POINT);
				if (is_shop_clicked == 1) {
					buttons[15].scale.x = CP_Math_LerpFloat(buttons[15].scale.x, 3.0f, CP_System_GetDt() * 5);
					buttons[15].scale.y = CP_Math_LerpFloat(buttons[15].scale.y, 3.0f, CP_System_GetDt() * 5);
					buttons[15].text_size = CP_Math_LerpFloat(buttons[15].text_size, 2.0f, CP_System_GetDt() * 5);
					if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
					{
						shop_page = 1;
					}
				}
			}
			if (is_shop_clicked) {
				buttons[15].scale.x = CP_Math_LerpFloat(buttons[15].scale.x, 2.0f, CP_System_GetDt() * 5);
				buttons[15].scale.y = CP_Math_LerpFloat(buttons[15].scale.y, 2.0f, CP_System_GetDt() * 5);
				buttons[15].text_size = CP_Math_LerpFloat(buttons[15].text_size, 1.3f, CP_System_GetDt() * 5);
			}
			else {
				buttons[15].scale.x = 0;
				buttons[15].scale.y = 0;
				buttons[15].text_size = 0;
			}
		break;

		case 1:

			if (shop_page == 1) {
				//Drawing the button
				CP_Image_DrawAdvanced(buttons[16].button_image, coords2.x, coords2.y, local_scale2.x, local_scale2.y, 255, 0.f);
				draw_text(buttons[16].text_size, buttons[16].text, coords2.x, coords2.y);
				//telling what the button should do
				if (check_mouse_pos(mouse_x, mouse_y, coords2.x, coords2.y, local_scale2.x, local_scale2.y))
				{
					GameSetting_SetCursor(CURSOR_POINT);
					if (is_shop_clicked == 1) {
						buttons[16].scale.x = CP_Math_LerpFloat(buttons[16].scale.x, 3.0f, CP_System_GetDt() * 5);
						buttons[16].scale.y = CP_Math_LerpFloat(buttons[16].scale.y, 3.0f, CP_System_GetDt() * 5);
						buttons[16].text_size = CP_Math_LerpFloat(buttons[16].text_size, 2.0f, CP_System_GetDt() * 5);
						if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
						{
							shop_page = 0;
						}
					}
				}
				if (is_shop_clicked) {
					buttons[16].scale.x = CP_Math_LerpFloat(buttons[16].scale.x, 2.0f, CP_System_GetDt() * 5);
					buttons[16].scale.y = CP_Math_LerpFloat(buttons[16].scale.y, 2.0f, CP_System_GetDt() * 5);
					buttons[16].text_size = CP_Math_LerpFloat(buttons[16].text_size, 1.3f, CP_System_GetDt() * 5);
				}
				else {
					buttons[16].scale.x = 0;
					buttons[16].scale.y = 0;
					buttons[16].text_size = 0;
				}
			}
			break;

		}
}

/*
@brief This function is called when rendering and animating the back buttons for setting, level select and shop
@param takes in the position of the mouse (x and y axis)
*/
void back_buttons(float mouse_x, float mouse_y) {
	//BACK BUTTON FOR LEVEL SELECT
	CP_Vector coords = defining_position(buttons[3].pos.x, buttons[3].pos.y);
	CP_Vector local_scale = defining_coords(buttons[3].scale.x, buttons[3].scale.y);

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced(buttons[3].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);
	draw_text(buttons[3].text_size, buttons[3].text, coords.x, coords.y);

	if (is_clicked == 1) {
		buttons[3].pos.x = CP_Math_LerpFloat(buttons[3].pos.x, buttons[3].ending_pos.x, CP_System_GetDt() * 4);
	}
	else {
		buttons[3].pos.x = CP_Math_LerpFloat(buttons[3].pos.x, buttons[3].start_pos.x, CP_System_GetDt() * 4);
	}

	//telling what the button should do
	if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y))
	{
		GameSetting_SetCursor(CURSOR_POINT);
		buttons[3].scale.x = CP_Math_LerpFloat(buttons[3].scale.x, 3.0f, CP_System_GetDt() * 5);
		buttons[3].scale.y = CP_Math_LerpFloat(buttons[3].scale.y, 3.0f, CP_System_GetDt() * 5);
		buttons[3].text_size = CP_Math_LerpFloat(buttons[3].text_size, 3.f, CP_System_GetDt() * 5);
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
		{
			CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
			buttons[3].func();
		}
	}
	else {
		buttons[3].scale.x = CP_Math_LerpFloat(buttons[3].scale.x, 2.0f, CP_System_GetDt() * 5);
		buttons[3].scale.y = CP_Math_LerpFloat(buttons[3].scale.y, 2.0f, CP_System_GetDt() * 5);
		buttons[3].text_size = CP_Math_LerpFloat(buttons[3].text_size, 2.0f, CP_System_GetDt() * 5);
	}

	//BACK BUTTON FOR SETTINGS PAGE
	coords = defining_position(buttons[7].pos.x, buttons[7].pos.y);
	local_scale = defining_coords(buttons[7].scale.x, buttons[7].scale.y);

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced(buttons[7].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);
	draw_text(buttons[7].text_size, buttons[7].text, coords.x, coords.y);

	//telling what the button should do
	if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y))
	{
		GameSetting_SetCursor(CURSOR_POINT);
		if (is_setting_clicked == 1) {
			buttons[7].scale.x = CP_Math_LerpFloat(buttons[7].scale.x, 3.0f, CP_System_GetDt() * 5);
			buttons[7].scale.y = CP_Math_LerpFloat(buttons[7].scale.y, 3.0f, CP_System_GetDt() * 5);
			buttons[7].text_size = CP_Math_LerpFloat(buttons[7].text_size, 2.0f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
			{
				CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
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
	coords = defining_position(buttons[10].pos.x, buttons[10].pos.y);
	local_scale = defining_coords(buttons[10].scale.x, buttons[10].scale.y);

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//Drawing the button
	CP_Image_DrawAdvanced(buttons[10].button_image, coords.x, coords.y, local_scale.x, local_scale.y, 255, 0.f);
	draw_text(buttons[10].text_size, buttons[10].text, coords.x, coords.y);

	//telling what the button should do
	if (check_mouse_pos(mouse_x, mouse_y, coords.x, coords.y, local_scale.x, local_scale.y))
	{
		GameSetting_SetCursor(CURSOR_POINT);
		if (is_shop_clicked == 1) {
			buttons[10].scale.x = CP_Math_LerpFloat(buttons[10].scale.x, 3.0f, CP_System_GetDt() * 5);
			buttons[10].scale.y = CP_Math_LerpFloat(buttons[10].scale.y, 3.0f, CP_System_GetDt() * 5);
			buttons[10].text_size = CP_Math_LerpFloat(buttons[10].text_size, 2.0f, CP_System_GetDt() * 5);
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && !GameState_IsTransitioning())
			{
				CP_Sound_Play(Asset_GetAudio(BUTTON_SFX_ASSET));
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

/*
@brief This function is called to define the position of the various UI elements drawn onto the screen from world to screen space
@param takes in the position of the mouse (x and y axis)
*/
CP_Vector defining_position(float pos_x, float pos_y) {
	return World_To_Screen(pos_x, pos_y, world_width1, world_height1);
}

/*
@brief This function is called to define the size/scale of the various UI elements drawn onto the screen from world to screen space
@param takes in the scale factor of the object (x and y)
*/
CP_Vector defining_coords(float scale_x, float scale_y) {
	CP_Vector scale = {
		scale_x * CP_System_GetWindowWidth() / world_width1,
		scale_y * CP_System_GetWindowHeight() / world_height1
	};

	return scale;
}

/*
@brief This function is called to check if the users mouse is hovering over a button
@param takes in the mouse position, the buttons position and scale
*/
bool check_mouse_pos(float mouse_x, float mouse_y, float pos_x, float pos_y, float scale_x, float scale_y) {
	return mouse_x >= pos_x - scale_x * .5f && mouse_y >= pos_y - scale_y * .5f && mouse_x <= pos_x + scale_x * .5f && mouse_y <= pos_y + scale_y * .5f;
}

/*
@brief This function is called to check if the users mouse is hovering over the volume button
@param takes in the mouse position, the volume position and scale
*/
bool check_mouse_pos_volume(float mouse_x, float mouse_y, float pos_x, float pos_y, float scale_x, float scale_y) {
	return mouse_x >= pos_x && mouse_x <= pos_x + scale_x && mouse_y >= pos_y && mouse_y <= pos_y + scale_y;
}

/*
@brief This function is called to draw text anywhere specified by the pos
@param takes in the text size, text, and the position you want the text to draw at (x and y)
*/
void draw_text(float text_size, char* text, float pos_x, float pos_y) {
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Settings_TextSize(text_size * CP_System_GetWindowHeight() / world_height1);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(text, pos_x, pos_y);
}

/*
@brief This function is called to draw text onto the two volume bars and adjust the text according to the scale of the volume bar
@param takes in the text size, the position, and the limit
*/
void draw_volume(float text_size, float pos_x, float pos_y, float limit) {
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Settings_TextSize(text_size);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	char p[50];  sprintf_s(p, 50, "%.1f", limit * 100);
	CP_Font_DrawText(p, pos_x, pos_y);
}

/*
@brief This function is called to lerp in the level select buttons and lerp out the main menu buttons
@param nothing
*/
void play_button(void) {
	is_clicked = !is_clicked;
}

/*
@brief This function is called to open the setting menu
@param nothing
*/
void setting_button(void) {
	is_setting_clicked = !is_setting_clicked;
	is_overlayed = !is_overlayed;
}

/*
@brief This function is called to open the shop menu
@param nothing
*/
void shop_button(void) {
	is_shop_clicked = !is_shop_clicked;
	is_overlayed = !is_overlayed;
}

/*
@brief This function is called for the shop buy buttons to check if the player have bought the skin
	   and deduct the relevant amount of coins
@param takes in the button number
*/
void buy_button(int num) {
	switch (num) {
	case 12:
		if (coins >= 10) {
			if (!is_buy1_clicked) {
				deduct_coins(10);
			}
			is_buy1_clicked = true;
			equip_ball_button(num);
		}
		break;
	case 13:
		if (coins >= 20) {
			if (!is_buy2_clicked) {
				deduct_coins(20);
			}
			is_buy2_clicked = true;
			equip_ball_button(num);
		}
		break;

	case 18:
		if (coins >= 10) {
			if (!is_buy3_clicked) {
				deduct_coins(10);
			}
			is_buy3_clicked = true;
			equip_effect_button(num);
		}
		break;

	case 19:
		if (coins >= 20) {
			if (!is_buy4_clicked) {
				deduct_coins(20);
			}
			is_buy4_clicked = true;
			equip_effect_button(num);
		}
		break;
	}
}

/*
@brief This function is called for the shop buy buttons to check if the player have equipped the skin
@param takes in the button number
*/
void equip_ball_button(int num) {

	int index = num - 11;

	if (num == 12 && !is_buy1_clicked) return; 
	if (num == 13 && !is_buy2_clicked) return;  

	equipped_ball_index = index;

	if (equipped_ball_index == 0) {
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
	else if (equipped_ball_index == 1) {
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
	else if (equipped_ball_index == 2) {
		buttons[13].button_image = Asset_GetImage(BUTTON_GREY_ASSET);
		buttons[13].text = "EQUIPPED";
	}
	else {
		buttons[13].button_image = Asset_GetImage(BUTTON_YELLOW_ASSET);
		buttons[13].text = "EQUIP";
	}

}

void equip_effect_button(int num) {

	int index = num - 17;

	if (num == 18 && !is_buy3_clicked) return;
	if (num == 19 && !is_buy4_clicked) return;

	equipped_effect_index = index;

	if (equipped_effect_index == 0) {
		buttons[17].button_image = Asset_GetImage(BUTTON_GREY_ASSET);
		buttons[17].text = "EQUIPPED";
	}
	else {
		buttons[17].button_image = Asset_GetImage(BUTTON_YELLOW_ASSET);
		buttons[17].text = "EQUIP";
	}

	if (!is_buy3_clicked) {
		buttons[18].button_image = Asset_GetImage(BUTTON_GREEN_ASSET);
		buttons[18].text = "BUY: 10 Coins";
	}
	else if (equipped_effect_index == 1) {
		buttons[18].button_image = Asset_GetImage(BUTTON_GREY_ASSET);
		buttons[18].text = "EQUIPPED";
	}
	else {
		buttons[18].button_image = Asset_GetImage(BUTTON_YELLOW_ASSET);
		buttons[18].text = "EQUIP";
	}

	if (!is_buy4_clicked) {
		buttons[19].button_image = Asset_GetImage(BUTTON_GREEN_ASSET);
		buttons[19].text = "BUY: 20 Coins";
	}
	else if (equipped_effect_index == 2) {
		buttons[19].button_image = Asset_GetImage(BUTTON_GREY_ASSET);
		buttons[19].text = "EQUIPPED";
	}
	else {
		buttons[19].button_image = Asset_GetImage(BUTTON_YELLOW_ASSET);
		buttons[19].text = "EQUIP";
	}

}

/*
@brief This function is called to draw the GIF logo at the main menu, and to lerp out when play button is clicked
@param nothing
*/
void draw_logo_image(void) {

	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	Sprite_Draw(Asset_GetSprite(TITLE_SPRITE_ASSET, 12, 5, 1.5f), logo_x, logo_y,
		CP_System_GetWindowWidth() * 0.5f,
		CP_System_GetWindowHeight() * 0.5f, 255);

	if (!is_clicked) {
		logo_x = CP_Math_LerpFloat(logo_x, logo_start_x, CP_System_GetDt() * 4);
	}
	else {
		logo_x = CP_Math_LerpFloat(logo_x, -500.0f * (CP_System_GetWindowWidth() / 1600.f), CP_System_GetDt() * 4);
	}
}

/*
@brief This function is called to change the game state to STATE_GAME which are the various levels.
@param nothing
*/
void change_screen(void) {
	GameState_SetNext(STATE_GAME);
}

/*
@brief This function is called to change the resolution.
@param resolution button number
*/
void change_resolution(int num) {
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
	GameSetting_SetInt("resolution", num);
	logo_start_x = CP_System_GetWindowWidth() * 0.5f;
	logo_x = CP_System_GetWindowWidth() * 0.5f;
	logo_y = CP_System_GetWindowHeight() * 0.3f;

	level_select_start = CP_System_GetWindowWidth() * 1.5f;
	level_select_end = CP_System_GetWindowWidth() * 0.5f;
	level_select_x = level_select_start;
	level_select_y = CP_System_GetWindowHeight() * 0.1f;
}

/*
@brief This function is called to activate or deactivate the shadow feature for the golf ball.
@param nothing
*/
void is_shadow(void) {
	is_shadowed = !is_shadowed;
	buttons[8].button_image = is_shadowed ? Asset_GetImage(BUTTON_GREEN_ASSET): Asset_GetImage(BUTTON_GREY_ASSET);
}

/*
@brief This function is called to activate or deactivate the inverse contols for the golf ball.
@param nothing
*/
void is_inverse(void) {
	is_inversed = !is_inversed;
	buttons[14].button_image = is_inversed ? Asset_GetImage(BUTTON_GREEN_ASSET) : Asset_GetImage(BUTTON_GREY_ASSET);
}

/*
@brief This function is called to return the value of is_shadowed (true or false).
@param nothing
*/
bool get_shadow_setting(void) {
	return is_shadowed;
}

/*
@brief This function is called to return the value of is_inverse (true or false).
@param nothing
*/
bool get_inverse_setting(void) {
	return is_inversed;
}

/*
@brief This function is called to free the images when exiting the application.
@param nothing
*/
void main_menu_exit(void) 
{
	Particles_Free();
	GameSetting_SetFloat("sfx_volume", sfx_limit);
	GameSetting_SetFloat("music_volume", music_limit);
	GameSetting_SetBool("high_quality", is_shadowed);
	GameSetting_SetInt("coins", getCoins());
	GameSetting_SetInt("equipped", equipped_ball_index + 11);
	GameSetting_SetInt("equipped_effect", equipped_effect_index + 17);
	GameSetting_SetBool("bought1", is_buy1_clicked);
	GameSetting_SetBool("bought2", is_buy2_clicked);
	GameSetting_SetBool("bought3", is_buy3_clicked);
	GameSetting_SetBool("bought4", is_buy4_clicked);
	GameSetting_SetBool("inverse_controls", is_inversed);
	GameSetting_Save();
	Assets_Free();
	printf("Exit main menu\n");
}