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

		walls[i].angle = CP_Random_RangeFloat(0.f, 45.f);
	}
}

// dotproduct: 1, they are facing the exact same direction (Parallel)
// = 0, they are perpendicular
// = -1, they are facing the exact opposite direction
// > 0, they are facing relatively the same direction
// < 0, they are facing relatively opposite directions
char check_collision(Wall* wall) {
	/*
	CP_Vector distance = CP_Vector_Subtract(wall->pos, golf_pos);
	float distance_length = CP_Vector_Length(distance);
	CP_Vector normal = { wall->normal.x, wall->normal.y };
	CP_Vector left = { wall->left.x, wall->left.y };
	if (CP_Vector_DotProduct(distance, normal) < 0) {
		normal = CP_Vector_Negate(normal);
	}

	float dot = CP_Vector_DotProduct(distance, normal);
	float dot_left = CP_Vector_DotProduct(distance, left);
	*/
	CP_Vector wall_min = { -wall->scale.x / 2.f, -wall->scale.y / 2.f };
	CP_Vector wall_max = { wall->scale.x / 2.f, wall->scale.y / 2.f };

	// ball location position in respect to the wall
	CP_Vector r = CP_Vector_Subtract(golf_pos, wall->pos);

	// rotate local position back to axis
	r = CP_Vector_Rotate(r, CP_Math_Radians(wall->angle));
	CP_Vector local_golf_pos = CP_Vector_Add(r, CP_Vector_Scale(wall_max, 0.5f));

	// AABB collision testing
	CP_Vector closest_point = { local_golf_pos.x, local_golf_pos.y };
	if (closest_point.x < wall_min.x)
		closest_point.x = wall_min.x;
	else if (closest_point.x > wall_max.x)
		closest_point.x = wall_max.x;

	if (closest_point.y < wall_min.y)
		closest_point.y = wall_min.y;
	else if (closest_point.y > wall_max.y)
		closest_point.y = wall_max.y;

	// get back the distance between the golf pos and the closest point
	CP_Vector ball_to_wall = CP_Vector_Subtract(local_golf_pos, closest_point);

	char collision = 0;
	if (CP_Vector_Length(ball_to_wall) <= golf_diameter * 0.5f) {
		collision = 1;
	}

	if (debug_enabled) {
		// draw the closest point after rotating it back to original
		closest_point = CP_Vector_Rotate(closest_point, -CP_Math_Radians(wall->angle));
		CP_Vector c = world_to_screen(wall->pos.x + closest_point.x, wall->pos.y + closest_point.y, world_width, world_height);
		CP_Settings_Fill(CP_Color_Create(200, 0, 200, 255));
		CP_Settings_Stroke(CP_Color_Create(200, 0, 200, 255));
		CP_Graphics_DrawPoint(c.x, c.y);
	}
	return collision;
}

// What to do when the golf ball hits this wall
void collision_response(Wall* wall, CP_Vector normal) {

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

	// Make golf ball jump in any place
	if (CP_Input_KeyDown(KEY_SPACE)) {
		golf_vel.y += 25 * CP_System_GetDt();
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
		golf_vel = CP_Vector_Scale(CP_Vector_Subtract(mouse_world, mouse_prev), 10.f);
	}
	else {
		dragging = 0;
	}

	// Apply gravity to the golf ball if not on ground or not being dragged
	if (!dragging) {
		golf_vel.y += gravity * CP_System_GetDt();
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
		Wall* w = walls + i;
		//w->angle += 10.f * CP_System_GetDt();

		update_wall(w);

		CP_Vector c = world_to_screen(w->pos.x, w->pos.y, world_width, world_height);

		// draw wall
		CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
		CP_Settings_Fill(CP_Color_Create(250, 250, 250, 255));
		CP_Graphics_DrawRectAdvanced(c.x, c.y, w->scale.x * (CP_System_GetWindowWidth() / world_width), w->scale.y * (CP_System_GetWindowWidth() / world_width), w->angle, 0.f);

		// check collision detection
		char collided = check_collision(w);

		if (debug_enabled && collided) {
			// draw line from ball
			CP_Color color = CP_Color_Create(0, 255, 0, 255);
			CP_Settings_Fill(color);
			CP_Settings_Stroke(color);
			CP_Graphics_DrawLine(c.x, c.y, coords.x, coords.y);
		}
	}

	if (debug_enabled)
		debug_draw();

	// reset the color and stroke settings
	CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
}

void game_exit(void)
{
	// free the font memory
	CP_Font_Free(&font);
}