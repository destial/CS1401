#include "scoring.h"
#include "cprocessing.h"
#include "utils.h"
#include <stdio.h>

#include "game.h"
#include "asset_manager.h"
#include "sprite_manager.h"
#include "state_manager.h"
#include "currency_manager.h"
#include "game_settings.h"
#include "level_manager.h"
#include <math.h>
//void scoring_update();
//void scoring_exit();
/*!
@author Htet (heinhtet.lin.edu)
@date 10 October 2025
@course CSD1401
@brief this file contains functions and code that handles the Scoring system
*/




//Initialise the variables
int stroke = 0;
int lvl_par = 4;
int bogey, birdie, eagle, d_bogey, t_bogey, albatross;
int par;

//setting button
typedef struct Button {
	CP_Vector pos;
	CP_Vector ending_pos;
	CP_Vector scale;
	CP_Image button_image;
	FunctionPtr func;
} Button;

Button sbButton[10];

float clickbb_x, clickbb_y;
float hover_x, hover_y;

typedef struct Buttonbb {
	CP_Image button_img;
	CP_Image button_icon;
	float x;
	float y;
	float width;
	float height;
	float scale;
	float icon_width;
	float icon_height;

} bb;

typedef struct Star {
	CP_Image img;
	float x;
	float y;
	float width;
	float height;
} star;

typedef struct TextBox {
	float x;
	float y;
} TextBox;
//CP_Graphics_DrawRect(0.3 * CP_System_GetWindowWidth(), 0.7f * CP_System_GetWindowHeight(), 8.0f * CP_System_GetWindowWidth() / world_width, 4.f * CP_System_GetWindowHeight() / world_height);
//Draw exit buttons

bb scorebb[3];
star scoress[3];
star scoregs[3];
TextBox Text[4];

CP_Color white_colour;

CP_Font sb_button_font;

static int IsAreaClicked(float area_center_x, float area_center_y, float area_width, float area_height, float click_x, float click_y)
{
	//area_center_x, area_center_y is the center coord of rectangle
	//Find the left most and right most side of rectangle
	float half_width = area_width * 0.5f;
	float half_height = area_height * 0.5f;

	//Left most area
	float left_width_x = area_center_x - half_width;
	//Right most area
	float right_width_x = area_center_x + half_width;

	//Top most area
	float top_height = area_center_y - half_height;
	//Bottom most area
	float bottom_height = area_center_y + half_height;

	/*click_x = CP_Input_GetMouseX();
	click_y = CP_Input_GetMouseY();*/

	if (left_width_x < click_x && click_x < right_width_x
		&& top_height < click_y && click_y < bottom_height) {
		return 1;
	}
	else {
		return 0;
	}

}

static float GetPulseScale(float baseScale, float amplitude, float speed) {
	float t = CP_System_GetDt();
	return baseScale + amplitude * sinf(t * speed);
}

//Currency Logic
int coins_earned_last_level = 0;
int extra_coins = 0;

int get_extra_coins(void) {
	return extra_coins;
}

void add_extra_coins(int add_coin_amt) {
	extra_coins += add_coin_amt;
}

//Global var to prevent dupe coins
int reward_given = 0;

int calculate_reward(int level, int stars) {
	// base reward: 10 coins per level
	const int base_reward = level * 10;
	// 20 coins per star
	const int bonus_per_star = 20;

	int extra_coins = get_extra_coins();

	return base_reward + (stars * bonus_per_star) + extra_coins;
}

void reward_player(int reward) {
	add_coins(reward);
}

