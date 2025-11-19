#include "cprocessing.h"
#include "state_manager.h"
#include "particle_manager.h"
#ifdef _DEBUG
#include <windows.h>
#include <stdio.h>
#include "utils.h"
#include "data_manager.h"
#include "level_manager.h"

#ifndef ASSERT_TEST
#define ASSERT_TEST(test, err) if(test) {printf(err);exit(1);}
#endif

static void run_data_tests(void) {
	Map map = Map_Load("test.txt");
	if (map) {
		Map_EditString(map, "a", "test");
	}
	else {
		map = Map_AddString(map, "a", "test");
	}
	Map_EditFloat(map, "b", 1.67f);
	Map_EditInt(map, "c", 2);
	Map_EditVector(map, "v", CP_Vector_Set(6.f, 7.f));

	char* f_test = Map_GetString(map, "a");
	ASSERT_TEST(strcmp(f_test, "test"), "Failed test 1!");

	float s_test = Map_GetFloat(map, "b", -1.f);
	ASSERT_TEST(s_test != 1.67f, "Failed test 2!");

	int t_test = Map_GetInt(map, "c", -1);
	ASSERT_TEST(t_test != 2, "Failed test 3!");

	CP_Vector v_test = Map_GetVector(map, "v");
	ASSERT_TEST(v_test.x != 6.f && v_test.y != 7.f, "Failed test 4!");

	Map_EditString(map, "a", "test2");
	Map_EditFloat(map, "b", 3.21f);
	Map_EditInt(map, "c", 5);

	f_test = Map_GetString(map, "a");
	ASSERT_TEST(strcmp(f_test, "test2"), "Failed test 5!");

	s_test = Map_GetFloat(map, "b", -1.f);
	ASSERT_TEST(s_test != 3.21f, "Failed test 6!");

	t_test = Map_GetInt(map, "c", -1);
	ASSERT_TEST(t_test != 5, "Failed test 7!");

	Map_EditFloat(map, "d", 2.53f);
	s_test = Map_GetFloat(map, "d", -1.f);
	ASSERT_TEST(s_test != 2.53f, "Failed test 8!");

	map = Map_Remove(map, "d");
	s_test = Map_GetFloat(map, "d", -1.f);
	ASSERT_TEST(s_test != -1.f, "Failed test 9!");

	map = Map_Remove(map, "a");
	f_test = Map_GetString(map, "a");
	ASSERT_TEST(f_test, "Failed test 10!");

	v_test = Map_GetVector(map, "v");
	ASSERT_TEST(v_test.x != 6.f && v_test.y != 7.f, "Failed test 11!");

	ASSERT_TEST(!Map_Save("test.txt", map), "Failed test 12!");

	Map_Free(map);
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
	GameState_SetNext(STATE_SPLASH);
	CP_Engine_Run(0);
#ifdef _DEBUG
	FreeConsole();
#endif

	return 0;
}