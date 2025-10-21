/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 23 September 2025
@course CSD1401
@brief Definition file for debug and level editor UI
*/

#include "editor_manager.h"
#include "utils.h"
#include "asset_manager.h"

typedef enum EditingType {
	NONE = 0,
	WALL,
	HOLE,
	GOLF_BALL,
	ITEM
} EditingType;

void* current_editing = NULL;
void* current_dragging = NULL;
CP_Vector current_editing_offset;
EditingType current_type = NONE;

CP_BOOL check_on_quad(Wall* wall) {
	CP_Vector wall_min = { -wall->scale.x * 0.5f, -wall->scale.y * 0.5f };
	CP_Vector wall_max = { wall->scale.x * 0.5f, wall->scale.y * 0.5f };

	// mouse location in respect to wall
	CP_Vector mouse_world = get_mouse_world();
	CP_Vector r = CP_Vector_Subtract(mouse_world, wall->pos);

	// rotate local position back to axis
	CP_Vector local_mouse_pos = CP_Vector_Rotate(r, wall->angle_radians);

	return (local_mouse_pos.x >= wall_min.x && local_mouse_pos.x <= wall_max.x &&
		local_mouse_pos.y >= wall_min.y && local_mouse_pos.y <= wall_max.y);
}

CP_BOOL check_on_circle(CP_Vector pos, float diameter) {
	CP_Vector mouse_world = get_mouse_world();
	return CP_Vector_Distance(pos, mouse_world) <= diameter;
}

void update_editor_walls(Wall* walls, int size) {
	CP_Vector mouse_world = get_mouse_world();

	if (CP_Input_KeyTriggered(KEY_P)) {
		Wall* wall = add_wall(mouse_world);
		if (wall) {
			size++;
			current_editing = wall;
			current_editing_offset = CP_Vector_Subtract(wall->pos, mouse_world);
			current_type = WALL;
		}
	}

	if (CP_Input_KeyTriggered(KEY_DELETE) && current_editing != NULL && current_type == WALL) {
		remove_wall((Wall*)current_editing);
		current_editing = NULL;
		current_dragging = NULL;
		current_type = NONE;
		size--;
	}

	for (int i = 0; i < size; i++) {
		Wall* wall = walls + i;
		if (wall == NULL)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && check_on_quad(wall)) {
			current_editing = wall;
			current_editing_offset = CP_Vector_Subtract(wall->pos, mouse_world);
			current_type = WALL;
		}

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_2) && current_editing == wall) {
			current_editing_offset = CP_Vector_Subtract(wall->pos, mouse_world);
		}
	}
}

void update_editor_holes(Hole* holes, int size) {
	CP_Vector mouse_world = get_mouse_world();
	for (int i = 0; i < size; i++) {
		Hole* hole = holes + i;
		if (hole == NULL )
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && check_on_circle(hole->pos, hole->diameter)) {
			current_editing = hole;
			current_editing_offset = CP_Vector_Subtract(hole->pos, mouse_world);
			current_type = HOLE;
		}
	}
}

void update_editor_golfs(GolfBall* balls, int size) {
	CP_Vector mouse_world = get_mouse_world();
	for (int i = 0; i < size; i++) {
		GolfBall* golf = balls + i;
		if (golf == NULL)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && check_on_circle(golf->pos, golf->diameter)) {
			current_editing = golf;
			current_editing_offset = CP_Vector_Subtract(golf->pos, mouse_world);
			current_type = GOLF_BALL;
		}
	}
}

void update_editor_items(Item* items, int size) {
	CP_Vector mouse_world = get_mouse_world();

	if (CP_Input_KeyTriggered(KEY_1)) {
		Item* item = add_item(mouse_world, BOOST);
		if (item) {
			size++;
			current_editing = item;
			current_editing_offset = CP_Vector_Subtract(item->pos, mouse_world);
			current_type = ITEM;
		}
	} else if (CP_Input_KeyTriggered(KEY_2)) {
		Item* item = add_item(mouse_world, BIG);
		if (item) {
			size++;
			current_editing = item;
			current_editing_offset = CP_Vector_Subtract(item->pos, mouse_world);
			current_type = ITEM;
		}
	} else if (CP_Input_KeyTriggered(KEY_3)) {
		Item* item = add_item(mouse_world, SMALL);
		if (item) {
			size++;
			current_editing = item;
			current_editing_offset = CP_Vector_Subtract(item->pos, mouse_world);
			current_type = ITEM;
		}
	} else if (CP_Input_KeyTriggered(KEY_4)) {
		Item* item = add_item(mouse_world, REVERSE);
		if (item) {
			size++;
			current_editing = item;
			current_editing_offset = CP_Vector_Subtract(item->pos, mouse_world);
			current_type = ITEM;
		}
	}

	if (CP_Input_KeyTriggered(KEY_DELETE) && current_editing != NULL && current_type == ITEM) {
		remove_item((Item*)current_editing);
		current_editing = NULL;
		current_dragging = NULL;
		current_type = NONE;
		size--;
	}

	for (int i = 0; i < size; i++) {
		Item* item = items + i;
		if (item == NULL)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && check_on_circle(item->pos, item->diameter)) {
			current_editing = item;
			current_editing_offset = CP_Vector_Subtract(item->pos, mouse_world);
			current_type = ITEM;
		}
	}
}

