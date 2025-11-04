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
	NONE = 0,
	WALL,
	HOLE,
	GOLF_BALL,
	ITEM,
	TERRAIN
} EditingType;

void* current_editing = NULL;
void* current_dragging = NULL;
void* current_click = NULL;
CP_Vector current_editing_offset;
EditingType current_type = NONE;
float editor_saving = 0.f;
bool editor_dirty = false;

bool Check_On_Wall(Wall* wall) {
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
	return Check_Terrain(terrain, Get_Mouse_WorldSpace());
}

bool Check_On_Circle(CP_Vector pos, float diameter) {
	CP_Vector mouse_world = Get_Mouse_WorldSpace();
	return CP_Vector_Distance(pos, mouse_world) <= diameter * 0.5f;
}

void Editor_Update_Walls(ObjectArray* walls) {
	CP_Vector mouse_world = Get_Mouse_WorldSpace();

	if (CP_Input_KeyTriggered(KEY_W)) {
		Wall* wall = Game_AddWall(mouse_world);
		if (wall) {
			current_editing = wall;
			current_editing_offset = CP_Vector_Subtract(wall->pos, mouse_world);
			current_type = WALL;
		}
	}

	for (int i = 0; i < walls->length; i++) {
		Wall* wall = (Wall*)walls->arr + i;
		if (wall == NULL || wall->destroyed)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && Check_On_Wall(wall)) {
			current_editing = wall;
			current_click = wall;
			current_editing_offset = CP_Vector_Subtract(wall->pos, mouse_world);
			current_type = WALL;
		}

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_RIGHT) && current_editing == wall) {
			current_editing_offset = CP_Vector_Subtract(wall->pos, mouse_world);
		}
	}

	if (CP_Input_KeyTriggered(KEY_DELETE) && current_editing != NULL && current_type == WALL) {
		Game_RemoveWall((Wall*)current_editing);
		current_editing = NULL;
		current_dragging = NULL;
		current_type = NONE;
		editor_dirty = true;
	}
}

void Editor_Update_Terrains(ObjectArray* terrains) {
	CP_Vector mouse_world = Get_Mouse_WorldSpace();

	if (CP_Input_KeyTriggered(KEY_8)) {
		Terrain* terrain = Game_AddTerrain(mouse_world, ICE);
		if (terrain) {
			current_editing = terrain;
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
			current_type = TERRAIN;
			editor_dirty = true;
		}
	}

	if (CP_Input_KeyTriggered(KEY_9)) {
		Terrain* terrain = Game_AddTerrain(mouse_world, SAND);
		if (terrain) {
			current_editing = terrain;
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
			current_type = TERRAIN;
			editor_dirty = true;
		}
	}

	if (CP_Input_KeyTriggered(KEY_0)) {
		Terrain* terrain = Game_AddTerrain(mouse_world, PIT);
		if (terrain) {
			current_editing = terrain;
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
			current_type = TERRAIN;
			editor_dirty = true;
		}
	}

	for (int i = 0; i < terrains->length; i++) {
		Terrain* terrain = (Terrain*)terrains->arr + i;
		if (terrain == NULL || terrain->destroyed)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && Check_On_Terrain(terrain)) {
			current_editing = terrain;
			current_click = terrain;
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
			current_type = TERRAIN;
		}

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_RIGHT) && current_editing == terrain) {
			current_editing_offset = CP_Vector_Subtract(terrain->pos, mouse_world);
		}
	}

	if (current_editing != NULL && current_type == TERRAIN) {
		Terrain* t = (Terrain*)current_editing;
		if (CP_Input_KeyTriggered(KEY_RIGHT)) {
			++t->type;
			if (t->type == TERRAIN_LAST) {
				t->type = 0;
			}
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_LEFT)) {
			--t->type;
			if (t->type == -1) {
				t->type = TERRAIN_LAST - 1;
			}
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_DELETE)) {
			Game_RemoveTerrain(t);
			current_editing = NULL;
			current_dragging = NULL;
			current_type = NONE;
			editor_dirty = true;
		}
	}
}

