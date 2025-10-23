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
#include "utils.h"

/*!
@brief Check if mouse is on quad
@param wall - The wall to check
*/
CP_BOOL check_on_quad(Wall* wall);

/*!
@brief Check if mouse is on circle
@param pos - The center position of circle in world space
@param diameter - The diameter of the circle in world space
*/
CP_BOOL check_on_circle(CP_Vector pos, float diameter);

/*!
@brief Process wall editor
@param walls - The array of walls in the current level
@param size - The number of walls in the current level
*/
void update_editor_walls(ObjectArray* walls);

/*!
@brief Process hole editor
@param holes - The array of holes in the current level
@param size - The number of holes in the current level
*/
void update_editor_holes(ObjectArray* holes);

/*!
@brief Process ball editor
@param golfs - The array of golf balls in the current level
@param size - The number of golf balls in the current level
*/
void update_editor_golfs(ObjectArray* golfs);

/*!
@brief Process items editor
@param items - The array of items in the current level
@param size - The number of items in the current level
*/
void update_editor_items(ObjectArray* items);

/*!
@brief Final process editors
*/
void update_editor_utils(void);

/*!
@brief Render editor overlays
*/
void render_editor_utils(void);

#endif