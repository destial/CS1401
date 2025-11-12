/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 September 2025
@course CSD1401
@brief Definition file for all main game functions and logic
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cprocessing.h"
#include "game.h"
#include "utils.h"
#include "main_menu.h"

#include "scoring.h"
#include "level_manager.h"
#include "asset_manager.h"
#include "editor_manager.h"
#include "state_manager.h"
#include "particle_manager.h"
#include "ui_manager.h"
#include "tutorial_manager.h"

// initialize global variables here
CP_Font font;
CP_Image golf_ball_image, background_image;
CP_Sound hit_sound;
void* item_image_cache[ITEM_LAST];

// mouse positions in screen and world space
CP_Vector mouse = { 0, 0 }, mouse_world = { 0, 0 };

// currently dragging ball
GolfBall* current;
float swinging = 0.f;

// scoreboard position
CP_Vector sBoard;

#ifdef _DEBUG
bool debug_enabled = true;
#else
bool debug_enabled = false;
#endif
bool editor_mode = false;

// editable setting
float resistance = 1.25f;
float power = 1.f;
char current_level = 1;
float current_time;

// particles
ParticleType particle_type = PARTICLE_FLAME;
float particle_counter = 0.f;

// camera
CP_Vector camera_pos;
CP_Matrix camera_matrix;
float camera_scale;

// reset on level loading
float scale_transition = 1.f, turn_now = 0.f, turn_now_hold = 0.f, pause_menu_transition = -1.f;
bool playable = false, win = false, has_won = false, all_balls_stopped = true, resetting = false, turn_now_revert = false, pause_menu = false;
#define MIN_VEL 0.05f

// load based on level
ObjectArray golf_balls, holes, walls, items, terrains;

CP_Vector world_limits, og_world_limits; // 16x9 aspect ratio cuz its a requirement (x is width, y is height) 

static Wall* Get_Walls() {
	return walls.arr;
}

static Item* Get_Items() {
	return items.arr;
}

static GolfBall* Get_Golfs() {
	return golf_balls.arr;
}

static Hole* Get_Holes() {
	return holes.arr;
}

static Terrain* Get_Terrains() {
	return terrains.arr;
}

static void load_borders(float middle_wall_angle) {
	// middle wall is always the first (never in the level file)
	Wall* middle_wall = walls.arr;
	middle_wall->pos.x = world_limits.x * 0.5f;
	middle_wall->pos.y = world_limits.y * 0.5f;
	middle_wall->scale.x = world_limits.x * 0.02f;
	middle_wall->scale.y = world_limits.y * 2.5f;
	middle_wall->color = Get_Color(COLOR_GRAY);
	middle_wall->angle = middle_wall_angle;
	middle_wall->rotation_rate = 0.f;
	middle_wall->image = Asset_GetImage(BORDER_WALL_ASSET);
	middle_wall->destroyed = false;

	Wall* top_wall = middle_wall + 1;
	top_wall->scale.x = world_limits.x * 0.05f;
	top_wall->scale.y = world_limits.y * 2.5f;
	top_wall->pos.x = world_limits.x * 0.5f;
	top_wall->pos.y = world_limits.y - (top_wall->scale.x * 0.5f);
	top_wall->color = Get_Color(COLOR_GRAY);
	top_wall->angle = 90.f;
	top_wall->rotation_rate = 0.f;
	top_wall->image = Asset_GetImage(BORDER_WALL_ASSET);
	top_wall->destroyed = false;

	Wall* bot_wall = middle_wall + 2;
	bot_wall->scale.x = world_limits.x * 0.02f;
	bot_wall->scale.y = world_limits.y * 2.5f;
	bot_wall->pos.x = world_limits.x * 0.5f;
	bot_wall->pos.y = bot_wall->scale.x * 0.5f;
	bot_wall->color = Get_Color(COLOR_GRAY);
	bot_wall->angle = 90.f;
	bot_wall->rotation_rate = 0.f;
	bot_wall->image = Asset_GetImage(BORDER_WALL_ASSET);
	bot_wall->destroyed = false;

	Wall* left_wall = middle_wall + 3;
	left_wall->scale.x = world_limits.x * 0.02f;
	left_wall->scale.y = world_limits.y * 2.5f;
	left_wall->pos.x = left_wall->scale.x * 0.5f;
	left_wall->pos.y = world_limits.y * 0.5f;
	left_wall->color = Get_Color(COLOR_GRAY);
	left_wall->angle = 0.f;
	left_wall->rotation_rate = 0.f;
	left_wall->image = Asset_GetImage(BORDER_WALL_ASSET);
	left_wall->destroyed = false;

	Wall* right_wall = middle_wall + 4;
	right_wall->scale.x = world_limits.x * 0.02f;
	right_wall->scale.y = world_limits.y * 2.5f;
	right_wall->pos.x = world_limits.x - (right_wall->scale.x * 0.5f);
	right_wall->pos.y = world_limits.y * 0.5f;
	right_wall->color = Get_Color(COLOR_GRAY);
	right_wall->angle = 0.f;
	right_wall->rotation_rate = 0.f;
	right_wall->image = Asset_GetImage(BORDER_WALL_ASSET);
	right_wall->destroyed = false;
}

// Initialize the font, golf positions and wall positions
void game_init(void)
{
	Set_Current_State(STATE_GAME);
	printf("Init game\n");
	font = Asset_GetFont(ARIAL_FONT_ASSET);
	golf_ball_image = Asset_GetImage(GOLF_BALL_ASSET);
	background_image = Asset_GetImage(GAME_BACKGROUND_ASSET);
	hit_sound = Asset_GetAudio(GOLF_HIT_WALL_ASSET);
	item_image_cache[ITEM_BOOST - 1] = Asset_GetImage(BOOST_ITEM_ASSET);
	item_image_cache[ITEM_GROW - 1] = Asset_GetImage(BIG_ITEM_ASSET);
	item_image_cache[ITEM_SHRINK - 1] = Asset_GetImage(SMALL_ITEM_ASSET);
	item_image_cache[ITEM_REVERSE - 1] = Asset_GetImage(REVERSE_ITEM_ASSET);
	item_image_cache[ITEM_COIN - 1] = Asset_GetSprite(COIN_ITEM_SPRITE_ASSET, 2, 4, 0.5f);
	item_image_cache[ITEM_PORTAL - 1] = Asset_GetSprite(BLUE_PORTAL_SPRITE_ASSET, 4, 6, 1.5f);
	item_image_cache[ITEM_PORTAL] = Asset_GetSprite(ORANGE_PORTAL_SPRITE_ASSET, 4, 6, 1.5f);
	
	CP_Settings_TextSize(20.f);
	CP_Font_Set(font);
	CP_Settings_ImageMode(CP_POSITION_CORNER);

	items.arr = NULL;
	walls.arr = NULL;
	terrains.arr = NULL;
	holes.arr = NULL;
	golf_balls.arr = NULL;

	// load the current level file set from current_level variable
	Game_Load();

	// reset on level loading
	scale_transition = 1.f;
	turn_now = 0.f;
	turn_now_hold = 0.f;
	turn_now_revert = false;
	playable = false;
	win = false;
	has_won = false;
	all_balls_stopped = true;
	editor_mode = false;
	pause_menu_transition = -1.f;
	pause_menu = false;

	// fix timestep
	current_time = CP_System_GetSeconds();

	// camera matrix transformation
	camera_matrix = CP_Matrix_Identity();
	camera_scale = 1.f;
	camera_pos = CP_Vector_Set(0, 0);
	
	scoring_init();
	Game_SetResetting(false);
	CP_Sound_PlayMusic(Asset_GetAudio(BACKGROUND_MUSIC_ASSET));
}

void game_update(void)
{
	Process_Inputs();

	if (Tutorial_GetMode()) {
		Tutorial_Update(); 
		if (!Tutorial_GetMode()) {
			current_time = CP_System_GetSeconds();
		}
	}
	else {
		// update pause menu
		if (pause_menu_transition >= 0.f) {
			if (pause_menu) {
				pause_menu_transition += (3.f * CP_System_GetDt());
				pause_menu_transition = min(1.f, pause_menu_transition);
			}
			else {
				pause_menu_transition -= (3.f * CP_System_GetDt());
			}

			// continue button
			if (UI_ButtonImageText(
				CP_System_GetWindowWidth() * 0.5f,
				CP_System_GetWindowHeight() - (CP_System_GetWindowHeight() * 0.55f * pause_menu_transition),
				300, 100,
				Asset_GetImage(BUTTON_BLUE_ASSET),
				Asset_GetImage(BUTTON_YELLOW_ASSET),
				"CONTINUE")) {
				pause_menu_transition = 0.f;
				pause_menu = !pause_menu;
			}

			// quit button
			if (UI_ButtonImageText(
				CP_System_GetWindowWidth() * 0.5f,
				CP_System_GetWindowHeight() - (CP_System_GetWindowHeight() * 0.4f * pause_menu_transition),
				300, 100,
				Asset_GetImage(BUTTON_BLUE_ASSET),
				Asset_GetImage(BUTTON_YELLOW_ASSET),
				"  QUIT  ")) {
				Set_Next_State(STATE_MAINMENU);
			}
		}
		else {
			// semi-fixed timestep for physics time
			// incase the game gets too laggy for slow pc
			float new_time = CP_System_GetSeconds();
			float frame_time = new_time - current_time;
			current_time = new_time;

			float dt;
			// run timestep for every lost frame if < 60fps
			while (frame_time > 0.f) {
				dt = min(frame_time, 1 / 60.f);
				Game_UpdatePhysics(dt);
				frame_time -= dt;
			}

			// tab is only for debug purposes, will be removed in the actual build
			if ((win || CP_Input_KeyDown(KEY_TAB)) && !editor_mode)
				scoring_update();
		}
	}

	// rendering does not need to be in physics time
	CP_Settings_ApplyMatrix(camera_matrix);
	Game_Render();
	CP_Settings_ResetMatrix();
}

