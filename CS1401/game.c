#include "cprocessing.h"
#include "game.h"
#include "physics.h"
#include <stdio.h>

typedef struct Wall {
	CP_Vector pos;
	CP_Vector scale;
	CP_Vector normal;

	float angle;
} Wall;

CP_Font font;
CP_Vector mouse = { 0, 0 };
CP_Vector golf_pos = { 0, 0 };
CP_Vector golf_vel = { 0, 0 };
float golf_radius;

float world_width = 50.f, world_height = 50.f;
static const int WALLS_AMOUNT = 5;
Wall walls[5];

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

void debug_draw(void) {
	CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
	CP_Settings_Scale(1.f, 1.f);
	char fps[50];
	sprintf_s(fps, 50, "FPS: %f", 1 / CP_System_GetDt());
	CP_Font_DrawText(fps, 0, 20);

	char m[50];
	int x = mouse.x;
	int y = mouse.y;
	sprintf_s(m, 50, "Screen Mouse: (%d, %d)", x, y);
	CP_Font_DrawText(m, 0, 40);

	CP_Vector world = screen_to_world(mouse.x, mouse.y);
	char wm[50];
	sprintf_s(wm, 50, "World Mouse: (%f, %f)", world.x, world.y);
	CP_Font_DrawText(wm, 0, 60);
}


void game_init(void) {
	font = CP_Font_Load("Assets/arial.ttf");
	CP_Settings_TextSize(20.f);
	CP_Font_Set(font);
	golf_radius = 1.f;

	golf_vel.x = 1.f;
	golf_vel.y = 5.f;
	
	golf_pos.x = 0.f;
	golf_pos.y = 0.f;

	for (int i = 0; i < WALLS_AMOUNT; i++) {
		walls[i].pos.x = CP_Random_RangeFloat(5.f, 45.f);
		walls[i].pos.y = CP_Random_RangeFloat(5.f, 45.f);

		walls[i].scale.x = 1.f;
		walls[i].scale.y = 1.f;

		walls[i].angle = CP_Random_RangeFloat(0.f, 360.f);
	}
}

// dotproduct: 1, they are facing the exact same direction (Parallel)
// = 0, they are perpendicular
// = -1, they are facing the exact opposite direction
// > 0, they are facing relatively the same direction
// < 0, they are facing relatively opposite directions
char check_collision(Wall* wall) {
	CP_Vector distance = CP_Vector_Subtract(wall->pos, golf_pos);
	CP_Vector normal = wall->normal;
	CP_Vector right = { normal.y, -normal.x };
	if (CP_Vector_DotProduct(distance, normal) < 0) {
		normal = CP_Vector_Negate(normal);
	}

	if (CP_Vector_DotProduct(distance, normal) < golf_radius + (wall->scale.x / 2.f)
		&&
		abs(CP_Vector_DotProduct(distance, right)) < (wall->scale.y / 2.f)
		) {
		return 1;
	}
	return 0;
}

void collision_response(Wall* wall) {

}

void game_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
	mouse.x = CP_Input_GetMouseX();
	mouse.y = CP_Input_GetMouseY();

	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		CP_Engine_Terminate();
	}

	if (CP_Input_KeyDown(KEY_W)) {
		golf_vel.y += 25 * CP_System_GetDt();
	}
	if (CP_Input_KeyDown(KEY_S)) {
		golf_vel.y += -25 * CP_System_GetDt();
	}

	if (CP_Input_KeyDown(KEY_A)) {
		golf_vel.x += -25 * CP_System_GetDt();
	}

	if (CP_Input_KeyDown(KEY_D)) {
		golf_vel.x += 25 * CP_System_GetDt();
	}

	if (CP_Vector_Length(golf_vel) > 0.1f) {
		CP_Vector vel = CP_Vector_Scale(golf_vel, CP_System_GetDt());
		golf_vel = CP_Vector_Subtract(golf_vel, vel);
		golf_pos = CP_Vector_Add(golf_pos, vel);
	}

	// Render
	CP_Vector coords = world_to_screen(golf_pos.x, golf_pos.y);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Graphics_DrawCircle(coords.x, coords.y, golf_radius * CP_System_GetDisplayWidth() / world_width);

	CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
	char p[50];
	sprintf_s(p, 50, "Golf Pos: (%f, %f)", golf_pos.x, golf_pos.y);
	CP_Font_DrawText(p, coords.x, coords.y + 40);

	char b[50];
	sprintf_s(b, 50, "Golf Vel: (%f, %f)", golf_vel.x, golf_vel.y);
	CP_Font_DrawText(b, coords.x, coords.y + 60);

	// rot += 1.f;
	for (int i = 0; i < WALLS_AMOUNT; i++) {
		Wall* w = &walls[i];
		w->angle += 10.f * CP_System_GetDt();
		if (w->angle > 360.f) {
			w->angle = 360.f - w->angle;
		}
		w->normal = CP_Vector_Set(sin(CP_Math_Radians(w->angle)), cos(CP_Math_Radians(w->angle)));

		CP_Vector c = world_to_screen(w->pos.x - (w->scale.x / 2.f), w->pos.y - (w->scale.y / 2.f));

		// draw wall
		CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
		CP_Settings_Fill(CP_Color_Create(250, 250, 250, 255));
		CP_Graphics_DrawRectAdvanced(c.x, c.y, w->scale.x / 2.f * (CP_System_GetDisplayWidth() / world_width), w->scale.y / 2.f * (CP_System_GetDisplayWidth() / world_width), w->angle, 0.f);

		// draw normal
		CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255));
		CP_Settings_Stroke(CP_Color_Create(0, 0, 255, 255));
		CP_Graphics_DrawLine(c.x, c.y, c.x + w->normal.x * world_width, c.y - w->normal.y * world_width);

		// draw normal ortho
		CP_Settings_Fill(CP_Color_Create(255, 0, 255, 255));
		CP_Settings_Stroke(CP_Color_Create(255, 0, 255, 255));
		CP_Graphics_DrawLine(c.x, c.y, c.x + w->normal.y * world_width, c.y + w->normal.x * world_width);

		// check collision detection
		char collided = check_collision(w);

		// draw line from ball
		CP_Color color = collided ? CP_Color_Create(0, 200, 255, 255) : CP_Color_Create(255, 0, 0, 255);
		CP_Settings_Fill(color);
		CP_Settings_Stroke(color);
		CP_Graphics_DrawLine(c.x, c.y, coords.x, coords.y);
	}

	debug_draw();
}

void game_exit(void)
{
	CP_Font_Free(&font);
}