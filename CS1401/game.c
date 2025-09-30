/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 September 2025
@course CSD1401
@brief Definition file for all main game functions and logic
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "math.h"
#include "utils.h"
#include "scoring.h"
#include "level_manager.h"
#include "asset_manager.h"
#include "debug_manager.h"
#include "end_state.h"
#include "main_menu.h"

// initialize global variables here
CP_Font* font;
CP_Image* golf_ball_image;
CP_Image* background_image;

// mouse positions in screen and world space
CP_Vector mouse = { 0, 0 };
CP_Vector mouse_world = { 0, 0 };

// golfball and hole positions
GolfBall golf_balls[2];
Hole holes[2];

// currently dragging ball
GolfBall* current;

// scoreboard position
CP_Vector sBoard;

// editable setting
#ifdef _DEBUG
CP_BOOL debug_enabled = TRUE;
#else
CP_BOOL debug_enabled = FALSE;
#endif

float resistance = 1.25f;
float power = 1.f;
char current_level = 1;

// reset on level loading
float translate = 1.f;
CP_BOOL playable = FALSE;
CP_BOOL win = FALSE;
CP_BOOL has_won = FALSE;
CP_BOOL all_balls_stopped = TRUE;

// load based on level
Wall* walls;
int walls_amount;
CP_Vector world_limits; // 16x9 aspect ratio cuz its a requirement (x is width, y is height)
float current_time;

// Initialize the font, golf positions and wall positions
void game_init(void)
{
	font = get_font(ESL_FONT_ASSET);
	golf_ball_image = get_image(GOLF_BALL_ASSET);
	background_image = get_image(GAME_BACKGROUND_ASSET);
	CP_Settings_TextSize(20.f);
	CP_Font_Set((CP_Font) font);
	CP_Settings_ImageMode(CP_POSITION_CORNER);

	// load the current level file set from current_level variable
	load();

	// reset on level loading
	translate = 1.f;
	playable = FALSE;
	win = FALSE;
	has_won = FALSE;
	all_balls_stopped = TRUE;

	// fix timestep
	current_time = CP_System_GetSeconds();
}

void game_update(void)
{
	mouse.x = CP_Input_GetMouseX();
	mouse.y = CP_Input_GetMouseY();
	mouse_world = screen_to_world(mouse.x, mouse.y, world_limits.x, world_limits.y);

	// Quit the engine if pressed escape key
	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		//save_level();
		CP_Engine_SetNextGameStateForced(main_menu_init, main_menu_update, main_menu_exit);
		return;
	}

	// Enable debug drawing (game data, hitboxes, collision line)
	if (CP_Input_KeyTriggered(KEY_G)) {
		debug_enabled = !debug_enabled;
	}

	if (CP_Input_KeyTriggered(KEY_S) && debug_enabled) {
		save_level();
	}

	if (CP_Input_KeyTriggered(KEY_R)) { // does not reset strokes
		game_exit();
		game_init();
		translate = 0.f;
		return;
	}

	if (debug_enabled) {
		if (CP_Input_KeyTriggered(KEY_E)) {
			resistance += 0.1f;
		}
		if (CP_Input_KeyTriggered(KEY_Q)) {
			resistance -= 0.1f;
			resistance = max(1.f, resistance);
		}

		update_debug_walls(walls, walls_amount);
		update_debug_holes(holes, 2);
		update_debug_golfs(golf_balls, 2);

		update_debug_utils();
	}
	else {
		if (CP_Input_MouseWheel() > 0.f) {
			power += 0.1f;
		}
		if (CP_Input_MouseWheel() < 0.f) {
			power -= 0.1f;
			power = max(0.1f, power);
		}
	}

	float new_time = CP_System_GetSeconds();
	float frame_time = new_time - current_time;
	current_time = new_time;

	// semi-fixed timestep for physics time
	// incase the game gets too laggy for slow pc
	float dt;
	while (frame_time > 0.f) {
		dt = min(frame_time, 1 / 60.f);
		game_physics_update(dt);
		frame_time -= dt;
	}

	// rendering does not need to be in physics time
	game_render();
}

