#include "scoring.h"
#include "cprocessing.h"
#include "utils.h"
#include <stdio.h>
#include "main_menu.h"
#include "game.h"
#include "asset_manager.h"
#include "sprite_manager.h"
#include "end_state.h"
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

typedef struct Buttonbb {
	CP_Image button_img;
	CP_Image button_icon;
	float x;
	float y;
	float width;
	float height;
} bb;

typedef struct Star {
	CP_Image img;
	float x;
	float y;
	float width;
	float height;
} star;
//CP_Graphics_DrawRect(0.3 * CP_System_GetWindowWidth(), 0.7f * CP_System_GetWindowHeight(), 8.0f * CP_System_GetWindowWidth() / world_width, 4.f * CP_System_GetWindowHeight() / world_height);
//Draw exit buttons

bb scorebb[3];
star scoress[3];

CP_Color white_colour;



int IsAreaClicked(float area_center_x, float area_center_y, float area_width, float area_height, float click_x, float click_y)
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
void scoring_init() {
	float win_width = CP_System_GetWindowWidth();
	float win_height = CP_System_GetWindowHeight();
	//Initialise exit buttons
	scorebb[0].x = 0.2f * win_width;
	scorebb[0].y = 0.7f * win_height;
	scorebb[0].width = 0.2f * win_width;
	scorebb[0].height = 0.1f * win_height;
	scorebb[0].button_img = get_image(BUTTON_GREY_ASSET);
	scorebb[0].button_icon = get_image(RED_CROSS);

	//Initialise Replay buttons
	scorebb[1].x = 0.5f * win_width;
	scorebb[1].y = 0.7f * win_height;
	scorebb[1].width = 0.2f * win_width;
	scorebb[1].height = 0.1f * win_height;
	scorebb[1].button_img = get_image(BUTTON_YELLOW_ASSET);
	scorebb[1].button_icon = get_image(DARK_REPLAY_BUTTON);

	//Initialise Next Lvl buttons
	scorebb[2].x = 0.8f * win_width;
	scorebb[2].y = 0.7f * win_height;
	scorebb[2].width = 0.2f * win_width;
	scorebb[2].height = 0.1f * win_height;
	scorebb[2].button_img = get_image(BUTTON_BLUE_ASSET);
	scorebb[2].button_icon = get_image(DARK_PLAY_BUTTON);

	//Initialise 3 stars
	scoress[0].x = 0.5f * win_width;
	scoress[0].y = 0.2f * win_height;
	scoress[0].width = 0.1f * win_width;
	scoress[0].height = 0.1f * win_height;
	scoress[0].img = get_image(GREY_STAR);

	int stroke = 0;
	int lvl_par = 4;
	int bogey, birdie, eagle, d_bogey, t_bogey, albatross;
	CP_Color white_colour = CP_Color_Create(255, 255, 255, 255);
	//reset the clicks
	clickbb_x = 0; 
	clickbb_y = 0;

	
};


void draw_hole(CP_Vector hole, float diameter, float world_width, float world_height, float animation) {
	CP_Settings_Stroke(Get_Color(COLOR_WHITE));
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Vector coords = World_To_Screen(hole.x, hole.y, world_width, world_height);
	CP_Graphics_DrawCircle(
		CP_Math_LerpFloat(coords.x, CP_System_GetWindowWidth() * 1.f, animation),
		coords.y, 
		diameter * CP_System_GetWindowWidth() / world_width);
}

