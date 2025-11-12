/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 28 October 2025
@course CSD1401
@brief Header file for loading and saving text files
*/

#pragma once
#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "cprocessing.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

typedef struct MapStruct *Map;

typedef struct MapStruct {
    char* key, * value;
    Map next;
} *Map;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the memory allocated for this map of the text file, also recursively goes through each next pointer
@param map - The root map
*/
void Map_Free(Map map);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load the file into a searchable map
@param filename - The filename
@return The loaded map in heap, nullable. Ensure to free_map() after using!!!!
*/
Map Map_Load(const char* filename);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Add a string entry to this map. If map is NULL, will create a new map with the return as the root.
@param map - The root map, nullable
@param key - The string key
@param value - The string value
@return The root map node, if map parameter is NULL, will allocate a new map root
*/
Map Map_AddString(Map map, const char* key, char* value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Add an integer entry to this map. If map is NULL, will create a new map with the return as the root.
@param map - The root map, nullable
@param key - The string key
@param value - The integer value
@return The root map node, if map parameter is NULL, will allocate a new map root
*/
Map Map_AddInt(Map map, const char* key, int value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Add a float entry to this map. If map is NULL, will create a new map with the return as the root.
@param map - The root map, nullable
@param key - The string key
@param value - The float value
@return The root map node, if map parameter is NULL, will allocate a new map root
*/
Map Map_AddFloat(Map map, const char* key, float value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Add a vector entry to this map. If map is NULL, will create a new map with the return as the root.
@param map - The root map, nullable
@param key - The string key
@param value - The vector value
@return The root map node, if map parameter is NULL, will allocate a new map root
*/
Map Map_AddVector(Map map, const char* key, CP_Vector value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Add a boolean entry to this map. If map is NULL, will create a new map with the return as the root.
@param map - The root map, nullable
@param key - The string key
@param value - The boolean value
@return The root map node, if map parameter is NULL, will allocate a new map root
*/
Map Map_AddBool(Map map, const char* key, bool value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Edit a string entry of this map from the given key. If the key doesn't exist, it will add a new entry
@param map - The root map, NOT nullable
@param key - The string key
@param value - The string value
*/
void Map_EditString(Map map, const char* key, char* value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Edit a integer entry of this map from the given key. If the key doesn't exist, it will add a new entry
@param map - The root map, NOT nullable
@param key - The integer key
@param value - The string value
*/
void Map_EditInt(Map map, const char* key, int value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Edit a float entry of this map from the given key. If the key doesn't exist, it will add a new entry
@param map - The root map, NOT nullable
@param key - The string key
@param value - The float value
*/
void Map_EditFloat(Map map, const char* key, float value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Edit a vector entry of this map from the given key. If the key doesn't exist, it will add a new entry
@param map - The root map, NOT nullable
@param key - The string key
@param value - The vector value
*/
void Map_EditVector(Map map, const char* key, CP_Vector value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Edit a boolean entry of this map from the given key. If the key doesn't exist, it will add a new entry
@param map - The root map, NOT nullable
@param key - The string key
@param value - The boolean value
*/
void Map_EditBool(Map map, const char* key, bool value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Remove an entry from the given map
@param map - The root map object
@param key - The key to remove
@return The new root map pointer if we removed the first map node, else the same root node
*/
Map Map_Remove(Map map, const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Save the searchable map into a file
@param filename - The filename
@param map - The loaded map
@return TRUE if saved, FALSE if not
*/
bool Map_Save(const char* filename, Map map);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the string value in the given map based on the given string key
@param map - The root map
@param key - The string key
*/
char* Map_GetString(Map map, const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the float value in the given map based on the given string key
@param map - The root map
@param key - The string key
@param def - The default value if it doesn't exist
*/
float Map_GetFloat(Map map, const char* key, float def);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the integer value in the given map based on the given string key
@param map - The root map
@param key - The string key
@param def - The default value if it doesn't exist
*/
int Map_GetInt(Map map, const char* key, int def);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the boolean value in the given map based on the given string key
@param map - The root map
@param key - The string key
@param def - The default value if it doesn't exist
*/
bool Map_GetBool(Map map, const char* key, bool def);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the vector value in the given map based on the given string key
@param map - The root map
@param key - The string key
@return If the vector doesn't exist, it's a {0,0} vector
*/
CP_Vector Map_GetVector(Map map, const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Read the contents of the text file. Sourced from https://stackoverflow.com/questions/3381080/reading-all-content-from-a-text-file-c
@param filename - The file name to read the text file from
@return The contents of the text file. Ensure to free() the contents after using!!!!
*/
char* read_content(const char* filename);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Write the contents to the text file
@param filename - The file name to write the text file to
@return The contents of the text file
*/
void write_content(const char* filename, char* content);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Parse the given line as a vector (e.g. 123.43,421.07)
@param line - The string line to parse
@return The parsed vector
*/
CP_Vector parse_vector(char* line);

#endif