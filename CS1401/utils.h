/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 September 2025
@course CSD1401
@brief Header file for implementing utility functions like world to screen coordinates
	   and extra vector functions and other memory optimizations
*/

#pragma once
#ifndef UTILS_H
#define UTILS_H
#include "cprocessing.h"

#define min_max(v, mn, mx) min(mx, max(mn, v))
#define FREE_MEMORY(ptr) if((ptr)) { gc_free((ptr)); (ptr) = NULL; }

typedef unsigned char bool;
#define true (unsigned char)1
#define false (unsigned char)0

typedef enum ColorCodeEnum {
	COLOR_BLACK = 0,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_LIGHT_BLUE,
	COLOR_BLUE,
	COLOR_PURPLE,
	COLOR_VIOLET,
	COLOR_ORANGE,
	COLOR_YELLOW,
	COLOR_CYAN,
	COLOR_PINK,
	COLOR_WHITE,
	COLOR_CLEAR,
	COLOR_GRAY,
	COLOR_CARMINE,
	LAST_COLOR
} ColorCode;

typedef struct ObjectArrayStruct {
	void* arr;
	int length;
} ObjectArray;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Converts world coordinates to screen coordinates
@param x - World Point X
@param y - World Point Y
@return A vector in screen coordinates
*/
CP_Vector World_To_Screen(float x, float y, float world_width, float world_height);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Converts screen coordinates to world coordinates
@param x - Screen Point X
@param y - Screen Point Y
@return A vector in world coordinates
*/
CP_Vector Screen_To_World(float x, float y, float world_width, float world_height);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector math for projection
@param a - The base vector of projection
@param b - The vector to project
@return The projection vector of b onto a
*/
CP_Vector CP_Vector_Project(CP_Vector a, CP_Vector b);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector math for rotating a vector
@param vect - The vector to rotate
@param radians - The radians to rotate by
@return The rotated vector
*/
CP_Vector CP_Vector_Rotate(CP_Vector vect, float radians);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector utility to clamp a vector's magnitude
@param vect - The vector to clamp
@param length - The maximum length permitted
@return The clamped vector
*/
CP_Vector CP_Vector_Max(CP_Vector vect, float length);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector utility to check if a vector has near-zero magnitude
@param vect - The vector to check
@return true if length < 0.000001f
*/
CP_BOOL CP_Vector_IsZero(CP_Vector vect);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector optimization because sqrt is slow
@param vect - The vector to get the length square of
@return float length squared
*/
float CP_Vector_LengthSquared(CP_Vector vect);

/*!
@brief Get a defined color
@param code - The color code
@return A defined color in memory
*/
CP_Color Get_Color(ColorCode code);

/*!
@brief Get a defined color with a varied alpha
@param code - The color code
@param alpha - The alpha level
@return A copy of the defined color with the desired alpha level
*/
CP_Color Get_ColorAlpha(ColorCode code, int alpha);

/*!
@brief The greatest common divisor between two integers (e.g. a / b, to find the simplest fraction)
@param a - The first int
@param b - The second int
*/
int gcd(int a, int b);

// Memory management. Replace your std allocs with this for proper memory debugging
void* gc_malloc(size_t size);
void* gc_calloc(size_t count, size_t size);
void* gc_realloc(void* ptr, size_t size);
void gc_free(void* ptr);
int gc_counter(void);

#endif