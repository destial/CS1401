#include "pokemon.h"
#include "cprocessing.h"
#include "asset_manager.h"
#include "sprite_manager.h"
#include "particle_manager.h"
#include "utils.h"
#include <stdio.h>

void pokemon_render(void);
void tile_render(void);
void overworld_render(void);

typedef enum TileTypeEnum {
	TILE_NORMAL = 0,
	TILE_GRASS,
	TILE_TREE_0_0,
	TILE_TREE_0_1,
	TILE_TREE_1_0,
	TILE_TREE_1_1,
	TILE_TREE_2_0,
	TILE_TREE_2_1,
	TILE_TREE_3_0,
	TILE_TREE_3_1,
} TileType;

typedef struct PlayerStruct {
	CP_Vector pos;
	CP_Vector prev_pos;
	CP_Vector tile_pos;
	CP_Vector dir;
	SpriteSheet sprite;
	float moved;
	int col;
} Player;

typedef struct TileStruct {
	TileType tile;
	CP_Color background;
} Tile;

/////////////////////////////////////////////////

static CP_Vector world;
static CP_Vector ratios;
static Player player;
static float movement_speed;
static const float DT = 1 / 30.f;
static SpriteSheet tilesheet;
static Tile tilemap[30][50];

void pokemon_init(void) {
	player.pos = CP_Vector_Set(0, 0);
	player.prev_pos = CP_Vector_Set(0, 0);
	player.tile_pos = CP_Vector_Set(0, 0);
	player.moved = -1;
	CP_Font_Set(Asset_GetFont(CARTER_FONT_ASSET));
	player.sprite = Asset_GetSprite("Assets/Pokemon/pokemon_player_sprite.png", 4, 4, 0.f);
	player.col = 0;
	world = CP_Vector_Set(25, 15);
	ratios = CP_Vector_Set(CP_System_GetWindowWidth() / world.x, CP_System_GetWindowHeight() / world.y);
	tilesheet = Asset_GetSprite("Assets/Pokemon/tile_map.png", 31, 28, 0.f);
	CP_Random_NoiseSeed(100);

	for (float x = 0.f; x < world.x * 2; ++x) {
		for (float y = 0.f; y < world.y * 2; ++y) {
			float r = CP_Random_Noise(x / world.x, y / world.y, 0.5f);
			tilemap[(int)y][(int)x].background = Get_Color(COLOR_GREEN);
			tilemap[(int)y][(int)x].tile = TILE_NORMAL;
			if (r >= 0.4f) {
				tilemap[(int)y][(int)x].tile = TILE_GRASS;
			}
		}
	}
}

void pokemon_update(void) {
	if (player.moved < 0.f) {
		CP_Vector prev_dir = CP_Vector_Set(player.dir.x, player.dir.y);
		if (CP_Input_KeyDown(KEY_W)) {
			player.dir.x = 0;
			player.dir.y = 1;
			player.moved = 0.f;
			player.prev_pos = CP_Vector_Set(player.pos.x, player.pos.y);
			if (prev_dir.x != player.dir.x || prev_dir.y != player.dir.y) {
				player.col = 0;
			}
		}
		else if (CP_Input_KeyDown(KEY_S)) {
			player.dir.x = 0;
			player.dir.y = -1;
			player.moved = 0.f;
			player.prev_pos = CP_Vector_Set(player.pos.x, player.pos.y);
			if (prev_dir.x != player.dir.x || prev_dir.y != player.dir.y) {
				player.col = 0;
			}
		}
		else if (CP_Input_KeyDown(KEY_A)) {
			player.dir.x = -1;
			player.dir.y = 0;
			player.moved = 0.f;
			player.prev_pos = CP_Vector_Set(player.pos.x, player.pos.y);
			if (prev_dir.x != player.dir.x || prev_dir.y != player.dir.y) {
				player.col = 0;
			}
		}
		else if (CP_Input_KeyDown(KEY_D)) {
			player.dir.x = 1;
			player.dir.y = 0;
			player.moved = 0.f;
			player.prev_pos = CP_Vector_Set(player.pos.x, player.pos.y);
			if (prev_dir.x != player.dir.x || prev_dir.y != player.dir.y) {
				player.col = 0;
			}
		}

		movement_speed = 2.f;
		if (CP_Input_KeyDown(KEY_LEFT_SHIFT)) {
			movement_speed = 1.f;
		}
	}

	if (player.moved >= 0.f) {
		player.moved += DT;
		if (player.moved / (player.col + 1) >= movement_speed * 0.25f) {
			player.col++;
			if (player.col > 3) {
				player.col = 0;
			}
		}
		bool bounds = false;
		if ((player.pos.x >= world.x - 1 && player.dir.x > 0) ||
			(player.pos.x <= -world.x && player.dir.x < 0) ||
			(player.pos.y <= -world.y && player.dir.y < 0) ||
			(player.pos.y >= world.y - 1 && player.dir.y > 0)) {
			bounds = true;
			
		}
		if (!bounds) {
			player.pos = CP_Vector_Add(player.pos, CP_Vector_Scale(player.dir, DT / movement_speed));
			if (player.moved >= movement_speed * 0.5f) {
				player.tile_pos = CP_Vector_Add(player.prev_pos, player.dir);
			}
		}
		if (player.moved >= movement_speed) {
			player.moved = -1.f;
			if (!bounds) {
				player.pos = CP_Vector_Add(player.prev_pos, player.dir);
			}
		}
	}

	pokemon_render();
}

