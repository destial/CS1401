/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 16 September 2025
@course CSD1401
@brief Definition file for loading levels from text file into memory
*/

#pragma once
#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H
#include "cprocessing.h"
#include "game.h"
#include "utils.h"

#ifdef _DEBUG
#define TOTAL_LEVELS 11
#endif
#ifndef _DEBUG
#define TOTAL_LEVELS 10
#endif
typedef struct LevelStruct *Level;

typedef struct LevelStruct { // ordered in this way to minimize struct size
    float world_width, world_height;
    float* golf_balls; // [0 = x, 1 = y, 2 = diameter]
    float* holes; // [0 = x, 1 = y, 2 = diameter]
    float* walls; // [0 = x, 1 = y, 2 = scale.x, 3 = scale.y, 4 = angle, 5 = rotation rate]
    float* items; // [0 = x, 1 = y, 2 = diameter, 3 = angle, 4 = type]
    float* terrains; // [0 = x, 1 = y, 2 = scale.x, 3 = scale.y, 4 = angle, 5 = type]
    int golf_balls_size, holes_size, walls_size, items_size, terrains_size;
    float middle_wall_angle;
    int par, tutorial_level, timer;
} *Level;

// Offsets the float array by its respective values ^^ (see above)
static const int WALLS_OFFSET = 6;
static const int ITEMS_OFFSET = 5;
static const int TERRAINS_OFFSET = 6;
static const int BALL_HOLE_OFFSET = 3;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load the level based on the given level from a level text file. 
This is what you should use to get the level. Make sure to free_level() the returned level before exiting the game.
@param level - The level to load from text file (e.g. level_%d.lvl)
@return The loaded level
*/
Level Level_Load(int level);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Save the level based on the given level to a level text file.
@param level - The level to load from text file (e.g. level_%d.lvl)
@param walls - The walls array to save
@param golf_balls - The golf balls array to save
@param holes - The holes array to save
@param items - The items array to save
@param world_limits - The world width and height
@param par - The par of the level
*/
void Level_Save(int level, ObjectArray* walls, ObjectArray* golf_balls, ObjectArray* holes, ObjectArray* items, ObjectArray* terrains, CP_Vector world_limits, int par, int timer);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the memory allocated for the level loaded
@param level - The level returned from get_level(int)
*/
void Level_Free(Level level);


#endif