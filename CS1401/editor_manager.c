/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 23 September 2025
@course CSD1401
@brief Definition file for debug and level editor UI
*/

#include <math.h>
#include <stdio.h>
#include "editor_manager.h"
#include "utils.h"
#include "asset_manager.h"

typedef enum EditingType {
	EDIT_NONE = 0,
	EDIT_WALL,
	EDIT_MIDDLE_WALL,
	EDIT_HOLE,
	EDIT_BALL,
	EDIT_ITEM,
	EDIT_TERRAIN
} EditingType;

void* current_editing = NULL;
void* current_dragging = NULL;
void* current_click = NULL;
CP_Vector current_editing_offset;
EditingType current_type = EDIT_NONE;
float editor_saving = 0.f;
bool editor_dirty = false;

bool Check_On_Wall(Wall* wall) {
	// get bounds of the wall
	CP_Vector wall_min = { -wall->scale.x * 0.5f, -wall->scale.y * 0.5f };
	CP_Vector wall_max = { wall->scale.x * 0.5f, wall->scale.y * 0.5f };

	// mouse location in respect to wall
	CP_Vector mouse_world = Get_Mouse_WorldSpace();
	CP_Vector r = CP_Vector_Subtract(mouse_world, wall->pos);

	// rotate local position back to axis
	CP_Vector local_mouse_pos = CP_Vector_Rotate(r, CP_Math_Radians(wall->angle));

	return (local_mouse_pos.x >= wall_min.x && local_mouse_pos.x <= wall_max.x &&
		local_mouse_pos.y >= wall_min.y && local_mouse_pos.y <= wall_max.y);
}

bool Check_On_Terrain(Terrain* terrain) {
	// use game function to check terrain
	return Check_Terrain(terrain, Get_Mouse_WorldSpace());
}

bool Check_On_Circle(CP_Vector pos, float diameter) {
	return CP_Vector_Distance(pos, Get_Mouse_WorldSpace()) <= diameter * 0.5f;
}

void Editor_Update_Walls(ObjectArray* walls) {
	CP_Vector mouse_world = Get_Mouse_WorldSpace();

	// add a new wall when 'W' is pressed at the current mouse coordinates
	if (CP_Input_KeyTriggered(KEY_W)) {
		Wall* wall = Game_AddWall(mouse_world);
		if (wall) { // set the current editing object to be the newly created wall
			current_editing = wall;
			current_editing_offset = CP_Vector_Subtract(wall->pos, mouse_world);
			current_type = EDIT_WALL;
			editor_dirty = true;
		}
	}

	Wall* middle_wall = (Wall*)walls->arr;
	// if clicked on this wall, set as the current editing object
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && Check_On_Wall(middle_wall)) {
		current_editing = middle_wall;
		current_click = middle_wall;
		current_editing_offset = CP_Vector_Subtract(middle_wall->pos, mouse_world);
		current_type = EDIT_MIDDLE_WALL;
	}

	for (int i = 5; i < walls->length; i++) {
		Wall* wall = (Wall*)walls->arr + i;
		if (wall == NULL || wall->destroyed)
			continue;

		// if clicked on this wall, set as the current editing object
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && Check_On_Wall(wall)) {
			current_editing = wall;
			current_click = wall;
			current_editing_offset = CP_Vector_Subtract(wall->pos, mouse_world);
			current_type = EDIT_WALL;
		}

		// offset is used for translation and rotation
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_RIGHT) && current_editing == wall) {
			current_editing_offset = CP_Vector_Subtract(wall->pos, mouse_world);
		}
	}

	// remove this wall from the level if 'DEL' key was pressed
	if (CP_Input_KeyTriggered(KEY_DELETE) && current_editing != NULL && current_type == EDIT_WALL) {
		Game_RemoveWall((Wall*)current_editing);
		current_editing = NULL;
		current_dragging = NULL;
		current_type = EDIT_NONE;
		editor_dirty = true;
	}
}