void Editor_Update_Holes(ObjectArray* holes) {
	CP_Vector mouse_world = Get_Mouse_WorldSpace();

	if (CP_Input_KeyTriggered(KEY_H)) {
		Hole* hole = Game_AddHole(mouse_world);
		if (hole) {
			current_editing = hole;
			current_editing_offset = CP_Vector_Subtract(hole->pos, mouse_world);
			current_type = HOLE;
			editor_dirty = true;
		}
	}

	for (int i = 0; i < holes->length; i++) {
		Hole* hole = (Hole*) holes->arr + i;
		if (hole == NULL || hole->destroyed)
			continue;

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT) && Check_On_Circle(hole->pos, hole->diameter)) {
			current_editing = hole;
			current_click = hole;
			current_editing_offset = CP_Vector_Subtract(hole->pos, mouse_world);
			current_type = HOLE;
		}
	}

	if (current_editing != NULL && current_type == HOLE) {
		Hole* hole = (Hole*)current_editing;
		if (CP_Input_MouseWheel() != 0.f) {
			if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
				hole->diameter += CP_Input_MouseWheel();
			}
		}

		if (CP_Input_KeyTriggered(KEY_DELETE)) {
			Game_RemoveHole(hole);
			current_editing = NULL;
			current_dragging = NULL;
			current_type = NONE;
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
			current_type = GOLF_BALL;
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
			current_type = GOLF_BALL;
		}
	}

	if (current_editing != NULL && current_type == GOLF_BALL) {
		GolfBall* ball = (GolfBall*)current_editing;
		if (CP_Input_MouseWheel() != 0.f) {
			if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
				ball->diameter += CP_Input_MouseWheel();
				editor_dirty = true;
			}
		}

		if (CP_Input_KeyTriggered(KEY_DELETE)) {
			Game_RemoveGolf(ball);
			current_editing = NULL;
			current_dragging = NULL;
			current_type = NONE;
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
				current_type = ITEM;
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
			current_type = ITEM;
		}
	}

	if (current_editing != NULL && current_type == ITEM) {
		Item* i = (Item*)current_editing;

		if (CP_Input_MouseWheel() != 0.f) {
			if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
				i->diameter += CP_Input_MouseWheel();
				editor_dirty = true;
			}
		}

		if (CP_Input_KeyTriggered(KEY_RIGHT)) {
			++i->type;
			if (i->type == ITEM_LAST) {
				i->type = NO_ITEM_TYPE + 1;
			}
			i->angle = 0.f;
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_LEFT)) {
			--i->type;
			if (i->type == NO_ITEM_TYPE) {
				i->type = ITEM_LAST - 1;
			}
			i->angle = 0.f;
			editor_dirty = true;
		}
		else if (CP_Input_KeyTriggered(KEY_DELETE)) {
			Game_RemoveItem(i);
			current_editing = NULL;
			current_dragging = NULL;
			current_type = NONE;
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

	if (editor_saving) {
		editor_saving -= CP_System_GetDt();
		editor_saving = max(0.f, editor_saving);
	}

	if (current_click == NULL) {
		current_editing = NULL;
	}

	if (current_editing == NULL)
		return;

	if (CP_Input_MouseDragged(MOUSE_BUTTON_LEFT)) {
		if (current_type == WALL && Check_On_Wall((Wall*)current_editing)) {
			current_dragging = current_editing;
		}
		else if (current_type == HOLE && Check_On_Circle(((Hole*)current_editing)->pos, ((Hole*)current_editing)->diameter)) {
			current_dragging = current_editing;
		}
		else if (current_type == GOLF_BALL && Check_On_Circle(((GolfBall*)current_editing)->pos, ((GolfBall*)current_editing)->diameter)) {
			current_dragging = current_editing;
		}
		else if (current_type == ITEM && Check_On_Circle(((Item*)current_editing)->pos, ((Item*)current_editing)->diameter)) {
			current_dragging = current_editing;
		} 
		else if (current_type == TERRAIN && Check_On_Terrain((Terrain*)current_editing)) {
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
			if (current_type == WALL) {
				Wall* wall = (Wall*)current_dragging;
				wall->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				wall->image = wall->pos.x <= Get_World_Limits().x * 0.5f ? Asset_GetImage(WALL_LEFT_ASSET) : Asset_GetImage(WALL_RIGHT_ASSET);
				editor_dirty = true;
			}
			else if (current_type == HOLE) {
				Hole* hole = (Hole*)current_dragging;
				hole->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				editor_dirty = true;

			}
			else if (current_type == GOLF_BALL) {
				GolfBall* ball = (GolfBall*)current_dragging;
				ball->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				editor_dirty = true;
			}
			else if (current_type == ITEM) {
				Item* item = (Item*)current_dragging;
				item->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				editor_dirty = true;
			}
			else if (current_type == TERRAIN) {
				Terrain* terrain = (Terrain*)current_dragging;
				terrain->pos = CP_Vector_Add(mouse_world, current_editing_offset);
				editor_dirty = true;
			}
		}
		else if (CP_Input_MouseDragged(MOUSE_BUTTON_RIGHT)) { // rotate
			if (current_type == WALL) {
				Wall* wall = (Wall*)current_dragging;
				wall->angle = CP_Vector_AngleCW(CP_Vector_Subtract(wall->pos, mouse_world), current_editing_offset);

				// angle snapping
				if (fabs((double)(45.f - wall->angle)) <= 3.f) {
					wall->angle = 45.f;
				}

				if (fabs((double)(90.f - wall->angle)) <= 3.f) {
					wall->angle = 90.f;
				}

				if (fabs((double)(135.f - wall->angle)) <= 3.f) {
					wall->angle = 135.f;
				}

				if (fabs((double)(180.f - wall->angle)) <= 3.f) {
					wall->angle = 180.f;
				}

				if (fabs((double)(225.f - wall->angle)) <= 3.f) {
					wall->angle = 225.f;
				}

				if (fabs((double)(270.f - wall->angle)) <= 3.f) {
					wall->angle = 270.f;
				}

				if (fabs((double)(315.f - wall->angle)) <= 3.f) {
					wall->angle = 315.f;
				}

				if (fabs((double)(wall->angle)) <= 3.f) {
					wall->angle = 0.f;
				}
				editor_dirty = true;
			} else if (current_type == TERRAIN) {
				Terrain* terrain = (Terrain*)current_dragging;
				terrain->angle = CP_Vector_AngleCW(CP_Vector_Subtract(terrain->pos, mouse_world), current_editing_offset);
				editor_dirty = true;
			}
		}
	}
	
	if (current_type == WALL) {
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

	if (current_type == TERRAIN) {
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
	CP_Settings_Fill(Get_Color(COLOR_GREEN));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_TextSize(50.f);
	char e[100]; sprintf_s(e, 100, "%sEditor Mode! Press P to toggle modes!", editor_dirty ? "*" : "");
	CP_Font_DrawText(e, CP_System_GetWindowWidth() * 0.5f, 50.f);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_TextSize(20.f);
	CP_Font_DrawText("Left Click: Select/Drag | Right Click: Rotate | Mouse Wheel + Left/Right Click: Scale (Walls Only)", 5.f, CP_System_GetWindowHeight() - 120.f);
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

	if (current_type == WALL) {
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
	else if (current_type == HOLE) {
		Hole* hole = (Hole*)current_editing;
		CP_Vector screen_pos = World_To_Screen(hole->pos.x, hole->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(Get_Color(COLOR_RED));
		CP_Settings_Stroke(Get_Color(COLOR_RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * hole->diameter * 0.5f);
	}
	else if (current_type == GOLF_BALL) {
		GolfBall* ball = (GolfBall*)current_editing;
		CP_Vector screen_pos = World_To_Screen(ball->pos.x, ball->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(Get_Color(COLOR_RED));
		CP_Settings_Stroke(Get_Color(COLOR_RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * ball->diameter * 0.5f);
	}
	else if (current_type == ITEM) {
		Item* item = (Item*)current_editing;
		CP_Vector screen_pos = World_To_Screen(item->pos.x, item->pos.y, world_limits.x, world_limits.y);

		CP_Settings_Fill(Get_Color(COLOR_RED));
		CP_Settings_Stroke(Get_Color(COLOR_RED));
		CP_Graphics_DrawLine(screen_pos.x, screen_pos.y, screen_pos.x, screen_pos.y - world_ratios.y * item->diameter * 0.5f);
	} 
	else if (current_type == TERRAIN) {
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