void scoring_init() {
	float win_width = (float) CP_System_GetWindowWidth();
	float win_height = (float) CP_System_GetWindowHeight();
	//------------------------------------------------------------------------------------------------
	//Initialise exit buttons
	scorebb[0].x = 0.35f * win_width;
	scorebb[0].y = 0.70f * win_height;
	//Button size and asset
	scorebb[0].width = 0.1f * win_width;
	scorebb[0].height = 0.1f * win_height;
	scorebb[0].button_img = Asset_GetImage(SB_BUTTON_GREY);

	//Button icon and asset
	scorebb[0].icon_width = 50.0f;
	scorebb[0].icon_height = 50.0f;
	scorebb[0].button_icon = Asset_GetImage(RED_CROSS);
	//------------------------------------------------------------------------------------------------
	//Initialise Replay buttons
	scorebb[1].x = 0.5f * win_width;
	scorebb[1].y = 0.70f * win_height;

	//Button size and asset
	scorebb[1].width = 0.1f * win_width;
	scorebb[1].height = 0.1f * win_height;
	scorebb[1].button_img = Asset_GetImage(SB_BUTTON_YELLOW);

	//Button icon and asset
	scorebb[1].icon_width = 50.0f;
	scorebb[1].icon_height = 50.0f;
	scorebb[1].button_icon = Asset_GetImage(DARK_REPLAY_BUTTON);

	//------------------------------------------------------------------------------------------------
	//Initialise Next Lvl buttons
	scorebb[2].x = 0.65f * win_width;
	scorebb[2].y = 0.70f * win_height;

	//Button size and asset
	scorebb[2].width = 0.1f * win_width;
	scorebb[2].height = 0.1f * win_height;
	scorebb[2].button_img = Asset_GetImage(SB_BUTTON_GREEN);

	//Button icon and asset
	scorebb[2].icon_width = 50.0f;
	scorebb[2].icon_height = 50.0f;
	scorebb[2].button_icon = Asset_GetImage(DARK_PLAY_BUTTON);
	//------------------------------------------------------------------------------------------------
	//Initialise 3 stars
	scoress[0].x = 0.35f * win_width;
	scoress[0].y = 0.35f * win_height;
	scoress[0].width = 0.15f * win_width;
	scoress[0].height = 0.20f * win_height;
	scoress[0].img = Asset_GetImage(GREY_STAR);

	scoress[1].x = 0.5f * win_width;
	scoress[1].y = 0.30f * win_height;
	scoress[1].width = 0.15f * win_width;
	scoress[1].height = 0.20f * win_height;
	scoress[1].img = Asset_GetImage(GREY_STAR);

	scoress[2].x = 0.65f * win_width;
	scoress[2].y = 0.35f * win_height;
	scoress[2].width = 0.15f * win_width;
	scoress[2].height = 0.20f * win_height;
	scoress[2].img = Asset_GetImage(GREY_STAR);
	//------------------------------------------------------------------------------------------------								
	//Initialise 3 Gold stars YELLOW_STAR
	scoregs[0].x = 0.35f * win_width;
	scoregs[0].y = 0.35f * win_height;
	scoregs[0].width = 0.15f * win_width;
	scoregs[0].height = 0.20f * win_height;
	scoregs[0].img = Asset_GetImage(YELLOW_STAR);

	scoregs[1].x = 0.5f * win_width;
	scoregs[1].y = 0.30f * win_height;
	scoregs[1].width = 0.15f * win_width;
	scoregs[1].height = 0.20f * win_height;
	scoregs[1].img = Asset_GetImage(YELLOW_STAR);

	scoregs[2].x = 0.65f * win_width;
	scoregs[2].y = 0.35f * win_height;
	scoregs[2].width = 0.15f * win_width;
	scoregs[2].height = 0.20f * win_height;
	scoregs[2].img = Asset_GetImage(YELLOW_STAR);

	//------------------------------------------------------------------------------------------------
	//Initialise the text pos
	//Scoreboard text
	Text[0].x = 0.5f * win_width;
	Text[0].y = 0.165f * win_height;

	//Game win/lose pos


	//reset the clicks
	clickbb_x = 0; 
	clickbb_y = 0;

	sb_button_font = Asset_GetFont(CARTER_FONT_ASSET);

	CP_Font_Set(sb_button_font);
};

