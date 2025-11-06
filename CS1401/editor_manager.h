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
@brief Check if mouse is on wall
@param wall - The wall to check
*/
bool Check_On_Wall(Wall* wall);

/*!
@brief Check if mouse is on terrain
@param terrain - The terrain to check
*/
bool Check_On_Terrain(Terrain* terrain);

/*!
@brief Check if mouse is on circle
@param pos - The center position of circle in world space
@param diameter - The diameter of the circle in world space
*/
bool Check_On_Circle(CP_Vector pos, float diameter);

/*!
@brief Start process editors
*/
void Editor_Update_Start(void);

/*!
@brief Process wall editor
@param walls - The array of walls in the current level
*/
void Editor_Update_Walls(ObjectArray* walls);

/*!
@brief Process hole editor
@param holes - The array of holes in the current level
*/
void Editor_Update_Holes(ObjectArray* holes);

/*!
@brief Process ball editor
@param golfs - The array of golf balls in the current level
*/
void Editor_Update_Golfballs(ObjectArray* golfs);

/*!
@brief Process items editor
@param items - The array of items in the current level
*/
void Editor_Update_Items(ObjectArray* items);

/*!
@brief Process terrain editor
@param terrains - The array of terrains in the current level
*/
void Editor_Update_Terrains(ObjectArray* terrains);

/*!
@brief Final process editors
*/
void Editor_Update_End(void);

/*!
@brief Render editor overlays
*/
void Editor_Render(void);

#endif