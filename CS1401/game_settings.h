#pragma once
#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H
#include "utils.h"

bool GameSetting_GetBool(const char* key);
int GameSetting_GetInt(const char* key);
float GameSetting_GetFloat(const char* key);
char* GameSetting_GetString(const char* key);

void GameSetting_SetBool(const char* key, bool value);
void GameSetting_SetInt(const char* key, int value);
void GameSetting_SetFloat(const char* key, float value);
void GameSetting_SetString(const char* key, const char* value);

void GameSetting_Save(void);

#endif