void scoring_update(void) {
	float win_width = (float)CP_System_GetWindowWidth();
	float win_height = (float)CP_System_GetWindowHeight();

	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
	{
		clickbb_x = CP_Input_GetMouseX();
		clickbb_y = CP_Input_GetMouseY();

		//click on exit
		if (IsAreaClicked(scorebb[0].x, scorebb[0].y, scorebb[0].width, scorebb[0].height, clickbb_x, clickbb_y) == 1) {
			reward_given = 0;
			GameState_SetNext(STATE_MAINMENU);
		}
		//click on replay
		if (IsAreaClicked(scorebb[1].x, scorebb[1].y, scorebb[1].width, scorebb[1].height, clickbb_x, clickbb_y) == 1) {
			//Set state to current level
			//Set_Current_Level(Get_Current_Level());
			reward_given = 0;
			GameState_SetNext(STATE_GAME);
		}
		//click on next level
		if (IsAreaClicked(scorebb[2].x, scorebb[2].y, scorebb[2].width, scorebb[2].height, clickbb_x, clickbb_y) == 1) {
			if (Game_GetLevel() == TOTAL_LEVELS) {
				reward_given = 0;
				GameState_SetNext(STATE_MAINMENU);
			}
			else {
				reward_given = 0;
				Game_SetLevel(Game_GetLevel() + 1);
				GameState_SetNext(STATE_GAME);
			}
		}
	}

	//Mouse hover
	hover_x = CP_Input_GetMouseX();
	hover_y = CP_Input_GetMouseY();
	//Button Animation
	float sbb_width, sbb_height, sbb_iheight, sbb_iwidth;
	sbb_width = 0.1f * win_width;
	sbb_height = 0.1f * win_height;
	sbb_iwidth = 50.0f;
	sbb_iheight = 50.0f;

	for (int i = 0; i < 3; ++i) {
		if (IsAreaClicked(scorebb[i].x, scorebb[i].y, scorebb[i].width, scorebb[i].height, hover_x, hover_y) == 1) {
			//button animation
			scorebb[i].width = CP_Math_LerpFloat(scorebb[i].width, sbb_width * 1.2f, CP_System_GetDt() * 10.0f);
			scorebb[i].height = CP_Math_LerpFloat(scorebb[i].height, sbb_height * 1.2f, CP_System_GetDt() * 10.0f);
			scorebb[i].icon_width = CP_Math_LerpFloat(scorebb[i].icon_width, sbb_iwidth * 1.2f, CP_System_GetDt() * 10.0f);
			scorebb[i].icon_height = CP_Math_LerpFloat(scorebb[i].icon_height, sbb_iheight * 1.2f, CP_System_GetDt() * 10.0f);
			GameSetting_SetCursor(CURSOR_POINT);
		}
		else {
			scorebb[i].width = CP_Math_LerpFloat(scorebb[i].width, sbb_width, CP_System_GetDt() * 10.0f);
			scorebb[i].height = CP_Math_LerpFloat(scorebb[i].height, sbb_height, CP_System_GetDt() * 10.0f);
			scorebb[i].icon_width = CP_Math_LerpFloat(scorebb[i].icon_width, sbb_iwidth, CP_System_GetDt() * 10.0f);
			scorebb[i].icon_height = CP_Math_LerpFloat(scorebb[i].icon_height, sbb_iheight, CP_System_GetDt() * 10.0f);
		}
	}

	//Moving stars
	float gs_width, gs_height;

	gs_width = 0.15f * win_width;
	gs_height = 0.20f * win_height;

	float star_mul = (sinf(5.f * CP_System_GetSeconds()) / 20.f) + 1.f;

	scoregs[0].width = gs_width * star_mul;
	scoregs[0].height = gs_height * star_mul;

	scoregs[1].width = gs_width * star_mul;
	scoregs[1].height = gs_height * star_mul;

	scoregs[2].width = gs_width * star_mul;
	scoregs[2].height = gs_height * star_mul;

	//Initialise the text pos
	//Scoreboard text
	Text[0].x = 0.5f * win_width;
	Text[0].y = 0.165f * win_height;

	char a[50];

	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Settings_TextSize(70.0f);
	sprintf_s(a, 50, "SCOREBOARD");
	CP_Font_DrawText(a, Text[0].x, Text[0].y);

}