void update_editor_utils(void) {
	if (current_editing == NULL)
		return;

	if (CP_Input_MouseDragged(MOUSE_BUTTON_1)) {
		if (current_type == WALL && check_on_quad((Wall*)current_editing)) {
			current_dragging = current_editing;
		}
		else if (current_type == HOLE && check_on_circle(((Hole*)current_editing)->pos, ((Hole*)current_editing)->diameter)) {
			current_dragging = current_editing;
		}
		else if (current_type == GOLF_BALL && check_on_circle(((GolfBall*)current_editing)->pos, ((GolfBall*)current_editing)->diameter)) {
			current_dragging = current_editing;
		}
		else if (current_type == ITEM && check_on_circle(((Item*)current_editing)->pos, ((Item*)current_editing)->diameter)) {
			current_dragging = current_editing;
		}
	}
	else if (CP_Input_MouseDragged(MOUSE_BUTTON_2)) {
		current_dragging = current_editing;
	}
	else {
		current_dragging = NULL;
	}

	CP_Vector mouse_world = get_mouse_world();
	if (current_dragging != NULL) {
		if (CP_Input_MouseDragged(MOUSE_BUTTON_1)) { // translate
			if (current_type == WALL) {
				Wall* wall = (Wall*)current_dragging;
				wall->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				wall->image = wall->pos.x <= get_world_limits().x * 0.5f ? get_image(WALL_LEFT_ASSET) : get_image(WALL_RIGHT_ASSET);
			}
			else if (current_type == HOLE) {
				Hole* hole = (Hole*)current_dragging;
				hole->pos = CP_Vector_Add(mouse_world, current_editing_offset);

			}
			else if (current_type == GOLF_BALL) {
				GolfBall* ball = (GolfBall*)current_dragging;
				ball->pos = CP_Vector_Add(mouse_world, current_editing_offset);
			}
			else if (current_type == ITEM) {
				Item* item = (Item*)current_dragging;
				item->pos = CP_Vector_Add(mouse_world, current_editing_offset);
			}
		}
		else if (CP_Input_MouseDragged(MOUSE_BUTTON_2)) { // rotate
			if (current_type == WALL) {
				Wall* wall = (Wall*)current_dragging;
				wall->angle = CP_Vector_AngleCW(CP_Vector_Subtract(wall->pos, mouse_world), current_editing_offset);
			}
		}
	}
	
	if (current_type == WALL) {
		Wall* wall = (Wall*)current_editing;
		if (CP_Input_KeyTriggered(KEY_UP)) {
			wall->rotation_rate += 1.f;
		}
		else if (CP_Input_KeyTriggered(KEY_DOWN)) {
			wall->rotation_rate -= 1.f;
		}

		if (CP_Input_MouseWheel() != 0.f) {
			if (CP_Input_MouseDown(MOUSE_BUTTON_1)) {
				wall->scale.x += CP_Input_MouseWheel() * 0.1f;
				wall->scale.x = max(0.1f, wall->scale.x);
			}

			if (CP_Input_MouseDown(MOUSE_BUTTON_2)) {
				wall->scale.y += CP_Input_MouseWheel() * 0.1f;
				wall->scale.y = max(0.1f, wall->scale.y);
			}
		}
	}
}

void render_editor_utils(void) {
	CP_Settings_Fill(get_color(GREEN));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_TextSize(50.f);
	CP_Font_DrawText("Game Paused! Press G to toggle editor mode!", CP_System_GetWindowWidth() * 0.5f, 50.f);

	if (current_editing == NULL)
		return;

	CP_Vector world_limits = get_world_limits();
	CP_Vector world_ratios = get_world_ratios();

	if (current_type == WALL) {
		Wall* wall = (Wall*)current_editing;
		CP_Vector screen_pos = world_to_screen(wall->pos.x, wall->pos.y, world_limits.x, world_limits.y);
		CP_Vector normal = world_to_screen(wall->pos.x + wall->normal.x, wall->pos.y + wall->normal.y, world_limits.x, world_limits.y);
		CP_Vector right = world_to_screen(wall->pos.x + wall->right.x, wall->pos.y + wall->right.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(get_color(RED));
		CP_Settings_Stroke(get_color(RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, normal.x, normal.y);

		CP_Settings_Fill(get_color(BLUE));
		CP_Settings_Stroke(get_color(BLUE));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, right.x, right.y);
	}
	else if (current_type == HOLE) {
		Hole* hole = (Hole*)current_editing;
		CP_Vector screen_pos = world_to_screen(hole->pos.x, hole->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(get_color(RED));
		CP_Settings_Stroke(get_color(RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * hole->diameter * 0.5f);
	}
	else if (current_type == GOLF_BALL) {
		GolfBall* ball = (GolfBall*)current_editing;
		CP_Vector screen_pos = world_to_screen(ball->pos.x, ball->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(get_color(RED));
		CP_Settings_Stroke(get_color(RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * ball->diameter * 0.5f);
	}
	else if (current_type == ITEM) {
		Item* item = (Item*)current_editing;
		CP_Vector screen_pos = world_to_screen(item->pos.x, item->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(get_color(RED));
		CP_Settings_Stroke(get_color(RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * item->diameter * 0.5f);
	}
}