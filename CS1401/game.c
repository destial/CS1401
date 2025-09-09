#include <stdio.h>
#include "game.h"
#include "math.h"
#include "physics.h"

CP_Font font;
CP_Vector mouse = { 0, 0 };
CP_Vector golf_pos = { 0, 0 };
CP_Vector golf_vel = { 0, 0 };
float golf_diameter;
int debug_enabled = 1;
int dragging = 0;
float gravity = -10.f;

const float world_width = 48, world_height = 27; // 16x9 aspect ratio cuz full screen
static const int WALLS_AMOUNT = 10;
Wall walls[10];
int COLLISION_MAX_POINTS_DEBUG = 500;
CP_Vector collision_points[500];
int collision_points_length = 0;
int collision_points_counter = 0;

void update_wall(Wall* wall) {
	wall->normal.x = cos(CP_Math_Radians(-wall->angle));
	wall->normal.y = sin(CP_Math_Radians(-wall->angle));

	wall->left.x = -wall->normal.y;
	wall->left.y = wall->normal.x;
}

void debug_draw(void) {
	CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
	CP_Settings_Scale(1.f, 1.f);
	char fps[50];
	sprintf_s(fps, 50, "FPS: %d", (int) (1 / CP_System_GetDt()));
	CP_Font_DrawText(fps, 0, 20);

	char m[50];
	int x = mouse.x;
	int y = mouse.y;
	sprintf_s(m, 50, "Screen Mouse: (%d, %d)", x, y);
	CP_Font_DrawText(m, 0, 40);

	CP_Vector world = screen_to_world(mouse.x, mouse.y, world_width, world_height);
	char wm[50];
	sprintf_s(wm, 50, "World Mouse: (%f, %f)", world.x, world.y);
	CP_Font_DrawText(wm, 0, 60);

	CP_Vector coords = world_to_screen(golf_pos.x, golf_pos.y, world_width, world_height);
	CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
	char p[50];
	sprintf_s(p, 50, "Golf Pos: (%f, %f)", golf_pos.x, golf_pos.y);
	CP_Font_DrawText(p, coords.x, coords.y + 40);

	char b[50];
	sprintf_s(b, 50, "Golf Vel: (%f, %f)", golf_vel.x, golf_vel.y);
	CP_Font_DrawText(b, coords.x, coords.y + 60);
}