void Editor_Update_Terrains(ObjectArray* terrains) {
	CP_Vector mouse_world = Get_Mouse_WorldSpace();

	// add an ice terrain when '8' is pressed at the current mouse coordinates
	if (CP_Input_KeyTriggered(KEY_8)) {
		Terrain* terrain = Game_AddTerrain(mouse_world, TERRAIN_ICE);
		if (terrain) { // set the current editing object to be the newly added terrain
			current_editing = terrain;
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
			current_type = EDIT_TERRAIN;
			editor_dirty = true;
		}
	}

	// add an ice terrain when '9' is pressed at the current mouse coordinates
	if (CP_Input_KeyTriggered(KEY_9)) {
		Terrain* terrain = Game_AddTerrain(mouse_world, TERRAIN_SAND);
		if (terrain) { // set the current editing object to be the newly added terrain
			current_editing = terrain;
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
			current_type = EDIT_TERRAIN;
			editor_dirty = true;
		}
	}

	// add an ice terrain when '0' is pressed at the current mouse coordinates
	if (CP_Input_KeyTriggered(KEY_0)) {
		Terrain* terrain = Game_AddTerrain(mouse_world, TERRAIN_PIT);
		if (terrain) { // set the current editing object to be the newly added terrain
			current_editing = terrain;
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
			current_type = EDIT_TERRAIN;
			editor_dirty = true;
		}
	}

	for (int i = 0; i < terrains->length; i++) {
		Terrain* terrain = (Terrain*)terrains->arr + i;
		if (terrain == NULL || terrain->destroyed)
			continue;

		// if clicked on this terrain, set as the current editing object
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && Check_On_Terrain(terrain)) {
			current_editing = terrain;
			current_click = terrain;
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
			current_type = EDIT_TERRAIN;
		}

		// offset is used for translation and rotation
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_RIGHT) && current_editing == terrain) {
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
		}
	}

	if (current_editing != NULL && current_type == EDIT_TERRAIN) {
		Terrain* t = (Terrain*)current_editing;
		// change the type of the terrain by using the left/right arrow keys
		if (CP_Input_KeyTriggered(KEY_RIGHT)) {
			if (++t->type == TERRAIN_LAST) {
				t->type = 0;
			}
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_LEFT)) {
			if (--t->type == -1) {
				t->type = TERRAIN_LAST - 1;
			}
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_DELETE)) { // remove this terrain from the level if 'DEL' key was pressed
			Game_RemoveTerrain(t);
			current_editing = NULL;
			current_dragging = NULL;
			current_type = EDIT_NONE;
			editor_dirty = true;
		}
	}
}

void Editor_Update_Holes(ObjectArray* holes) {
	CP_Vector mouse_world = Get_Mouse_WorldSpace();

	// add a hole when 'H' key is pressed at the current mouse coordinates
	if (CP_Input_KeyTriggered(KEY_H)) {
		Hole* hole = Game_AddHole(mouse_world);
		if (hole) { // set the current edit object to be the newly added hole
			current_editing = hole;
			current_editing_offset = CP_Vector_Subtract(hole->pos, mouse_world);
			current_type = EDIT_HOLE;
			editor_dirty = true;
		}
	}

	for (int i = 0; i < holes->length; i++) {
		Hole* hole = (Hole*) holes->arr + i;
		if (hole == NULL || hole->destroyed)
			continue;

		// set the current edit object to be whatever the mouse is on
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && Check_On_Circle(hole->pos, hole->diameter)) {
			current_editing = hole;
			current_click = hole;
			current_editing_offset = CP_Vector_Subtract(hole->pos, mouse_world);
			current_type = EDIT_HOLE;
		}
	}

	if (current_editing != NULL && current_type == EDIT_HOLE) {
		Hole* hole = (Hole*)current_editing;
		if (CP_Input_MouseWheel() != 0.f) { // increase the size of the hole by the mouse wheel
			if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
				hole->diameter += CP_Input_MouseWheel() * 0.5f;
				editor_dirty = true;
			}
		}

		// remove this hole from the level if 'DEL' key was pressed
		if (CP_Input_KeyTriggered(KEY_DELETE)) { 
			Game_RemoveHole(hole);
			current_editing = NULL;
			current_dragging = NULL;
			current_type = EDIT_NONE;
			editor_dirty = true;
		}
	}
}

