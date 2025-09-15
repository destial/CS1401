#include "scoring.h"
#include "cprocessing.h"
#include "physics.h"
#include <stdio.h>

int stroke = 0;
void draw_hole(CP_Vector hole, float world_width, float world_height) {

	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));

	CP_Vector coords = world_to_screen(hole.x, hole.y, world_width, world_height);
	CP_Graphics_DrawCircle(
		coords.x, 
		coords.y, 
		1.5f * CP_System_GetWindowWidth() / world_width);
}

void add_stroke(void) {
	stroke = stroke + 1;
}

int get_strokes(void) {
	return stroke;
}

void draw_scoreboard(CP_Vector sBoard, float world_width, float world_height) {
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

	CP_Vector coords = world_to_screen(sBoard.x, sBoard.y, world_width, world_height);
	CP_Graphics_DrawRect(
		coords.x,
		coords.y,
		20.0f * CP_System_GetWindowWidth() / world_width,
		7.f * CP_System_GetWindowHeight() / world_height);


	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	char b[50];
	sprintf_s(b, 50, "Strokes: %d", stroke);
	CP_Font_DrawText(b, coords.x, coords.y);
}