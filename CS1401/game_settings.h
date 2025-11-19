/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 13 November 2025
@course CSD1401
@brief Header file for game settings management
*/

#pragma once
#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H
#include "utils.h"

typedef enum CursorEnum {
	CURSOR_DEFAULT = 0,
	CURSOR_POINT,
	CURSOR_GRAB,
	CURSOR_ROTATE
} CursorType;

void GameSetting_SetCursor(CursorType cursor);

void GameSetting_RenderCursor(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get a game setting value as a boolean
@param key - The setting key
@return The boolean value of the setting
*/
bool GameSetting_GetBool(const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get a game setting value as an integer
@param key - The setting key
@return The integer value of the setting
*/
int GameSetting_GetInt(const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get a game setting value as a float
@param key - The setting key
@return The float value of the setting
*/
float GameSetting_GetFloat(const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get a game setting value as a string
@param key - The setting key
@return The string value of the setting
*/
const char* GameSetting_GetString(const char* key);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Set a game setting value as a boolean
@param key - The setting key
@param value - The boolean value to set
*/
void GameSetting_SetBool(const char* key, bool value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Set a game setting value as an int
@param key - The setting key
@param value - The int value to set
*/
void GameSetting_SetInt(const char* key, int value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Set a game setting value as a float
@param key - The setting key
@param value - The float value to set
*/
void GameSetting_SetFloat(const char* key, float value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Set a game setting value as a string
@param key - The setting key
@param value - The string value to set
*/
void GameSetting_SetString(const char* key, const char* value);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Save the current game settings to file (user.data)
*/
void GameSetting_Save(void);

#endif