void draw_hole(CP_Vector hole, float diameter, float world_width, float world_height, float animation) {
	CP_Settings_Stroke(Get_Color(COLOR_WHITE));
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Vector coords = World_To_Screen(hole.x, hole.y, world_width, world_height);
	CP_Graphics_DrawCircle(
		CP_Math_LerpFloat(coords.x, CP_System_GetWindowWidth() * 1.f, animation),
		coords.y, 
		diameter * CP_System_GetWindowWidth() / world_width);
}

void scoreboard_exit(void) {

}

void add_stroke(void) {
	stroke = stroke + 1;
}

void reset_strokes(void) {
	stroke = 0;
}

int get_strokes(void) {
	return stroke;
}

void score(float x, float y) {
	//define all the scoring variables
	bogey = lvl_par + 1;
	d_bogey = lvl_par + 2;
	t_bogey = lvl_par + 3;
	birdie = lvl_par - 1;
	eagle = lvl_par - 2;
	albatross = lvl_par - 3;

	int stroke = get_strokes();
	char w[50];
	if (stroke == lvl_par) {
		sprintf_s(w, 50, "You got a par!!");
	}
	else if (stroke == 1) {
		sprintf_s(w, 50, "You got a hole in one!!");
	}
	else if (stroke == bogey) {
		sprintf_s(w, 50, "You got a bogey");
	}
	else if (stroke == d_bogey) {
		sprintf_s(w, 50, "You got a double bogey");
	}
	else if (stroke == t_bogey) {
		sprintf_s(w, 50, "You got a double bogey");
	}
	else if (stroke == birdie) {
		sprintf_s(w, 50, "You got a birdie");
	}
	else if (stroke == eagle) {
		sprintf_s(w, 50, "You got an eagle");
	}
	else if (stroke == albatross) {
		sprintf_s(w, 50, "You got a albatross");
	}
	else {
		sprintf_s(w, 50, "You suck lol");
	}
	CP_Font_DrawText(w, x, y);
}

