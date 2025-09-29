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

#define clamp(x, minimum, maximum) max(minimum, min(x, maximum))

typedef enum ColorCode {
	BLACK = 0,
	RED,
	GREEN,
	BLUE,
	PURPLE,
	VIOLET,
	ORANGE,
	YELLOW,
	CYAN,
	PINK,
	WHITE,
	CLEAR,
	LAST
} ColorCode;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Converts world coordinates to screen coordinates
@param x - World Point X
@param y - World Point Y
@return A vector in screen coordinates
*/
CP_Vector world_to_screen(float x, float y, float world_width, float world_height);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Converts screen coordinates to world coordinates
@param x - Screen Point X
@param y - Screen Point Y
@return A vector in world coordinates
*/
CP_Vector screen_to_world(float x, float y, float world_width, float world_height);

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
*/
CP_Color get_color(ColorCode code);

#endif