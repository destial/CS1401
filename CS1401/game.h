/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 September 2025
@course CSD1401
@brief Header file for implementing the main game functions
*/

#pragma once
#ifndef GAME_H
#define GAME_H
#include "cprocessing.h"

typedef enum ItemTypeEnum {
	NO_ITEM_TYPE = 0,
	BOOST,
	BIG,
	SMALL,
	REVERSE,
	PORTAL,
	ITEM_LAST
} ItemType;

typedef enum TerrainTypeEnum {
	SAND = 0,
	ICE,
	PIT,
	TERRAIN_LAST
} TerrainType;

typedef struct WallStruct {
	CP_Vector pos, scale, normal, right; // wall position, scale, normal, right (world space)
	CP_Color color; // wall color if no base image
	CP_Image image; // wall image
	float angle, angle_radians, rotation_rate; // wall rotation angle & rate
	CP_BOOL destroyed; // if this wall is disabled (for object pooling)

	// debug only values
	CP_Vector _collision_point, _wall_to_ball;
} Wall;

typedef struct GolfBallStruct {
	CP_Vector pos, vel, prev_pos, stroke_pos; // golfball position, velocity, prev pos & last stroke pos (world space)
	float diameter, angle; // golfball diameter (world space) & rotation angle
	CP_BOOL dragging, halt, scored, scoring, destroyed, teleported;
	// dragging  player currently striking
	// halt = if one ball is currently striking, halt the other ball from moving
	// scored = if this ball has scored
	// scoring = if this ball is falling in the hole (for animation purposes)
	// destroyed = if this ball is disabled (for object pooling)
} GolfBall;

typedef struct ItemStruct {
	CP_Vector pos; // item position (world space)
	float diameter, angle; // item diameter (world space) & rotation angle
	ItemType type; // item type
	CP_BOOL collected, destroyed; // if item has collected or destroyed (for object pooling)
} Item;

typedef struct HoleStruct {
	CP_Vector pos; // hole position (world space)
	float diameter; // hole diameter (world space)
	CP_BOOL destroyed; // if this hole is disabled (for object pooling)
} Hole;

typedef struct TerrainStruct {
	CP_Vector pos, scale; // terrain pos & scale (world space)
	TerrainType type; // terrain type
	float angle, angle_radians; // terrain rotation angle
	CP_BOOL destroyed; // if this terrain is disabled (for object pooling)
} Terrain;

/*!
@brief Game scene init
*/
void game_init(void);

/*!
@brief Game scene processing
*/
void game_update(void);

/*!
@brief Game physics processing
*/
void game_physics_update(float dt);

/*!
@brief Game scene rendering
*/
void game_render(void);

/*!
@brief Render items
*/
void render_items(void);

/*!
@brief Render walls
*/
void render_walls(void);

/*!
@brief Render terrains
*/
void render_terrains(void);

/*!
@brief Render balls
*/
void render_balls(void);

/*!
@brief Render holes
*/
void render_holes(void);

/*!
@brief Render ui
*/
void render_ui(void);

/*!
@brief Game scene deallocation
*/
void game_exit(void);

/*!
@brief Update wall normals and rotation
*/
void update_wall(Wall* wall, float dt);

/*!
@brief Check the collision between a wall and a golf ball
@param wall - The wall to check
@param ball - The golf ball to check
@return TRUE if collided, FALSE if not
*/
CP_BOOL check_collision(Wall* wall, GolfBall* ball);

/*!
@brief Check if point is on terrain
@param terrain - The terrain to check
@param pos - The position in world space
@return TRUE if on terrain, FALSE if not
*/
CP_BOOL check_terrain(Terrain* terrain, CP_Vector pos);

/*!
@brief Check the collision between golf balls
@param ball_1 - The 1st golf ball
@param ball_2 - The 2nd golf ball
@return TRUE if collided, FALSE if not
*/
CP_BOOL check_collision_ball(GolfBall* ball_1, GolfBall* ball_2);

/*!
@brief Check if golfball rolled over item
@param item - The item to check
@param ball - The golf ball to check
@return TRUE if over item, FALSE if not
*/
CP_BOOL check_item(Item* item, GolfBall* ball);

/*!
@brief Respond to the collision between a wall and a ball according to the wall's normal
@param wall - The wall it hit
@param ball - The golf ball that hit
@param point - The point at which the golf ball hit the wall
@param normal - The normal of the wall to the ball
*/
void collision_response(Wall* wall, GolfBall* ball, CP_Vector point, CP_Vector normal);

/*!
@brief Respond to the collision between a ball and a ball according to the wall's normal
@param ball_1 - The 1st golf ball that hit
@param ball_2 - The 2nd golf ball that hit
*/
void collision_response_ball(GolfBall* ball_1, GolfBall* ball_2);

/*!
@brief Save the current level data to the text file under "Assets/Levels/level_{current_level}.lvl"
*/
void save(void);

/*!
@brief Load the current level from text file under "Assets/Levels/level_{current_level}.lvl"
*/
void load(void);

/*!
@brief Set the current game level. To be called before load(). Does nothing if not before game init
@param level - The level to load from "Assets/Levels/level_{level}.lvl"
*/
void set_game_level(char level);

/*!
@brief Get the current game level that is being played on
*/
char get_game_level(void);

/*!
@brief Render debug information such as FPS, mouse coords, ball positions & velocity and collision markers
*/
void debug_render_info(void);

/*!
@brief Get the mouse position in world space
*/
CP_Vector get_mouse_world(void);

/*!
@brief Get the world width and height
*/
CP_Vector get_world_limits(void);

/*!
@brief Get the world width and height ratios according to the display width and height (e.g. displayWidth / worldWidth)
*/
CP_Vector get_world_ratios(void);

/*!
@brief Get the terrain resistance at this position
*/
float get_resistance(CP_Vector pos);

/*!
@brief Add a wall to the level
*/
Wall* add_wall(CP_Vector pos);

/*!
@brief Add an item to the level
*/
Item* add_item(CP_Vector pos, ItemType type);

/*!
@brief Remove a wall from the level
*/
void remove_wall(Wall* wall);

/*!
@brief Remove item from the level
*/
void remove_item(Item* item);

/*!
@brief Add a wall to the level
*/
GolfBall* add_ball(CP_Vector pos);

/*!
@brief Add an item to the level
*/
Hole* add_hole(CP_Vector pos);

/*!
@brief Remove a wall from the level
*/
void remove_ball(GolfBall* ball);

/*!
@brief Remove item from the level
*/
void remove_hole(Hole* hole);

/*!
@brief Remove a terrain from the level
*/
void remove_terrain(Terrain* terrain);

/*!
@brief Add a terrain to the level
*/
Terrain* add_terrain(CP_Vector pos, TerrainType type);

#endif