void tile_render(void) {
	for (float x = -world.x; x < world.x; ++x) {
		for (float y = -world.y; y < world.y; ++y) {
			CP_Vector t = World_To_Screen(x, y, world.x, world.y);
			Tile* tile = &tilemap[(int)(y + world.y)][(int)(x + world.x)];
			CP_Settings_Fill(tile->background);
			CP_Graphics_DrawRect(t.x, t.y, ratios.x, ratios.y);
		}
	}
}

void overworld_render(void) {
	for (float x = -world.x; x < world.x; ++x) {
		int xx = (int)(x + world.x);
		for (float y = world.y - 1; y >= -world.y; --y) {
			CP_Vector t = World_To_Screen(x, y, world.x, world.y);
			int yy = (int)(y + world.y);
			Tile* tile = &tilemap[yy][xx];
			if (tile->tile == TILE_GRASS) {
				Sprite_DrawFrame(tilesheet, 30, 21, t.x, t.y, ratios.x, ratios.y, 255, 0.f);

				if (player.moved >= 0.f && (int)(player.tile_pos.y) == y && (int)(player.tile_pos.x) == x) {
					Particles_Spawn(PARTICLE_GRASS, CP_Vector_Add(player.pos, CP_Vector_Set(0.5f, -0.5f)), 1, 1.f, 0.5f, 1.f, 0.1f, 0.3f);
				}
			}
			else if (tile->tile == TILE_TREE_0_0) {
				Sprite_DrawFrame(tilesheet, 27, 26, t.x, t.y, ratios.x, ratios.y, 255, 0.f);
			}
			else if (tile->tile == TILE_TREE_0_1) {
				Sprite_DrawFrame(tilesheet, 27, 27, t.x, t.y, ratios.x, ratios.y, 255, 0.f);
			}
			else if (tile->tile == TILE_TREE_1_0) {
				Sprite_DrawFrame(tilesheet, 28, 26, t.x, t.y, ratios.x, ratios.y, 255, 0.f);
			}
			else if (tile->tile == TILE_TREE_1_1) {
				Sprite_DrawFrame(tilesheet, 28, 27, t.x, t.y, ratios.x, ratios.y, 255, 0.f);
			}
			else if (tile->tile == TILE_TREE_2_0) {
				Sprite_DrawFrame(tilesheet, 29, 26, t.x, t.y, ratios.x, ratios.y, 255, 0.f);
			}
			else if (tile->tile == TILE_TREE_2_1) {
				Sprite_DrawFrame(tilesheet, 29, 27, t.x, t.y, ratios.x, ratios.y, 255, 0.f);
			}
			else if (tile->tile == TILE_TREE_3_0) {
				Sprite_DrawFrame(tilesheet, 30, 26, t.x, t.y, ratios.x, ratios.y, 255, 0.f);
			}
			else if (tile->tile == TILE_TREE_3_1) {
				Sprite_DrawFrame(tilesheet, 30, 27, t.x, t.y, ratios.x, ratios.y, 255, 0.f);
			}
		}
	}
}

void pokemon_render(void) {
	CP_Graphics_ClearBackground(Get_Color(COLOR_GREEN));
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_RectMode(CP_POSITION_CORNER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);
	CP_Settings_NoStroke();

	CP_Vector coords = World_To_Screen(player.pos.x, player.pos.y, world.x, world.y);
	CP_Vector camera = CP_Vector_Subtract(coords, CP_Vector_Set((CP_System_GetWindowWidth() * 0.5f) - (ratios.x * 0.5f), (CP_System_GetWindowHeight() * 0.5f) - (ratios.y * 0.5f)));
	CP_Settings_ApplyMatrix(CP_Matrix_Multiply(CP_Matrix_Identity(), CP_Matrix_Translate(CP_Vector_Negate(camera))));

	tile_render();

	overworld_render();
	int row = 0;
	if (player.dir.x < 0) {
		row = 1;
	}
	if (player.dir.x > 0) {
		row = 2;
	}
	if (player.dir.y > 0) {
		row = 3;
	}

	CP_Settings_Save();
	Particles_Update(DT);
	Particles_Render(world, ratios);
	CP_Settings_Restore();

	if (tilemap[(int)(player.tile_pos.y + world.y)][(int)(player.tile_pos.x + world.x)].tile == TILE_GRASS) {
		Sprite_DrawCropped(player.sprite, row, (int)player.col, coords.x, coords.y - 10.f, ratios.x, ratios.y, 10.f);
	}
	else {
		Sprite_DrawFrame(player.sprite, row, (int)player.col, coords.x, coords.y - 10.f, ratios.x, ratios.y + 10.f, 255, 0);
	}

	CP_Settings_ResetMatrix();

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
	CP_Vector s_scale = Screen_Scale();
	CP_Settings_Fill(Get_Color(COLOR_RED));
	char fps[16]; sprintf_s(fps, 16, "FPS: %d", (int)(1 / CP_System_GetDt()));
	CP_Settings_TextSize(20.f * s_scale.y);
	CP_Font_DrawText(fps, 0, s_scale.y);

	char m[64]; sprintf_s(m, 64, "Pos: (%.0f, %.0f)", player.tile_pos.x, player.tile_pos.y);
	CP_Settings_TextSize(20.f * s_scale.y);
	CP_Font_DrawText(m, 0, 20.f * s_scale.y);
}

void pokemon_end(void) {
	Assets_Free();
	Particles_Free();
}