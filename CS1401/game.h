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
#include "utils.h"

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
	CP_Vector _collision_point, _wall_to_ball; // debug only values
	CP_Image image; // wall image
	CP_Color color; // wall color if no base image
	float angle, rotation_rate; // wall rotation angle & rate
	bool destroyed : 2; // if this wall is disabled (for object pooling)
} Wall;

typedef struct GolfBallStruct {
	CP_Vector pos, vel, prev_pos, stroke_pos; // golfball position, velocity, prev pos & last stroke pos (world space)
	float diameter, angle; // golfball diameter (world space) & rotation angle
	bool dragging : 1, halt : 1, scored : 1, scoring : 1, destroyed : 2, teleported : 2;
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
	bool collected : 2, destroyed : 2; // if item has collected or destroyed (for object pooling)
} Item;

typedef struct HoleStruct {
	CP_Vector pos; // hole position (world space)
	float diameter; // hole diameter (world space)
	bool destroyed; // if this hole is disabled (for object pooling)
} Hole;

typedef struct TerrainStruct {
	CP_Vector pos, scale; // terrain pos & scale (world space)
	TerrainType type; // terrain type
	float angle, angle_radians; // terrain rotation angle
	bool destroyed; // if this terrain is disabled (for object pooling)
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
@brief Game scene deallocation
*/
void game_exit(void);

/*!
@brief Process user inputs
*/
void Process_Inputs(void);

/*!
@brief Game physics processing
*/
void Game_UpdatePhysics(float dt);

/*!
@brief Game scene rendering
*/
void Game_Render(void);

/*!
@brief Render terrains
*/
void Game_RenderTerrains(void);

/*!
@brief Render walls
*/
void Game_RenderWalls(void);

/*!
@brief Render holes
*/
void Game_RenderHoles(void);

/*!
@brief Render items
*/
void Game_RenderItems(void);

/*!
@brief Render balls
*/
void Game_RenderGolfs(void);

/*!
@brief Render shadows
*/
void Game_RenderShadows(void);

/*!
@brief Render UI
*/
void Game_RenderUI(void);

/*!
@brief Render debug information such as FPS, mouse coords, ball positions & velocity and collision markers
*/
void Game_RenderDebug(void);

/*!
@brief Save the current level data to the text file under "Assets/Levels/level_{current_level}.lvl"
*/
void Game_Save(void);

/*!
@brief Load the current level from text file under "Assets/Levels/level_{current_level}.lvl"
*/
void Game_Load(void);

/*!
@brief Update wall normals and rotation
@param wall - The wall to update
@param dt - The delta frametime
*/
void Game_UpdateWall(Wall* wall, float dt);

/*!
@brief Internal function to let the game know its resetting without changing state
*/
void Game_SetResetting(bool resetting);

/*!
@brief Get the camera transformation matrix
*/
CP_Matrix Get_Camera(void);

/*!
@brief Check the collision between a wall and a golf ball
@param wall - The wall to check
@param ball - The golf ball to check
@return TRUE if collided, FALSE if not
*/
bool Check_Wall_Collision(Wall* wall, GolfBall* ball);

/*!
@brief Check if point is on terrain
@param terrain - The terrain to check
@param pos - The position in world space
@return TRUE if on terrain, FALSE if not
*/
bool Check_Terrain(Terrain* terrain, CP_Vector pos);

/*!
@brief Check the collision between golf balls
@param ball_1 - The 1st golf ball
@param ball_2 - The 2nd golf ball
@return TRUE if collided, FALSE if not
*/
bool Check_Ball_Collision(GolfBall* ball_1, GolfBall* ball_2);

/*!
@brief Check if golfball rolled over item
@param item - The item to check
@param ball - The golf ball to check
@return TRUE if over item, FALSE if not
*/
bool Check_Item(Item* item, GolfBall* ball);

/*!
@brief Respond to the collision between a wall and a ball according to the wall's normal
@param wall - The wall it hit
@param ball - The golf ball that hit
@param point - The point at which the golf ball hit the wall
@param normal - The normal of the wall to the ball
*/
void Collision_Response_Wall(Wall* wall, GolfBall* ball, CP_Vector point, CP_Vector normal);

/*!
@brief Respond to the collision between a ball and a ball according to the wall's normal
@param ball_1 - The 1st golf ball that hit
@param ball_2 - The 2nd golf ball that hit
*/
void Collision_Response_Ball(GolfBall* ball_1, GolfBall* ball_2);

/*!
@brief Set the current game level. To be called before load(). Does nothing if not before game init
@param level - The level to load from "Assets/Levels/level_{level}.lvl"
*/
void Game_SetLevel(char level);

/*!
@brief Get the current game level that is being played on
*/
char Game_GetLevel(void);

/*!
@brief Get the mouse position in world space
*/
CP_Vector Get_Mouse_WorldSpace(void);

/*!
@brief Get the world width and height
*/
CP_Vector Get_World_Limits(void);

/*!
@brief Rescale the world limits
*/
void Scale_World(float scale);

/*!
@brief Get the world width and height ratios according to the display width and height (e.g. displayWidth / worldWidth)
*/
CP_Vector Get_World_Ratios(void);

/*!
@brief Get the terrain resistance at this position
*/
float Get_Resistance_At(CP_Vector pos);

/*!
@brief Add a wall to the level
*/
Wall* Game_AddWall(CP_Vector pos);

/*!
@brief Add an item to the level
*/
Item* Game_AddItem(CP_Vector pos, ItemType type);

/*!
@brief Remove a wall from the level
*/
void Game_RemoveWall(Wall* wall);

/*!
@brief Remove item from the level
*/
void Game_RemoveItem(Item* item);

/*!
@brief Add a wall to the level
*/
GolfBall* Game_AddGolf(CP_Vector pos);

/*!
@brief Add an item to the level
*/
Hole* Game_AddHole(CP_Vector pos);

/*!
@brief Remove a wall from the level
*/
void Game_RemoveGolf(GolfBall* ball);

/*!
@brief Remove item from the level
*/
void Game_RemoveHole(Hole* hole);

/*!
@brief Remove a terrain from the level
*/
void Game_RemoveTerrain(Terrain* terrain);

/*!
@brief Add a terrain to the level
*/
Terrain* Game_AddTerrain(CP_Vector pos, TerrainType type);

#endif