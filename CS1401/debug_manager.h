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

/*!
* @brief Check if mouse is on quad
*/
CP_BOOL check_on_quad(Wall* wall);

/*!
* @brief Check if mouse is on circle
*/
CP_BOOL check_on_circle(CP_Vector pos, float diameter);

/*!
* @brief Process wall editor
*/
void update_debug_walls(Wall* walls, int size);

/*!
* @brief Process hole editor
*/
void update_debug_holes(Hole* holes, int size);

/*!
* @brief Process ball editor
*/
void update_debug_golfs(GolfBall* golfs, int size);

/*!
* @brief Final process editors
*/
void update_debug_utils(void);

/*!
* @brief Render editor overlays
*/
void render_debug_utils(void);

#endif