void Game_UpdatePhysics(float dt)
{
	// entry animation
	if (scale_transition != 0.f) {
		scale_transition -= dt;
		scale_transition = max(0.f, scale_transition);
	}
	else if (!playable) {
		playable = true;
	}

	Item* item_arr = Get_Items();
	Wall* wall_arr = Get_Walls();
	GolfBall* golf_arr = Get_Golfs();
	Hole* hole_arr = Get_Holes();

	// golf ball player input logic (ensure all golf balls are stopped before player can putt)
	all_balls_stopped = true;
	for (int i = 0; i < golf_balls.length; ++i) {
		GolfBall* golf = golf_arr + i;
		if (golf->dragging || golf->halt || golf->scored || golf->destroyed)
			continue;

		// if ball is still moving, cant play
		if (CP_Vector_LengthSquared(golf->vel) > MIN_VEL) {
			all_balls_stopped = false;
			break;
		}
	}

	// update swinging animation
	if (swinging) {
		swinging -= dt;
		swinging = max(0.f, swinging);
		if (swinging == 0.f) {
			current = NULL;
			add_stroke();
			CP_Sound_Play(hit_sound);
			turn_now = 0.f;
			turn_now_hold = 0.f;
			turn_now_revert = false;
		}
	}
	// update golf balls
	win = true;
	for (int i = 0; i < golf_balls.length; ++i) {
		GolfBall* golf = golf_arr + i;
		if (golf->scored || golf->destroyed)
			continue;

		win = false;
		if (editor_mode)
			continue;

		// golf ball scoring animation
		if (golf->scoring) {
			golf->diameter -= dt;
			golf->diameter = max(golf->diameter, 0.f);
			if (golf->diameter == 0.f) {
				golf->scored = true;
			}
			continue;
		}

		if (swinging)
			continue;

		// Dragging the power of the ball
		if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT) && playable) {

			// make sure the mouse is on the ball and its currently not moving
			if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && CP_Vector_Distance(mouse_world, golf->pos) <= (golf->diameter * 0.5) && current == NULL) {
				if (golf->dragging = all_balls_stopped) { // intentional!!!!!!!!!!!
					current = golf;
				}
			}

			// manipulate the velocity based on the mouse coordinates
			if (golf->dragging && golf == current) {
				golf->vel = CP_Vector_Subtract(mouse_world, golf->pos);
				golf->vel = CP_Vector_Scale(golf->vel, power);

				// set the other balls velocity
				for (int j = 0; j < golf_balls.length; ++j) {
					GolfBall* other_golf = Get_Golfs() + j;
					if (other_golf->scored || i == j)
						continue;

					other_golf->vel = CP_Vector_Set(golf->vel.x, golf->vel.y);

					// make sure they dont move when setting this velocity
					other_golf->halt = true;
				}
			}
		}
		else {
			// if the previous frame was player input, that means the golf ball has been released and been putted
			if (golf->dragging) {
				golf->stroke_pos = CP_Vector_Set(golf->pos.x, golf->pos.y);
				golf->dragging = false;
				golf->halt = false;
				swinging = 0.5f;
				break;
			}
			golf->dragging = false;
			golf->halt = false;
			current = NULL;
		}

		// apply velocity to the golf ball if not being dragged
		if (!golf->dragging && !golf->halt) {
			// this is to ensure the length cannot be 0 because anything divided by 0 is an error
			if (CP_Vector_LengthSquared(golf->vel) > MIN_VEL) {
				CP_Vector vel = CP_Vector_Scale(golf->vel, dt);
				float r = Get_Resistance_At(golf->pos);
				if (r < 0.f) {
					golf->pos = golf->stroke_pos;
					golf->vel.x = 0;
					golf->vel.y = 0;
					continue;
				}
				float friction = r - 1;
				if (friction != 0) {
					golf->vel = CP_Vector_Subtract(golf->vel, CP_Vector_Scale(vel, (r * r) - 1.f));
				}

				golf->pos = CP_Vector_Add(golf->pos, vel);

				bool powerup = false;
				// item powerup checks here !!!!!
				for (int j = 0; j < items.length; ++j) {
					Item* item = item_arr + j;
					if (item == NULL || item->collected)
						continue;

					if (Check_Item(item, golf))
						powerup = true;
				}
				// ensure the ball is no longer in the boundaries of the portal
				// after teleportation, dont want to teleport every frame
				if (!powerup && golf->teleported) {
					golf->teleported = false;
				}
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
				CP_Sound_Play(hit_sound);
			}

			// bound the ball at the left border
			if (golf->pos.x < golf->diameter * 0.5f + (world_limits.x - world_limits.x)) {
				golf->pos.x = golf->diameter * 0.5f + (world_limits.x - world_limits.x);
				golf->vel.x = -golf->vel.x / resistance;
				golf->vel.y = golf->vel.y / resistance;
				CP_Sound_Play(hit_sound);
			}

			// bound the ball at the top border
			if (golf->pos.y > world_limits.y - golf->diameter * 0.5f) {
				golf->pos.y = world_limits.y - golf->diameter * 0.5f;
				golf->vel.y = -golf->vel.y / resistance;
				golf->vel.x = golf->vel.x / resistance;
				CP_Sound_Play(hit_sound);
			}

			// bound the ball at the bottom border
			if (golf->pos.y < golf->diameter * 0.5f + (world_limits.x - world_limits.x)) {
				golf->pos.y = golf->diameter * 0.5f + (world_limits.x - world_limits.x);
				golf->vel.y = -golf->vel.y / resistance;
				golf->vel.x = golf->vel.x / resistance;
				CP_Sound_Play(hit_sound);
			}

			// rotate the ball based on its x-axis velocity
			golf->angle += (CP_Vector_Length(golf->vel) * 0.5f) * (golf->vel.x > 0 ? 1 : -1);
			if (golf->angle > 360.f) {
				golf->angle -= 360.f;
			}
			else if (golf->angle < 0.f) {
				golf->angle += 360.f;
			}
		}

		// spawn the particles every 0.1s
		if ((particle_counter += dt) >= 0.1f) {
			int amount = 1;
			float radius = particle_type == PARTICLE_STAR ? golf->diameter * 0.1f : golf->diameter * 0.5f;
			float speed = particle_type == PARTICLE_STAR ? 0.f : 1.f;
			float maxSize = particle_type == PARTICLE_STAR ? 0.25f * golf->diameter : 0.75f * golf->diameter;
			float duration = particle_type == PARTICLE_STAR ? 1.f : 0.5f;
			Particles_Spawn(particle_type, golf->pos, amount, radius, speed, duration, 0.25f * golf->diameter, maxSize);
			particle_counter = 0.f;
		}

		// check ball to ball collision
		for (int j = i; j < golf_balls.length; ++j) {
			GolfBall* other = golf_arr + j;
			if (i == j || other == NULL || other->scored || other->destroyed)
				continue;

			Check_Ball_Collision(golf, other);
		}

		// calculate if golf ball is inside the hole
		for (int h = 0; h < holes.length; ++h) {
			Hole* hole = hole_arr + h;
			if (hole == NULL || hole->destroyed)
				continue;

			if (CP_Vector_Distance(golf->pos, hole->pos) < (hole->diameter * 0.5f) - (golf->diameter * 0.5f)) { // hole 1
				golf->scoring = true;
			}
		}
	}

	for (int i = 0; i < walls.length; ++i) {
		Wall* wall = wall_arr + i;
		if (wall == NULL || wall->destroyed)
			continue;

		// update wall's normal
		Game_UpdateWall(wall, dt);

		if (editor_mode)
			continue;

		for (int j = 0; j < golf_balls.length; ++j) {
			GolfBall* ball = golf_arr + j;
			if (ball->scored)
				continue;

			// check collision detection
			if (i == 0 || (wall->pos.x <= world_limits.x * 0.5f && ball->pos.x <= world_limits.x * 0.5f))
				Check_Wall_Collision(wall, ball);
			else if (wall->pos.x >= world_limits.x * 0.5f && ball->pos.x >= world_limits.x * 0.5f)
				Check_Wall_Collision(wall, ball);
		}
	}

	// after all updates, set the previous position to here
	for (int i = 0; i < golf_balls.length; ++i) {
		GolfBall* ball = golf_arr + i;
		if (ball->scored || ball->destroyed)
			continue;

		ball->prev_pos = CP_Vector_Set(ball->pos.x, ball->pos.y);
	}

	Particles_Update(dt);

	if (win && !has_won) {
		has_won = true;
		CP_Sound_Play(Asset_GetAudio(VICTORY_ASSET));
	}
}

