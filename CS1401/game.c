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
	float angle_radians;

} Wall;

typedef struct GolfBall {
	CP_Vector pos;
	CP_Vector prev_pos;
	CP_Vector vel;
	float diameter;
	CP_Image image;
	float angle;
	char dragging;
};

CP_Font font;
CP_Image golf_ball_image;
CP_Image background_image;
CP_Vector mouse = { 0, 0 };
CP_Color NO_STROKE;
float resistance = 1.f;
int debug_enabled = 1;

const float world_width = 48, world_height = 27; // 16x9 aspect ratio cuz requirement
static const int WALLS_AMOUNT = 50;
Wall walls[50];
GolfBall golf_balls[2];

// Initialize the font, golf positions and wall positions
void game_init(void) {
	font = CP_Font_Load("Assets/Fonts/esl.ttf");
	golf_ball_image = CP_Image_Load("Assets/Images/golf_ball.png");
	background_image = CP_Image_Load("Assets/Images/game_background.png");
	NO_STROKE = CP_Color_Create(0, 0, 0, 0);
	CP_Settings_TextSize(20.f);
	CP_Font_Set(font);

	golf_balls[0].diameter = 2.f;
	golf_balls[0].pos.x = world_width / 2.f;
	golf_balls[0].pos.y = world_height;
	golf_balls[0].vel.x = 0.f;
	golf_balls[0].vel.y = 15.f;

	golf_balls[1].diameter = 2.f;
	golf_balls[1].pos.x = world_width / 4.f;
	golf_balls[1].pos.y = 0;
	golf_balls[1].vel.x = 10.f;
	golf_balls[1].vel.y = -15.f;

	for (int i = 0; i < WALLS_AMOUNT; i++) {
		walls[i].pos.x = CP_Random_RangeFloat(5.f, world_width - 5.f);
		walls[i].pos.y = CP_Random_RangeFloat(5.f, world_height - 5.f);

		walls[i].scale.x = CP_Random_RangeFloat(1.f, 2.f);
		walls[i].scale.y = CP_Random_RangeFloat(1.f, 2.f);

		walls[i].angle = CP_Random_RangeFloat(0.f, 45.f);

		walls[i].color = CP_Color_Create(CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), 255);
	}
}

