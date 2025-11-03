/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 September 2025
@course CSD1401
@brief Definition file for implementing basic vector math that is not included in CProcessing, but useful for conversion into game coordinates
*/

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "math.h"

static bool colors_init = false;
static CP_Color colors[LAST_COLOR];

CP_Vector World_To_Screen(float x, float y, float world_width, float world_height) {
	CP_Vector a = { 
		x * (CP_System_GetWindowWidth() / world_width),
		CP_System_GetWindowHeight() - (y * (CP_System_GetWindowHeight() / world_height))
	};
	return a;
}

CP_Vector Screen_To_World(float x, float y, float world_width, float world_height) {
	CP_Vector a = { 
		x * (world_width / CP_System_GetWindowWidth()),
		world_height - (y * (world_height / CP_System_GetWindowHeight())) 
	};
	return a;
}

CP_Vector CP_Vector_Project(CP_Vector a, CP_Vector b) {
	CP_Vector normalized = CP_Vector_Normalize(a);
	float dot = CP_Vector_DotProduct(normalized, b);
	return CP_Vector_Scale(a, dot);
}

CP_Vector CP_Vector_Rotate(CP_Vector vect, float radians) {
	CP_Vector rotated = {
		vect.x * cosf(radians) - vect.y * sinf(radians),
		vect.x * sinf(radians) + vect.y * cosf(radians)
	};
	return rotated;
}

CP_Vector CP_Vector_Max(CP_Vector vect, float length) {
	float current = CP_Vector_Length(vect);
	if (current < length) {
		return vect;
	}
	return CP_Vector_Scale(vect, length / current);
}

CP_BOOL CP_Vector_IsZero(CP_Vector vect) {
	return CP_Vector_LengthSquared(vect) <= 0.0001f;
}

float CP_Vector_LengthSquared(CP_Vector vect) {
	return CP_Math_Square(vect.x) + CP_Math_Square(vect.y);
}

CP_Color Get_Color(ColorCode code) {
	if (!colors_init) {
		colors_init = true;
		colors[COLOR_BLACK] = CP_Color_Create(0, 0, 0, 255);
		colors[COLOR_WHITE] = CP_Color_Create(255, 255, 255, 255);
		colors[COLOR_RED] = CP_Color_Create(255, 0, 0, 255);
		colors[COLOR_GREEN] = CP_Color_Create(3, 127, 81, 255);
		colors[COLOR_LIGHT_BLUE] = CP_Color_Create(173, 216, 255, 255);
		colors[COLOR_BLUE] = CP_Color_Create(0, 0, 255, 255);
		colors[COLOR_PURPLE] = CP_Color_Create(255, 0, 255, 255);
		colors[COLOR_VIOLET] = CP_Color_Create(128, 0, 255, 255);
		colors[COLOR_CYAN] = CP_Color_Create(0, 255, 255, 255);
		colors[COLOR_ORANGE] = CP_Color_Create(255, 128, 0, 255);
		colors[COLOR_YELLOW] = CP_Color_Create(255, 255, 0, 255);
		colors[COLOR_PINK] = CP_Color_Create(255, 0, 200, 255);
		colors[COLOR_CLEAR] = CP_Color_Create(0, 0, 0, 0);
	}
	return colors[code];
}

int gcd(int a, int b) {
	// Find Minimum of a and b
	int result = min(a, b);
	while (result > 0) {
		// Check if both a and b are divisible by result
		if (a % result == 0 && b % result == 0) {
			break;
		}
		--result;
	}
	// return gcd of a nd b
	return result;
}

static int gccounter = 0;
void* gc_malloc(size_t size) {
	++gccounter;
	printf("m: %d, ", gccounter);
	return malloc(size);
}

void* gc_calloc(size_t count, size_t size) {
	++gccounter;
	printf("c: %d, ", gccounter);
	return calloc(count, size);
}

void* gc_realloc(void* ptr, size_t size) {
	if (ptr) {
		//++counter;
		printf("r: %d, ", gccounter);
		return realloc(ptr, size);
	}
	return NULL;
}

void gc_free(void* ptr) {
	if (ptr) {
		--gccounter;
		printf("freed: %d\n", gccounter);
		free(ptr);
	}
}

int gc_counter(void) {
	return gccounter;
}