void game_physics_update(float dt)
{
	// entry animation
	if (translate != 0.f) {
		translate -= dt;
		translate = max(0.f, translate);
	}
	else if (!playable) {
		playable = TRUE;
	}

	win = TRUE;
	for (int i = 0; i < 2; i++) {
		GolfBall* golf = golf_balls + i;
		if (golf->scored)
			continue;

		win = FALSE;
		if (debug_enabled)
			continue;

		all_balls_stopped = TRUE;
		for (int j = 0; j < 2; j++) {
			GolfBall* other_golf = golf_balls + j;
			if (other_golf->dragging || other_golf->halt || other_golf->scored)
				continue;

			// if ball is still moving, cant play
			if (CP_Vector_LengthSquared(other_golf->vel) > 0.01f) {
				all_balls_stopped = FALSE;
				break;
			}
		}

		// Dragging the power of the ball
		if (CP_Input_MouseDown(MOUSE_BUTTON_1) && playable) {

			// make sure the mouse is on the ball and its currently not moving
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Vector_Distance(mouse_world, golf->pos) <= (golf->diameter * 0.5) && current == NULL) {
				golf->dragging = all_balls_stopped;
				if (all_balls_stopped) {
					current = golf;
				}
			}

			// manipulate the velocity based on the mouse coordinates
			if (golf->dragging && golf == current) {
				golf->vel = CP_Vector_Subtract(mouse_world, golf->pos);
				golf->vel = CP_Vector_Scale(golf->vel, power);

				for (int j = 0; j < 2; j++) {
					GolfBall* other_golf = golf_balls + j;
					if (other_golf->scored || i == j)
						continue;

					other_golf->vel = CP_Vector_Set(golf->vel.x, golf->vel.y);
					other_golf->halt = TRUE;
				}
			}
		}
		else {
			if (golf->dragging) {
				add_stroke();
				CP_Sound_Play((CP_Sound)get_audio(GOLF_HIT_WALL_ASSET));
			}
			golf->dragging = FALSE;
			golf->halt = FALSE;
			current = NULL;
		}

		// apply velocity to the golf ball if not being dragged
		if (!golf->dragging && !golf->halt) {

			// this is to ensure the length cannot be 0 because anything divided by 0 is an error
			if (CP_Vector_LengthSquared(golf->vel) > 0.01f) {
				CP_Vector vel = CP_Vector_Scale(golf->vel, dt);
				float friction = resistance - 1;
				if (friction != 0) {
					golf->vel = CP_Vector_Subtract(golf->vel, CP_Vector_Scale(vel, (resistance * resistance) - 1));
				}
				golf->pos = CP_Vector_Add(golf->pos, vel);
			}
			else {
				golf->vel.x = 0;
				golf->vel.y = 0;
			}

			// bound the ball at the right border
			if (golf->pos.x > world_limits.x - golf->diameter * 0.5f) {
				golf->pos.x = world_limits.x - golf->diameter * 0.5f;
				golf->vel.x = -golf->vel.x / resistance;
				CP_Sound_Play((CP_Sound)get_audio(GOLF_HIT_WALL_ASSET));
			}

			// bound the ball at the left border
			if (golf->pos.x < golf->diameter * 0.5f) {
				golf->pos.x = golf->diameter * 0.5f;
				golf->vel.x = -golf->vel.x / resistance;
				CP_Sound_Play((CP_Sound)get_audio(GOLF_HIT_WALL_ASSET));
			}

			// bound the ball at the top border
			if (golf->pos.y > world_limits.y - golf->diameter * 0.5f) {
				golf->pos.y = world_limits.y - golf->diameter * 0.5f;
				golf->vel.y = -golf->vel.y / resistance;
				CP_Sound_Play((CP_Sound)get_audio(GOLF_HIT_WALL_ASSET));
			}

			// bound the ball at the bottom border
			if (golf->pos.y < golf->diameter * 0.5f) {
				golf->pos.y = golf->diameter * 0.5f;
				golf->vel.y = -golf->vel.y / resistance;
				CP_Sound_Play((CP_Sound)get_audio(GOLF_HIT_WALL_ASSET));
			}

			// rotate the ball based on its x-axis velocity
			golf->angle += (CP_Vector_Length(golf->vel) * 0.5f) * (golf->vel.x > 0 ? 1 : -1);
			if (golf->angle > 360.f) {
				golf->angle -= 360.f;
			}
			else if (golf->angle < 0.f) {
				golf->angle += 360.f;
			}

			// calculate if golf ball is inside the hole
			if (CP_Vector_Distance(golf->pos, holes[0].pos) < (holes[0].diameter * 0.5f) - (golf->diameter * 0.5f)) {
				golf->scored = TRUE;
			}
			if (CP_Vector_Distance(golf->pos, holes[1].pos) < (holes[1].diameter * 0.5f) - (golf->diameter * 0.5f)) {
				golf->scored = TRUE;
			}
		}
	}

	for (int i = 0; i < walls_amount; i++) {
		Wall* wall = walls + i;
		if (wall == NULL || wall->destroyed)
			continue;

		// update wall's normal
		update_wall(wall, dt);

		if (debug_enabled)
			continue;

		for (int j = 0; j < 2; j++) {
			GolfBall* ball = golf_balls + j;
			if (ball->scored)
				continue;

			// check collision detection
			if (wall->pos.x <= world_limits.x * 0.5f && ball->pos.x <= world_limits.x * 0.5f)
				check_collision(wall, ball);
			else if (wall->pos.x >= world_limits.x * 0.5f && ball->pos.x >= world_limits.x * 0.5f)
				check_collision(wall, ball);
		}
	}

	// after all updates, set the previous position to here
	for (int j = 0; j < 2; j++) {
		GolfBall* ball = golf_balls + j;
		if (ball->scored)
			continue;

		ball->prev_pos = CP_Vector_Set(ball->pos.x, ball->pos.y);
	}

	if (win && !has_won) {
		has_won = TRUE;
		CP_Sound_Play((CP_Sound) get_audio(VICTORY_ASSET));
	}

}