void game_update(void)
{
	// Clear background and render the background image
	CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));

	// reset the color and stroke settings
	CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Image_Draw(background_image, 0, 0, CP_System_GetWindowWidth(), CP_System_GetWindowHeight(), 255);

	mouse.x = CP_Input_GetMouseX();
	mouse.y = CP_Input_GetMouseY();
	CP_Vector mouse_world = screen_to_world(mouse.x, mouse.y, world_width, world_height);

	// Quit the engine if pressed escape key
	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		CP_Engine_Terminate();
	}

	// Enable debug drawing (game data, hitboxes, collision line)
	if (CP_Input_KeyTriggered(KEY_G)) {
		debug_enabled = !debug_enabled;
	}

	if (debug_enabled) {
		if (CP_Input_KeyTriggered(KEY_E)) {
			resistance += 0.5f;
		}
		if (CP_Input_KeyTriggered(KEY_Q)) {
			resistance -= 0.5f;
		}
	}

	for (int i = 0; i < 2; i++) {
		GolfBall* golf = golf_balls + i;
		// move golf ball (WASD)
		if (CP_Input_KeyDown(KEY_W)) {
			golf->vel.y += 10 * CP_System_GetDt();
		}
		if (CP_Input_KeyDown(KEY_S)) {
			golf->vel.y -= 10 * CP_System_GetDt();
		}
		if (CP_Input_KeyDown(KEY_A)) {
			golf->vel.x -= 10 * CP_System_GetDt();
		}
		if (CP_Input_KeyDown(KEY_D)) {
			golf->vel.x += 10 * CP_System_GetDt();
		}


		// Drag the golf ball
		if (CP_Input_MouseDown(MOUSE_BUTTON_1) && CP_Vector_Distance(mouse_world, golf->pos) < golf->diameter) {
			golf->dragging = 1;
			golf->pos = CP_Vector_Set(mouse_world.x, mouse_world.y);
			CP_Vector mouse_prev = screen_to_world(CP_Input_GetMousePreviousX(), CP_Input_GetMousePreviousY(), world_width, world_height);
			golf->vel = CP_Vector_Scale(CP_Vector_Subtract(mouse_world, mouse_prev), 20.f);
		}
		else {
			golf->dragging = 0;
		}

		// apply gravity to the golf ball if not on ground or not being dragged
		if (!golf->dragging) {
			// golf_vel.y += gravity * CP_System_GetDt();
			golf->vel = CP_Vector_Max(golf->vel, 15.f);

			// this is to ensure the length cannot be 0 because anything divided by 0 is an error
			if (CP_Vector_Length(golf->vel) > 0.01f) {
				CP_Vector vel = CP_Vector_Scale(golf->vel, CP_System_GetDt());
				float friction = resistance - 1;
				if (friction != 0) {
					golf->vel = CP_Vector_Subtract(golf->vel, CP_Vector_Scale(vel, resistance - 1));
				}
				golf->pos = CP_Vector_Add(golf->pos, vel);
			}
			else {
				golf->vel.x = 0;
				golf->vel.y = 0;
			}

			// bound the ball at the right border
			if (golf->pos.x > world_width - golf->diameter * 0.5f) {
				golf->pos.x = world_width - golf->diameter * 0.5f;
				golf->vel.x = -golf->vel.x / resistance;
			}

			// bound the ball at the left border
			if (golf->pos.x < golf->diameter * 0.5f) {
				golf->pos.x = golf->diameter * 0.5f;
				golf->vel.x = -golf->vel.x / resistance;
			}

			// bound the ball at the top border
			if (golf->pos.y > world_height - golf->diameter * 0.5f) {
				golf->pos.y = world_height - golf->diameter * 0.5f;
				golf->vel.y = -golf->vel.y / resistance;
			}

			// bound the ball at the bottom border
			if (golf->pos.y < golf->diameter * 0.5f) {
				golf->pos.y = golf->diameter * 0.5f;
				golf->vel.y = -golf->vel.y / resistance;
			}
		}

		// Render golf and wall to screen
		CP_Vector coords = world_to_screen(golf->pos.x, golf->pos.y, world_width, world_height);
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Image_DrawAdvanced(golf_ball_image,
			coords.x - golf->diameter * 0.5f * CP_System_GetWindowWidth() / world_width,
			coords.y - golf->diameter * 0.5f * CP_System_GetWindowWidth() / world_width,
			golf->diameter * CP_System_GetWindowWidth() / world_width,
			golf->diameter * CP_System_GetWindowWidth() / world_width,
			255,
			golf->angle += (golf->vel.x * 0.25f));
		//CP_Graphics_DrawCircle(coords.x, coords.y, golf_diameter * CP_System_GetWindowWidth() / world_width);

		for (int i = 0; i < WALLS_AMOUNT; i++) {
			Wall* wall = walls + i;
			wall->angle += 20.f * CP_System_GetDt();

			update_wall(wall);

			CP_Vector c = world_to_screen(wall->pos.x, wall->pos.y, world_width, world_height);

			// draw wall
			CP_Settings_Fill(wall->color);
			CP_Settings_Stroke(NO_STROKE);
			CP_Graphics_DrawRectAdvanced(c.x, c.y, wall->scale.x * (CP_System_GetWindowWidth() / world_width), wall->scale.y * (CP_System_GetWindowWidth() / world_width), wall->angle, 0.f);

			// check collision detection
			check_collision(wall, golf);
		}

		golf->prev_pos = CP_Vector_Set(golf->pos.x, golf->pos.y);

		if (debug_enabled) {
			CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
			char p[50];
			sprintf_s(p, 50, "Golf Pos: (%f, %f)", golf->pos.x, golf->pos.y);
			CP_Font_DrawText(p, coords.x, coords.y + 40);

			char b[50];
			sprintf_s(b, 50, "Golf Vel: (%f, %f)", golf->vel.x, golf->vel.y);
			CP_Font_DrawText(b, coords.x, coords.y + 60);
		}
	}

	if (debug_enabled)
		debug_draw();

}

