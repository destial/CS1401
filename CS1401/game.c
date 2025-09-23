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
#include "physics.h"
#include "scoring.h"
#include "level_manager.h"
#include "asset_manager.h"
#include "debug_manager.h"
#include "end_state.h"

// static
CP_Color colors[LAST];

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
float resistance = 1.25f;
float power = 1.f;
#ifdef _DEBUG
char debug_enabled = 1;
#else
char debug_enabled = 0;
#endif
char current_level = 1;

// reset on level loading
short playable = 0;
float translate = 1.f;
short win = 0;
short has_won = 0;
short all_balls_stopped;

// load based on level
Wall* walls;
int walls_amount;
int world_width = 48, world_height = 27; // 16x9 aspect ratio cuz its a requirement

// Initialize the font, golf positions and wall positions
void game_init(void)
{
	font = get_font("Assets/Fonts/esl.ttf");
	golf_ball_image = get_image("Assets/Images/golf_ball.png");
	background_image = get_image("Assets/Images/game_background.png");
	CP_Settings_TextSize(20.f);
	CP_Font_Set(font);
	CP_Settings_ImageMode(CP_POSITION_CORNER);

	// world_width = CP_System_GetWindowWidth() / 33.3f;
	// world_height = CP_System_GetWindowHeight() / 33.3f;

	colors[BLACK] = CP_Color_Create(0, 0, 0, 255);
	colors[WHITE] = CP_Color_Create(255, 255, 255, 255);
	colors[RED] = CP_Color_Create(255, 0, 0, 255);
	colors[GREEN] = CP_Color_Create(0, 255, 0, 255);
	colors[BLUE] = CP_Color_Create(0, 0, 255, 255);
	colors[PURPLE] = CP_Color_Create(255, 0, 255, 255);
	colors[VIOLET] = CP_Color_Create(128, 0, 255, 255);
	colors[CYAN] = CP_Color_Create(0, 255, 255, 255);
	colors[ORANGE] = CP_Color_Create(255, 128, 0, 255);
	colors[YELLOW] = CP_Color_Create(255, 255, 0, 255);
	colors[PINK] = CP_Color_Create(255, 0, 200, 255);
	colors[CLEAR] = CP_Color_Create(0, 0, 0, 0);

	// load the current level file set from current_level variable
	load();

	// reset on level loading
	translate = 1.f;
	playable = 0.f;
	win = 0;
	has_won = 0;
	all_balls_stopped = 1;
}

