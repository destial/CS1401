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
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

typedef struct MapStruct Map;

typedef struct MapStruct {
    char* key, * value;
    Map* next;
} Map;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the memory allocated for this map of the text file, also recursively goes through each next pointer
@param map - The root map
*/
void free_map(Map* map);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load the file into a searchable map
@param filename - The filename
@return The loaded map in heap, nullable. Ensure to free_map() after using!!!!
*/
Map* load_map(const char* filename);

Map* add_entry(Map* map, const char* key, char* value);
Map* add_entry_int(Map* map, const char* key, int value);
Map* add_entry_float(Map* map, const char* key, float value);
Map* add_entry_vector(Map* map, const char* key, CP_Vector vector);

void edit_entry(Map* map, const char* key, char* value);
void edit_entry_int(Map* map, const char* key, int value);
void edit_entry_float(Map* map, const char* key, float value);
void edit_entry_vector(Map* map, const char* key, CP_Vector value);

/*!
@brief Remove an entry from the given map
@param map - The root map object
@param key - The key to remove
@return The new root map pointer if we removed the first map node, else the same root node
*/
Map* remove_entry(Map* map, const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Save the searchable map into a file
@param filename - The filename
@param map - The loaded map in heap
@return TRUE if saved, FALSE if not
*/
CP_BOOL save_map(const char* filename, Map* map);

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
@brief Get the value in the given map based on the given string key
@param map - The root map
@param key - The string key
*/
char* get_value(Map* map, const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the value as a float in the given map based on the given string key
@param map - The root map
@param key - The string key
@param def - The default value if it doesn't exist
*/
float get_float(Map* map, const char* key, float def);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the value as an int in the given map based on the given string key
@param map - The root map
@param key - The string key
@param def - The default value if it doesn't exist
*/
int get_int(Map* map, const char* key, int def);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the value as an int in the given map based on the given string key
@param map - The root map
@param key - The string key
@return If the vector doesn't exist, it's a {0,0} vector
*/
CP_Vector get_vector(Map* map, const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Parse the given line as a vector (e.g. 123,421)
@param line - The string line to parse
@return The parsed vector
*/
CP_Vector parse_vector(char* line);

#endif