void game_exit(void)
{
	// free the allocated memory
	CP_Font_Free(&font);
	CP_Image_Free(&golf_ball_image);
	CP_Image_Free(&background_image);
}

// Update the wall's normal based on its angle of rotation
void update_wall(Wall* wall) {
	if (wall->angle > 360.f) {
		wall->angle = 360.f - wall->angle;
	}
	wall->angle_radians = CP_Math_Radians(wall->angle);
	wall->normal.x = cos(-wall->angle_radians);
	wall->normal.y = sin(-wall->angle_radians);

	wall->left.x = -wall->normal.y;
	wall->left.y = wall->normal.x;
}

// collision detection between golf_ball and walls
char check_collision(Wall* wall, GolfBall* ball) {
	CP_Vector wall_min = { -wall->scale.x / 2.f, -wall->scale.y / 2.f };
	CP_Vector wall_max = { wall->scale.x / 2.f, wall->scale.y / 2.f };

	// ball location position in respect to the wall
	CP_Vector r = CP_Vector_Subtract(ball->pos, wall->pos);

	// rotate local position back to axis
	CP_Vector local_golf_pos = CP_Vector_Rotate(r, wall->angle_radians);

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
	if (CP_Vector_Length(ball_to_wall) <= ball->diameter * 0.5f) {
		collision = 1;
	}

	if (collision) {
		// make sure it doesnt clip through lmao
		ball->pos = ball->prev_pos;
		collision_response(wall, ball, CP_Vector_Rotate(CP_Vector_Normalize(ball_to_wall), -wall->angle_radians));
	}

	if (debug_enabled) {
		// draw the closest point after rotating it back to original
		CP_Color green = CP_Color_Create(0, 255, 0, 255);
		closest_point = CP_Vector_Rotate(closest_point, -wall->angle_radians);
		CP_Vector c = world_to_screen(wall->pos.x + closest_point.x, wall->pos.y + closest_point.y, world_width, world_height);
		CP_Settings_Fill(green);
		CP_Settings_Stroke(green);
		CP_Graphics_DrawPoint(c.x, c.y);

		// draw the collision normal
		if (collision) {
			local_golf_pos = CP_Vector_Rotate(local_golf_pos, -wall->angle_radians);
			CP_Vector coords = world_to_screen(wall->pos.x + local_golf_pos.x, wall->pos.y + local_golf_pos.y, world_width, world_height);
			CP_Settings_Fill(green);
			CP_Settings_Stroke(green);
			CP_Graphics_DrawLine(c.x, c.y, coords.x, coords.y);
		}
	}
	return collision;
}

void collision_response(Wall* wall, GolfBall* ball, CP_Vector normal) {
	// r = d - 2(d . n)n
	// sourced from https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector

	// todo: add rolling support (it gets stuck if too slow)
	float dot = 2 * CP_Vector_DotProduct(ball->vel, normal);
	CP_Vector rhs = { dot * normal.x, dot * normal.y };
	CP_Vector reflection = CP_Vector_Subtract(ball->vel, rhs);
	ball->vel.x = reflection.x / resistance;
	ball->vel.y = reflection.y / resistance;
}

// Drawing FPS, mouse positions in screen and world coordinates, and golf data
void debug_draw(void) {
	CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
	CP_Settings_Scale(1.f, 1.f);
	char fps[50];
	sprintf_s(fps, 50, "FPS: %d", (int)(1 / CP_System_GetDt()));
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

	char r[50];
	sprintf_s(r, 50, "Resistance: %f", resistance);
	CP_Font_DrawText(r, 0, 80);
}