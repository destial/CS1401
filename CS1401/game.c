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
#include "editor_manager.h"
#include "end_state.h"
#include "main_menu.h"
#include "cprocessing.h"

// initialize global variables here
CP_Font font;
CP_Image golf_ball_image;
CP_Image background_image;

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

#ifdef _DEBUG
CP_BOOL debug_enabled = TRUE;
#else
CP_BOOL debug_enabled = FALSE;
#endif
CP_BOOL editor_mode = FALSE;

// editable setting
float resistance = 1.25f;
float power = 1.f;
char current_level = 1;
float current_time;

// reset on level loading
float translate = 1.f;
float saving = 0.f;
CP_BOOL playable = FALSE;
CP_BOOL win = FALSE;
CP_BOOL has_won = FALSE;
CP_BOOL all_balls_stopped = TRUE;

// load based on level
Wall* walls;
int walls_amount;
CP_Vector world_limits; // 16x9 aspect ratio cuz its a requirement (x is width, y is height)
Item* items;
int items_amount;

// Initialize the font, golf positions and wall positions
void game_init(void)
{
	font = get_font(ESL_FONT_ASSET);
	golf_ball_image = get_image(GOLF_BALL_ASSET);
	background_image = get_image(GAME_BACKGROUND_ASSET);
	CP_Settings_TextSize(20.f);
	CP_Font_Set(font);
	CP_Settings_ImageMode(CP_POSITION_CORNER);

	// load the current level file set from current_level variable
	load();

	// reset on level loading
	translate = 1.f;
	playable = FALSE;
	win = FALSE;
	has_won = FALSE;
	all_balls_stopped = TRUE;
	editor_mode = FALSE;

	// fix timestep
	current_time = CP_System_GetSeconds();
}