void draw_scoreboard(CP_Vector sBoard, float world_width, float world_height) {

	//Use if Game_LevelWon(void);
	//Use if Game_levelLost(void)


	CP_Font_Set(sb_button_font);
	float win_width = (float)CP_System_GetWindowWidth();
	float win_height = (float)CP_System_GetWindowHeight();

	//Make sure reward given only once
	int stars = 0;
	if (get_strokes() <= get_lvl_par() - 1) {
		stars = 3;
	}
	else if (get_strokes() == get_lvl_par()) {
		stars = 2;
	}
	else {
		stars = 1;
	}

	//Give coins once per sb
	if (!reward_given) {
		// get current level
		int level = Game_GetLevel();
		int reward = calculate_reward(level, stars);
		reward_player(reward);
		// store reward for UI
		coins_earned_last_level = reward;
		// prevent duplicate reward
		reward_given = 1;
	}

	//Load all images
	CP_Image sbb_bg = Asset_GetImage(SCOREBOARD_BG);
	CP_Image stroke_bg = Asset_GetImage(STROKE_BG);
	CP_Image sbb_h = Asset_GetImage(BANNER);

	CP_Settings_Fill(Get_Color(COLOR_WHITE));
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Vector coords = World_To_Screen(sBoard.x, sBoard.y, world_width, world_height);

	//Drawing the scoreboard background
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	CP_Image_DrawAdvanced(
		sbb_bg,
		coords.x,
		coords.y,
		0.50f * win_width,
		0.65f * win_height,
		255,
		0.0f);

	if (Game_LevelWon()) {
		CP_Settings_TextAlignment(coords.x, coords.y);
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		char b[50];
		char a[100];
		//Write strokes
		/*sprintf_s(b, 50, "Strokes: %d", stroke);
		CP_Image_DrawAdvanced(stroke_bg, coords.x, coords.y - 20.0f, 0.3f * win_width , 0.2f * win_height, 255, 0.0f);
		CP_Settings_TextAlignment(coords.x, coords.y);
		score();
		CP_Font_DrawText(b, coords.x, coords.y);*/

		//Change level clear colour based on user score.
		CP_Settings_TextSize(60.0f);
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		sprintf_s(b, 50, "LEVEL CLEARED!");
		CP_Font_DrawText(b, coords.x, coords.y);
		//score(coords.x, coords.y *1.1f);

		char coin_info[100];

		// Showing coins earned
		CP_Settings_TextSize(40.0f);
		sprintf_s(coin_info, 50, "Coins Earned: +%d", coins_earned_last_level);
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Font_DrawText(coin_info, coords.x, coords.y * 1.13f);

		// Showing total coins
		sprintf_s(coin_info, 50, "Total Coins:  %d", getCoins());
		CP_Font_DrawText(coin_info, coords.x, coords.y * 1.23f);

		CP_Settings_Fill(Get_Color(COLOR_WHITE));

		//Draw level buttons
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Image_DrawAdvanced(scorebb[2].button_img, scorebb[2].x, scorebb[2].y, scorebb[2].width, scorebb[2].height, 255, 0.0f);
		CP_Image_DrawAdvanced(scorebb[2].button_icon, scorebb[2].x, scorebb[2].y, scorebb[2].icon_width, scorebb[2].icon_height, 255, 0.0f);
		/*sprintf_s(a, 50, "NEXT LEVEL");
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Font_DrawText(a, scorebb[2].x, scorebb[2].y);*/

		//Draw exit buttons
		CP_Settings_Fill(Get_Color(COLOR_WHITE));
		CP_Image_DrawAdvanced(scorebb[0].button_img, scorebb[0].x, scorebb[0].y, scorebb[0].width, scorebb[0].height, 255, 0.0f);
		CP_Image_DrawAdvanced(scorebb[0].button_icon, scorebb[0].x, scorebb[0].y, scorebb[0].icon_width, scorebb[0].icon_height, 255, 0.0f);
		/*sprintf_s(a, 50, "EXIT");
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Font_DrawText(a, scorebb[0].x, scorebb[0].y);*/

		//Draw Replay button
		CP_Settings_Fill(Get_Color(COLOR_WHITE));
		CP_Image_DrawAdvanced(scorebb[1].button_img, scorebb[1].x, scorebb[1].y, scorebb[1].width, scorebb[1].height, 255, 0.0f);
		CP_Image_DrawAdvanced(scorebb[1].button_icon, scorebb[1].x, scorebb[1].y, scorebb[1].icon_width, scorebb[1].icon_height, 255, 0.0f);
		/*sprintf_s(a, 50, "REPLAY");
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Font_DrawText(a, scorebb[1].x, scorebb[1].y);*/

		//Write scoreboard with banner
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Image_DrawAdvanced(sbb_h, 0.5f * win_width, 0.15f * win_height, 0.65f * win_width, 0.15f * win_height, 255, 0.0f);
		CP_Settings_TextSize(70.0f);
		sprintf_s(a, 50, "SCOREBOARD");
		CP_Font_DrawText(a, Text[0].x, Text[0].y);

		//Drawing the grey star points based on the points they get.
		CP_Image_DrawAdvanced(scoress[0].img, scoress[0].x, scoress[0].y, scoress[0].width, scoress[0].height, 255, 335.0f);
		CP_Image_DrawAdvanced(scoress[1].img, scoress[1].x, scoress[1].y, scoress[1].width, scoress[1].height, 255, 0.0f);
		CP_Image_DrawAdvanced(scoress[2].img, scoress[2].x, scoress[2].y, scoress[2].width, scoress[2].height, 255, 25.0f);


		//Drawing Yellow stars

		if (get_strokes() > get_lvl_par()) {
			CP_Image_DrawAdvanced(scoregs[0].img, scoregs[0].x, scoregs[0].y, scoregs[0].width, scoregs[0].height, 255, 335.0f);
		}
		else if (get_strokes() == get_lvl_par()) {
			CP_Image_DrawAdvanced(scoregs[0].img, scoregs[0].x, scoregs[0].y, scoregs[0].width, scoregs[0].height, 255, 335.0f);
			CP_Image_DrawAdvanced(scoregs[1].img, scoregs[1].x, scoregs[1].y, scoregs[1].width, scoregs[1].height, 255, 0.0f);
		}
		else if (get_strokes() < get_lvl_par()) {
			CP_Image_DrawAdvanced(scoregs[0].img, scoregs[0].x, scoregs[0].y, scoregs[0].width, scoregs[0].height, 255, 335.0f);
			CP_Image_DrawAdvanced(scoregs[1].img, scoregs[1].x, scoregs[1].y, scoregs[1].width, scoregs[1].height, 255, 0.0f);
			CP_Image_DrawAdvanced(scoregs[2].img, scoregs[2].x, scoregs[2].y, scoregs[2].width, scoregs[2].height, 255, 25.0f);
		}
	}

	else if (Game_LevelLost()) {
		//Draw the game over screen
		CP_Settings_TextAlignment(coords.x, coords.y);
		char a[100];

		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Image_DrawAdvanced(sbb_h, 0.5f * win_width, 0.15f * win_height, 0.65f * win_width, 0.15f * win_height, 255, 0.0f);
		CP_Settings_TextSize(70.0f);
		sprintf_s(a, 50, "SCOREBOARD");
		CP_Font_DrawText(a, Text[0].x, Text[0].y);


		CP_Settings_TextSize(60.0f);
		//Say Times up!, you lost
		//try again
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		/*sprintf_s(a, 50, "TIMES UP, YOU LOST!");
		CP_Font_DrawText(a, 0.5f * win_width, 0.5f * win_height);*/

		sprintf_s(a, 50, "TIMES UP!");
		CP_Font_DrawText(a, 0.5f * win_width, 0.55f * win_height);

		//Draw level buttons
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Image_DrawAdvanced(scorebb[2].button_img, scorebb[2].x, scorebb[2].y, scorebb[2].width, scorebb[2].height, 255, 0.0f);
		CP_Image_DrawAdvanced(scorebb[2].button_icon, scorebb[2].x, scorebb[2].y, scorebb[2].icon_width, scorebb[2].icon_height, 255, 0.0f);
		/*sprintf_s(a, 50, "NEXT LEVEL");
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Font_DrawText(a, scorebb[2].x, scorebb[2].y);*/

		//Draw exit buttons
		CP_Settings_Fill(Get_Color(COLOR_WHITE));
		CP_Image_DrawAdvanced(scorebb[0].button_img, scorebb[0].x, scorebb[0].y, scorebb[0].width, scorebb[0].height, 255, 0.0f);
		CP_Image_DrawAdvanced(scorebb[0].button_icon, scorebb[0].x, scorebb[0].y, scorebb[0].icon_width, scorebb[0].icon_height, 255, 0.0f);
		/*sprintf_s(a, 50, "EXIT");
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Font_DrawText(a, scorebb[0].x, scorebb[0].y);*/

		//Draw Replay button
		CP_Settings_Fill(Get_Color(COLOR_WHITE));
		CP_Image_DrawAdvanced(scorebb[1].button_img, scorebb[1].x, scorebb[1].y, scorebb[1].width, scorebb[1].height, 255, 0.0f);
		CP_Image_DrawAdvanced(scorebb[1].button_icon, scorebb[1].x, scorebb[1].y, scorebb[1].icon_width, scorebb[1].icon_height, 255, 0.0f);
		/*sprintf_s(a, 50, "REPLAY");
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Font_DrawText(a, scorebb[1].x, scorebb[1].y);*/

		//Draw empty grey stars
		CP_Image_DrawAdvanced(scoress[0].img, scoress[0].x, scoress[0].y, scoress[0].width, scoress[0].height, 255, 335.0f);
		CP_Image_DrawAdvanced(scoress[1].img, scoress[1].x, scoress[1].y, scoress[1].width, scoress[1].height, 255, 0.0f);
		CP_Image_DrawAdvanced(scoress[2].img, scoress[2].x, scoress[2].y, scoress[2].width, scoress[2].height, 255, 25.0f);
	}


}




void set_lvl_par(int par) {
	lvl_par = par;
}

int get_lvl_par(void) {
	return lvl_par;
}


