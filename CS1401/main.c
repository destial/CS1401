#include "cprocessing.h"
#include "splash_screen.h"
#ifdef _DEBUG
#include <windows.h>
#include <stdio.h>
#include "utils.h"
#include "data_manager.h"
#include "level_manager.h"

static void run_data_tests(void) {
	Map* map = load_map("test.txt");
	map = add_entry(map, "a", "test");
	map = add_entry_float(map, "b", 1.67f);
	map = add_entry_int(map, "c", 2);
	map = add_entry_vector(map, "v", CP_Vector_Set(6.f, 7.f));

	char* f_test = get_value(map, "a");
	if (strcmp(f_test, "test")) {
		fprintf(stderr, "Failed test 1!\n");
	}
	float s_test = get_float(map, "b", -1.f);
	if (s_test != 1.67f) {
		fprintf(stderr, "Failed test 2!\n");
	}
	int t_test = get_int(map, "c", -1);
	if (t_test != 2) {
		fprintf(stderr, "Failed test 3!\n");
	}
	CP_Vector v_test = get_vector(map, "v");
	if (v_test.x != 6.f && v_test.y != 7.f) {
		fprintf(stderr, "Failed test 4!\n");
	}

	edit_entry(map, "a", "test2");
	edit_entry_float(map, "b", 3.21f);
	edit_entry_int(map, "c", 5);

	f_test = get_value(map, "a");
	if (strcmp(f_test, "test2")) {
		fprintf(stderr, "Failed test 5!\n");
	}
	s_test = get_float(map, "b", -1.f);
	if (s_test != 3.21f) {
		fprintf(stderr, "Failed test 6!\n");
	}
	t_test = get_int(map, "c", -1);
	if (t_test != 5) {
		fprintf(stderr, "Failed test 7!\n");
	}

	edit_entry_float(map, "d", 2.53f);
	s_test = get_float(map, "d", -1.f);
	if (s_test != 2.53f) {
		fprintf(stderr, "Failed test 8!\n");
	}

	map = remove_entry(map, "d");
	s_test = get_float(map, "d", -1.f);
	if (s_test != -1.f) {
		fprintf(stderr, "Failed test 9!\n");
	}

	map = remove_entry(map, "a");
	f_test = get_value(map, "a");
	if (f_test) {
		fprintf(stderr, "Failed test 10!\n");
	}

	v_test = get_vector(map, "v");
	if (v_test.x != 6.f && v_test.y != 7.f) {
		fprintf(stderr, "Failed test 11!\n");
	}

	if (!save_map("test.txt", map)) {
		fprintf(stderr, "Failed test 12!\n");
	}

	free_map(map);
	map = NULL;
	if (get_value(map, "b")) {
		fprintf(stderr, "Failed test 13!\n");
	}
	fprintf(stdout, "Passed all data tests!\n");
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
	CP_Engine_SetNextGameState(splash_screen_init, splash_screen_update, splash_screen_exit);
	CP_Engine_Run(0);
#ifdef _DEBUG
	FreeConsole();
#endif

	return 0;
}