void game_update(void)
{
	mouse.x = CP_Input_GetMouseX();
	mouse.y = CP_Input_GetMouseY();
	mouse_world = screen_to_world(mouse.x, mouse.y, world_limits.x, world_limits.y);

	// go to main menu if pressed escape key
	if (CP_Input_KeyDown(KEY_ESCAPE)) {
		CP_Engine_SetNextGameStateForced(main_menu_init, main_menu_update, main_menu_exit);
		return;
	}

	// enable debug drawing (game data, hitboxes, collision line)
	if (CP_Input_KeyTriggered(KEY_G)) {
		debug_enabled = !debug_enabled;
	}

	// enable debug drawing (game data, hitboxes, collision line)
	if (CP_Input_KeyTriggered(KEY_P)) {
		editor_mode = !editor_mode;
	}

	// reset level
	if (CP_Input_KeyTriggered(KEY_R)) { 
		game_exit();
		game_init();
		translate = 0.f;
		return;
	}

	if (editor_mode) {
		if (CP_Input_KeyTriggered(KEY_E)) { // increase golf ball resistance
			resistance += 0.1f;
		}
		if (CP_Input_KeyTriggered(KEY_Q)) { // decrease golf ball resistance
			resistance -= 0.1f;
			resistance = max(1.f, resistance); // limit to minimum of 1.f
		}

		update_editor_walls(walls, walls_amount);
		update_editor_holes(holes, 2);
		update_editor_golfs(golf_balls, 2);
		update_editor_items(items, items_amount);

		update_editor_utils();

		// save level if debug enabled
		if (CP_Input_KeyTriggered(KEY_S)) {
			save();
		}

		if (saving) {
			saving -= CP_System_GetDt();
			saving = max(0.f, saving);
		}
	}
	else {
		if (CP_Input_MouseWheel() > 0.f) { // increase power of putt
			power += 0.1f;
			power = min(3.f, power); // limit to maximum of 3f
		}
		if (CP_Input_MouseWheel() < 0.f) { // decrease power of putt
			power -= 0.1f;
			power = max(0.1f, power); // limit to minimum of 0.1f
		}
	}

	// semi-fixed timestep for physics time
	// incase the game gets too laggy for slow pc
	float new_time = CP_System_GetSeconds();
	float frame_time = new_time - current_time;
	current_time = new_time;

	float dt;
	// run timestep for every lost frame if < 60fps
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

	// golf ball player input logic (ensure all golf balls are stopped before player can putt)
	all_balls_stopped = TRUE;
	for (int i = 0; i < 2; ++i) {
		GolfBall* golf = golf_balls + i;
		if (golf->dragging || golf->halt || golf->scored)
			continue;

		// if ball is still moving, cant play
		if (CP_Vector_LengthSquared(golf->vel) > 0.01f) {
			all_balls_stopped = FALSE;
			break;
		}
	}

	// update golf balls
	win = TRUE;
	for (int i = 0; i < 2; ++i) {
		GolfBall* golf = golf_balls + i;
		if (golf->scored)
			continue;

		win = FALSE;
		if (editor_mode)
			continue;

		// golf ball scoring animation
		if (golf->scoring) {
			golf->diameter -= dt;
			golf->diameter = max(golf->diameter, 0.f);
			if (golf->diameter == 0.f) {
				golf->scored = TRUE;
			}
			continue;
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

				// set the other balls velocity
				for (int j = 0; j < 2; ++j) {
					GolfBall* other_golf = golf_balls + j;
					if (other_golf->scored || i == j)
						continue;

					other_golf->vel = CP_Vector_Set(golf->vel.x, golf->vel.y);

					// make sure they dont move when setting this velocity
					other_golf->halt = TRUE;
				}
			}
		}
		else {
			// if the previous frame was player input, that means the golf ball has been released and been putted
			if (golf->dragging) {
				add_stroke();
				CP_Sound_Play(get_audio(GOLF_HIT_WALL_ASSET));
			}
			golf->dragging = FALSE;
			golf->halt = FALSE;
			current = NULL;
		}

		// apply velocity to the golf ball if not being dragged
		if (!golf->dragging && !golf->halt) {
			// this is to ensure the length cannot be 0 because anything divided by 0 is an error
			if (CP_Vector_LengthSquared(golf->vel) > 0.01f) {
				// item ability checking here !!!!!
				for (int j = 0; j < items_amount; ++j) {
					Item* item = items + j;
					if (item == NULL || item->collected) continue;

					if (check_item(item, golf)) {
						switch (item->type) {
						case BOOST:
							golf->vel = CP_Vector_Scale(golf->vel, 3.f);
							break;
						case BIG:
							golf->diameter *= 2.f;
							break;
						case SMALL:
							golf->diameter *= 0.5f;
							break;
						case REVERSE:
							golf->vel = CP_Vector_Negate(golf->vel);
							break;
						default: break;
						}
						item->collected = TRUE;
					}
				}

				CP_Vector vel = CP_Vector_Scale(golf->vel, dt);
				float friction = resistance - 1;
				if (friction != 0) {
					golf->vel = CP_Vector_Subtract(golf->vel, CP_Vector_Scale(vel, (resistance * resistance) - 1));
				}

				golf->pos = CP_Vector_Add(golf->pos, vel);
			}
			else {

				// hard stop the ball if too slow
				golf->vel.x = 0;
				golf->vel.y = 0;
			}

			// bound the ball at the right border
			if (golf->pos.x > world_limits.x - golf->diameter * 0.5f) {
				golf->pos.x = world_limits.x - golf->diameter * 0.5f;
				golf->vel.x = -golf->vel.x / resistance;
				golf->vel.y = golf->vel.y / resistance;
				CP_Sound_Play(get_audio(GOLF_HIT_WALL_ASSET));
			}

			// bound the ball at the left border
			if (golf->pos.x < golf->diameter * 0.5f) {
				golf->pos.x = golf->diameter * 0.5f;
				golf->vel.x = -golf->vel.x / resistance;
				golf->vel.y = golf->vel.y / resistance;
				CP_Sound_Play(get_audio(GOLF_HIT_WALL_ASSET));
			}

			// bound the ball at the top border
			if (golf->pos.y > world_limits.y - golf->diameter * 0.5f) {
				golf->pos.y = world_limits.y - golf->diameter * 0.5f;
				golf->vel.y = -golf->vel.y / resistance;
				golf->vel.x = golf->vel.x / resistance;
				CP_Sound_Play(get_audio(GOLF_HIT_WALL_ASSET));
			}

			// bound the ball at the bottom border
			if (golf->pos.y < golf->diameter * 0.5f) {
				golf->pos.y = golf->diameter * 0.5f;
				golf->vel.y = -golf->vel.y / resistance;
				golf->vel.x = golf->vel.x / resistance;
				CP_Sound_Play(get_audio(GOLF_HIT_WALL_ASSET));
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
			for (int h = 0; h < 2; ++h) {
				Hole* hole = holes + h;
				if (CP_Vector_Distance(golf->pos, hole->pos) < (hole->diameter * 0.5f) - (golf->diameter * 0.5f)) { // hole 1
					golf->scoring = TRUE;
				}
			}
		}
	}

	for (int i = 0; i < walls_amount; ++i) {
		Wall* wall = walls + i;
		if (wall == NULL || wall->destroyed)
			continue;

		// update wall's normal
		update_wall(wall, dt);

		if (editor_mode)
			continue;

		for (int j = 0; j < 2; ++j) {
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
	for (int i = 0; i < 2; ++i) {
		GolfBall* ball = golf_balls + i;
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
	CP_Image_Draw(background_image, 0.f, 0.f, (float) CP_System_GetWindowWidth(), (float) CP_System_GetWindowHeight() * 1.f, 255);

	CP_Vector world_ratios = get_world_ratios();

	// render walls
	for (int i = 0; i < walls_amount; ++i) {
		Wall* wall = walls + i;
		if (wall == NULL || wall->destroyed)
			continue;

		CP_Vector c = world_to_screen(wall->pos.x, wall->pos.y, world_limits.x, world_limits.y);
		if (wall->image == NULL) { // theres no image for this wall (most likely the middle wall)
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
		else { // theres image for this wall, use that
			CP_Settings_Fill(get_color(WHITE));
			CP_Settings_Stroke(get_color(WHITE));
			CP_Settings_ImageMode(CP_POSITION_CENTER);
			CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
			CP_Image_DrawAdvanced(wall->image,
				CP_Math_LerpFloat(c.x, (float) CP_System_GetWindowWidth(), translate), 
				c.y,
				wall->scale.x * world_ratios.x,
				wall->scale.y * world_ratios.y,
				255,
				wall->angle
			);
		}

		if (debug_enabled) {
			// display wall data on the screen
			CP_Settings_Fill(get_color(GREEN));
			CP_Settings_TextSize(20.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
			char p[50]; sprintf_s(p, 50, "Index: %d", i);
			CP_Font_DrawText(p, c.x, c.y + 40);

			char b[50]; sprintf_s(b, 50, "Pos: (%.2f, %.2f)", wall->pos.x, wall->pos.y);
			CP_Font_DrawText(b, c.x, c.y + 60);

			char s[50]; sprintf_s(s, 50, "Scale: (%.2f, %.2f)", wall->scale.x, wall->scale.y);
			CP_Font_DrawText(s, c.x, c.y + 80);

			char n[100]; sprintf_s(n, 100, "Normal: (%.2f, %.2f) (Deg: %.2f)", wall->normal.x, wall->normal.y, wall->angle);
			CP_Font_DrawText(n, c.x, c.y + 100);

			char r[100]; sprintf_s(r, 100, "Right: (%.2f, %.2f) (Rad: %.2f)", wall->right.x, wall->right.y, wall->angle_radians);
			CP_Font_DrawText(r, c.x, c.y + 120);

			char rr[100]; sprintf_s(rr, 100, "Rot Rate: %.2f", wall->rotation_rate);
			CP_Font_DrawText(rr, c.x, c.y + 140);

			if (!editor_mode) {
				// draw the closest point after rotating it back to original
				CP_Vector closest_point = CP_Vector_Rotate(wall->_collision_point, -wall->angle_radians);
				CP_Vector c = world_to_screen(wall->pos.x + closest_point.x, wall->pos.y + closest_point.y, world_limits.x, world_limits.y);
				CP_Settings_Fill(get_color(RED));
				CP_Settings_Stroke(get_color(RED));
				CP_Graphics_DrawPoint(c.x, c.y);

				// draw the collision normal
				CP_Vector col_normal = CP_Vector_Rotate(wall->_wall_to_ball, -wall->angle_radians);
				CP_Vector coords = world_to_screen(wall->pos.x + closest_point.x + col_normal.x, wall->pos.y + closest_point.y + col_normal.y, world_limits.x, world_limits.y);
				CP_Settings_Fill(get_color(PURPLE));
				CP_Settings_Stroke(get_color(PURPLE));
				CP_Graphics_DrawLine(c.x, c.y, coords.x, coords.y);
			}
		}
	}

	// render holes
	for (int i = 0; i < 2; ++i) {
		draw_hole(holes[i].pos, holes[i].diameter, world_limits.x, world_limits.y, translate);

		if (debug_enabled) {
			CP_Vector coords = world_to_screen(holes[i].pos.x, holes[i].pos.y, world_limits.x, world_limits.y);
			CP_Settings_Fill(get_color(GREEN));
			CP_Settings_TextSize(20.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
			char p[50]; sprintf_s(p, 50, "Pos: (%.2f, %.2f)", holes[i].pos.x, holes[i].pos.y);
			CP_Font_DrawText(p, coords.x, coords.y + 50);
		}
	}

	for (int i = 0; i < items_amount; ++i) {
		Item* item = items + i;
		if (item == NULL || item->collected || item->destroyed) 
			continue;

		CP_Vector coords = world_to_screen(item->pos.x, item->pos.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(get_color(WHITE));
		CP_Settings_Stroke(get_color(WHITE));
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
		CP_Image image;
		switch (item->type) {
		case BOOST:
			image = get_image(BOOST_ITEM_ASSET);
			break;
		case BIG:
			image = get_image(BIG_ITEM_ASSET);
			break;
		case SMALL:
			image = get_image(SMALL_ITEM_ASSET);
			break;
		case REVERSE:
			image = get_image(REVERSE_ITEM_ASSET);
			break;
		default: 
			image = NULL;
			break;
		}

		if (image != NULL) {
			if (item->rotation_rate) {
				item->angle += item->rotation_rate * CP_System_GetDt();
				item->angle = item->angle < 360.f ? item->angle : 360.f - item->angle;
			}
			float anim_mul = (sinf(5.f * CP_System_GetSeconds()) / 10.f) + 1.f;
			CP_Image_DrawAdvanced(image,
				CP_Math_LerpFloat(coords.x, (float)CP_System_GetWindowWidth(), translate),
				coords.y,
				item->diameter * world_ratios.x * anim_mul,
				item->diameter * world_ratios.x * anim_mul,
				255,
				item->angle);
		}

		if (debug_enabled) {
			CP_Settings_Fill(get_color(GREEN));
			CP_Settings_TextSize(20.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
			char in[64]; sprintf_s(in, 64, "Index: %d", i);
			CP_Font_DrawText(in, coords.x, coords.y + 40);

			char p[64]; sprintf_s(p, 64, "Pos: (%.2f, %.2f)", item->pos.x, item->pos.y);
			CP_Font_DrawText(p, coords.x, coords.y + 60);

			char rr[64]; sprintf_s(rr, 64, "Rot Rate: %.2f", item->rotation_rate);
			CP_Font_DrawText(rr, coords.x, coords.y + 80);

			CP_Settings_NoFill();
			CP_Settings_Stroke(get_color(GREEN));
			CP_Graphics_DrawCircle(coords.x, coords.y, item->diameter * world_ratios.x);
		}
	}

	// render golf balls
	for (int i = 0; i < 2; ++i) {
		GolfBall* golf = golf_balls + i;
		if (golf->scored) // ignore if golf ball is already scored
			continue;

		// render golf ball
		CP_Vector coords = world_to_screen(golf->pos.x, golf->pos.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(get_color(WHITE));
		CP_Settings_Stroke(get_color(WHITE));
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
		CP_Image_DrawAdvanced(golf_ball_image,
			CP_Math_LerpFloat(coords.x, (float) CP_System_GetWindowWidth(), translate),
			coords.y,
			golf->diameter * world_ratios.x,
			golf->diameter * world_ratios.x,
			255,
			golf->angle);

		// render current stroke
		if (golf->dragging && golf == current) {

			// render the current power of the ball
			CP_Vector a = world_to_screen(golf->pos.x, golf->pos.y, world_limits.x, world_limits.y);
			CP_Vector b = world_to_screen(golf->pos.x + golf->vel.x, golf->pos.y + golf->vel.y, world_limits.x, world_limits.y);
			int x = (int) (CP_Vector_LengthSquared(golf->vel) * (world_limits.x / world_limits.y));
			int red = CP_Math_ClampInt(x, 0, 255);
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
			char p[50]; sprintf_s(p, 50, "Pos: (%.2f, %.2f)", golf->pos.x, golf->pos.y);
			CP_Font_DrawText(p, coords.x, coords.y + 40);

			char b[50]; sprintf_s(b, 50, "Vel: (%.2f, %.2f)", golf->vel.x, golf->vel.y);
			CP_Font_DrawText(b, coords.x, coords.y + 60);
		}
	}

	// draw current strokes
	CP_Settings_TextSize(40.f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BOTTOM);
	CP_Settings_Fill(get_color(BLACK));
	CP_Settings_Stroke(get_color(BLACK));
	char b[32]; sprintf_s(b, 32, "Strokes: %d", get_strokes());
	CP_Font_DrawText(b, 0, CP_System_GetWindowHeight() * 0.5f);

	char p[32]; sprintf_s(p, 32, "Par: %d", get_lvl_par());
	CP_Font_DrawText(p, 0, CP_System_GetWindowHeight() * 0.5f + 40.f);

	if (win || CP_Input_KeyDown(KEY_TAB)) {
		draw_scoreboard(sBoard, world_limits.x, world_limits.y);
		/*
		@todo: add button to cycle to next level or replay the same level
		*/
	} else if (playable && all_balls_stopped && !editor_mode) {
		CP_Settings_Fill(get_color(ORANGE));
		CP_Settings_Stroke(get_color(ORANGE));
		CP_Settings_TextSize(50.f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("It's your turn now!", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.9f);
	}

	// render debug text on top of everything

	if (editor_mode) {
		render_editor_utils();
		if (saving) {
			CP_Settings_Fill(get_color(RED));
			CP_Settings_Stroke(get_color(RED));
			CP_Settings_TextSize(50.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Font_DrawText("Level Saved!", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f);
		}
	}

	if (debug_enabled) {
		debug_render_info();
	}
}

void game_exit(void)
{
	// free level allocations
	free(walls);
	free(items);
	walls = NULL;
	items = NULL;
	reset_strokes();
	free_assets();
}

// Update the wall's normal based on its angle of rotation
void update_wall(Wall* wall, float dt) {
	if (!editor_mode) {
		wall->angle += wall->rotation_rate * dt;
	}

	if (wall->angle > 360.f) {
		wall->angle = 360.f - wall->angle;
	}
	else if (wall->angle < 0) {
		wall->angle += 360.f;
	}

	// update wall normal based on wall angle rotation
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

	// get back the distance between the golf pos and the closest point
	// this is also the normal for collision response
	CP_Vector wall_to_ball = CP_Vector_Subtract(local_golf_pos, closest_point);

	// now the actual collision checking, the distance between ball center and closest point to wall is less than ball radius
	CP_BOOL collision = FALSE;
	if (CP_Vector_LengthSquared(wall_to_ball) <= CP_Math_Square(ball->diameter * 0.5f)) {
		collision = TRUE;
		if (debug_enabled) {
			printf("Collision normal: %.2f, %.2f\n", wall_to_ball.x, wall_to_ball.y);
			printf("Ball vel: %.2f, %.2f\n", ball->vel.x, ball->vel.y);
			printf("Local collision point: %.2f, %.2f\n", closest_point.x, closest_point.y);
			printf("~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~\n");
		}
	}

	// set values here to display if debug enabled
	wall->_wall_to_ball = CP_Vector_Normalize(wall_to_ball);
	wall->_collision_point = closest_point;

	if (collision) {
		// make sure it doesnt clip through lmao
		ball->pos = ball->prev_pos;

		// do the appropriate collision response if detected
		collision_response(wall, ball, wall->_collision_point, CP_Vector_Rotate(wall->_wall_to_ball, -wall->angle_radians));
	}
	return collision;
}

CP_BOOL check_item(Item* item, GolfBall* ball) {
	return CP_Vector_LengthSquared(CP_Vector_Subtract(item->pos, ball->pos)) <= CP_Math_Square(item->diameter * 0.5f) + CP_Math_Square(ball->diameter * 0.5f);
}

// as the name implies, it handles what to do when the ball hits a wall
void collision_response(Wall* wall, GolfBall* ball, CP_Vector point, CP_Vector normal) {
	/*
	* /@todo: what to do when colliding against a spinning wall (???)
	*/
#if 0
	if (wall->rotation_rate) {
		float dist = CP_Vector_Distance(wall->pos, point);
		CP_Vector force = CP_Vector_Scale(normal, dist / (wall->rotation_rate * 2.f));
		if (CP_Vector_DotProduct(normal, wall->right) > 0 || CP_Vector_DotProduct(normal, CP_Vector_Negate(wall->right)) > 0) {
			force = CP_Vector_Negate(force);
		}
		ball->vel = CP_Vector_Add(ball->vel, force);
	}
#endif
	
	/*
	r = d - 2(d . n)n
	source: https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
	*/
	//if (!CP_Vector_IsZero(ball->vel)) {
		// reflect the ball velocity from the normal
		float dot = 2 * CP_Vector_DotProduct(ball->vel, normal); // 2 * (d . n)
		CP_Vector rhs = { dot * normal.x, dot * normal.y }; // * n
		CP_Vector reflection = CP_Vector_Subtract(ball->vel, rhs);
		ball->vel.x = reflection.x / resistance;
		ball->vel.y = reflection.y / resistance;
	//} 

	CP_Sound_Play(get_audio(GOLF_HIT_WALL_ASSET));
}

// Drawing FPS, mouse positions in screen and world coordinates, and golf data
void debug_render_info(void) {
	CP_Settings_Fill(get_color(GREEN));
	CP_Settings_TextSize(20.f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BOTTOM);
	char fps[50]; sprintf_s(fps, 50, "FPS: %d", (int)(1 / CP_System_GetDt()));
	CP_Font_DrawText(fps, 0, 20);

	char m[50]; sprintf_s(m, 50, "Screen Mouse: (%.2f, %.2f)", mouse.x, mouse.y);
	CP_Font_DrawText(m, 0, 40);

	CP_Vector world = screen_to_world(mouse.x, mouse.y, world_limits.x, world_limits.y);
	char wm[50]; sprintf_s(wm, 50, "World Mouse: (%.2f, %.2f)", world.x, world.y);
	CP_Font_DrawText(wm, 0, 60);

	char r[50]; sprintf_s(r, 50, "Resistance: %.2f", resistance);
	CP_Font_DrawText(r, 0, 80);

	char p[50]; sprintf_s(p, 50, "Mouse Wheel: %.2f", CP_Input_MouseWheel());
	CP_Font_DrawText(p, 0, 100);
}

/*!
@brief Save the current level to text file under "Assets/Levels/level_{current_level}.lvl"
*/
void save(void) {
	save_level(current_level, walls, walls_amount, golf_balls, holes, items, items_amount, get_world_limits(), get_lvl_par());
	saving = 1.5f;
}

/*!
@brief Load the current level from text file under "Assets/Levels/level_{current_level}.lvl"
*/
void load(void) {
	// initialize golf balls default values
	for (int j = 0; j < 2; ++j) {
		golf_balls[j].vel = CP_Vector_Set(0.f, 0.f);
		golf_balls[j].angle = 0.f;
		golf_balls[j].diameter = 1.f;
		golf_balls[j].halt = FALSE;
		golf_balls[j].dragging = FALSE;
		golf_balls[j].scored = FALSE;
		golf_balls[j].scoring = FALSE;

		// make sure hole diameter is larger than golf ball so it can actually fit
		holes[j].diameter = 1.5f;
	}

	// get the currently set level and load the level file
	Level* loaded_level = get_level(current_level);

	// if the level file exists, load that one
	if (loaded_level != NULL) {
		world_limits.x = loaded_level->world_width;
		world_limits.y = loaded_level->world_height;

		sBoard = CP_Vector_Set(world_limits.x * 0.5f, world_limits.y * 0.5f);

		golf_balls[0].pos = loaded_level->golf_pos_1;
		golf_balls[1].pos = loaded_level->golf_pos_2;

		holes[0].pos = loaded_level->hole_1;
		holes[1].pos = loaded_level->hole_2;

		set_lvl_par(loaded_level->par);
		items_amount = loaded_level->items_size;

		if (items_amount) {
			items = malloc(sizeof(Item) * items_amount);
			int item_index = 0;
			for (int i = 0; i < loaded_level->items_size * ITEMS_OFFSET; i += ITEMS_OFFSET) {
				Item* item = items + item_index;
				if (item == NULL)
					continue;
				// load the items according to index in array
				item->pos.x = loaded_level->items[i];
				item->pos.y = loaded_level->items[i + 1];

				item->diameter = loaded_level->items[i + 2];
				item->angle = loaded_level->items[i + 3];
				item->rotation_rate = loaded_level->items[i + 4];
				item->type = (int) loaded_level->items[i + 5];

				item->collected = FALSE;
				item->destroyed = FALSE;
				++item_index;
			}
		}
		else {
			items = NULL;
		}

		walls_amount = loaded_level->walls_size + 1;
		walls = malloc(sizeof(Wall) * walls_amount);

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

				wall->image = wall->pos.x <= world_limits.x * 0.5f ? get_image(WALL_LEFT_ASSET) : get_image(WALL_RIGHT_ASSET);
				wall->destroyed = FALSE;
				++wall_index;
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
			// level loading failed, quit the game because heap allocation went wrong!!
			CP_Engine_SetNextGameStateForced(end_state_init, end_state_update, end_state_end);
		}

		free_level(loaded_level);
		loaded_level = NULL;
		return;
	}

	// load a default randomized level if no level file exists
	world_limits = CP_Vector_Set(48.f, 27.f);

	// initialize standard default values
	sBoard = CP_Vector_Set(world_limits.x * 0.5f, world_limits.y * 0.5f);

	golf_balls[0].pos = CP_Vector_Set(world_limits.x * 0.25f, 2.f);
	golf_balls[1].pos = CP_Vector_Set(world_limits.x * 0.75f, 2.f);

	holes[0].pos = CP_Vector_Set(world_limits.x * 0.25f, world_limits.y * 0.75f);
	holes[1].pos = CP_Vector_Set(world_limits.x * 0.75f, world_limits.y * 0.75f);

	walls_amount = 11;
	walls = malloc(sizeof(Wall) * walls_amount);
	if (walls) {
		int wall_index = 1;
		// create the random left side walls
		for (; wall_index < (walls_amount * 0.5f); ++wall_index) {
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

		// create the random right side walls
		for (; wall_index < walls_amount; ++wall_index) {
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

		// level loading failed, quit the game because heap allocation went wrong!!
		CP_Engine_SetNextGameStateForced(end_state_init, end_state_update, end_state_end);
	}

	// set a default layout of item powerups
	items_amount = 4;
	items = malloc(sizeof(Item) * items_amount);
	if (items) {
		items[0].type = BOOST;
		items[0].pos = CP_Vector_Set(world_limits.x * 0.25f, world_limits.y * 0.5f);
		items[0].diameter = 1.f;
		items[0].angle = 0.f;
		items[0].rotation_rate = 0.f;
		items[0].collected = FALSE;
		items[0].destroyed = FALSE;

		items[1].type = BIG;
		items[1].pos = CP_Vector_Set(world_limits.x * 0.75f, world_limits.y * 0.5f);
		items[1].diameter = 1.f;
		items[1].angle = 0.f;
		items[1].rotation_rate = 0.f;
		items[1].collected = FALSE;
		items[1].destroyed = FALSE;

		items[2].type = SMALL;
		items[2].pos = CP_Vector_Set(world_limits.x * 0.65f, world_limits.y * 0.3f);
		items[2].diameter = 1.f;
		items[2].angle = 0.f;
		items[2].rotation_rate = 0.f;
		items[2].collected = FALSE;
		items[2].destroyed = FALSE;

		items[3].type = REVERSE;
		items[3].pos = CP_Vector_Set(world_limits.x * 0.45f, world_limits.y * 0.3f);
		items[3].diameter = 1.f;
		items[3].angle = 0.f;
		items[3].rotation_rate = 45.f;
		items[3].collected = FALSE;
		items[3].destroyed = FALSE;
	}
}

Wall* add_wall(CP_Vector pos) {
	for (int wall_index = 1; wall_index < walls_amount; ++wall_index) {
		Wall* w = walls + wall_index;
		if (w == NULL)
			continue;
		
		// if there's an unused wall in the array, we overwrite it
		if (w->destroyed) {
			w->pos = pos;
			w->scale.x = 1.f;
			w->scale.y = 1.f;
			w->color = get_color(WHITE);
			w->image = w->pos.x <= world_limits.x * 0.5f ? get_image(WALL_LEFT_ASSET) : get_image(WALL_RIGHT_ASSET);
			w->angle = 0.f;
			w->rotation_rate = 0.f;
			w->destroyed = FALSE;
			return w;
		}
	}

	// we create a new wall in memory by resizing it and appending it to the back
	++walls_amount;
	walls = walls ? realloc(walls, sizeof(Wall) * walls_amount) : malloc(sizeof(Wall) * walls_amount);
	if (walls) {
		Wall* new_wall = walls + (walls_amount - 1);
		new_wall->pos = pos;
		new_wall->scale.x = 1.f;
		new_wall->scale.y = 1.f;
		new_wall->color = get_color(WHITE);
		new_wall->image = new_wall->pos.x <= world_limits.x * 0.5f ? get_image(WALL_LEFT_ASSET) : get_image(WALL_RIGHT_ASSET);
		new_wall->angle = 0.f;
		new_wall->rotation_rate = 0.f;
		new_wall->destroyed = FALSE;
		return new_wall;
	}
	return NULL;
}

void remove_wall(Wall* wall) {
	// make sure the middle wall is always skipped (element 0)
	for (int wall_index = 1; wall_index < walls_amount; ++wall_index) {
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

Item* add_item(CP_Vector pos, ItemType type) {
	for (int item_index = 0; item_index < items_amount; ++item_index) {
		Item* i = items + item_index;
		if (i == NULL)
			continue;

		// if there's an unused item in the array, we overwrite it
		if (i->destroyed) {
			i->pos = pos;
			i->diameter = 1.f;
			i->angle = 0.f;
			i->rotation_rate = type == REVERSE ? 45.f : 0.f;
			i->destroyed = FALSE;
			i->collected = FALSE;
			i->type = type;
			return i;
		}
	}

	// resize the array to accomodate a new item in memory
	++items_amount;
	items = items ? realloc(items, sizeof(Item) * items_amount) : malloc(sizeof(Item) * items_amount);
	if (items) {
		Item* new_item = items + (items_amount - 1);
		new_item->pos = pos;
		new_item->diameter = 1.f;
		new_item->angle = 0.f;
		new_item->rotation_rate = type == REVERSE ? 45.f : 0.f;
		new_item->destroyed = FALSE;
		new_item->collected = FALSE;
		new_item->type = type;
		return new_item;
	}
	return NULL;
}

void remove_item(Item* item) {
	for (int item_index = 0; item_index < items_amount; ++item_index) {
		Item* i = items + item_index;
		if (i == NULL)
			continue;

		if (i == item) {
			i->pos.x = -10.f;
			i->pos.y = -10.f;
			i->diameter = -1.f;
			i->rotation_rate = 0.f;
			i->angle = 0.f;
			i->destroyed = TRUE;
			i->collected = FALSE;
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
