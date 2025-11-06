#include "cprocessing.h"
#include "state_manager.h"
#include "particle_manager.h"
#ifdef _DEBUG
#include <windows.h>
#include <stdio.h>
#include "utils.h"
#include "data_manager.h"
#include "level_manager.h"

static void run_data_tests(void) {

	Map* map = Map_Load("test.txt");
	if (map) {
		Map_EditString(map, "a", "test");
	}
	else {
		map = Map_AddString(map, "a", "test");
	}
	Map_EditFloat(map, "b", 1.67f);
	Map_EditInt(map, "c", 2);
	Map_EditVector(map, "v", CP_Vector_Set(6.f, 7.f));

	CP_BOOL passed = TRUE;
	char* f_test = Map_GetString(map, "a");
	if (strcmp(f_test, "test")) {
		printf("Failed test 1!\n");
		passed = FALSE;
	}
	float s_test = Map_GetFloat(map, "b", -1.f);
	if (s_test != 1.67f) {
		printf("Failed test 2!\n");
		passed = FALSE;
	}
	int t_test = Map_GetInt(map, "c", -1);
	if (t_test != 2) {
		printf("Failed test 3!\n");
		passed = FALSE;
	}
	CP_Vector v_test = Map_GetVector(map, "v");
	if (v_test.x != 6.f && v_test.y != 7.f) {
		printf("Failed test 4!\n");
		passed = FALSE;
	}

	Map_EditString(map, "a", "test2");
	Map_EditFloat(map, "b", 3.21f);
	Map_EditInt(map, "c", 5);

	f_test = Map_GetString(map, "a");
	if (strcmp(f_test, "test2")) {
		printf("Failed test 5!\n");
		passed = FALSE;
	}
	s_test = Map_GetFloat(map, "b", -1.f);
	if (s_test != 3.21f) {
		printf("Failed test 6!\n");
		passed = FALSE;
	}
	t_test = Map_GetInt(map, "c", -1);
	if (t_test != 5) {
		printf("Failed test 7!\n");
		passed = FALSE;
	}

	Map_EditFloat(map, "d", 2.53f);
	s_test = Map_GetFloat(map, "d", -1.f);
	if (s_test != 2.53f) {
		printf("Failed test 8!\n");
		passed = FALSE;
	}

	map = Map_Remove(map, "d");
	s_test = Map_GetFloat(map, "d", -1.f);
	if (s_test != -1.f) {
		fprintf(stderr, "Failed test 9!\n");
		passed = FALSE;
	}

	map = Map_Remove(map, "a");
	f_test = Map_GetString(map, "a");
	if (f_test) {
		printf("Failed test 10!\n");
		passed = FALSE;
	}

	v_test = Map_GetVector(map, "v");
	if (v_test.x != 6.f && v_test.y != 7.f) {
		printf("Failed test 11!\n");
		passed = FALSE;
	}

	if (!Map_Save("test.txt", map)) {
		printf("Failed test 12!\n");
		passed = FALSE;
	}

	Map_Free(map);
	map = NULL;
	if (Map_GetString(map, "b")) {
		printf("Failed test 13!\n");
	}

	if (passed)
		printf("Passed all data tests!\n");
}
#endif

int main(void)
{
#ifdef _DEBUG
	if (AllocConsole()) {
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONERR$", "w", stderr);
		run_data_tests();
	}
#endif
	Set_Next_State(STATE_SPLASH);
	CP_Engine_Run(0);
#ifdef _DEBUG
	FreeConsole();
#endif

	return 0;
}