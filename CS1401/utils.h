/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 September 2025
@course CSD1401
@brief Header file for implementing basic vector math that is not included in CProcessing, but useful for conversion into game coordinates
*/

#pragma once
#ifndef UTILS_H
#define UTILS_H
#include "cprocessing.h"

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Converts world coordinates to screen coordinates
@param 
x = World Point X, y = World Point Y
@return
A vector of the screen coordinates
*/
CP_Vector world_to_screen(float x, float y, float world_width, float world_height);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Converts screen coordinates to world coordinates
@param
x = Screen Point X, y = Screen Point Y
@return
A vector of the world coordinates
*/
CP_Vector screen_to_world(float x, float y, float world_width, float world_height);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector math for projection
@param
Projection of b onto a
@return
The projection vector of b onto a
*/
CP_Vector CP_Vector_Project(CP_Vector a, CP_Vector b);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector math for rotating a vector
@param
The vector to rotate, the radians to rotate
@return
The rotated vector
*/
CP_Vector CP_Vector_Rotate(CP_Vector vect, float radians);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector utility to clamp a vector's magnitude
@param
The vector to clamp, the maximum length permitted
@return
The clamped vector
*/
CP_Vector CP_Vector_Max(CP_Vector vect, float length);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector utility to check if a vector has near-zero magnitude
@param
The vector to check
@return
true if length < 0.000001f
*/
char CP_Vector_IsZero(CP_Vector vect);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Additional vector optimization because sqrt is slow
@param
The vector to get the length square of
@return
float length squared
*/
float CP_Vector_LengthSquared(CP_Vector vect);

#endif