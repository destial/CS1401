#include "cprocessing.h"
#include "splash_screen.h"
#include "asset_manager.h"

int main(void)
{
	CP_Engine_SetNextGameState(splash_screen_init, splash_screen_update, splash_screen_exit);
	CP_Engine_Run(0);
	return 0;
}