void Game_Render(void)
{
	// Clear background and render the background image
	CP_Graphics_ClearBackground(Get_Color(COLOR_BLACK));

	// reset the color and stroke settings
	CP_Settings_Stroke(Get_Color(COLOR_BLACK));
	CP_Settings_Fill(Get_Color(COLOR_BLACK));
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Font_Set(font);

	CP_Image_Draw(background_image, 0.f, 0.f, (float) CP_System_GetWindowWidth(), (float) CP_System_GetWindowHeight(), 255);

	// render terrains
	Game_RenderTerrains();

	// render holes
	Game_RenderHoles();

	// render shadows
	//if (get_shadow_setting())
	//	Game_RenderShadows();

	// render walls
	Game_RenderWalls();

	// render items
	Game_RenderItems();

	if (playable) 
		Particles_Render(Get_World_Limits(), Get_World_Ratios());

	// render golf balls
	Game_RenderGolfs();

	// render ui
	Game_RenderUI();

	if (editor_mode) {
		Editor_Render();
	}
	else {
		Tutorial_Render();
	}

	if (pause_menu_transition >= 0.f) {
		CP_Color fade = Get_ColorAlpha(COLOR_BLACK, 192);
		CP_Settings_NoStroke();
		CP_Settings_Fill(fade);
		CP_Settings_RectMode(CP_POSITION_CORNER);
		CP_Graphics_DrawRect(0.f, 0.f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());

		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Image_Draw(Asset_GetImage(SETTING_BACKGROUND_ASSET),
			CP_System_GetWindowWidth() * 0.5f,
			CP_System_GetWindowHeight() - (CP_System_GetWindowHeight() * 0.5f * pause_menu_transition),
			CP_System_GetWindowWidth() - 1000.f, CP_System_GetWindowHeight() - 300.f, 255
		);

		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_TextSize(60.f);
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Font_DrawText("Game Paused!",
			CP_System_GetWindowWidth() * 0.5f,
			CP_System_GetWindowHeight() - (CP_System_GetWindowHeight() * 0.7f * pause_menu_transition));
	}

	UI_Render();
}

