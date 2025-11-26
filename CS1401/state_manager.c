/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 3 November 2025
@course CSD1401
@brief Definition file for controlling the game states
*/


#include "state_manager.h"
#include "cprocessing.h"
#include "splash_screen.h"
#include "main_menu.h"
#include "game.h"
#include "end_state.h"
#include "pokemon.h"

static GameState current_gamestate = STATE_SPLASH;
static GameState next_gamestate = STATE_SPLASH;
static float state_transition = -1.f;

void GameState_SetNext(GameState state) {
	if (state_transition < 0.f) {
		next_gamestate = state;
		state_transition = 0.f;
	}
}

bool GameState_IsTransitioning(void) {
	return state_transition >= 0.f;
}

void GameState_GoNext(void) {
	switch (next_gamestate) {
	case STATE_SPLASH:
		CP_Engine_SetNextGameStateForced(splash_screen_init, splash_screen_update, splash_screen_exit);
		break;
	case STATE_MAINMENU:
		CP_Engine_SetNextGameStateForced(main_menu_init, main_menu_update, main_menu_exit);
		break;
	case STATE_GAME:
		if (current_gamestate == STATE_GAME) {
			Game_SetResetting(true);
		}
		CP_Engine_SetNextGameStateForced(game_init, game_update, game_exit);
		break;
	case STATE_EXIT:
		CP_Engine_SetNextGameStateForced(end_state_init, end_state_update, end_state_exit);
		break;
	case STATE_POKEMON:
		CP_Engine_SetNextGameStateForced(pokemon_init, pokemon_update, pokemon_end);
		break;
	}
	
	state_transition = -1.f;
	current_gamestate = next_gamestate;
}

void GameState_Update(void) {
	if (GameState_IsTransitioning()) {
		state_transition += CP_System_GetDt();

		CP_Settings_Fill(Get_ColorAlpha(COLOR_BLACK, (int)(state_transition * 255.f)));
		CP_Settings_Stroke(Get_ColorAlpha(COLOR_BLACK, (int)(state_transition * 255.f)));
		CP_Settings_RectMode(CP_POSITION_CORNER);
		CP_Graphics_DrawRect(0.f, 0.f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());

		if (state_transition >= 1.f) {
			GameState_GoNext();
		}
	}
}

void GameState_SetCurrent(GameState state) {
	current_gamestate = state;
}

GameState GameState_GetNext(void) {
	return next_gamestate;
}

GameState GameState_GetCurrent(void) {
	return current_gamestate;
}