void game_update(void)
{
	// entry animation
	if (translate != 0) {
		translate -= CP_System_GetDt();
	}
	else {
		playable = 1;
	}

	if (translate < 0) {
		translate = 0;
	}

	mouse.x = CP_Input_GetMouseX();
	mouse.y = CP_Input_GetMouseY();
	mouse_world = screen_to_world(mouse.x, mouse.y, world_width, world_height);

	// Quit the engine if pressed escape key
	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		//save_level();
		CP_Engine_SetNextGameState(end_state_init, end_state_update, end_state_end);
		return;
	}

	// Enable debug drawing (game data, hitboxes, collision line)
	if (CP_Input_KeyTriggered(KEY_G)) {
		debug_enabled = !debug_enabled;
	}

	if (CP_Input_KeyTriggered(KEY_S)) {
		save_level();
	}

	if (debug_enabled) {
		if (CP_Input_KeyTriggered(KEY_E)) {
			resistance += 0.1f;
		}
		if (CP_Input_KeyTriggered(KEY_Q)) {
			resistance -= 0.1f;
			if (resistance < 1.f) {
				resistance = 1.f;
			}
		}

		if (CP_Input_MouseWheel() > 0.f) {
			power += 0.1f;
		}
		if (CP_Input_MouseWheel() < 0.f) {
			power -= 0.1f;
			if (power <= 0.f) {
				power = 0.1f;
			}
		}

		update_debug_walls(walls, walls_amount - 1);
		update_debug_holes(holes, 2);
		update_debug_golfs(golf_balls, 2);

		update_debug_utils();
	}

	win = 1;
	for (int i = 0; i < 2; i++) {
		GolfBall* golf = golf_balls + i;
		if (golf->scored)
			continue;

		win = 0;
		if (debug_enabled)
			continue;

		all_balls_stopped = 1;
		for (int j = 0; j < 2; j++) {
			GolfBall* other_golf = golf_balls + j;
			if (other_golf->dragging || other_golf->halt || other_golf->scored)
				continue;

			// if ball is still moving, cant play
			if (CP_Vector_LengthSquared(other_golf->vel) > 0.01f) {
				all_balls_stopped = 0;
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
					other_golf->halt = 1;
				}
			}
		}
		else {
			golf->dragging = 0;
			golf->halt = 0;
			current = 0;
		}

		// apply velocity to the golf ball if not being dragged
		if (!golf->dragging && !golf->halt) {

			// this is to ensure the length cannot be 0 because anything divided by 0 is an error
			if (CP_Vector_LengthSquared(golf->vel) > 0.01f) {
				CP_Vector vel = CP_Vector_Scale(golf->vel, CP_System_GetDt());
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
			if (golf->pos.x > world_width - golf->diameter * 0.5f) {
				golf->pos.x = world_width - golf->diameter * 0.5f;
				golf->vel.x = -golf->vel.x / resistance;
				CP_Sound_Play(get_audio("Assets/Audio/golf_hit_wall.wav"));
			}

			// bound the ball at the left border
			if (golf->pos.x < golf->diameter * 0.5f) {
				golf->pos.x = golf->diameter * 0.5f;
				golf->vel.x = -golf->vel.x / resistance;
				CP_Sound_Play(get_audio("Assets/Audio/golf_hit_wall.wav"));
			}

			// bound the ball at the top border
			if (golf->pos.y > world_height - golf->diameter * 0.5f) {
				golf->pos.y = world_height - golf->diameter * 0.5f;
				golf->vel.y = -golf->vel.y / resistance;
				CP_Sound_Play(get_audio("Assets/Audio/golf_hit_wall.wav"));
			}

			// bound the ball at the bottom border
			if (golf->pos.y < golf->diameter * 0.5f) {
				golf->pos.y = golf->diameter * 0.5f;
				golf->vel.y = -golf->vel.y / resistance;
				CP_Sound_Play(get_audio("Assets/Audio/golf_hit_wall.wav"));
			}

			// rotate the ball based on its x-axis velocity
			golf->angle += (CP_Vector_Length(golf->vel) * 0.25f) * (golf->vel.x > 0 ? 1 : -1);
			if (golf->angle > 360.f) {
				golf->angle = 360.f - golf->angle;
			}
			else if (golf->angle < 0) {
				golf->angle = golf->angle + 360.f;
			}

			// calculate if golf ball is inside the hole
			if (CP_Vector_Distance(golf->pos, holes[0].pos) < (holes[0].diameter * 0.5f) - (golf->diameter * 0.5f)) {
				golf->scored = 1;
			}
			if (CP_Vector_Distance(golf->pos, holes[1].pos) < (holes[1].diameter * 0.5f) - (golf->diameter * 0.5f)) {
				golf->scored = 1;
			}
		}

		// add stroke
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && golf->dragging) {
			add_stroke();
		}
	}

	for (int i = walls_amount - 1; i > -1; i--) {
		Wall* wall = walls + i;
		if (wall == NULL)
			continue;

		// update wall's normal
		update_wall(wall);

		for (int j = 0; j < 2; j++) {
			GolfBall* ball = golf_balls + j;
			if (ball == NULL || ball->scored)
				continue;

			// check collision detection
			if (wall->pos.x <= world_width * 0.5f && ball->pos.x <= world_width * 0.5f)
				check_collision(wall, ball);
			else if (wall->pos.x >= world_width * 0.5f && ball->pos.x >= world_width * 0.5f)
				check_collision(wall, ball);
		}
	}

	for (int j = 0; j < 2; j++) {
		GolfBall* ball = golf_balls + j;
		if (ball == NULL || ball->scored)
			continue;

		ball->prev_pos = CP_Vector_Set(ball->pos.x, ball->pos.y);
	}

	if (win && !has_won) {
		has_won = 1;
		CP_Sound_Play(get_audio("Assets/Audio/victory.wav"));
	}

	game_render();
}

