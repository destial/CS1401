#include "scoring.h"
#include "cprocessing.h"
#include "physics.h"
#include <stdio.h>
//Initialise the variables
int stroke = 0;
int lvl_par = 4;
int bogey, birdie, eagle, d_bogey, t_bogey, albatross;

void draw_hole(CP_Vector hole, float diameter, float world_width, float world_height, float animation) {
	CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Vector coords = world_to_screen(hole.x, hole.y, world_width, world_height);
	CP_Graphics_DrawCircle(
		CP_Math_LerpFloat(coords.x, CP_System_GetWindowWidth(), animation),
		coords.y, 
		diameter * CP_System_GetWindowWidth() / world_width);
}

void add_stroke(void) {
	stroke = stroke + 1;
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

void draw_scoreboard(CP_Vector sBoard, float world_width, float world_height) {
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_ImageMode(CP_POSITION_CORNER);

	CP_Vector coords = world_to_screen(sBoard.x, sBoard.y, world_width, world_height);
	CP_Graphics_DrawRect(
		coords.x,
		coords.y,
		20.0f * CP_System_GetWindowWidth() / world_width,
		7.f * CP_System_GetWindowHeight() / world_height);


	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	char b[50];
	sprintf_s(b, 50, "Strokes: %d", stroke);
	CP_Settings_TextAlignment(coords.x, coords.y);
	score();
	CP_Font_DrawText(b, coords.x, coords.y);
}

//char is_win(void) {
//	if golf
//}
