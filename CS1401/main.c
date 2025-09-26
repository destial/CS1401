#include "cprocessing.h"
#include "splash_screen.h"

int main(void)
{
	CP_Engine_SetNextGameState(splash_screen_init, splash_screen_update, splash_screen_exit);
	int show_console = 0;
#ifdef _DEBUG
	show_console = 1;
#endif
	CP_Engine_Run(show_console);
	return 0;
}