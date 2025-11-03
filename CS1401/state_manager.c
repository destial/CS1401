/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 3 November 2025
@course CSD1401
@brief Definition file for controlling the game states
*/

#include "cprocessing.h"
#include "splash_screen.h"
#include "main_menu.h"
#include "game.h"
#include "end_state.h"
#include "state_manager.h"
#include "utils.h"

GameState current_gamestate = STATE_SPLASH;
GameState next_gamestate = STATE_SPLASH;

void Set_Next_State(GameState state) {
	next_gamestate = state;
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
	}
	current_gamestate = state;
}

GameState Get_Next_State(void) {
	return next_gamestate;
}

GameState Get_Current_State(void) {
	return current_gamestate;
}