void Editor_Update_Golfballs(ObjectArray* golfs) {
	CP_Vector mouse_world = Get_Mouse_WorldSpace();

	if (CP_Input_KeyTriggered(KEY_B)) {
		GolfBall* ball = Game_AddGolf(mouse_world);
		if (ball) {
			current_editing = ball;
			current_editing_offset = CP_Vector_Subtract(ball->pos, mouse_world);
			current_type = EDIT_BALL;
			editor_dirty = true;
		}
	}

	for (int i = 0; i < golfs->length; i++) {
		GolfBall* golf = (GolfBall*) golfs->arr + i;
		if (golf == NULL || golf->destroyed)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && Check_On_Circle(golf->pos, golf->diameter)) {
			current_editing = golf;
			current_click = golf;
			current_editing_offset = CP_Vector_Subtract(golf->pos, mouse_world);
			current_type = EDIT_BALL;
		}
	}

	if (current_editing != NULL && current_type == EDIT_BALL) {
		GolfBall* ball = (GolfBall*)current_editing;
		if (CP_Input_MouseWheel() != 0.f) {
			if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
				ball->diameter += CP_Input_MouseWheel() * 0.5f;
				editor_dirty = true;
			}
		}

		if (CP_Input_KeyTriggered(KEY_DELETE)) {
			Game_RemoveGolf(ball);
			current_editing = NULL;
			current_dragging = NULL;
			current_type = EDIT_NONE;
			editor_dirty = true;
		}
	}
}

void Editor_Update_Items(ObjectArray* items) {
	CP_Vector mouse_world = Get_Mouse_WorldSpace();

	for (int i = KEY_1; i < KEY_1 + ITEM_LAST - 1; ++i) {
		if (CP_Input_KeyTriggered(i)) {
			Item* item = Game_AddItem(mouse_world, i - KEY_1 + 1);
			if (item) {
				current_editing = item;
				current_editing_offset = CP_Vector_Subtract(item->pos, mouse_world);
				current_type = EDIT_ITEM;
			}
		}
	}

	for (int i = 0; i < items->length; i++) {
		Item* item = (Item*)items->arr + i;
		if (item == NULL || item->destroyed)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && Check_On_Circle(item->pos, item->diameter)) {
			current_editing = item;
			current_click = item;
			current_editing_offset = CP_Vector_Subtract(item->pos, mouse_world);
			current_type = EDIT_ITEM;
		}
	}

	if (current_editing != NULL && current_type == EDIT_ITEM) {
		Item* i = (Item*)current_editing;

		if (CP_Input_MouseWheel() != 0.f) {
			if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
				i->diameter += CP_Input_MouseWheel() * 0.5f;
				editor_dirty = true;
			}
		}

		if (CP_Input_KeyTriggered(KEY_RIGHT)) {
			if (++i->type == ITEM_LAST) {
				i->type = ITEM_TYPELESS + 1;
			}
			i->angle = 0.f;
			i->diameter = i->type == ITEM_PORTAL ? 2.f : 1.f;
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_LEFT)) {
			if (--i->type == ITEM_TYPELESS) {
				i->type = ITEM_LAST - 1;
			}
			i->angle = 0.f;
			i->diameter = i->type == ITEM_PORTAL ? 2.f : 1.f;
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_DELETE)) {
			Game_RemoveItem(i);
			current_editing = NULL;
			current_dragging = NULL;
			current_type = EDIT_NONE;
			editor_dirty = true;
		}
	}
}

float world_rescaling = 1.f;

