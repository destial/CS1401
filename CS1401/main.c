#include "cprocessing.h"
#include "splash_screen.h"

int main(void)
{
	CP_Engine_SetNextGameState(splash_screen_init, splash_screen_update, splash_screen_exit);
	CP_BOOL show_console = FALSE;
#ifdef _DEBUG
	show_console = TRUE;
#endif
	CP_Engine_Run(show_console);
	return 0;
}