void game_render(void)
{
	// Clear background and render the background image
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Graphics_ClearBackground(get_color(BLACK));

	// reset the color and stroke settings
	CP_Settings_Stroke(get_color(BLACK));
	CP_Settings_Fill(get_color(BLACK));
	CP_Image_Draw((CP_Image) background_image, 0.f, 0.f, (float) CP_System_GetWindowWidth(), (float) CP_System_GetWindowHeight() * 1.f, 255);

	CP_Settings_TextSize(40.f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BOTTOM);
	char b[50];
	sprintf_s(b, 50, "Strokes: %d", get_strokes());
	CP_Font_DrawText(b, 0, CP_System_GetWindowHeight() * 0.5f);

	CP_Vector world_ratios = get_world_ratios();

	// render walls
	for (int i = 0; i < walls_amount; i++) {
		Wall* wall = walls + i;
		if (wall == NULL || wall->destroyed)
			continue;

		CP_Vector c = world_to_screen(wall->pos.x, wall->pos.y, world_limits.x, world_limits.y);
		if (wall->image == NULL) {
			CP_Settings_Fill(wall->color);
			CP_Settings_Stroke(get_color(CLEAR));
			CP_Graphics_DrawRectAdvanced(
				CP_Math_LerpFloat(c.x, (float) CP_System_GetWindowWidth(), translate), 
				c.y,
				wall->scale.x * world_ratios.x,
				wall->scale.y * world_ratios.y,
				wall->angle,
				0.f
			);
		}
		else {
			CP_Settings_Fill(get_color(WHITE));
			CP_Settings_Stroke(get_color(WHITE));
			CP_Settings_ImageMode(CP_POSITION_CENTER);
			CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
			CP_Image_DrawAdvanced((CP_Image)wall->image,
				CP_Math_LerpFloat(c.x, (float) CP_System_GetWindowWidth(), translate), 
				c.y,
				wall->scale.x * world_ratios.x,
				wall->scale.y * world_ratios.y,
				255,
				wall->angle
			);
		}

		if (debug_enabled) {
			CP_Settings_Fill(get_color(GREEN));
			CP_Settings_TextSize(20.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
			char p[50];
			sprintf_s(p, 50, "Wall Index: %d", i);
			CP_Font_DrawText(p, c.x, c.y + 40);

			char b[50];
			sprintf_s(b, 50, "Wall Pos: (%f, %f)", wall->pos.x, wall->pos.y);
			CP_Font_DrawText(b, c.x, c.y + 60);

			char s[50];
			sprintf_s(s, 50, "Wall Scale: (%f, %f)", wall->scale.x, wall->scale.y);
			CP_Font_DrawText(s, c.x, c.y + 80);

			char n[100];
			sprintf_s(n, 100, "Wall Normal: (%f, %f) (Degrees: %.2f)", wall->normal.x, wall->normal.y, wall->angle);
			CP_Font_DrawText(n, c.x, c.y + 100);

			char r[100];
			sprintf_s(r, 100, "Wall Right: (%f, %f) (Radians: %.2f)", wall->right.x, wall->right.y, wall->angle_radians);
			CP_Font_DrawText(r, c.x, c.y + 120);
		}
	}

	// render holes
	for (int i = 0; i < 2; i++) {
		draw_hole(holes[i].pos, holes[i].diameter, world_limits.x, world_limits.y, translate);
	}

	// render golf balls
	for (int i = 0; i < 2; i++) {
		GolfBall* golf = golf_balls + i;
		if (golf->scored) // ignore if golf ball is already scored
			continue;

		CP_Vector coords = world_to_screen(golf->pos.x, golf->pos.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(get_color(WHITE));
		CP_Settings_Stroke(get_color(WHITE));
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
		CP_Image_DrawAdvanced((CP_Image)golf_ball_image,
			CP_Math_LerpFloat(coords.x, (float) CP_System_GetWindowWidth(), translate),
			coords.y,
			golf->diameter * world_ratios.x,
			golf->diameter * world_ratios.x,
			255,
			golf->angle);

		// render current stroke
		if (golf->dragging && golf == current) {
			golf->vel = CP_Vector_Subtract(mouse_world, golf->pos);
			golf->vel = CP_Vector_Scale(golf->vel, power);

			for (int j = 0; j < 2; j++) {
				GolfBall* other_golf = golf_balls + j;
				if (other_golf->scored)
					continue;
				if (i == j) continue;
				other_golf->vel = CP_Vector_Set(golf->vel.x, golf->vel.y);
				other_golf->halt = TRUE;
			}

			// render the current power of the ball
			CP_Vector a = world_to_screen(golf->pos.x, golf->pos.y, world_limits.x, world_limits.y);
			CP_Vector b = world_to_screen(golf->pos.x + golf->vel.x, golf->pos.y + golf->vel.y, world_limits.x, world_limits.y);
			int x = (int) (CP_Vector_LengthSquared(golf->vel) * (world_limits.x / world_limits.y));
			int red = clamp(x, 0, 255);
			int green = 255 - red;
			CP_Settings_Fill(CP_Color_Create(red, green, 0, 255));
			CP_Settings_Stroke(CP_Color_Create(red, green, 0, 255));
			CP_Settings_LineCapMode(CP_LINE_CAP_ROUND);
			CP_Graphics_DrawLine(a.x, a.y, b.x, b.y);
		}

		if (debug_enabled) {
			CP_Settings_Fill(get_color(GREEN));
			CP_Settings_TextSize(20.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
			char p[50];
			sprintf_s(p, 50, "Golf Pos: (%f, %f)", golf->pos.x, golf->pos.y);
			CP_Font_DrawText(p, coords.x, coords.y + 40);

			char b[50];
			sprintf_s(b, 50, "Golf Vel: (%f, %f)", golf->vel.x, golf->vel.y);
			CP_Font_DrawText(b, coords.x, coords.y + 60);
		}
	}

	if (win || CP_Input_KeyDown(KEY_TAB)) {
		draw_scoreboard(sBoard, world_limits.x, world_limits.y);
		/*
		@todo: win condition response (all balls have scored)
		*/
	} else if (playable && all_balls_stopped && !debug_enabled) {
		CP_Settings_Fill(get_color(ORANGE));
		CP_Settings_Stroke(get_color(ORANGE));
		CP_Settings_TextSize(50.f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("It's your turn now!", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.9f);
	}

	// render debug text on top of everything
	if (debug_enabled) {
		debug_draw();
		render_debug_utils();
	}
}

void game_exit(void)
{
	// free level allocations
	free(walls);
	free_assets();
	reset_strokes();
	walls = NULL;
}

// Update the wall's normal based on its angle of rotation
void update_wall(Wall* wall, float dt) {
	if (!debug_enabled) {
		wall->angle += wall->rotation_rate * dt;
	}

	if (wall->angle > 360.f) {
		wall->angle = 360.f - wall->angle;
	}
	else if (wall->angle < 0) {
		wall->angle += 360.f;
	}

	wall->angle_radians = CP_Math_Radians(wall->angle);
	wall->normal.x = cosf(-wall->angle_radians);
	wall->normal.y = sinf(-wall->angle_radians);

	wall->right.x = wall->normal.y;
	wall->right.y = -wall->normal.x;
}

// collision detection between golf_ball and walls
CP_BOOL check_collision(Wall* wall, GolfBall* ball) {
	CP_Vector wall_min = { -wall->scale.x * 0.5f, -wall->scale.y * 0.5f };
	CP_Vector wall_max = { wall->scale.x * 0.5f, wall->scale.y * 0.5f };

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

	CP_Vector point_to_check = { 
		closest_point.x / wall->scale.x, 
		closest_point.y / wall->scale.y 
	};
	// get back the distance between the golf pos and the closest point
	// this is also the normal for collision response
	CP_Vector ball_to_wall = CP_Vector_Subtract(local_golf_pos, closest_point);

	CP_BOOL collision = FALSE;
	if (CP_Vector_LengthSquared(ball_to_wall) <= pow(ball->diameter * 0.5f, 2)) {
		collision = TRUE;
	}

	if (collision) {
		// make sure it doesnt clip through lmao
		ball->pos = CP_Vector_Set(ball->prev_pos.x, ball->prev_pos.y);

		// do the appropriate collision response if detected
		collision_response(wall, ball, CP_Vector_Rotate(CP_Vector_Normalize(ball_to_wall), -wall->angle_radians));
	}

	if (debug_enabled) {
		// draw the closest point after rotating it back to original
		closest_point = CP_Vector_Rotate(closest_point, -wall->angle_radians);
		CP_Vector c = world_to_screen(wall->pos.x + closest_point.x, wall->pos.y + closest_point.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(get_color(GREEN));
		CP_Settings_Stroke(get_color(GREEN));
		CP_Graphics_DrawPoint(c.x, c.y);

		// draw the collision normal
		if (collision) {
			local_golf_pos = CP_Vector_Rotate(local_golf_pos, -wall->angle_radians);
			CP_Vector coords = world_to_screen(wall->pos.x + local_golf_pos.x, wall->pos.y + local_golf_pos.y, world_limits.x, world_limits.y);
			CP_Settings_Fill(get_color(GREEN));
			CP_Settings_Stroke(get_color(GREEN));
			CP_Settings_LineJointMode(CP_LINE_JOINT_BEVEL);
			CP_Graphics_DrawLine(c.x, c.y, coords.x, coords.y);
		}
	}
	return collision;
}

// as the name implies, it handles what to do when the ball hits a wall
void collision_response(Wall* wall, GolfBall* ball, CP_Vector normal) {
	/*
	r = d - 2(d . n)n
	sourced from https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
	@todo: add rolling support (it gets stuck if too slow)
	* 
	*/
	
	float dot = 2 * CP_Vector_DotProduct(ball->vel, normal);
	CP_Vector rhs = { dot * normal.x, dot * normal.y };
	CP_Vector reflection = CP_Vector_Subtract(ball->vel, rhs);
	ball->vel.x = reflection.x / resistance;
	ball->vel.y = reflection.y / resistance;

	CP_Sound_Play((CP_Sound) get_audio(GOLF_HIT_WALL_ASSET));
}

// Drawing FPS, mouse positions in screen and world coordinates, and golf data
void debug_draw(void) {
	CP_Settings_Fill(get_color(GREEN));
	CP_Settings_TextSize(20.f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BOTTOM);
	char fps[50];
	sprintf_s(fps, 50, "FPS: %d", (int)(1 / CP_System_GetDt()));
	CP_Font_DrawText(fps, 0, 20);

	char m[50];
	sprintf_s(m, 50, "Screen Mouse: (%.f, %.f)", mouse.x, mouse.y);
	CP_Font_DrawText(m, 0, 40);

	CP_Vector world = screen_to_world(mouse.x, mouse.y, world_limits.x, world_limits.y);
	char wm[50];
	sprintf_s(wm, 50, "World Mouse: (%f, %f)", world.x, world.y);
	CP_Font_DrawText(wm, 0, 60);

	char r[50];
	sprintf_s(r, 50, "Resistance: %f", resistance);
	CP_Font_DrawText(r, 0, 80);

	char p[50];
	sprintf_s(p, 50, "Mouse Wheel: %f", CP_Input_MouseWheel());
	CP_Font_DrawText(p, 0, 100);
}

#define FILE_SIZE 6000
void save_level(void) {
	char level_path[100];
	sprintf_s(level_path, 100, LEVEL_FILEPATH, current_level);
	FILE* fptr = fopen(level_path, "w");
	
	char level_file[FILE_SIZE];
	sprintf_s(level_file, FILE_SIZE,
		"width=%f\nheight=%f\ngolf_1=%f,%f\ngolf_2=%f,%f\nhole_1=%f,%f\nhole_2=%f,%f\nmiddle_wall_angle=%f", 
		world_limits.x, world_limits.y,
		golf_balls[0].pos.x, golf_balls[0].pos.y, 
		golf_balls[1].pos.x, golf_balls[1].pos.y,
		holes[0].pos.x, holes[0].pos.y,
		holes[1].pos.x, holes[1].pos.y,
		walls->angle
		);

	int wall_index = 0;
	for (int i = 1; i < walls_amount; i++) {
		Wall* wall = walls + i;
		if (wall == NULL || wall->destroyed)
			continue;

		char wall_data[1000];
		sprintf_s(wall_data, 1000,
			"\nwall_%d_pos=%f,%f\nwall_%d_scale=%f,%f\nwall_%d_angle=%f\nwall_%d_rot_rate=%f", 
			wall_index, wall->pos.x, wall->pos.y,
			wall_index, wall->scale.x, wall->scale.y,
			wall_index, wall->angle,
			wall_index, wall->rotation_rate
		);
		strcat_s(level_file, FILE_SIZE, wall_data);
		wall_index++;
	}

	char last[50];
	sprintf_s(last, 50, "\nwalls_amount=%d", wall_index);
	strcat_s(level_file, FILE_SIZE, last);

	fputs(level_file, fptr);
	fclose(fptr);
	fptr = NULL;
}

/*!
@brief Load the current level from text file under "Assets/Levels/level_{current_level}.lvl"
*/
void load(void) {
	// initialize golf balls default values
	for (int j = 0; j < 2; j++) {
		golf_balls[j].scored = 0;
		golf_balls[j].halt = 0;
		golf_balls[j].dragging = 0;
		golf_balls[j].angle = 0;
		golf_balls[j].diameter = 1.f;
		golf_balls[j].vel.x = 0.f;
		golf_balls[j].vel.y = 0.f;
		holes[j].diameter = 1.5f;
	}

	// get the currently set level and load the level file
	Level* loaded_level = get_level(current_level);

	// if the level file exists, load that one
	if (loaded_level != NULL) {
		world_limits.x = loaded_level->world_width;
		world_limits.y = loaded_level->world_height;

		// initialize standard default values
		sBoard.x = world_limits.x * 0.5f;
		sBoard.y = world_limits.y * 0.5f;

		golf_balls[0].pos = loaded_level->golf_pos_1;
		golf_balls[1].pos = loaded_level->golf_pos_2;

		holes[0].pos = loaded_level->hole_1;
		holes[1].pos = loaded_level->hole_2;

		walls_amount = loaded_level->walls_size + 1;
		walls = malloc(sizeof(Wall) * walls_amount);

		// @todo: set par here into scoring
		// set_lvl_par(loaded_level->par);

		if (walls) {
			int wall_index = 1;
			for (int i = 0; i < loaded_level->walls_size * WALLS_OFFSET; i += WALLS_OFFSET) {
				Wall* wall = walls + wall_index;
				if (wall == NULL)
					continue;
				// loading walls according to index in array
				wall->pos.x = loaded_level->walls[i];
				wall->pos.y = loaded_level->walls[i + 1];

				wall->scale.x = loaded_level->walls[i + 2];
				wall->scale.y = loaded_level->walls[i + 3];

				wall->angle = loaded_level->walls[i + 4];
				wall->rotation_rate = loaded_level->walls[i + 5];

				wall->color = get_color(WHITE);

				if (wall->pos.x <= world_limits.x * 0.5f) {
					wall->image = get_image(WALL_LEFT_ASSET);
				}
				else {
					wall->image = get_image(WALL_RIGHT_ASSET);
				}
				wall->destroyed = FALSE;
				wall_index++;
			}

			// middle wall is always the first (never in the level file)
			Wall* middle_wall = walls;
			middle_wall->pos.x = world_limits.x * 0.5f;
			middle_wall->pos.y = world_limits.y * 0.5f;
			middle_wall->scale.x = world_limits.x * 0.01f;
			middle_wall->scale.y = world_limits.y * 2.5f;
			middle_wall->angle = loaded_level->middle_wall_angle;
			middle_wall->rotation_rate = 0.f;
			middle_wall->color = get_color(BLACK);
			middle_wall->image = NULL;
			middle_wall->destroyed = FALSE;
		}
		else {
			CP_Engine_SetNextGameStateForced(end_state_init, end_state_update, end_state_end);
		}

		free_level(loaded_level);
		loaded_level = NULL;
		return;
	}

	// load a default randomized level if no level file exists
	world_limits.x = 48.f;
	world_limits.y = 27.f;

	// initialize standard default values
	sBoard.x = world_limits.x * 0.5f;
	sBoard.y = world_limits.y * 0.5f;

	golf_balls[0].pos.x = world_limits.x * 0.25f;
	golf_balls[0].pos.y = 2.f;

	golf_balls[1].pos.x = world_limits.x * 0.75f;
	golf_balls[1].pos.y = 2.f;

	holes[0].pos = CP_Vector_Set(world_limits.x * 0.25f, world_limits.y * 0.75f);
	holes[1].pos = CP_Vector_Set(world_limits.x * 0.75f, world_limits.y * 0.75f);

	walls_amount = 11;
	walls = malloc(sizeof(Wall) * walls_amount);
	if (walls) {
		int wall_index = 1;
		for (; wall_index < (walls_amount * 0.5f); wall_index++) {
			Wall* wall = walls + wall_index;
			if (wall == NULL)
				continue;

			// randomize the values
			wall->pos.x = CP_Random_RangeFloat(1.f, world_limits.x * 0.5f - 1.f);
			wall->pos.y = CP_Random_RangeFloat(1.f, world_limits.y - 1.f);

			wall->scale.x = CP_Random_RangeFloat(0.5f, 5.f);
			wall->scale.y = CP_Random_RangeFloat(0.5f, 5.f);

			wall->angle = CP_Random_RangeFloat(0.f, 90.f);
			wall->color = CP_Color_Create(CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), 255);

			wall->image = get_image(WALL_LEFT_ASSET);
			wall->rotation_rate = 0.f;
			wall->destroyed = FALSE;
		}

		for (; wall_index < walls_amount; wall_index++) {
			Wall* wall = walls + wall_index;
			if (wall == NULL)
				continue;

			// randomize the values
			wall->pos.x = (world_limits.x * 0.5f) + CP_Random_RangeFloat(1.f, world_limits.x * 0.5f - 1.f);
			wall->pos.y = CP_Random_RangeFloat(1.f, world_limits.y - 1.f);

			wall->scale.x = CP_Random_RangeFloat(0.5f, 5.f);
			wall->scale.y = CP_Random_RangeFloat(0.5f, 5.f);

			wall->angle = CP_Random_RangeFloat(0.f, 90.f);
			wall->color = CP_Color_Create(CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), 255);

			wall->image = get_image(WALL_RIGHT_ASSET);
			wall->rotation_rate = 0.f;
			wall->destroyed = FALSE;
		}

		// middle wall is always the first
		Wall* middle_wall = walls;

		middle_wall->pos.x = world_limits.x * 0.5f;
		middle_wall->pos.y = world_limits.y * 0.5f;
		middle_wall->scale.x = world_limits.x * 0.01f;
		middle_wall->scale.y = world_limits.y * 1.1f;
		middle_wall->color = get_color(BLACK);
		middle_wall->image = NULL;
		middle_wall->angle = 0.f;
		middle_wall->rotation_rate = 0.f;
		middle_wall->destroyed = FALSE;
	} else {
		CP_Engine_SetNextGameStateForced(end_state_init, end_state_update, end_state_end);
	}
}

Wall* add_wall(CP_Vector pos) {
	for (int wall_index = 1; wall_index < walls_amount; wall_index++) {
		Wall* w = walls + wall_index;
		if (w == NULL)
			continue;

		if (w->destroyed) {
			w->pos = pos;
			w->scale.x = 1.f;
			w->scale.y = 1.f;
			w->color = get_color(WHITE);
			if (w->pos.x <= world_limits.x * 0.5f) {
				w->image = get_image(WALL_LEFT_ASSET);
			}
			else {
				w->image = get_image(WALL_RIGHT_ASSET);
			}
			w->angle = 0.f;
			w->rotation_rate = 0.f;
			w->destroyed = FALSE;
			return w;
		}
	}

	walls_amount++;
	walls = realloc(walls, sizeof(Wall) * walls_amount);

	Wall* new_wall = walls + (walls_amount - 1);
	new_wall->pos = pos;
	new_wall->scale.x = 1.f;
	new_wall->scale.y = 1.f;
	new_wall->color = get_color(WHITE);
	if (new_wall->pos.x <= world_limits.x * 0.5f) {
		new_wall->image = get_image(WALL_LEFT_ASSET);
	}
	else {
		new_wall->image = get_image(WALL_RIGHT_ASSET);
	}
	new_wall->angle = 0.f;
	new_wall->rotation_rate = 0.f;
	new_wall->destroyed = FALSE;
	return new_wall;
}

void remove_wall(Wall* wall) {
	for (int wall_index = 1; wall_index < walls_amount; wall_index++) {
		Wall* w = walls + wall_index;
		if (w == NULL)
			continue;

		if (w == wall) {
			w->pos.x = -10.f;
			w->pos.y = -10.f;
			w->scale.x = 0.f;
			w->scale.y = 0.f;
			w->rotation_rate = 0.f;
			w->angle = 0.f;
			w->destroyed = TRUE;
		}
	}
}

void set_game_level(char level) {
	current_level = level;
}

char get_game_level(void) {
	return current_level;
}

CP_Vector get_mouse_world(void) {
	return mouse_world;
}

CP_Vector get_world_limits(void) {
	return world_limits;
}

CP_Vector get_world_ratios(void) {
	return CP_Vector_Set(CP_System_GetWindowWidth() / world_limits.x, CP_System_GetWindowHeight() / world_limits.y);
}