void game_init(void) {
	font = CP_Font_Load("Assets/esl.ttf");
	CP_Settings_TextSize(20.f);
	CP_Font_Set(font);
	golf_diameter = 2.f;

	golf_pos.x = world_width / 2.f;
	golf_pos.y = world_height;
	
	golf_vel.x = 0.f;
	golf_vel.y = 0.f;

	for (int i = 0; i < WALLS_AMOUNT; i++) {
		walls[i].pos.x = CP_Random_RangeFloat(5.f, world_width - 5.f);
		walls[i].pos.y = CP_Random_RangeFloat(5.f, world_height - 5.f);

		walls[i].scale.x = 2.f;
		walls[i].scale.y = 2.f;

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
	float distance_length = CP_Vector_Length(distance);
	CP_Vector normal = { wall->normal.x, wall->normal.y };
	CP_Vector left = { wall->left.x, wall->left.y };
	if (CP_Vector_DotProduct(distance, normal) < 0) {
		normal = CP_Vector_Negate(normal);
	}

	float dot = CP_Vector_DotProduct(distance, normal);
	float dot_left = CP_Vector_DotProduct(distance, left);

	char collision = 0;
	if (dot < (golf_diameter / 2.f) + (wall->scale.x / 2.f)
		&&
		abs(dot_left) < (wall->scale.y / 2.f)) {
		collision = 1;
	}

	if (collision) {
		if (collision_points_counter >= COLLISION_MAX_POINTS_DEBUG)
			collision_points_counter = 0;

		if (collision_points_length < COLLISION_MAX_POINTS_DEBUG)
			collision_points_length++;
		collision_points[collision_points_counter++] = CP_Vector_Set(golf_pos.x, golf_pos.y);
	}

	if (debug_enabled) {
		// draw normal
		CP_Vector c = world_to_screen(wall->pos.x, wall->pos.y, world_width, world_height);
		CP_Vector n = world_to_screen(wall->pos.x + normal.x, wall->pos.y + normal.y, world_width, world_height);
		CP_Vector l = world_to_screen(wall->pos.x + left.x, wall->pos.y + left.y, world_width, world_height);
		CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
		CP_Settings_Stroke(CP_Color_Create(200, 200, 200, 255));
		CP_Graphics_DrawLine(c.x, c.y, n.x, n.y);
		CP_Graphics_DrawLine(c.x, c.y, l.x, l.y);

		// draw top face
		CP_Vector up_start = CP_Vector_Rotate(wall->normal, CP_Math_Radians(45));
		CP_Vector up_end = CP_Vector_Rotate(wall->normal, CP_Math_Radians(-45));
		CP_Vector u_s = world_to_screen(wall->pos.x + up_start.x, wall->pos.y + up_start.y, world_width, world_height);
		CP_Vector u_e = world_to_screen(wall->pos.x + up_end.x, wall->pos.y + up_end.y, world_width, world_height);
		CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
		CP_Settings_Stroke(CP_Color_Create(200, 200, 200, 255));
		CP_Graphics_DrawLine(u_s.x, u_s.y, u_e.x, u_e.y);

		// draw bottom face
		CP_Vector bot_start = CP_Vector_Rotate(CP_Vector_Negate(wall->normal), CP_Math_Radians(45));
		CP_Vector bot_end = CP_Vector_Rotate(CP_Vector_Negate(wall->normal), CP_Math_Radians(-45));
		CP_Vector b_s = world_to_screen(wall->pos.x + bot_start.x, wall->pos.y + bot_start.y, world_width, world_height);
		CP_Vector b_e = world_to_screen(wall->pos.x + bot_end.x, wall->pos.y + bot_end.y, world_width, world_height);
		CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
		CP_Settings_Stroke(CP_Color_Create(200, 200, 200, 255));
		CP_Graphics_DrawLine(b_s.x, b_s.y, b_e.x, b_e.y);

		// draw left face
		CP_Vector left_start = CP_Vector_Rotate(wall->left, CP_Math_Radians(45));
		CP_Vector left_end = CP_Vector_Rotate(wall->left, CP_Math_Radians(-45));
		CP_Vector l_s = world_to_screen(wall->pos.x + left_start.x, wall->pos.y + left_start.y, world_width, world_height);
		CP_Vector l_e = world_to_screen(wall->pos.x + left_end.x, wall->pos.y + left_end.y, world_width, world_height);
		CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
		CP_Settings_Stroke(CP_Color_Create(200, 200, 200, 255));
		CP_Graphics_DrawLine(l_s.x, l_s.y, l_e.x, l_e.y);

		// draw right face
		CP_Vector right_start = CP_Vector_Rotate(CP_Vector_Negate(wall->left), CP_Math_Radians(45));
		CP_Vector right_end = CP_Vector_Rotate(CP_Vector_Negate(wall->left), CP_Math_Radians(-45));
		CP_Vector r_s = world_to_screen(wall->pos.x + right_start.x, wall->pos.y + right_start.y, world_width, world_height);
		CP_Vector r_e = world_to_screen(wall->pos.x + right_end.x, wall->pos.y + right_end.y, world_width, world_height);
		CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
		CP_Settings_Stroke(CP_Color_Create(200, 200, 200, 255));
		CP_Graphics_DrawLine(r_s.x, r_s.y, r_e.x, r_e.y);
	}
	return collision;
}

void collision_response(Wall* wall, CP_Vector normal) {

}

void game_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
	mouse.x = CP_Input_GetMouseX();
	mouse.y = CP_Input_GetMouseY();
	CP_Vector mouse_world = screen_to_world(mouse.x, mouse.y, world_width, world_height);

	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		CP_Engine_Terminate();
	}

	if (CP_Input_KeyDown(KEY_SPACE)) {
		golf_vel.y += 25 * CP_System_GetDt();
	}

	if (CP_Input_KeyTriggered(KEY_G)) {
		debug_enabled = !debug_enabled;
	}

	if (CP_Input_MouseDown(MOUSE_BUTTON_1) && CP_Vector_Distance(mouse_world, golf_pos) < golf_diameter) {
		dragging = 1;
		golf_pos = CP_Vector_Set(mouse_world.x, mouse_world.y);
		CP_Vector mouse_prev = screen_to_world(CP_Input_GetMousePreviousX(), CP_Input_GetMousePreviousY(), world_width, world_height);
		golf_vel = CP_Vector_Scale(CP_Vector_Subtract(mouse_world, mouse_prev), 10.f);
	}
	else {
		dragging = 0;
	}

	if (!dragging) {
		golf_vel.y += gravity * CP_System_GetDt();
		golf_vel = CP_Vector_Max(golf_vel, 15.f);

		if (CP_Vector_Length(golf_vel) > 0.01f) {
			CP_Vector vel = CP_Vector_Scale(golf_vel, CP_System_GetDt());
			golf_vel = CP_Vector_Subtract(golf_vel, vel);
			golf_pos = CP_Vector_Add(golf_pos, vel);
		}
		else {
			golf_vel.x = 0;
			golf_vel.y = 0;
		}

		if (golf_pos.y < golf_diameter / 2.f) {
			golf_pos.y = golf_diameter / 2.f;
			golf_vel.y = -golf_vel.y / 1.5f;
		}
	}
	
	// Render
	CP_Vector coords = world_to_screen(golf_pos.x, golf_pos.y, world_width, world_height);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Graphics_DrawCircle(coords.x, coords.y, golf_diameter * CP_System_GetWindowWidth() / world_width);

	// rot += 1.f;
	for (int i = 0; i < WALLS_AMOUNT; i++) {
		Wall* w = walls + i;
		//w->angle += 10.f * CP_System_GetDt();
		if (w->angle > 360.f) {
			w->angle = 360.f - w->angle;
		}

		update_wall(w);

		CP_Vector c = world_to_screen(w->pos.x, w->pos.y, world_width, world_height);

		// draw wall
		CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
		CP_Settings_Fill(CP_Color_Create(250, 250, 250, 255));
		CP_Graphics_DrawRectAdvanced(c.x, c.y, w->scale.x / 2.f * (CP_System_GetWindowWidth() / world_width), w->scale.y / 2.f * (CP_System_GetWindowWidth() / world_width), w->angle, 0.f);

		// check collision detection
		char collided = check_collision(w);

		if (debug_enabled) {
			// draw line from ball
			CP_Color color = collided ? CP_Color_Create(0, 255, 0, 255) : CP_Color_Create(255, 0, 0, 255);
			CP_Settings_Fill(color);
			CP_Settings_Stroke(color);
			CP_Graphics_DrawLine(c.x, c.y, coords.x, coords.y);
		}
	}

	for (int i = 0; i < collision_points_length; i++) {
		CP_Vector* point = collision_points + i;
		CP_Settings_Stroke(CP_Color_Create(0, 255, 0, 255));
		CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
		CP_Vector p = world_to_screen(point->x, point->y, world_width, world_height);
		CP_Graphics_DrawPoint(p.x, p.y);
	}

	if (debug_enabled)
		debug_draw();

	CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
}

void game_exit(void)
{
	CP_Font_Free(&font);
}