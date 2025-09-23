/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 23 September 2025
@course CSD1401
@brief Header file for debug and level editor UI
*/

#pragma once
#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H
#include "cprocessing.h"
#include "game.h"

CP_BOOL check_on_quad(Wall* wall);

CP_BOOL check_on_circle(CP_Vector pos, float diameter);

void update_debug_walls(Wall* walls, int size);

void update_debug_holes(Hole* holes, int size);

void update_debug_golfs(GolfBall* golfs, int size);

void update_debug_utils(void);

void render_debug_utils(void);

#endif