#include <stdio.h>
#include "game.h"
#include "math.h"
#include "physics.h"

typedef struct Wall {
	CP_Vector pos;
	CP_Vector scale;
	CP_Vector normal;
	CP_Vector left;
	CP_Color color;
	float angle;

} Wall;

CP_Font font;
CP_Vector mouse = { 0, 0 };
CP_Vector golf_pos = { 0, 0 };
CP_Vector golf_prev_pos = { 0, 0 };
CP_Vector golf_vel = { 0, 0 };
CP_Color NO_STROKE;
float golf_diameter;
int debug_enabled = 1;
int dragging = 0;

const float world_width = 48, world_height = 27; // 16x9 aspect ratio cuz full screen
static const int WALLS_AMOUNT = 10;
Wall walls[10];

// Update the wall's normal based on its angle of rotation
void update_wall(Wall* wall) {
	if (wall->angle > 360.f) {
		wall->angle = 360.f - wall->angle;
	}
	wall->normal.x = cos(CP_Math_Radians(-wall->angle));
	wall->normal.y = sin(CP_Math_Radians(-wall->angle));

	wall->left.x = -wall->normal.y;
	wall->left.y = wall->normal.x;
}

// Drawing FPS, mouse positions in screen and world coordinates, and golf data
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

// Initialize the font, golf positions and wall positions
void game_init(void) {
	font = CP_Font_Load("Assets/esl.ttf");
	NO_STROKE = CP_Color_Create(0, 0, 0, 0);
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

		walls[i].scale.x = CP_Random_RangeFloat(1.f, 2.f);
		walls[i].scale.y = CP_Random_RangeFloat(1.f, 2.f);

		walls[i].angle = CP_Random_RangeFloat(0.f, 45.f);

		walls[i].color = CP_Color_Create(CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), 255);
	}
}

// collision detection between golf_ball and walls
char check_collision(Wall* wall) {
	CP_Vector wall_min = { -wall->scale.x / 2.f, -wall->scale.y / 2.f };
	CP_Vector wall_max = { wall->scale.x / 2.f, wall->scale.y / 2.f };

	// ball location position in respect to the wall
	CP_Vector r = CP_Vector_Subtract(golf_pos, wall->pos);

	// rotate local position back to axis
	CP_Vector local_golf_pos = CP_Vector_Rotate(r, CP_Math_Radians(wall->angle));

	// AABB collision testing
	CP_Vector closest_point = { local_golf_pos.x, local_golf_pos.y };
	if (closest_point.x < wall_min.x) {
		closest_point.x = wall_min.x;
	}
	else if (closest_point.x > wall_max.x) {
		closest_point.x = wall_max.x;
	}

	if (closest_point.y < wall_min.y) {
		closest_point.y = wall_min.y;
	}
	else if (closest_point.y > wall_max.y) {
		closest_point.y = wall_max.y;
	}

	CP_Vector normal_face = wall->normal;
	CP_Vector point_to_check = { closest_point.x * 1.f / wall->scale.x, closest_point.y * 1.f / wall->scale.y };
	// get back the distance between the golf pos and the closest point
	// this is also the normal for collision response
	CP_Vector ball_to_wall = CP_Vector_Subtract(local_golf_pos, closest_point);

	char collision = 0;
	if (CP_Vector_Length(ball_to_wall) <= golf_diameter * 0.5f) {
		collision = 1;
	}

	if (collision) {
		// make sure it doesnt clip through lmao
		golf_pos = golf_prev_pos;
		collision_response(wall, CP_Vector_Rotate(CP_Vector_Normalize(ball_to_wall), -CP_Math_Radians(wall->angle)));
	}

	if (debug_enabled) {
		// draw the closest point after rotating it back to original
		CP_Color green = CP_Color_Create(0, 255, 0, 255);
		closest_point = CP_Vector_Rotate(closest_point, -CP_Math_Radians(wall->angle));
		CP_Vector c = world_to_screen(wall->pos.x + closest_point.x, wall->pos.y + closest_point.y, world_width, world_height);
		CP_Settings_Fill(green);
		CP_Settings_Stroke(green);
		CP_Graphics_DrawPoint(c.x, c.y);

		if (collision) {
			local_golf_pos = CP_Vector_Rotate(local_golf_pos, -CP_Math_Radians(wall->angle));
			CP_Vector coords = world_to_screen(wall->pos.x + local_golf_pos.x, wall->pos.y + local_golf_pos.y, world_width, world_height);
			CP_Settings_Fill(green);
			CP_Settings_Stroke(green);
			CP_Graphics_DrawLine(c.x, c.y, coords.x, coords.y);
		}
	}
	return collision;
}

