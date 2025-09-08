#include "cprocessing.h"
#include "game.h"
#include "physics.h"
#include <stdio.h>

typedef struct Wall {
	CP_Vector pos;
	float rot;
	CP_Vector scale;
} Wall;

CP_Font font;
CP_Vector mouse = { 0, 0 };
CP_Vector golf_pos = { 0, 0 };
CP_Vector golf_vel = { 0, 0 };
float golf_radius;
Wall walls[1];
float world_width = 50.f, world_height = 50.f;

void to_string(int f, const char* buffer)
{
	sprintf_s(buffer, 50, "%d", f);
}

CP_Vector world_to_screen(float x, float y) {
	CP_Vector a = { x * (CP_System_GetDisplayWidth() / world_width), CP_System_GetDisplayHeight() - (y * (CP_System_GetDisplayHeight() / world_height)) };
	return a;
}

CP_Vector screen_to_world(float x, float y) {
	CP_Vector a = { x * (world_width / CP_System_GetDisplayWidth()), world_height - (y * (world_height / CP_System_GetDisplayHeight())) };
	return a;
}

void debug_draw(void)
{
	CP_Settings_Scale(1.f, 1.f);
	char fps[50];
	sprintf_s(fps, 50, "FPS: %f", 1 / CP_System_GetDt());
	CP_Font_DrawText(fps, 512, 20);

	char m[50];
	int x = mouse.x;
	int y = mouse.y;
	sprintf_s(m, 50, "Mouse: (%d, %d)", x, y);
	CP_Font_DrawText(m, 512, 40);

	CP_Vector world = screen_to_world(mouse.x, mouse.y);
	char wm[50];
	sprintf_s(wm, 50, "World Mouse: (%f, %f)", world.x, world.y);
	CP_Font_DrawText(wm, 512, 60);

	CP_Vector back = world_to_screen(world.x, world.y);
	char b[50];
	sprintf_s(b, 50, "Back to screen: (%f, %f)", back.x, back.y);
	CP_Font_DrawText(b, 512, 80);
}


void game_init(void)
{
	font = CP_Font_Load("Assets/arial.ttf");
	CP_Font_Set(font);
	golf_radius = 2.f;

	golf_vel.x = 1.f;
	golf_vel.y = 5.f;
	
	golf_pos.x = 0.f;
	golf_pos.y = 0.f;

	walls[0].pos.x = 25;
	walls[0].pos.y = 25;
	walls[0].scale.x = 1;
	walls[0].scale.y = 5;
	walls[0].rot = 5;

	walls[1].pos.x = 10;
	walls[1].pos.y = 10;
	walls[1].scale.x = 1;
	walls[1].scale.y = 5;
	walls[1].rot = 45;
	CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
}

void game_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
	mouse.x = CP_Input_GetMouseX();
	mouse.y = CP_Input_GetMouseY();

	CP_Vector vel = CP_Vector_Scale(golf_vel, CP_System_GetDt());
	golf_pos = CP_Vector_Add(golf_pos, vel);

	// Render
	CP_Vector coords = world_to_screen(golf_pos.x, golf_pos.y);
	CP_Graphics_DrawCircle(coords.x, coords.y, golf_radius * CP_System_GetDisplayWidth() / world_width);

	for (int i = 0; i < 2; i++) {
		Wall w = walls[i];
		CP_Vector c = world_to_screen(w.pos.x - (w.scale.x / 2.f), w.pos.y - (w.scale.y / 2.f));
		CP_Graphics_DrawRectAdvanced(c.x, c.y, w.scale.x * (CP_System_GetDisplayWidth() / world_width), w.scale.y * (CP_System_GetDisplayWidth() / world_width), w.rot, 0.f);
	}

	debug_draw();

	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		CP_Engine_Terminate();
	}
}

void game_exit(void)
{
	CP_Font_Free(&font);
}