/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 September 2025
@course CSD1401
@brief Definition file for implementing basic vector math that is not included in CProcessing, but useful for conversion into game coordinates
*/

#include "utils.h"
#include "math.h"

CP_Vector world_to_screen(float x, float y, float world_width, float world_height) {
	CP_Vector a = { x * (CP_System_GetWindowWidth() / world_width), CP_System_GetWindowHeight() - (y * (CP_System_GetWindowHeight() / world_height)) };
	return a;
}

CP_Vector screen_to_world(float x, float y, float world_width, float world_height) {
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

char CP_Vector_IsZero(CP_Vector vect) {
	return CP_Vector_Length(vect) <= 0.000001f;
}

float CP_Vector_LengthSquared(CP_Vector vect) {
	return CP_Math_Square(vect.x) + CP_Math_Square(vect.y);
}
