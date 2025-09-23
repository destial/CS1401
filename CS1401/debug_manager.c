/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 23 September 2025
@course CSD1401
@brief Definition file for debug and level editor UI
*/

#include "debug_manager.h"
#include "physics.h"

typedef enum EditMode {
	TRANSLATE = 0,
	ROTATE,
	SCALE
} EditMode;

typedef enum EditingType {
	WALL = 1,
	HOLE,
	GOLF_BALL
} EditingType;

void* current_editing = NULL;
EditMode current_mode = TRANSLATE;
EditingType current_type = 0;

CP_BOOL check_on_quad(Wall* wall) {
	CP_Vector wall_min = { -wall->scale.x * 0.5f, -wall->scale.y * 0.5f };
	CP_Vector wall_max = { wall->scale.x * 0.5f, wall->scale.y * 0.5f };

	// mouse location in respect to wall
	CP_Vector mouse = get_mouse_world();
	CP_Vector r = CP_Vector_Subtract(mouse, wall->pos);

	// rotate local position back to axis
	CP_Vector local_mouse_pos = CP_Vector_Rotate(r, wall->angle_radians);

	return (local_mouse_pos.x >= wall_min.x && local_mouse_pos.x <= wall_max.x &&
		local_mouse_pos.y >= wall_min.y && local_mouse_pos.y <= wall_max.y);
}

CP_BOOL check_on_circle(CP_Vector pos, float diameter) {
	CP_Vector mouse = get_mouse_world();
	return CP_Vector_Distance(pos, mouse) <= diameter;
}

void update_debug_walls(Wall* walls, int size) {
	for (int i = 0; i < size; i++) {
		Wall* wall = walls + i;
		if (wall == NULL || current_editing == wall)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && check_on_quad(wall)) {
			current_editing = wall;
			current_type = WALL;
		}
	}
}

void update_debug_holes(Hole* holes, int size) {
	for (int i = 0; i < size; i++) {
		Hole* hole = holes + i;
		if (hole == NULL || current_editing == hole)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && check_on_circle(hole->pos, hole->diameter)) {
			current_editing = hole;
			current_type = HOLE;
		}
	}
}

void update_debug_golfs(GolfBall* balls, int size) {
	for (int i = 0; i < size; i++) {
		GolfBall* golf = balls + i;
		if (golf == NULL || current_editing == golf)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && check_on_circle(golf->pos, golf->diameter)) {
			current_editing = golf;
			current_type = GOLF_BALL;
		}

	}
}

void update_debug_utils(void) {
	if (current_editing == NULL)
		return;

	CP_Vector world_limits = get_world_limits();
	CP_Vector world_ratios = get_world_ratios();

	if (current_type == WALL) {
		Wall* wall = (Wall*)current_editing;
		if (CP_Input_MouseDown(MOUSE_BUTTON_1) && check_on_quad(wall)) {
			if (current_mode == TRANSLATE) {
				wall->pos = get_mouse_world();
			}
		}
	}
	else if (current_type == HOLE) {
		Hole* hole = (Hole*)current_editing;
		if (CP_Input_MouseDown(MOUSE_BUTTON_1) && check_on_circle(hole->pos, hole->diameter)) {
			if (current_mode == TRANSLATE) {
				hole->pos = get_mouse_world();
			}
		}
	}
	else if (current_type == GOLF_BALL) {
		GolfBall* ball = (GolfBall*)current_editing;
		if (CP_Input_MouseDown(MOUSE_BUTTON_1) && check_on_circle(ball->pos, ball->diameter)) {
			if (current_mode == TRANSLATE) {
				ball->pos = get_mouse_world();
			}
		}
	}
}

void render_debug_utils(void) {
	CP_Settings_Fill(get_color(GREEN));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_TextSize(50.f);
	CP_Font_DrawText("Game Paused! Press G to toggle editor mode!", CP_System_GetWindowWidth() * 0.5f, 0);

	if (current_editing == NULL)
		return;

	CP_Vector world_limits = get_world_limits();
	CP_Vector world_ratios = get_world_ratios();

	if (current_type == WALL) {
		Wall* wall = (Wall*)current_editing;
		CP_Vector screen_pos = world_to_screen(wall->pos.x, wall->pos.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(get_color(RED));
		CP_Settings_Stroke(get_color(RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x + (wall->normal.x * world_ratios.x * wall->scale.x * 0.5f), screen_pos.y + (wall->normal.y * world_ratios.y * wall->scale.y * 0.5f));

		CP_Settings_Fill(get_color(BLUE));
		CP_Settings_Stroke(get_color(BLUE));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x + (wall->left.x * world_ratios.x * wall->scale.x * 0.5f), screen_pos.y + (wall->left.y * world_ratios.y * wall->scale.y * 0.5f));
	}
	else if (current_type == HOLE) {
		Hole* hole = (Hole*)current_editing;
		CP_Vector screen_pos = world_to_screen(hole->pos.x, hole->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(get_color(RED));
		CP_Settings_Stroke(get_color(RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * hole->diameter * 0.5);
	}
	else if (current_type == GOLF_BALL) {
		GolfBall* ball = (GolfBall*)current_editing;
		CP_Vector screen_pos = world_to_screen(ball->pos.x, ball->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(get_color(RED));
		CP_Settings_Stroke(get_color(RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * ball->diameter * 0.5);
	}
}