/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 22 September 2025
@course CSD1401
@brief Definition file for unloading the game's allocation
*/

#include "end_state.h"
#include "asset_manager.h"

void end_state_init(void) {}

void end_state_update(void) {
	CP_Engine_Terminate();
}

void end_state_end(void) {
	free_assets();
}