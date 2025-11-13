
#include "game_settings.h"
#include "data_manager.h"

Map settings_file = NULL;

static void load_settings_file(void) {
	settings_file = Map_Load("user.data");
	if (!settings_file) {
		printf("User data file not found! Creating new one!\n");
		settings_file = Map_AddFloat(settings_file, "sfx_volume", 1.f);
		settings_file = Map_AddFloat(settings_file, "music_volume", 1.f);
		settings_file = Map_AddBool(settings_file, "high_quality", false);
		settings_file = Map_AddInt(settings_file, "coins", 0);
	}
}

bool GameSetting_GetBool(const char* key) {
	if (!settings_file)
		load_settings_file();

	return Map_GetBool(settings_file, key, false);
}

int GameSetting_GetInt(const char* key) {
	if (!settings_file)
		load_settings_file();

	return Map_GetInt(settings_file, key, -1);
}

float GameSetting_GetFloat(const char* key) {
	if (!settings_file)
		load_settings_file();

	return Map_GetFloat(settings_file, key, -1.f);
}

char* GameSetting_GetString(const char* key) {
	if (!settings_file)
		load_settings_file();

	return Map_GetString(settings_file, key);
}

void GameSetting_SetBool(const char* key, bool value) {
	if (!settings_file)
		load_settings_file();

	Map_EditBool(settings_file, key, value);
}

void GameSetting_SetInt(const char* key, int value) {
	if (!settings_file)
		load_settings_file();

	Map_EditInt(settings_file, key, value);
}

void GameSetting_SetFloat(const char* key, float value) {
	if (!settings_file)
		load_settings_file();

	Map_EditFloat(settings_file, key, value);
}

void GameSetting_SetString(const char* key, const char* value) {
	if (!settings_file)
		load_settings_file();

	Map_EditString(settings_file, key, value);
}

void GameSetting_Save(void) {
	if (!settings_file)
		return;

	Map_Save("user.data", settings_file);
	Map_Free(settings_file);
	settings_file = NULL;
}