void Editor_Update_Start(void) {
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
		current_click = NULL;
	}

	if (CP_Input_KeyDown(KEY_LEFT_CONTROL) || CP_Input_KeyDown(KEY_RIGHT_CONTROL)) {
		if (CP_Input_KeyTriggered(KEY_EQUAL)) {
			world_rescaling -= 0.1f;
			world_rescaling = max(0.1f, world_rescaling);
			Scale_World(world_rescaling);
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_MINUS)) {
			world_rescaling += 0.1f;
			Scale_World(world_rescaling);
			editor_dirty = true;
		}
	}
}

void Editor_Update_End(void) {
	// save level if debug enabled
	if (CP_Input_KeyTriggered(KEY_S)) {
		Game_Save();
		editor_dirty = false;
		editor_saving = 1.5f;
	}

	// update the saving text lifespan
	if (editor_saving) {
		editor_saving -= CP_System_GetDt();
		editor_saving = max(0.f, editor_saving);
	}

	// if mouse clicked but not on object, deselect
	if (current_click == NULL) {
		current_editing = NULL;
	}
	
	// if we are not selected on anything, we can return here
	if (current_editing == NULL)
		return;

	if (CP_Input_MouseDragged(MOUSE_BUTTON_LEFT)) {
		if ((current_type == EDIT_WALL || current_type == EDIT_MIDDLE_WALL) && Check_On_Wall((Wall*)current_editing)) {
			current_dragging = current_editing;
		}
		else if (current_type == EDIT_HOLE && Check_On_Circle(((Hole*)current_editing)->pos, ((Hole*)current_editing)->diameter)) {
			current_dragging = current_editing;
		}
		else if (current_type == EDIT_BALL && Check_On_Circle(((GolfBall*)current_editing)->pos, ((GolfBall*)current_editing)->diameter)) {
			current_dragging = current_editing;
		}
		else if (current_type == EDIT_ITEM && Check_On_Circle(((Item*)current_editing)->pos, ((Item*)current_editing)->diameter)) {
			current_dragging = current_editing;
		} 
		else if (current_type == EDIT_TERRAIN && Check_On_Terrain((Terrain*)current_editing)) {
			current_dragging = current_editing;
		}
	}
	else if (CP_Input_MouseDragged(MOUSE_BUTTON_RIGHT)) {
		current_dragging = current_editing;
	}
	else {
		current_dragging = NULL;
	}

	CP_Vector mouse_world = Get_Mouse_WorldSpace();
	if (current_dragging != NULL) {
		if (CP_Input_MouseDragged(MOUSE_BUTTON_LEFT)) { // translate
			if (current_type == EDIT_WALL) {
				Wall* wall = (Wall*)current_dragging;
				wall->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				wall->image = wall->pos.x <= Get_World_Limits().x * 0.5f ? Asset_GetImage(WALL_LEFT_ASSET) : Asset_GetImage(WALL_RIGHT_ASSET);
				editor_dirty = true;
			}
			else if (current_type == EDIT_HOLE) {
				Hole* hole = (Hole*)current_dragging;
				hole->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				editor_dirty = true;

			}
			else if (current_type == EDIT_BALL) {
				GolfBall* ball = (GolfBall*)current_dragging;
				ball->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				editor_dirty = true;
			}
			else if (current_type == EDIT_ITEM) {
				Item* item = (Item*)current_dragging;
				item->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				editor_dirty = true;
			}
			else if (current_type == EDIT_TERRAIN) {
				Terrain* terrain = (Terrain*)current_dragging;
				terrain->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				editor_dirty = true;
			}
		}
		else if (CP_Input_MouseDragged(MOUSE_BUTTON_RIGHT)) { // rotate
			if (current_type == EDIT_WALL || current_type == EDIT_MIDDLE_WALL) {
				Wall* wall = (Wall*)current_dragging;
				wall->angle = CP_Vector_AngleCW(CP_Vector_Subtract(wall->pos, mouse_world), current_editing_offset);
				for (float a = 0.f; a <= 360.f; a += 45.f) {
					// angle snapping
					if (fabs((double)(a - wall->angle)) <= 3.f) {
						wall->angle = a;
					}
				}
				editor_dirty = true;
			} else if (current_type == EDIT_TERRAIN) {
				Terrain* terrain = (Terrain*)current_dragging;
				terrain->angle = CP_Vector_AngleCW(CP_Vector_Subtract(terrain->pos, mouse_world), current_editing_offset);
				for (float a = 0.f; a <= 360.f; a += 45.f) {
					// angle snapping
					if (fabs((double)(a - terrain->angle)) <= 3.f) {
						terrain->angle = a;
					}
				}
				editor_dirty = true;
			}
		}
	}
	
	if (current_type == EDIT_WALL) {
		Wall* wall = (Wall*)current_editing;
		if (CP_Input_KeyTriggered(KEY_UP)) {
			wall->rotation_rate += 1.f;
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_DOWN)) {
			wall->rotation_rate -= 1.f;
			editor_dirty = true;
		}

		if (CP_Input_MouseWheel() != 0.f) {
			if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
				wall->scale.x += CP_Input_MouseWheel();
				wall->scale.x = max(1.f, wall->scale.x);
				editor_dirty = true;
			}

			if (CP_Input_MouseDown(MOUSE_BUTTON_RIGHT)) {
				wall->scale.y += CP_Input_MouseWheel();
				wall->scale.y = max(1.f, wall->scale.y);
				editor_dirty = true;
			}
		}
	}

	if (current_type == EDIT_TERRAIN) {
		Terrain* terrain = (Terrain*)current_editing;
		if (CP_Input_MouseWheel() != 0.f) {
			if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
				terrain->scale.x += CP_Input_MouseWheel() * 0.1f;
				terrain->scale.x = max(0.1f, terrain->scale.x);
				editor_dirty = true;
			}

			if (CP_Input_MouseDown(MOUSE_BUTTON_RIGHT)) {
				terrain->scale.y += CP_Input_MouseWheel() * 0.1f;
				terrain->scale.y = max(0.1f, terrain->scale.y);
				editor_dirty = true;
			}
		}
	}
}

