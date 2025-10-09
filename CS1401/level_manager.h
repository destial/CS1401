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

typedef struct Map {
    char* key;
    char* value;
    struct Map* next;
} Map;

typedef struct Level {
    float world_width, world_height;
    CP_Vector golf_pos_1, golf_pos_2;
    CP_Vector hole_1, hole_2;
    int par;
    int walls_size;
    float* walls; // [0 = x, 1 = y, 2 = scale.x, 3 = scale.y, 4 = angle, 5 = rotation rate]
    float middle_wall_angle;

    int items_size;
    float* items; // [0 = x, 1 = y, 2 = diameter, 3 = angle, 4 = rotation rate, 5 = type]
} Level;

// Offsets the float array by its respective values ^^ (see above)
static const int WALLS_OFFSET = 6;
static const int ITEMS_OFFSET = 5;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the memory allocated for this map of the text file, also recursively goes through each next pointer
@param map - The root map
*/
void free_map(Map* map);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Read the contents of the text file. Sourced from https://stackoverflow.com/questions/3381080/reading-all-content-from-a-text-file-c
@param filename - The file name to read the text file from
@return The contents of the text file
*/
char* read_content(const char* filename);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the value in the given map based on the given string key
@param map - The root map
@param key - The string key
*/
char* get_value(Map* map, const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load the level based on the given level from a level text file. 
This is what you should use to get the level. Make sure to free_level() the returned level before exiting the game.
@param
level - The level to load from text file (e.g. level_%d.lvl)
@return
The loaded level
*/
Level* get_level(int level);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the memory allocated for the level loaded
@param level - The level returned from get_level(int)
*/
void free_level(Level* level);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Parse the given line as a vector (e.g. 123,421)
@param line - The string line to parse
@return The parsed vector
*/
CP_Vector get_vector(char* line);

#endif