void game_render(void)
{
	// Clear background and render the background image
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Graphics_ClearBackground(colors[BLACK]);

	// reset the color and stroke settings
	CP_Settings_Stroke(colors[BLACK]);
	CP_Settings_Fill(colors[BLACK]);
	CP_Image_Draw(background_image, 0, 0, CP_System_GetWindowWidth(), CP_System_GetWindowHeight(), 255);

	CP_Settings_TextSize(40.f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BOTTOM);
	char b[50];
	sprintf_s(b, 50, "Strokes: %d", get_strokes());
	CP_Font_DrawText(b, 0, CP_System_GetWindowHeight() * 0.5f);

	for (int i = walls_amount - 1; i > -1; i--) {
		Wall* wall = walls + i;
		if (wall == NULL)
			continue;

		// draw wall
		CP_Vector c = world_to_screen(wall->pos.x, wall->pos.y, world_width, world_height);
		if (wall->image == NULL) {
			CP_Settings_Fill(wall->color);
			CP_Settings_Stroke(colors[CLEAR]);
			CP_Graphics_DrawRectAdvanced(
				CP_Math_LerpFloat(c.x, CP_System_GetWindowWidth(), translate), c.y,
				wall->scale.x * ((float) CP_System_GetWindowWidth() / world_width),
				wall->scale.y * ((float) CP_System_GetWindowHeight() / world_height),
				wall->angle,
				0.f
			);
		}
		else {
			CP_Settings_Fill(colors[WHITE]);
			CP_Settings_Stroke(colors[WHITE]);
			CP_Settings_ImageMode(CP_POSITION_CENTER);
			CP_Image_DrawAdvanced(wall->image,
				CP_Math_LerpFloat(c.x, CP_System_GetWindowWidth(), translate), c.y,
				wall->scale.x * ((float) CP_System_GetWindowWidth() / world_width),
				wall->scale.y * ((float) CP_System_GetWindowHeight() / world_height),
				255,
				wall->angle
			);
		}
	}

	// draw hole && golf balls
	for (int i = 0; i < 2; i++) {
		draw_hole(holes[i].pos, holes[i].diameter, world_width, world_height, translate);

		GolfBall* golf = golf_balls + i;
		if (golf->scored) // ignore if golf ball is already scored
			continue;

		CP_Vector coords = world_to_screen(golf->pos.x, golf->pos.y, world_width, world_height);
		CP_Settings_Fill(colors[WHITE]);
		CP_Settings_Stroke(colors[WHITE]);
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Image_DrawAdvanced(golf_ball_image,
			CP_Math_LerpFloat(coords.x, CP_System_GetWindowWidth(), translate),
			coords.y,
			golf->diameter * (float) CP_System_GetWindowWidth() / world_width,
			golf->diameter * (float) CP_System_GetWindowWidth() / world_width,
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
				other_golf->halt = 1;
			}

			// render the current power of the ball
			CP_Vector a = world_to_screen(golf->pos.x, golf->pos.y, world_width, world_height);
			CP_Vector b = world_to_screen(golf->pos.x + golf->vel.x, golf->pos.y + golf->vel.y, world_width, world_height);
			int red = CP_Math_ClampInt(CP_Vector_LengthSquared(golf->vel) * (world_width / world_height), 0, 255);
			int green = 255 - red;
			CP_Settings_Fill(CP_Color_Create(red, green, 0, 255));
			CP_Settings_Stroke(CP_Color_Create(red, green, 0, 255));
			CP_Settings_LineCapMode(CP_LINE_CAP_ROUND);
			CP_Graphics_DrawLine(a.x, a.y, b.x, b.y);
		}

		if (debug_enabled) {
			CP_Settings_Fill(colors[GREEN]);
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

	if (win) {
		draw_scoreboard(sBoard, world_width, world_height);
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
	walls = NULL;
}

// Update the wall's normal based on its angle of rotation
void update_wall(Wall* wall) {
	wall->angle += wall->rotation_rate * CP_System_GetDt();
	if (wall->angle > 360.f) {
		wall->angle = 360.f - wall->angle;
	}
	else if (wall->angle < 0) {
		wall->angle += 360.f;
	}

	wall->angle_radians = CP_Math_Radians(wall->angle);
	wall->normal.x = cos(-wall->angle_radians);
	wall->normal.y = sin(-wall->angle_radians);

	wall->left.x = -wall->normal.y;
	wall->left.y = wall->normal.x;
}

// collision detection between golf_ball and walls
char check_collision(Wall* wall, GolfBall* ball) {
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

	char collision = 0;
	if (CP_Vector_LengthSquared(ball_to_wall) <= pow(ball->diameter * 0.5f, 2)) {
		collision = 1;
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
		CP_Vector c = world_to_screen(wall->pos.x + closest_point.x, wall->pos.y + closest_point.y, world_width, world_height);
		CP_Settings_Fill(colors[GREEN]);
		CP_Settings_Stroke(colors[GREEN]);
		CP_Graphics_DrawPoint(c.x, c.y);

		// draw the collision normal
		if (collision) {
			local_golf_pos = CP_Vector_Rotate(local_golf_pos, -wall->angle_radians);
			CP_Vector coords = world_to_screen(wall->pos.x + local_golf_pos.x, wall->pos.y + local_golf_pos.y, world_width, world_height);
			CP_Settings_Fill(colors[GREEN]);
			CP_Settings_Stroke(colors[GREEN]);
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

	CP_Sound_Play(get_audio("Assets/Audio/golf_hit_wall.wav"));
}

// Drawing FPS, mouse positions in screen and world coordinates, and golf data
void debug_draw(void) {
	CP_Settings_Fill(colors[GREEN]);
	CP_Settings_TextSize(20.f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BOTTOM);
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

	char p[50];
	sprintf_s(p, 50, "Power: %f", power);
	CP_Font_DrawText(p, 0, 100);
}
	
void save_level(void) {
	char level_path[100];
	sprintf_s(level_path, 100, "Assets/Levels/level_%d.lvl", current_level + 1);
	FILE* fptr = fopen(level_path, "w");
	char level_file[4000];
	sprintf_s(level_file, 4000,
		"width=%d\nheight=%d\ngolf_1=%f,%f\ngolf_2=%f,%f\nhole_1=%f,%f\nhole_2=%f,%f\nwalls_amount=%d", 
		world_width, world_height, 
		golf_balls[0].pos.x, golf_balls[0].pos.y, 
		golf_balls[1].pos.x, golf_balls[1].pos.y,
		holes[0].pos.x, holes[0].pos.y,
		holes[1].pos.x, holes[1].pos.y,
		walls_amount
		);
	for (int i = 0; i < walls_amount; i++) {
		Wall* wall = walls + i;
		if (wall == NULL)
			continue;

		char wall_data[1000];
		sprintf_s(wall_data, 1000,
			"\nwall_%d_pos=%f,%f\nwall_%d_scale=%f,%f\nwall_%d_angle=%f\nwall_%d_rot_rate=%f", 
			i, wall->pos.x, wall->pos.y, 
			i, wall->scale.x, wall->scale.y,
			i, wall->angle,
			i, wall->rotation_rate
		);
		strcat_s(level_file, 4000, wall_data);
	}
	fputs(level_file, fptr);
	fclose(fptr);
	fptr = NULL;
}

/*!
@brief Load the current level from text file under "Assets/Levels/level_{current_level+1}.lvl"
*/
void load(void) {

	// initialize standard default values
	sBoard.x = world_width * 0.5f;
	sBoard.y = world_height * 0.5f;

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
	Level* loaded_level = get_level(current_level + 1);

	// if the level file exists, load that one
	if (loaded_level != NULL) {
		world_width = loaded_level->world_width;
		world_height = loaded_level->world_height;

		golf_balls[0].pos = loaded_level->golf_pos_1;
		golf_balls[1].pos = loaded_level->golf_pos_2;

		holes[0].pos = loaded_level->hole_1;
		holes[1].pos = loaded_level->hole_2;

		walls_amount = loaded_level->walls_size + 1;
		walls = malloc(sizeof(Wall) * walls_amount);

		// @todo: set par here into scoring
		// set_lvl_par(loaded_level->par);

		int wall_index = 0;
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

			wall->color = CP_Color_Create(255, 255, 255, 255);

			if (wall->pos.x <= world_width * 0.5f) {
				wall->image = get_image("Assets/Images/wall_left.png");
			}
			else {
				wall->image = get_image("Assets/Images/wall_right.png");
			}
			wall_index++;
		}

		// add the middle wall last (never in the level file)
		Wall* middle_wall = walls + wall_index;
		middle_wall->pos.x = world_width * 0.5f;
		middle_wall->pos.y = world_height * 0.5f;
		middle_wall->scale.x = 0.5f;
		middle_wall->scale.y = world_height * 1.1f;
		middle_wall->angle = 0;
		middle_wall->rotation_rate = 0;
		middle_wall->color = colors[BLACK];
		middle_wall->image = NULL;

		free_level(loaded_level);
		loaded_level = NULL;
		return;
	}

	// load a default randomized level if no level file exists
	golf_balls[0].pos.x = world_width * 0.25f;
	golf_balls[0].pos.y = 2.f;

	golf_balls[1].pos.x = world_width * 0.75f;
	golf_balls[1].pos.y = 2.f;

	holes[0].pos = CP_Vector_Set(world_width * 0.25f, world_height * 0.75f);
	holes[1].pos = CP_Vector_Set(world_width * 0.75f, world_height * 0.75f);

	walls_amount = 11;
	walls = malloc(sizeof(Wall) * walls_amount);
	if (walls) {
		int wall_index = 0;
		for (; wall_index < (walls_amount * 0.5f); wall_index++) {
			Wall* wall = walls + wall_index;
			if (wall == NULL)
				continue;

			// randomize the values
			wall->pos.x = CP_Random_RangeFloat(1.f, world_width * 0.5f - 1.f);
			wall->pos.y = CP_Random_RangeFloat(1.f, world_height - 1.f);

			wall->scale.x = CP_Random_RangeFloat(0.5f, 5.f);
			wall->scale.y = CP_Random_RangeFloat(0.5f, 5.f);

			wall->angle = CP_Random_RangeFloat(0.f, 90.f);
			wall->color = CP_Color_Create(CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), 255);

			wall->image = get_image("Assets/Images/wall_left.png");
			wall->rotation_rate = 0;
		}

		for (; wall_index < walls_amount - 1; wall_index++) {
			Wall* wall = walls + wall_index;
			if (wall == NULL)
				continue;

			// randomize the values
			wall->pos.x = (world_width * 0.5f) + CP_Random_RangeFloat(1.f, world_width * 0.5f - 1.f);
			wall->pos.y = CP_Random_RangeFloat(1.f, world_height - 1.f);

			wall->scale.x = CP_Random_RangeFloat(0.5f, 5.f);
			wall->scale.y = CP_Random_RangeFloat(0.5f, 5.f);

			wall->angle = CP_Random_RangeFloat(0.f, 90.f);
			wall->color = CP_Color_Create(CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), 255);

			wall->image = get_image("Assets/Images/wall_right.png");
			wall->rotation_rate = 0;
		}

		// add the middle wall last
		Wall* middle_wall = walls + wall_index;
		if (middle_wall == NULL)
			return;

		middle_wall->pos.x = world_width * 0.5f;
		middle_wall->pos.y = world_height * 0.5f;
		middle_wall->scale.x = 0.5f;
		middle_wall->scale.y = world_height * 1.1f;
		middle_wall->color = colors[BLACK];
		middle_wall->image = NULL;
		middle_wall->angle = 0;
		middle_wall->rotation_rate = 0;
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
	return CP_Vector_Set(world_width, world_height);
}

CP_Vector get_world_ratios(void) {
	return CP_Vector_Set(CP_System_GetWindowWidth() / (float) world_width, CP_System_GetWindowHeight() / (float) world_height);
}

CP_Color get_color(ColorCode code) {
	return colors[code];
}
