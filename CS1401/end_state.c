#include "end_state.h"
#include "asset_manager.h"

void end_state_init(void) {}

void end_state_update(void) {
	CP_Engine_Terminate();
}

void end_state_end(void) {
	free_assets();
}