void scoring_update(void) {
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
	{
		clickbb_x = CP_Input_GetMouseX();
		clickbb_y = CP_Input_GetMouseY();

		//click on exit
		if (IsAreaClicked(scorebb[0].x, scorebb[0].y, scorebb[0].width, scorebb[0].height, clickbb_x, clickbb_y) == 1) {
			CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
		}
		//click on replay
		if (IsAreaClicked(scorebb[1].x, scorebb[1].y, scorebb[1].width, scorebb[1].height, clickbb_x, clickbb_y) == 1) {
			//Set state to current level
			Set_Current_Level(Get_Current_Level());
			CP_Engine_SetNextGameStateForced(game_init, game_update, game_exit);
		}
		//click on next level
		if (IsAreaClicked(scorebb[2].x, scorebb[2].y, scorebb[2].width, scorebb[2].height, clickbb_x, clickbb_y) == 1) {
			Set_Current_Level(Get_Current_Level() + 1);
			CP_Engine_SetNextGameStateForced(game_init, game_update, game_exit);

		}

	}

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

void score(void) {
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
		sprintf_s(w, 50, "You got a par");
	}
	else if (stroke == 1) {
		sprintf_s(w, 50, "You got a hole in one");
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
	CP_Font_DrawText(w, 800, 400);
}


//Set the buttons to go back to main menu, replay level, next level
void scoreboard_buttons(void) {
	//defining the individual buttons
	sbButton[0].button_image = CP_Image_Load("Assets/Images/Play Button.png");
	sbButton[0].scale.x = 6.0f;
	sbButton[0].scale.y = 2.0f;
	sbButton[0].pos.x = 800;
	sbButton[0].pos.y = 500;
	//sbButton[0].func = change_screen;
}



void draw_scoreboard(CP_Vector sBoard, float world_width, float world_height) {
	CP_Image sbb_bg = get_image(SCOREBOARD_BG);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Vector coords = World_To_Screen(sBoard.x, sBoard.y, world_width, world_height);
	CP_Image_DrawAdvanced(
		sbb_bg,
		coords.x,
		coords.y,
		40.0f * CP_System_GetWindowWidth() / world_width,
		20.f * CP_System_GetWindowHeight() / world_height, 
		255, 
		0.0f);

	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	char b[50];
	char a[100];
	sprintf_s(b, 50, "Strokes: %d", stroke);
	CP_Settings_TextAlignment(coords.x, coords.y);
	score();
	CP_Font_DrawText(b, coords.x, coords.y);

	//Write scoreboard
	sprintf_s(a, 50, "SCOREBOARD");
	CP_Font_DrawText(a, 0.5 * CP_System_GetWindowWidth(), 0.3f * CP_System_GetWindowHeight());

	CP_Settings_Fill(Get_Color(COLOR_WHITE));

	//Draw level buttons
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Image_DrawAdvanced(scorebb[2].button_img, scorebb[2].x, scorebb[2].y, scorebb[2].width, scorebb[2].height, 255, 0.0f);
	CP_Image_DrawAdvanced(scorebb[2].button_icon, scorebb[2].x - 100.0f, scorebb[2].y - 10.0f , 50, 50, 255, 0.0f);
	sprintf_s(a, 50, "NEXT LEVEL");
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Font_DrawText(a, scorebb[2].x, scorebb[2].y);

	//Draw exit buttons
	CP_Settings_Fill(Get_Color(COLOR_WHITE));
	/*CP_Graphics_DrawRect(exit.x,exit.y, exit.width ,exit.height);*/
	CP_Image_DrawAdvanced(scorebb[0].button_img, scorebb[0].x, scorebb[0].y, scorebb[0].width, scorebb[0].height, 255, 0.0f);
	sprintf_s(a, 50, "EXIT");
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Font_DrawText(a, scorebb[0].x, scorebb[0].y);

	//Draw Replay button
	CP_Settings_Fill(Get_Color(COLOR_WHITE));
	CP_Image_DrawAdvanced(scorebb[1].button_img, scorebb[1].x, scorebb[1].y, scorebb[1].width, scorebb[1].height, 255, 0.0f);
	sprintf_s(a, 50, "REPLAY");
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Font_DrawText(a, scorebb[1].x, scorebb[1].y);


	
	//Write scoreboard
	sprintf_s(a, 50, "SCOREBOARD");
	CP_Font_DrawText(a, 0.5 * CP_System_GetWindowWidth(), 0.3f * CP_System_GetWindowHeight());

	//Drawing the grey star points based on the points they get.
	CP_Image_DrawAdvanced(scoress[0].img, scoress[0].x, scoress[0].y, scoress[0].width, scoress[0].height, 255, 0.0f);


}


void set_lvl_par(int par) {
	lvl_par = par;
}

int get_lvl_par(void) {
	return lvl_par;
}
