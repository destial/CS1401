#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H
#include "cprocessing.h"

CP_Vector world_to_screen(float x, float y, float world_width, float world_height);
CP_Vector screen_to_world(float x, float y, float world_width, float world_height);
CP_Vector CP_Vector_Project(CP_Vector a, CP_Vector b);
CP_Vector CP_Vector_Rotate(CP_Vector vect, float radians);
CP_Vector CP_Vector_Max(CP_Vector vect, float length);

#endif