void Editor_Render(void) {
	CP_Vector world_limits = Get_World_Limits();
	CP_Vector world_ratios = Get_World_Ratios();
	CP_Settings_ResetMatrix();
	CP_Font_Set(Asset_GetFont(ARIAL_FONT_ASSET));
	CP_Settings_Fill(Get_Color(COLOR_CYAN));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_TextSize(50.f);
	// editor dirty here to make sure that any changes need to be saved
	char e[100]; sprintf_s(e, 100, "%sEditor Mode! Press P to toggle modes!", editor_dirty ? "*" : "");
	CP_Font_DrawText(e, CP_System_GetWindowWidth() * 0.5f, 20.f);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_TextSize(20.f);
	CP_Font_DrawText("Left Click: Select/Drag | Right Click: Rotate | Mouse Wheel + Left/Right Click: Scale", 5.f, CP_System_GetWindowHeight() - 120.f);
	CP_Font_DrawText("Arrow Up/Down: Increase/Decrease Rotation Speed (Walls Only)", 5.f, CP_System_GetWindowHeight() - 100.f);
	CP_Font_DrawText("Arrow Left/Right: Change Item / Terrain Type", 5.f, CP_System_GetWindowHeight() - 80.f);
	char a[100]; sprintf_s(a, 100, "Press 'W' to add Wall | 'H' to add Hole | 'B' to add Ball | '1'-'%d' to add Items", ITEM_LAST - 1);
	CP_Font_DrawText(a, 5.f, CP_System_GetWindowHeight() - 60.f);
	CP_Font_DrawText("Press '8'-'0' to add Terrains | 'DEL' to delete selected", 5.f, CP_System_GetWindowHeight() - 40.f);
	CP_Font_DrawText("Press 'S' to Save Level to current file", 5.f, CP_System_GetWindowHeight() - 20.f);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_TOP);
	char wl[100]; sprintf_s(wl, 100, "World Size: (%.2f, %.2f)", world_limits.x, world_limits.y);
	CP_Font_DrawText(wl, CP_System_GetWindowWidth() - 5.f, 0.f);
	CP_Font_DrawText("Ctrl -/+ to change world size", CP_System_GetWindowWidth() - 5.f, 20.f);

	if (editor_saving) {
		CP_Settings_Fill(Get_Color(COLOR_RED));
		CP_Settings_Stroke(Get_Color(COLOR_RED));
		CP_Settings_TextSize(50.f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("Level Saved!", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f);
	}

	if (current_editing == NULL)
		return;

	CP_Settings_ApplyMatrix(Get_Camera());

	if (current_type == EDIT_WALL || current_type == EDIT_MIDDLE_WALL) {
		Wall* wall = (Wall*)current_editing;
		CP_Vector screen_pos = World_To_Screen(wall->pos.x, wall->pos.y, world_limits.x, world_limits.y);
		CP_Vector normal = World_To_Screen(wall->pos.x + wall->normal.x, wall->pos.y + wall->normal.y, world_limits.x, world_limits.y);
		CP_Vector right = World_To_Screen(wall->pos.x + wall->right.x, wall->pos.y + wall->right.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(Get_Color(COLOR_RED));
		CP_Settings_Stroke(Get_Color(COLOR_RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, normal.x, normal.y);

		CP_Settings_Fill(Get_Color(COLOR_BLUE));
		CP_Settings_Stroke(Get_Color(COLOR_BLUE));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, right.x, right.y);
	}
	else if (current_type == EDIT_HOLE) {
		Hole* hole = (Hole*)current_editing;
		CP_Vector screen_pos = World_To_Screen(hole->pos.x, hole->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(Get_Color(COLOR_RED));
		CP_Settings_Stroke(Get_Color(COLOR_RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * hole->diameter * 0.5f);
	}
	else if (current_type == EDIT_BALL) {
		GolfBall* ball = (GolfBall*)current_editing;
		CP_Vector screen_pos = World_To_Screen(ball->pos.x, ball->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(Get_Color(COLOR_RED));
		CP_Settings_Stroke(Get_Color(COLOR_RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * ball->diameter * 0.5f);
	}
	else if (current_type == EDIT_ITEM) {
		Item* item = (Item*)current_editing;
		CP_Vector screen_pos = World_To_Screen(item->pos.x, item->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(Get_Color(COLOR_RED));
		CP_Settings_Stroke(Get_Color(COLOR_RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * item->diameter * 0.5f);
	} 
	else if (current_type == EDIT_TERRAIN) {
		Terrain* terrain = (Terrain*)current_editing;
		CP_Vector screen_pos = World_To_Screen(terrain->pos.x, terrain->pos.y, world_limits.x, world_limits.y);
		terrain->angle_radians = CP_Math_Radians(terrain->angle);
		CP_Vector terrain_normal = { cosf(-terrain->angle_radians), sinf(-terrain->angle_radians) };
		CP_Vector terrain_right = { terrain_normal.y, -terrain_normal.x };
		CP_Vector normal = World_To_Screen(terrain->pos.x + terrain_normal.x, terrain->pos.y + terrain_normal.y, world_limits.x, world_limits.y);
		CP_Vector right = World_To_Screen(terrain->pos.x + terrain_right.x, terrain->pos.y + terrain_right.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(Get_Color(COLOR_RED));
		CP_Settings_Stroke(Get_Color(COLOR_RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, normal.x, normal.y);

		CP_Settings_Fill(Get_Color(COLOR_BLUE));
		CP_Settings_Stroke(Get_Color(COLOR_BLUE));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, right.x, right.y);
	}
}