void Game_RenderUI(void) {
	GolfBall* golf_arr = Get_Golfs();
	for (int i = 0; i < golf_balls.length; ++i) {
		GolfBall* golf = golf_arr + i;
		// render current stroke
		if (golf->dragging && golf == current) {

			// render the current power of the ball
			CP_Vector a = World_To_Screen(golf->pos.x, golf->pos.y, world_limits.x, world_limits.y);
			CP_Vector b = World_To_Screen(golf->pos.x + golf->vel.x, golf->pos.y + golf->vel.y, world_limits.x, world_limits.y);
			int x = (int)(CP_Vector_LengthSquared(golf->vel) * (world_limits.x / world_limits.y));
			int red = CP_Math_ClampInt(x, 0, 255);
			int green = 255 - red;
			CP_Settings_Fill(CP_Color_Create(red, green, 0, 255));
			CP_Settings_Stroke(CP_Color_Create(red, green, 0, 255));
			CP_Settings_LineCapMode(CP_LINE_CAP_ROUND);
			CP_Graphics_DrawLine(a.x, a.y, b.x, b.y);
		}
	}

	CP_Settings_ResetMatrix(); // make sure the UI is not affected by the camera matrix
	if (!editor_mode) {
		// draw current strokes
		CP_Settings_TextSize(40.f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Settings_Stroke(Get_Color(COLOR_BLACK));
		char str[32]; sprintf_s(str, 32, "Strokes: %d", get_strokes());
		CP_Font_DrawText(str, CP_System_GetWindowWidth() * 0.1f, 5);

		char par[32]; sprintf_s(par, 32, "Par: %d", get_lvl_par());
		CP_Font_DrawText(par, CP_System_GetWindowWidth() * 0.25f, 5);

		char lvl[32]; sprintf_s(lvl, 32, "Level: %d", Game_GetLevel());
		CP_Font_DrawText(lvl, CP_System_GetWindowWidth() * 0.4f, 5);
	}

	if ((win || CP_Input_KeyDown(KEY_TAB)) && !editor_mode) {
		draw_scoreboard(sBoard, world_limits.x, world_limits.y);
		/*
		@todo: add button to cycle to next level or replay the same level
		*/
	}
	else if (playable && all_balls_stopped && !editor_mode) {
		if (turn_now >= 0.f) {
			CP_Settings_Fill(Get_Color(COLOR_CARMINE));
			CP_Settings_Stroke(Get_Color(COLOR_RED));
			CP_Settings_TextSize(120.f * turn_now);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Font_DrawText("It's your turn now!", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f);
			if (turn_now_revert) {
				turn_now -= CP_System_GetDt();
				if (turn_now < 0.f) {
					turn_now_revert = false;
				}
			}
			else {
				turn_now += CP_System_GetDt();
				if (turn_now > 1.f) {
					turn_now = 1.f;
					turn_now_hold += CP_System_GetDt();
					if (turn_now_hold > 1.f) {
						turn_now_hold = 0.f;
						turn_now_revert = true;
					}
				}
			}
		}
	}

	// render debug text on top of everything
	CP_Settings_Fill(Get_Color(COLOR_GREEN));
	CP_Settings_TextSize(30.f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
	char fps[50]; sprintf_s(fps, 50, "FPS: %d", (int)(1 / CP_System_GetDt()));
	CP_Font_DrawText(fps, 5, 5);
	if (debug_enabled) {
		CP_Settings_TextSize(20.f);
		Game_RenderDebug();
	}

	// reapply camera matrix
	CP_Settings_ApplyMatrix(camera_matrix);
}

void Game_RenderTerrains(void) {
	CP_Vector world_ratios = Get_World_Ratios();
	Terrain* terrain_arr = Get_Terrains();
	CP_Settings_Stroke(Get_Color(COLOR_CLEAR));
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Color sand = Get_ColorAlpha(COLOR_ORANGE, 128);
	CP_Color ice = Get_ColorAlpha(COLOR_LIGHT_BLUE, 128);
	CP_Color pit = Get_ColorAlpha(COLOR_RED, 128);

	for (int i = 0; i < terrains.length; ++i) {
		Terrain* terrain = terrain_arr + i;
		if (terrain == NULL || terrain->destroyed)
			continue;

		CP_Vector c = World_To_Screen(terrain->pos.x, terrain->pos.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(terrain->type == TERRAIN_SAND ? sand : terrain->type == TERRAIN_ICE ? ice : pit);
		CP_Graphics_DrawRectAdvanced(
			c.x,
			c.y,
			terrain->scale.x * world_ratios.x * (1.f - scale_transition),
			terrain->scale.y * world_ratios.y * (1.f - scale_transition),
			terrain->angle,
			2.f
		);

		if (debug_enabled) {
			CP_Settings_Fill(Get_Color(COLOR_GREEN));
			CP_Settings_TextSize(20.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
			char p[50]; sprintf_s(p, 50, "Index: %d", i);
			CP_Font_DrawText(p, c.x, c.y + 40);

			char b[50]; sprintf_s(b, 50, "Pos: (%.2f, %.2f)", terrain->pos.x, terrain->pos.y);
			CP_Font_DrawText(b, c.x, c.y + 60);

			char s[50]; sprintf_s(s, 50, "Scale: (%.2f, %.2f)", terrain->scale.x, terrain->scale.y);
			CP_Font_DrawText(s, c.x, c.y + 80);

			char a[100]; sprintf_s(a, 100, "Degrees: %.2f | Radians: %.2f", terrain->angle, terrain->angle_radians);
			CP_Font_DrawText(a, c.x, c.y + 100);
		}
	}
}

void Game_RenderWalls(void) {
	CP_Vector world_ratios = Get_World_Ratios();
	Wall* wall_arr = Get_Walls();

	// main walls
	for (int i = 0; i < walls.length; ++i) {
		Wall* wall = wall_arr + i;
		if (wall == NULL || wall->destroyed)
			continue;

		CP_Vector c = World_To_Screen(wall->pos.x, wall->pos.y, world_limits.x, world_limits.y);
		if (wall->image == NULL) { // theres no image for this wall (most likely the middle wall)
			CP_Settings_Fill(wall->color);
			CP_Settings_Stroke(Get_Color(COLOR_CLEAR));
			CP_Settings_RectMode(CP_POSITION_CENTER);
			CP_Graphics_DrawRectAdvanced(
				c.x,
				c.y,
				wall->scale.x * world_ratios.x * (i > 4 ? (1.f - scale_transition) : 1.f),
				wall->scale.y * world_ratios.y * (i > 4 ? (1.f - scale_transition) : 1.f),
				wall->angle,
				0.f
			);
		}
		else { // theres image for this wall, use that
			CP_Settings_Save();

			CP_Settings_Fill(Get_Color(COLOR_WHITE));
			CP_Settings_Stroke(Get_Color(COLOR_CLEAR));
			CP_Settings_ImageMode(CP_POSITION_CENTER);
			CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
			CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_MIRROR);
			int w = CP_Image_GetWidth(wall->image);
			int h = CP_Image_GetHeight(wall->image);
			// used for tiling the wall sprite based on its scale
			int d = gcd((int)wall->scale.x, (int)wall->scale.y);
			CP_Image_DrawSubImageAdvanced(wall->image,
				c.x,
				c.y,
				wall->scale.x * world_ratios.x * (i > 4 ? (1.f - scale_transition) : 1.f),
				wall->scale.y * world_ratios.y * (i > 4 ? (1.f - scale_transition) : 1.f),
				0.f, 0.f, (i > 4 ? w * (wall->scale.x / d) : w * wall->scale.x), (i > 4 ? h * (wall->scale.y / d) : h * wall->scale.y),
				255, wall->angle
			);

			CP_Settings_Restore();
		}

		if (debug_enabled) {
			// display wall data on the screen
			CP_Settings_Fill(Get_Color(COLOR_GREEN));
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
			char r[100]; sprintf_s(r, 100, "Right: (%.2f, %.2f) (Rad: %.2f)", wall->right.x, wall->right.y, CP_Math_Radians(wall->angle));
			CP_Font_DrawText(r, c.x, c.y + 120);
			char rr[100]; sprintf_s(rr, 100, "Rot Rate: %.2f", wall->rotation_rate);
			CP_Font_DrawText(rr, c.x, c.y + 140);
			if (!editor_mode) {
				// draw the closest point after rotating it back to original
				CP_Vector closest_point = CP_Vector_Rotate(wall->_collision_point, -CP_Math_Radians(wall->angle));
				CP_Vector c = World_To_Screen(wall->pos.x + closest_point.x, wall->pos.y + closest_point.y, world_limits.x, world_limits.y);
				CP_Settings_Fill(Get_Color(COLOR_RED));
				CP_Settings_Stroke(Get_Color(COLOR_RED));
				CP_Graphics_DrawPoint(c.x, c.y);

				// draw the collision normal
				CP_Vector col_normal = CP_Vector_Rotate(wall->_wall_to_ball, -CP_Math_Radians(wall->angle));
				CP_Vector coords = World_To_Screen(wall->pos.x + closest_point.x + col_normal.x, wall->pos.y + closest_point.y + col_normal.y, world_limits.x, world_limits.y);
				CP_Settings_Fill(Get_Color(COLOR_PURPLE));
				CP_Settings_Stroke(Get_Color(COLOR_PURPLE));
				CP_Graphics_DrawLine(c.x, c.y, coords.x, coords.y);
			}
		}
	}
}

void Game_RenderHoles(void) {
	CP_Vector world_ratios = Get_World_Ratios();
	Hole* hole_arr = Get_Holes();
	for (int i = 0; i < holes.length; ++i) {
		Hole* hole = hole_arr + i;
		if (hole == NULL || hole->destroyed)
			continue;

		CP_Vector coords = World_To_Screen(hole->pos.x, hole->pos.y, world_limits.x, world_limits.y);
		
		CP_Settings_Stroke(Get_Color(COLOR_WHITE));
		CP_Settings_Fill(Get_Color(COLOR_BLACK));
		CP_Graphics_DrawCircle(
			coords.x,
			coords.y,
			hole->diameter * world_ratios.x * (1.f - scale_transition));

		if (debug_enabled) {
			CP_Vector coords = World_To_Screen(hole->pos.x, hole->pos.y, world_limits.x, world_limits.y);
			CP_Settings_Fill(Get_Color(COLOR_GREEN));
			CP_Settings_TextSize(20.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
			char p[50]; sprintf_s(p, 50, "Pos: (%.2f, %.2f)", hole->pos.x, hole->pos.y);
			CP_Font_DrawText(p, coords.x, coords.y + 50);
		}
	}
}

void Game_RenderItems(void) {
	CP_Vector world_ratios = Get_World_Ratios();
	Item* item_arr = Get_Items();
	float anim_mul = (sinf(5.f * CP_System_GetSeconds()) / 10.f) + 1.f;

	for (int i = 0; i < items.length; ++i) {
		Item* item = item_arr + i;
		if (item == NULL || item->destroyed || (item->collected && !editor_mode) )
			continue;

		CP_Vector coords = World_To_Screen(item->pos.x, item->pos.y, world_limits.x, world_limits.y);

		CP_Image image = NULL;
		SpriteSheet* sprite = NULL;
		switch (item->type) {
		case ITEM_PORTAL:
			sprite = item_image_cache[(item->pos.x <= world_limits.x * 0.5f ? ITEM_PORTAL - 1 : ITEM_PORTAL)];
			anim_mul = 1.f;
			break;
		case ITEM_COIN:
			sprite = item_image_cache[item->type - 1];
			break;
		default:
			image = item_image_cache[item->type - 1];
			break;
		}

		CP_Settings_Fill(Get_Color(COLOR_WHITE));
		CP_Settings_Stroke(Get_Color(COLOR_WHITE));
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
		if (image != NULL) {
			if (item->type == ITEM_REVERSE) {
				item->angle += 45.f * CP_System_GetDt();
				item->angle = item->angle < 360.f ? item->angle : 360.f - item->angle;
			}
			
			CP_Image_DrawAdvanced(image,
				coords.x,
				coords.y,
				item->diameter * world_ratios.x * anim_mul * (1.f - scale_transition),
				item->diameter * world_ratios.x * anim_mul * (1.f - scale_transition),
				255,
				item->angle);
		}

		if (sprite != NULL) {
			Sprite_Draw(sprite,
				coords.x,
				coords.y,
				item->diameter * world_ratios.x * anim_mul * (1.f - scale_transition),
				item->diameter * world_ratios.x * anim_mul * (1.f - scale_transition),
				255);
		}

		if (debug_enabled) {
			CP_Settings_Fill(Get_Color(COLOR_GREEN));
			CP_Settings_TextSize(20.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
			char in[64]; sprintf_s(in, 64, "Index: %d", i);
			CP_Font_DrawText(in, coords.x, coords.y + 40);

			char p[64]; sprintf_s(p, 64, "Pos: (%.2f, %.2f)", item->pos.x, item->pos.y);
			CP_Font_DrawText(p, coords.x, coords.y + 60);

			CP_Settings_NoFill();
			CP_Settings_Stroke(Get_Color(COLOR_GREEN));
			CP_Graphics_DrawCircle(coords.x, coords.y, item->diameter * world_ratios.x);
		}
	}
}

void Game_RenderGolfs(void) {
	CP_Vector world_ratios = Get_World_Ratios();
	GolfBall* golf_arr = Get_Golfs();

	for (int i = 0; i < golf_balls.length; ++i) {
		GolfBall* golf = golf_arr + i;
		if (golf->scored || golf->destroyed) // ignore if golf ball is already scored
			continue;

		// render golf ball
		CP_Vector coords = World_To_Screen(golf->pos.x, golf->pos.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(Get_Color(COLOR_WHITE));
		CP_Settings_Stroke(Get_Color(COLOR_WHITE));
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
		CP_Image_DrawAdvanced(golf_ball_image,
			coords.x,
			coords.y,
			golf->diameter * world_ratios.x * (1.f - scale_transition),
			golf->diameter * world_ratios.x * (1.f - scale_transition),
			255,
			golf->angle);

		if (swinging) {
			CP_Vector n = CP_Vector_Normalize(golf->vel);
			n = CP_Vector_Scale(n, (golf->diameter * 0.5f) + sqrtf(min(5.f, CP_Vector_Length(golf->vel) * swinging)));
			CP_Vector b = World_To_Screen(golf->pos.x - n.x, golf->pos.y - n.y, world_limits.x, world_limits.y);
			float angle = CP_Vector_AngleCCW(CP_Vector_Set(0, 1), n);

			CP_Settings_Save();

			CP_Settings_Fill(Get_Color(COLOR_WHITE));
			CP_Settings_Stroke(Get_Color(COLOR_BLACK));
			CP_Settings_StrokeWeight(1.f);
			CP_Settings_RectMode(CP_POSITION_CENTER);
			CP_Graphics_DrawRectAdvanced(b.x, b.y, 
				golf->diameter * world_ratios.x,
				golf->diameter * world_ratios.x * 0.5f, angle, 1.f);

			CP_Settings_Restore();
		}

		if (debug_enabled) {
			CP_Settings_Fill(Get_Color(COLOR_GREEN));
			CP_Settings_TextSize(20.f);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BOTTOM);
			char p[50]; sprintf_s(p, 50, "Pos: (%.2f, %.2f)", golf->pos.x, golf->pos.y);
			CP_Font_DrawText(p, coords.x, coords.y + 40);

			char b[50]; sprintf_s(b, 50, "Vel: (%.2f, %.2f)", golf->vel.x, golf->vel.y);
			CP_Font_DrawText(b, coords.x, coords.y + 60);
		}
	}
}

void Game_RenderShadows(void) {
	CP_Vector world_ratios = Get_World_Ratios();
	CP_Color shadow = CP_Color_Create(0, 0, 0, 64);

	Wall* wall_arr = Get_Walls();
	CP_Settings_Fill(shadow);
	CP_Settings_Stroke(Get_Color(COLOR_CLEAR));
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);

	// render wall shadows
	for (int i = 1; i < walls.length; ++i) {
		Wall* wall = wall_arr + i;
		if (wall == NULL || wall->destroyed)
			continue;

		CP_Vector c = World_To_Screen(wall->pos.x, wall->pos.y, world_limits.x, world_limits.y);
		CP_Graphics_DrawRectAdvanced(
			c.x,
			c.y + 5.f,
			wall->scale.x * world_ratios.x * i > 4 ? (1.f - scale_transition) : 1.f,
			wall->scale.y * world_ratios.y * i > 4 ? (1.f - scale_transition) : 1.f,
			wall->angle,
			0.f
		);
	}

	Item* item_arr = Get_Items();
	float anim_mul = (sinf(5.f * CP_System_GetSeconds()) / 10.f) + 1.f;
	// render item shadow
	for (int i = 0; i < items.length; ++i) {
		Item* item = item_arr + i;
		if (item == NULL || item->destroyed || (item->collected && !editor_mode))
			continue;

		CP_Vector coords = World_To_Screen(item->pos.x, item->pos.y, world_limits.x, world_limits.y);
		//CP_Settings_Fill(shadow);
		
		CP_Image image = NULL;
		switch (item->type) {
		case ITEM_BOOST:
			image = Asset_GetBlendedImage(BOOST_ITEM_ASSET, Get_Color(COLOR_BLACK), BLEND_SET);
			break;
		case ITEM_GROW:
			image = Asset_GetBlendedImage(BIG_ITEM_ASSET, Get_Color(COLOR_BLACK), BLEND_SET);
			break;
		case ITEM_SHRINK:
			image = Asset_GetBlendedImage(SMALL_ITEM_ASSET, Get_Color(COLOR_BLACK), BLEND_SET);
			break;
		case ITEM_REVERSE:
			image = Asset_GetBlendedImage(REVERSE_ITEM_ASSET, Get_Color(COLOR_BLACK), BLEND_SET);
			break;
		default:
			break;
		}

		if (image != NULL) {
			CP_Image_DrawAdvanced(image,
				coords.x,
				coords.y + 5.f,
				item->diameter * world_ratios.x * anim_mul * (1.f - scale_transition),
				item->diameter * world_ratios.x * anim_mul * (1.f - scale_transition),
				shadow.a,
				item->angle);
		}
	}

	GolfBall* golf_arr = Get_Golfs();
	// render ball shadow
	for (int i = 0; i < golf_balls.length; ++i) {
		GolfBall* golf = golf_arr + i;
		if (golf->scored || golf->destroyed) // ignore if golf ball is already scored
			continue;

		CP_Vector coords = World_To_Screen(golf->pos.x, golf->pos.y, world_limits.x, world_limits.y);
		CP_Graphics_DrawEllipse(
			coords.x,
			coords.y + 5.f,
			golf->diameter * world_ratios.x * (1.f - scale_transition),
			(golf->diameter * world_ratios.y + 5.f) * (1.f - scale_transition));
	}
}

void game_exit(void)
{
	// free level allocations
	FREE_MEMORY(walls.arr);
	FREE_MEMORY(items.arr);
	FREE_MEMORY(terrains.arr);
	FREE_MEMORY(golf_balls.arr);
	FREE_MEMORY(holes.arr);
	reset_strokes();
	Particles_Free();
	Assets_Free();
	printf("Exit game\n");
}

void Game_SetResetting(bool reset) {
	resetting = reset;
}

void Process_Inputs(void) {
	mouse.x = CP_Input_GetMouseX();
	mouse.y = CP_Input_GetMouseY();
	mouse = CP_Vector_MatrixMultiply(CP_Matrix_Inverse(camera_matrix), mouse);
	mouse_world = Screen_To_World(mouse.x, mouse.y, world_limits.x, world_limits.y);

	// go to main menu if pressed escape key
	if (CP_Input_KeyTriggered(KEY_ESCAPE)) {
		pause_menu_transition = 0.f;
		pause_menu = !pause_menu;
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

	if (CP_Input_KeyTriggered(KEY_F)) {
		if (++particle_type == PARTICLE_LAST) {
			particle_type = 0;
		}
	}

	// reset level
	if (CP_Input_KeyTriggered(KEY_R) && (!CP_Input_KeyDown(KEY_LEFT_CONTROL) && !CP_Input_KeyDown(KEY_RIGHT_CONTROL))) {
		Game_SetResetting(true);
		game_exit();
		game_init();
		Tutorial_Load(false);
		scale_transition = 0.f;
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

		// level editor functions
		Editor_Update_Start();
		Editor_Update_Terrains(&terrains);
		Editor_Update_Holes(&holes);
		Editor_Update_Walls(&walls);
		Editor_Update_Items(&items);
		Editor_Update_Golfballs(&golf_balls);
		Editor_Update_End();
	}
	else {
		// increase / decrease golf putt power
		if (!CP_Input_KeyDown(KEY_LEFT_CONTROL) && !CP_Input_KeyDown(KEY_RIGHT_CONTROL)) {
			power += CP_Input_MouseWheel() * 0.1f;
			power = min_max(power, 0.1f, 3.f); // limit to maximum of 3f, min 0.1f
		}
	}

	// camera controls
	if (CP_Input_KeyDown(KEY_LEFT_CONTROL) || CP_Input_KeyDown(KEY_RIGHT_CONTROL)) {
		if (CP_Input_MouseWheel()) {
			// camera zoom in/out
			float ratio = Get_World_Limits().y / Get_World_Limits().x;
			camera_scale += (CP_Input_MouseWheel() * 0.1f);
			camera_scale = max(1.f, camera_scale);
		}

		if (CP_Input_MouseDown(MOUSE_BUTTON_RIGHT)) {
			// camera movement
			camera_pos.x += CP_Input_GetMouseDeltaX();
			camera_pos.y += CP_Input_GetMouseDeltaY();
		}

		if (CP_Input_KeyTriggered(KEY_R)) {
			// reset camera location back to original
			camera_scale = 1.f;
			camera_pos.x = 0.f;
			camera_pos.y = 0.f;
		}

		// limit camera boundaries to within the visible world
		float x_lim = (CP_System_GetWindowWidth() * (1.f - camera_scale)) / camera_scale;
		float y_lim = (CP_System_GetWindowHeight() * (1.f - camera_scale)) / camera_scale;
		camera_pos.x = min_max(camera_pos.x, x_lim, 0.f);
		camera_pos.y = min_max(camera_pos.y, y_lim, 0.f);

		// apply transformation to matrix
		camera_matrix = CP_Matrix_Multiply(CP_Matrix_Identity(), CP_Matrix_Scale(CP_Vector_Set(camera_scale, camera_scale)));
		camera_matrix = CP_Matrix_Multiply(camera_matrix, CP_Matrix_Translate(camera_pos));
	}
}

// Update the wall's normal based on its angle of rotation
void Game_UpdateWall(Wall* wall, float dt) {
	if (!editor_mode) {
		// update the wall angle based on rotation rate (if any)
		wall->angle += wall->rotation_rate * dt;
	}

	// limit the wall angle to 0 <= x <= 360 degrees for ease of calculation
	if (wall->angle > 360.f) {
		wall->angle = 360.f - wall->angle;
	}
	else if (wall->angle < 0) {
		wall->angle += 360.f;
	}

	// update wall normal based on wall angle
	wall->normal.x = cosf(-CP_Math_Radians(wall->angle));
	wall->normal.y = sinf(-CP_Math_Radians(wall->angle));

	// update right of wall from normal
	wall->right.x = wall->normal.y;
	wall->right.y = -wall->normal.x;
}

// collision detection between golf_ball and walls
bool Check_Wall_Collision(Wall* wall, GolfBall* ball) {
	// get the boundaries of the wall
	CP_Vector wall_min = { -wall->scale.x * 0.5f, -wall->scale.y * 0.5f };
	CP_Vector wall_max = { wall->scale.x * 0.5f, wall->scale.y * 0.5f };

	// ball location position in respect to the wall
	CP_Vector r = CP_Vector_Subtract(ball->pos, wall->pos);

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

	// get back the distance between the golf pos and the closest point
	// this is also the normal for collision response
	CP_Vector wall_to_ball = CP_Vector_Subtract(local_golf_pos, closest_point);

	// now the actual collision checking, the distance between ball center and closest point to wall is less than ball radius
	bool collision = false;
	if (CP_Vector_LengthSquared(wall_to_ball) <= CP_Math_Square(ball->diameter * 0.5f)) {
		collision = true;
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
		Collision_Response_Wall(wall, ball, wall->_collision_point, CP_Vector_Rotate(wall->_wall_to_ball, -CP_Math_Radians(wall->angle)));
	}
	return collision;
}

bool Check_Terrain(Terrain* terrain, CP_Vector pos) {
	// get the boundaries of the terrain
	CP_Vector terrain_min = { -terrain->scale.x * 0.5f, -terrain->scale.y * 0.5f };
	CP_Vector terrain_max = { terrain->scale.x * 0.5f, terrain->scale.y * 0.5f };

	// ball location position in respect to the wall
	CP_Vector r = CP_Vector_Subtract(pos, terrain->pos);

	// rotate local position back to axis
	terrain->angle_radians = CP_Math_Radians(terrain->angle);
	CP_Vector local_pos = CP_Vector_Rotate(r, terrain->angle_radians);

	// AABB collision testing
	return local_pos.x >= terrain_min.x && local_pos.x <= terrain_max.x &&
		   local_pos.y >= terrain_min.y && local_pos.y <= terrain_max.y;
}

bool Check_Item(Item* item, GolfBall* ball) {
	// if the ball is over this item
	bool collected = CP_Vector_Length(CP_Vector_Subtract(item->pos, ball->pos)) <= (item->diameter * 0.5f) + (ball->diameter * 0.5f);
	if (collected) {
		switch (item->type) {
		case ITEM_BOOST:
			// boost the speed of the ball
			ball->vel = CP_Vector_Scale(ball->vel, 3.f);
			break;
		case ITEM_GROW:
			// increase the size of the ball
			ball->diameter *= 2.f;
			break;
		case ITEM_SHRINK:
			// decrease the size of the ball
			ball->diameter *= 0.5f;
			break;
		case ITEM_REVERSE:
			// flip the direction of the ball
			ball->vel = CP_Vector_Negate(ball->vel);
			break;
		case ITEM_PORTAL: {
			// make sure the ball isnt teleported every frame its inside the portal
			// only do it once
			if (ball->teleported)
				return collected;

			// find the next portal that isnt this
			// we only support 2 portals per level as of right now
			// implementing support for more than 2 portals would be a hassle :(
			Item* item_arr = Get_Items();
			for (int i = 0; i < items.length; ++i) {
				Item* it = item_arr + i;
				// get the next portal if exists
				if (it == NULL || it->destroyed || it == item || it->type != ITEM_PORTAL)
					continue;

				// teleport the ball to the other portal
				ball->pos = CP_Vector_Set(it->pos.x, it->pos.y);
				ball->teleported = true;
				return collected;
			}
			break;
		}
		default: break;
		}
		item->collected = true;
	}
	return collected;
}

bool Check_Ball_Collision(GolfBall* ball_1, GolfBall* ball_2) {
	// check if ball is in contact with the other ball
	bool collision = CP_Vector_Length(CP_Vector_Subtract(ball_1->pos, ball_2->pos)) <= (ball_1->diameter * 0.5f) + (ball_2->diameter * 0.5f);
	if (collision) {
		Collision_Response_Ball(ball_1, ball_2);
	}
	return collision;
}

void Collision_Response_Ball(GolfBall* ball_1, GolfBall* ball_2) {
	/*
	source: https://stackoverflow.com/questions/345838/ball-to-ball-collision-detection-and-handling
	*/
	CP_Vector delta = CP_Vector_Subtract(ball_1->pos, ball_2->pos);
	float mag = CP_Vector_Length(delta);
	if (mag == 0.f) { // if balls are on top of each other
		mag = (ball_1->diameter * 0.5f) + (ball_2->diameter * 0.5f) - 1.f;
		delta = CP_Vector_Set((ball_1->diameter * 0.5f) + (ball_2->diameter * 0.5f), 0.f);
	}

	// minimum translation distance to push balls apart after intersecting
	CP_Vector mtd = CP_Vector_Scale(delta, ((ball_1->diameter * 0.5f) + (ball_2->diameter * 0.5f) - mag) / mag);

	// substitute mass of ball with diameter
	// inverse of mass
	float im1 = 1.f / ball_1->diameter;
	float im2 = 1.f / ball_2->diameter;

	float total_im = im1 + im2;

	// push-pull them apart
	ball_1->pos = CP_Vector_Add(ball_1->pos, CP_Vector_Scale(mtd, im1 / total_im));
	ball_2->pos = CP_Vector_Subtract(ball_2->pos, CP_Vector_Scale(mtd, im2 / total_im));

	// impact speed
	CP_Vector v = CP_Vector_Subtract(ball_1->vel, ball_2->vel);
	float vn = CP_Vector_DotProduct(v, CP_Vector_Normalize(mtd));
	if (vn > 0.f) // if balls are intersecting but already moving away from each other, dont do anything
		return;

	// collision impulse
	float restitution = 1.f - (resistance - 1.f);
	float i = (-(1.f + restitution) * vn) / total_im;
	CP_Vector impulse = CP_Vector_Scale(CP_Vector_Normalize(mtd), i);

	// change in momentum
	ball_1->vel = CP_Vector_Add(ball_1->vel, CP_Vector_Scale(impulse, im1));
	ball_2->vel = CP_Vector_Subtract(ball_2->vel, CP_Vector_Scale(impulse, im2));

	CP_Sound_Play(hit_sound);
}

// as the name implies, it handles what to do when the ball hits a wall
void Collision_Response_Wall(Wall* wall, GolfBall* ball, CP_Vector point, CP_Vector normal) {
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

	CP_Sound_Play(hit_sound);
}

float Get_Resistance_At(CP_Vector pos) {
	// get default resistance
	float r = resistance;
	Terrain* terrain_arr = Get_Terrains();
	for (int i = 0; i < terrains.length; ++i) {
		// find the terrain(s) at this spot
		Terrain* terrain = terrain_arr + i;
		if (terrain == NULL || terrain->destroyed)
			continue;

		// multiply the current resistance by the terrain's resistance
		if (Check_Terrain(terrain, pos)) {
			r *= terrain->type == TERRAIN_SAND ? 2.f : terrain->type == TERRAIN_ICE ? 0.5f : terrain->type == TERRAIN_PIT ? -1.f : 1.f;
		}

		// returning -1 if its a pit
		if (r < 0.f)
			return -1.f;
	}
	
	// we minimum limit the resistance to 1 as 
	// anything < 1 will increase the speed of the ball
	r = max(1.f, r);
	return r;
}

// Drawing FPS, mouse positions in screen and world coordinates, and golf data
void Game_RenderDebug(void) {
	char m[50]; sprintf_s(m, 50, "Screen Mouse: (%.0f, %.0f)", mouse.x, mouse.y);
	CP_Font_DrawText(m, 0, 40);

	CP_Vector world = Screen_To_World(mouse.x, mouse.y, world_limits.x, world_limits.y);
	char wm[50]; sprintf_s(wm, 50, "World Mouse: (%.2f, %.2f)", world.x, world.y);
	CP_Font_DrawText(wm, 0, 60);

	char r[50]; sprintf_s(r, 50, "Resistance: %.2f", resistance);
	CP_Font_DrawText(r, 0, 80);

	char po[50]; sprintf_s(po, 50, "Shoot Power: %.2f", power);
	CP_Font_DrawText(po, 0, 100);

	char p[50]; sprintf_s(p, 50, "Mouse Wheel: %.2f", CP_Input_MouseWheel());
	CP_Font_DrawText(p, 0, 120);
}

/*!
@brief Save the current level to text file under "Assets/Levels/level_{current_level}.lvl"
*/
void Game_Save(void) {
	Level_Save(current_level, &walls, &golf_balls, &holes, &items, &terrains, Get_World_Limits(), get_lvl_par());
}

/*!
@brief Load the current level from text file under "Assets/Levels/level_{current_level}.lvl"
*/
void Game_Load(void) {
	// initialize golf balls default values

	// get the currently set level and load the level file
	Level* loaded_level = Level_Load(current_level);

	// if the level file exists, load that one
	if (loaded_level != NULL) {
		// set world size
		world_limits.x = loaded_level->world_width;
		world_limits.y = loaded_level->world_height;
		og_world_limits = CP_Vector_Set(world_limits.x, world_limits.y);
		int tut_mode = loaded_level->tutorial_level;
		if (tut_mode >= 0) {
			Tutorial_SetPhase(tut_mode);
			Tutorial_Load(true);
		}

		// set scoreboard position based on world size
		sBoard = CP_Vector_Set(world_limits.x * 0.5f, world_limits.y * 0.5f);

		// set the par of the level
		set_lvl_par(loaded_level->par);

		// load the golf balls in the level
		golf_balls.length = loaded_level->golf_balls_size;
		golf_balls.arr = gc_malloc(sizeof(GolfBall) * golf_balls.length);
		int ball_index = 0;
		for (int i = 0; i < loaded_level->golf_balls_size * BALL_HOLE_OFFSET; i += BALL_HOLE_OFFSET) {
			GolfBall* ball = Get_Golfs() + ball_index;
			if (ball == NULL)
				continue;

			ball->pos.x = loaded_level->golf_balls[i];
			ball->pos.y = loaded_level->golf_balls[i + 1];
			ball->stroke_pos.x = ball->pos.x;
			ball->stroke_pos.y = ball->pos.y;
			ball->diameter = loaded_level->golf_balls[i + 2];

			ball->vel = CP_Vector_Set(0.f, 0.f);
			ball->angle = 0.f;
			ball->halt = false;
			ball->dragging = false;
			ball->scored = false;
			ball->scoring = false;
			ball->destroyed = false;
			++ball_index;
		}

		// load the holes in the level
		holes.length = loaded_level->holes_size;
		holes.arr = gc_malloc(sizeof(Hole) * holes.length);
		int hole_index = 0;
		for (int i = 0; i < loaded_level->holes_size * BALL_HOLE_OFFSET; i += BALL_HOLE_OFFSET) {
			Hole* hole = Get_Holes() + hole_index;
			if (hole == NULL)
				continue;

			hole->pos.x = loaded_level->holes[i];
			hole->pos.y = loaded_level->holes[i + 1];
			hole->diameter = loaded_level->holes[i + 2];

			hole->destroyed = false;
			++hole_index;
		}

		// load the items in the level
		items.length = loaded_level->items_size;
		if (items.length) { // only if there are items
			items.arr = gc_malloc(sizeof(Item) * items.length);
			int item_index = 0;
			for (int i = 0; i < loaded_level->items_size * ITEMS_OFFSET; i += ITEMS_OFFSET) {
				Item* item = Get_Items() + item_index;
				if (item == NULL)
					continue;
				// load the items according to index in array
				item->pos.x = loaded_level->items[i];
				item->pos.y = loaded_level->items[i + 1];

				item->diameter = loaded_level->items[i + 2];
				item->angle = loaded_level->items[i + 3];
				item->type = (int) loaded_level->items[i + 4];

				item->collected = item->type == ITEM_COIN && resetting ? true : false;
				item->destroyed = false;
				++item_index;
			}
		}
		else {
			items.arr = NULL;
		}

		// load the terrains in the level
		terrains.length = loaded_level->terrains_size;
		if (terrains.length) { // only if there are terrains
			terrains.arr = gc_malloc(sizeof(Terrain) * terrains.length);
			int terrain_index = 0;
			for (int i = 0; i < loaded_level->terrains_size * TERRAINS_OFFSET; i += TERRAINS_OFFSET) {
				Terrain* terrain = Get_Terrains() + terrain_index;
				if (terrain == NULL)
					continue;
				// load the items according to index in array
				terrain->pos.x = loaded_level->terrains[i];
				terrain->pos.y = loaded_level->terrains[i + 1];

				terrain->scale.x = loaded_level->terrains[i + 2];
				terrain->scale.y = loaded_level->terrains[i + 3];

				terrain->angle = loaded_level->terrains[i + 4];
				terrain->type = (int)loaded_level->terrains[i + 5];

				terrain->destroyed = false;
				++terrain_index;
			}
		}
		else {
			terrains.arr = NULL;
		}

		// load the walls in the level
		// (+5 because there will always be a middle wall and 4 borders, which is always at index 0-4)
		walls.length = loaded_level->walls_size + 5;
		walls.arr = gc_malloc(sizeof(Wall) * walls.length);
		if (walls.arr) { // if the OS couldnt allocate any memory
			int wall_index = 5;
			for (int i = 0; i < loaded_level->walls_size * WALLS_OFFSET; i += WALLS_OFFSET) {
				Wall* wall = Get_Walls() + wall_index;
				if (wall == NULL)
					continue;
				// loading walls according to index in array
				wall->pos.x = loaded_level->walls[i];
				wall->pos.y = loaded_level->walls[i + 1];

				wall->scale.x = loaded_level->walls[i + 2];
				wall->scale.y = loaded_level->walls[i + 3];

				wall->angle = loaded_level->walls[i + 4];
				wall->rotation_rate = loaded_level->walls[i + 5];

				wall->color = Get_Color(COLOR_WHITE);

				wall->image = wall->pos.x <= world_limits.x * 0.5f ? Asset_GetImage(WALL_LEFT_ASSET) : Asset_GetImage(WALL_RIGHT_ASSET);
				wall->destroyed = false;
				++wall_index;
			}

			load_borders(loaded_level->middle_wall_angle);
		}
		else {
			// level loading failed, quit the game because heap allocation went wrong!!
			Set_Next_State(STATE_EXIT);
		}

		// free up the loaded level resources (!!!)
		Level_Free(loaded_level);
		loaded_level = NULL;

		return;
	}

	// load a default randomized level if no level file exists
	world_limits = CP_Vector_Set(48.f, 27.f);
	og_world_limits = CP_Vector_Set(world_limits.x, world_limits.y);

	// initialize standard default values
	sBoard = CP_Vector_Set(world_limits.x * 0.5f, world_limits.y * 0.5f);

	golf_balls.length = holes.length = 2;
	golf_balls.arr = gc_malloc(sizeof(GolfBall) * golf_balls.length);
	holes.arr = gc_malloc(sizeof(Hole) * holes.length);
	GolfBall* golfs_arr = Get_Golfs();
	Hole* holes_arr = Get_Holes();
	for (int j = 0; j < golf_balls.length; ++j) {
		GolfBall* ball = golfs_arr + j;
		ball->vel = CP_Vector_Set(0.f, 0.f);
		ball->angle = 0.f;
		ball->diameter = 1.f;
		ball->halt = false;
		ball->dragging = false;
		ball->scored = false;
		ball->scoring = false;
		ball->destroyed = false;
		ball->teleported = false;

		// make sure hole diameter is larger than golf ball so it can actually fit
		Hole* hole = holes_arr + j;
		hole->diameter = 1.5f;
		hole->destroyed = false;
	}

	golfs_arr[0].pos = CP_Vector_Set(world_limits.x * 0.25f, 2.f);
	golfs_arr[0].stroke_pos.x = golfs_arr[0].pos.x;
	golfs_arr[0].stroke_pos.y = golfs_arr[0].pos.y;

	golfs_arr[1].pos = CP_Vector_Set(world_limits.x * 0.75f, 2.f);
	golfs_arr[1].stroke_pos.x = golfs_arr[1].pos.x;
	golfs_arr[1].stroke_pos.y = golfs_arr[1].pos.y;

	holes_arr[0].pos = CP_Vector_Set(world_limits.x * 0.25f, world_limits.y * 0.75f);
	holes_arr[1].pos = CP_Vector_Set(world_limits.x * 0.75f, world_limits.y * 0.75f);

	walls.length = 11;
	walls.arr = gc_malloc(sizeof(Wall) * walls.length);
	if (walls.arr) {
		int wall_index = 1;
		// create the random left side walls
		for (; wall_index < (walls.length * 0.5f); ++wall_index) {
			Wall* wall = Get_Walls() + wall_index;
			if (wall == NULL)
				continue;

			// randomize the values
			wall->pos.x = CP_Random_RangeFloat(1.f, world_limits.x * 0.5f - 1.f);
			wall->pos.y = CP_Random_RangeFloat(1.f, world_limits.y - 1.f);

			wall->scale.x = (float)CP_Random_RangeInt(1, 5);
			wall->scale.y = (float)CP_Random_RangeInt(1, 5);

			wall->angle = CP_Random_RangeFloat(0.f, 90.f);
			wall->color = CP_Color_Create(CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), 255);

			wall->image = Asset_GetImage(WALL_LEFT_ASSET);
			wall->rotation_rate = 0.f;
			wall->destroyed = false;
		}

		// create the random right side walls
		for (; wall_index < walls.length; ++wall_index) {
			Wall* wall = Get_Walls() + wall_index;
			if (wall == NULL)
				continue;

			// randomize the values
			wall->pos.x = (world_limits.x * 0.5f) + CP_Random_RangeFloat(1.f, world_limits.x * 0.5f - 1.f);
			wall->pos.y = CP_Random_RangeFloat(1.f, world_limits.y - 1.f);

			wall->scale.x = (float)CP_Random_RangeInt(1, 5);
			wall->scale.y = (float)CP_Random_RangeInt(1, 5);

			wall->angle = CP_Random_RangeFloat(0.f, 90.f);
			wall->color = CP_Color_Create(CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), CP_Random_RangeInt(100, 255), 255);

			wall->image = Asset_GetImage(WALL_RIGHT_ASSET);
			wall->rotation_rate = 0.f;
			wall->destroyed = false;
		}

		load_borders(0.f);
	} else {

		// level loading failed, quit the game because heap allocation went wrong!!
		Set_Next_State(STATE_EXIT);
	}

	// set a default layout of item powerups
	items.length = 4;
	items.arr = gc_malloc(sizeof(Item) * items.length);
	if (items.arr) {
		Item* itemarr = Get_Items();
		// create a boost item
		itemarr[0].type = ITEM_BOOST;
		itemarr[0].pos = CP_Vector_Set(world_limits.x * 0.25f, world_limits.y * 0.5f);
		itemarr[0].diameter = 1.f;
		itemarr[0].angle = 0.f;
		itemarr[0].collected = false;
		itemarr[0].destroyed = false;

		// create a grow item
		itemarr[1].type = ITEM_GROW;
		itemarr[1].pos = CP_Vector_Set(world_limits.x * 0.75f, world_limits.y * 0.5f);
		itemarr[1].diameter = 1.f;
		itemarr[1].angle = 0.f;
		itemarr[1].collected = false;
		itemarr[1].destroyed = false;

		// create a shrink item
		itemarr[2].type = ITEM_SHRINK;
		itemarr[2].pos = CP_Vector_Set(world_limits.x * 0.65f, world_limits.y * 0.3f);
		itemarr[2].diameter = 1.f;
		itemarr[2].angle = 0.f;
		itemarr[2].collected = false;
		itemarr[2].destroyed = false;

		// create a reverse item
		itemarr[3].type = ITEM_REVERSE;
		itemarr[3].pos = CP_Vector_Set(world_limits.x * 0.45f, world_limits.y * 0.3f);
		itemarr[3].diameter = 1.f;
		itemarr[3].angle = 0.f;
		itemarr[3].collected = false;
		itemarr[3].destroyed = false;
	}

	// set a default layout of the terrains
	terrains.length = 2;
	terrains.arr = gc_malloc(sizeof(Terrain) * terrains.length);
	if (terrains.arr) {
		Terrain* terrain_arr = Get_Terrains();
		// create a sand terrain
		terrain_arr[0].type = TERRAIN_SAND;
		terrain_arr[0].pos = CP_Vector_Set(world_limits.x * 0.25f, world_limits.y * 0.5f);
		terrain_arr[0].scale = CP_Vector_Set(10.f, 10.f);
		terrain_arr[0].angle = 0.f;
		terrain_arr[0].angle_radians = 0.f;
		terrain_arr[0].destroyed = false;

		// create an ice terrain
		terrain_arr[1].type = TERRAIN_ICE;
		terrain_arr[1].pos = CP_Vector_Set(world_limits.x * 0.75f, world_limits.y * 0.5f);
		terrain_arr[1].scale = CP_Vector_Set(10.f, 10.f);
		terrain_arr[1].angle = 0.f;
		terrain_arr[1].angle_radians = 0.f;
		terrain_arr[1].destroyed = false;
	}
}

Wall* Game_AddWall(CP_Vector pos) {
	// object pooling, find an unused wall in memory first, if not we resize the array
	for (int wall_index = 1; wall_index < walls.length; ++wall_index) {
		Wall* w = Get_Walls() + wall_index;
		if (w == NULL)
			continue;
		
		// if there's an unused wall in the array, we overwrite it
		if (w->destroyed) {
			w->pos = pos;
			w->scale.x = 1.f;
			w->scale.y = 1.f;
			w->color = Get_Color(COLOR_WHITE);
			w->image = w->pos.x <= world_limits.x * 0.5f ? Asset_GetImage(WALL_LEFT_ASSET) : Asset_GetImage(WALL_RIGHT_ASSET);
			w->angle = 0.f;
			w->rotation_rate = 0.f;
			w->destroyed = false;
			return w;
		}
	}

	// we create a new wall in memory by resizing it and appending it to the back
	++walls.length;
	walls.arr = walls.arr ? gc_realloc(walls.arr, sizeof(Wall) * walls.length) : gc_malloc(sizeof(Wall) * walls.length);
	if (walls.arr) {
		Wall* new_wall = Get_Walls() + (walls.length - 1);
		new_wall->pos = pos;
		new_wall->scale.x = 1.f;
		new_wall->scale.y = 1.f;
		new_wall->color = Get_Color(COLOR_WHITE);
		new_wall->image = new_wall->pos.x <= world_limits.x * 0.5f ? Asset_GetImage(WALL_LEFT_ASSET) : Asset_GetImage(WALL_RIGHT_ASSET);
		new_wall->angle = 0.f;
		new_wall->rotation_rate = 0.f;
		new_wall->destroyed = false;
		return new_wall;
	}
	return NULL;
}

void Game_RemoveWall(Wall* wall) {
	// make sure the middle wall is always skipped (element 0)
	for (int wall_index = 1; wall_index < walls.length; ++wall_index) {
		Wall* w = Get_Walls() + wall_index;
		if (w == NULL)
			continue;

		if (w == wall) {
			w->pos.x = -10.f;
			w->pos.y = -10.f;
			w->scale.x = 0.f;
			w->scale.y = 0.f;
			w->rotation_rate = 0.f;
			w->angle = 0.f;
			w->destroyed = true;
			break;
		}
	}
}

Item* Game_AddItem(CP_Vector pos, ItemType type) {
	// object pooling, find an unused item in memory first, if not we resize the array
	for (int item_index = 0; item_index < items.length; ++item_index) {
		Item* i = Get_Items() + item_index;
		if (i == NULL)
			continue;

		// if there's an unused item in the array, we overwrite it
		if (i->destroyed) {
			i->pos = pos;
			i->diameter = 1.f;
			i->angle = 0.f;
			i->destroyed = false;
			i->collected = false;
			i->type = type;
			return i;
		}
	}

	// resize the array to accomodate a new item in memory
	++items.length;
	items.arr = items.arr ? gc_realloc(items.arr, sizeof(Item) * items.length) : gc_malloc(sizeof(Item) * items.length);
	if (items.arr) {
		Item* new_item = Get_Items() + (items.length - 1);
		new_item->pos = pos;
		new_item->diameter = 1.f;
		new_item->angle = 0.f;
		new_item->destroyed = false;
		new_item->collected = false;
		new_item->type = type;
		return new_item;
	}
	return NULL;
}

void Game_RemoveItem(Item* item) {
	for (int item_index = 0; item_index < items.length; ++item_index) {
		Item* i = Get_Items() + item_index;
		if (i == NULL)
			continue;

		if (i == item) {
			i->pos.x = -10.f;
			i->pos.y = -10.f;
			i->diameter = -1.f;
			i->angle = 0.f;
			i->destroyed = true;
			i->collected = false;
			break;
		}
	}
}

GolfBall* Game_AddGolf(CP_Vector pos) {
	// object pooling, find an unused ball in memory first, if not we resize the array
	for (int ball_index = 0; ball_index < golf_balls.length; ++ball_index) {
		GolfBall* b = Get_Golfs() + ball_index;
		if (b == NULL)
			continue;

		// if there's an unused ball in the array, we overwrite it
		if (b->destroyed) {
			b->pos = pos;
			b->prev_pos = pos;
			b->diameter = 1.f;
			b->angle = 0.f;
			b->halt = false;
			b->dragging = false;
			b->scored = false;
			b->scoring = false;
			b->destroyed = false;
			b->teleported = false;
			b->vel = CP_Vector_Set(0.f, 0.f);
			return b;
		}
	}

	// resize the array to accomodate a new golf ball in memory
	++golf_balls.length;
	golf_balls.arr = golf_balls.arr ? gc_realloc(golf_balls.arr, sizeof(GolfBall) * golf_balls.length) : gc_malloc(sizeof(GolfBall) * golf_balls.length);
	if (golf_balls.arr) {
		GolfBall* new_ball = Get_Golfs() + (golf_balls.length - 1);
		new_ball->pos = pos;
		new_ball->prev_pos = pos;
		new_ball->diameter = 1.f;
		new_ball->vel = CP_Vector_Set(0.f, 0.f);
		new_ball->angle = 0.f;
		new_ball->destroyed = false;
		new_ball->halt = false;
		new_ball->dragging = false;
		new_ball->scored = false;
		new_ball->scoring = false;
		new_ball->teleported = false;
		return new_ball;
	}
	return NULL;
}

Hole* Game_AddHole(CP_Vector pos) {
	// object pooling, find an unused hole in memory first, if not we resize the array
	for (int hole_index = 0; hole_index < holes.length; ++hole_index) {
		Hole* h = Get_Holes() + hole_index;
		if (h == NULL)
			continue;

		// if there's an unused ball in the array, we overwrite it
		if (h->destroyed) {
			h->pos = pos;
			h->diameter = 1.5f;
			h->destroyed = false;
			return h;
		}
	}

	// resize the array to accomodate a new hole in memory
	++holes.length;
	holes.arr = holes.arr ? gc_realloc(holes.arr, sizeof(Hole) * holes.length) : gc_malloc(sizeof(Hole) * holes.length);
	if (holes.arr) {
		Hole* new_hole = Get_Holes() + (holes.length - 1);
		new_hole->pos = pos;
		new_hole->diameter = 1.5f;
		new_hole->destroyed = false;
		return new_hole;
	}
	return NULL;
}

void Game_RemoveGolf(GolfBall* ball) {
	for (int ball_index = 0; ball_index < golf_balls.length; ++ball_index) {
		GolfBall* b = Get_Golfs() + ball_index;
		if (b == NULL)
			continue;

		if (b == ball) {
			b->pos.x = -10.f;
			b->pos.y = -10.f;
			b->diameter = -1.f;
			b->destroyed = true;
			break;
		}
	}
}

void Game_RemoveHole(Hole* hole) {
	for (int hole_index = 0; hole_index < holes.length; ++hole_index) {
		Hole* h = Get_Holes() + hole_index;
		if (h == NULL)
			continue;

		if (h == hole) {
			h->pos.x = -10.f;
			h->pos.y = -10.f;
			h->diameter = -1.f;
			h->destroyed = true;
			break;
		}
	}
}

void Game_RemoveTerrain(Terrain* terrain) {
	for (int terrain_index = 0; terrain_index < terrains.length; ++terrain_index) {
		Terrain* t = Get_Terrains() + terrain_index;
		if (t == NULL)
			continue;

		if (t == terrain) {
			t->pos.x = -10.f;
			t->pos.y = -10.f;
			t->scale.x = 0.f;
			t->scale.y = 0.f;
			t->destroyed = true;
		}
	}
}

Terrain* Game_AddTerrain(CP_Vector pos, TerrainType type) {
	// object pooling, find an unused terrain in memory first, if not we resize the array
	for (int terrain_index = 0; terrain_index < terrains.length; ++terrain_index) {
		Terrain* t = Get_Terrains() + terrain_index;
		if (t == NULL)
			continue;

		// if there's an unused terrain in the array, we overwrite it
		if (t->destroyed) {
			t->pos = pos;
			t->scale.x = 1.f;
			t->scale.y = 1.f;
			t->angle = 0.f;
			t->angle_radians = 0.f;
			t->type = type;
			t->destroyed = false;
			return t;
		}
	}

	// resize the array to accomodate a new terrain in memory
	++terrains.length;
	terrains.arr = terrains.arr ? gc_realloc(terrains.arr, sizeof(Terrain) * terrains.length) : gc_malloc(sizeof(Terrain) * terrains.length);
	if (terrains.arr) {
		Terrain* t = Get_Terrains() + (terrains.length - 1);
		t->pos = pos;
		t->scale.x = 1.f;
		t->scale.y = 1.f;
		t->angle = 0.f;
		t->angle_radians = 0.f;
		t->type = type;
		t->destroyed = false;
		return t;
	}
	return NULL;
}

void Game_SetLevel(char level) {
	current_level = level;
}

char Game_GetLevel(void) {
	return current_level;
}

CP_Vector Get_Mouse_WorldSpace(void) {
	return mouse_world;
}

CP_Vector Get_World_Limits(void) {
	return world_limits;
}

void Scale_World(float scale) {
	// get the previous world sizes
	float prev_x = world_limits.x;
	float prev_y = world_limits.y;

	// ensure the scaling is based on the original world size
	world_limits = CP_Vector_Scale(og_world_limits, scale);

	// since the scoreboard uses the world size as reference, we update it also
	sBoard = CP_Vector_Set(world_limits.x * 0.5f, world_limits.y * 0.5f);

	// go through each object in the level and update the position based on the updated world size
	Wall* walls_arr = Get_Walls();
	GolfBall* golfs_arr = Get_Golfs();
	Hole* holes_arr = Get_Holes();
	Item* items_arr = Get_Items();
	Terrain* terrains_arr = Get_Terrains();

	load_borders(walls_arr->angle);

	for (int i = 5; i < walls.length; ++i) {
		Wall* w = walls_arr + i;
		if (w == NULL)
			continue;

		w->pos.x *= (world_limits.x / prev_x);
		w->pos.y *= (world_limits.y / prev_y);
	}

	for (int i = 0; i < golf_balls.length; ++i) {
		GolfBall* g = golfs_arr + i;
		if (g == NULL)
			continue;

		g->pos.x *= (world_limits.x / prev_x);
		g->pos.y *= (world_limits.y / prev_y);
	}

	for (int i = 0; i < items.length; ++i) {
		Item* it = items_arr + i;
		if (it == NULL)
			continue;

		it->pos.x *= (world_limits.x / prev_x);
		it->pos.y *= (world_limits.y / prev_y);
	}

	for (int i = 0; i < holes.length; ++i) {
		Hole* h = holes_arr + i;
		if (h == NULL)
			continue;

		h->pos.x *= (world_limits.x / prev_x);
		h->pos.y *= (world_limits.y / prev_y);
	}

	for (int i = 0; i < terrains.length; ++i) {
		Terrain* t = terrains_arr + i;
		if (t == NULL)
			continue;
		
		t->pos.x *= (world_limits.x / prev_x);
		t->pos.y *= (world_limits.y / prev_y);
	}
}

CP_Vector Get_World_Ratios(void) {
	return CP_Vector_Set(CP_System_GetWindowWidth() / world_limits.x, CP_System_GetWindowHeight() / world_limits.y);
}

CP_Matrix Get_Camera(void) {
	return camera_matrix;
}