void collision_response(Wall* wall, CP_Vector normal) {
	// r = d - 2(d . n)n
	// sourced this from https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector

	// todo: add rolling support (it gets stuck if too slow)
	float dot = 2 * CP_Vector_DotProduct(golf_vel, normal);
	CP_Vector rhs = { dot * normal.x, dot * normal.y };
	CP_Vector reflection = CP_Vector_Subtract(golf_vel, rhs);
	golf_vel.x = reflection.x / 1.5f;
	golf_vel.y = reflection.y / 1.5f;
}

void game_update(void)
{
	// Clear background to black color
	CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
	mouse.x = CP_Input_GetMouseX();
	mouse.y = CP_Input_GetMouseY();
	CP_Vector mouse_world = screen_to_world(mouse.x, mouse.y, world_width, world_height);

	// Quit the engine if pressed escape key
	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		CP_Engine_Terminate();
	}

	// move golf ball (WASD)
	if (CP_Input_KeyDown(KEY_W)) {
		golf_vel.y += 10 * CP_System_GetDt();
	}
	if (CP_Input_KeyDown(KEY_S)) {
		golf_vel.y -= 10 * CP_System_GetDt();
	}
	if (CP_Input_KeyDown(KEY_A)) {
		golf_vel.x -= 10 * CP_System_GetDt();
	}
	if (CP_Input_KeyDown(KEY_D)) {
		golf_vel.x += 10 * CP_System_GetDt();
	}

	// Enable debug drawing (game data, hitboxes, collision line)
	if (CP_Input_KeyTriggered(KEY_G)) {
		debug_enabled = !debug_enabled;
	}

	// Drag the golf ball
	if (CP_Input_MouseDown(MOUSE_BUTTON_1) && CP_Vector_Distance(mouse_world, golf_pos) < golf_diameter) {
		dragging = 1;
		golf_pos = CP_Vector_Set(mouse_world.x, mouse_world.y);
		CP_Vector mouse_prev = screen_to_world(CP_Input_GetMousePreviousX(), CP_Input_GetMousePreviousY(), world_width, world_height);
		golf_vel = CP_Vector_Scale(CP_Vector_Subtract(mouse_world, mouse_prev), 20.f);
	}
	else {
		dragging = 0;
	}

	// apply gravity to the golf ball if not on ground or not being dragged
	if (!dragging) {
		// golf_vel.y += gravity * CP_System_GetDt();
		golf_vel = CP_Vector_Max(golf_vel, 15.f);

		// this is to ensure the length cannot be 0 because anything divided by 0 is an error
		if (CP_Vector_Length(golf_vel) > 0.01f) {
			CP_Vector vel = CP_Vector_Scale(golf_vel, CP_System_GetDt());
			golf_vel = CP_Vector_Subtract(golf_vel, vel);
			golf_pos = CP_Vector_Add(golf_pos, vel);
		}
		else {
			golf_vel.x = 0;
			golf_vel.y = 0;
		}

		// bound the ball at the right border
		if (golf_pos.x > world_width - golf_diameter * 0.5f) {
			golf_pos.x = world_width - golf_diameter * 0.5f;
			golf_vel.x = -golf_vel.x / 1.5f;
		}

		// bound the ball at the left border
		if (golf_pos.x < golf_diameter * 0.5f) {
			golf_pos.x = golf_diameter * 0.5f;
			golf_vel.x = -golf_vel.x / 1.5f;
		}

		// bound the ball at the top border
		if (golf_pos.y > world_height - golf_diameter * 0.5f) {
			golf_pos.y = world_height - golf_diameter * 0.5f;
			golf_vel.y = -golf_vel.y / 1.5f;
		}

		// bound the ball at the bottom border
		if (golf_pos.y < golf_diameter * 0.5f) {
			golf_pos.y = golf_diameter * 0.5f;
			golf_vel.y = -golf_vel.y / 1.5f;
		}
	}
	
	// Render golf and wall to screen
	CP_Vector coords = world_to_screen(golf_pos.x, golf_pos.y, world_width, world_height);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Graphics_DrawCircle(coords.x, coords.y, golf_diameter * CP_System_GetWindowWidth() / world_width);

	for (int i = 0; i < WALLS_AMOUNT; i++) {
		Wall* wall = walls + i;
		//w->angle += 10.f * CP_System_GetDt();

		update_wall(wall);

		CP_Vector c = world_to_screen(wall->pos.x, wall->pos.y, world_width, world_height);

		// draw wall
		CP_Settings_Fill(wall->color);
		CP_Settings_Stroke(NO_STROKE);
		CP_Graphics_DrawRectAdvanced(c.x, c.y, wall->scale.x * (CP_System_GetWindowWidth() / world_width), wall->scale.y * (CP_System_GetWindowWidth() / world_width), wall->angle, 0.f);

		// check collision detection
		check_collision(wall);
	}

	if (debug_enabled)
		debug_draw();

	// reset the color and stroke settings
	CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

	golf_prev_pos = CP_Vector_Set(golf_pos.x, golf_pos.y);
}

void game_exit(void)
{
	// free the font memory
	CP_Font_Free(&font);
}