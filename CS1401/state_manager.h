/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 3 November 2025
@course CSD1401
@brief Declaration file for controlling the game states
*/

#pragma once
#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H
#include "utils.h"

typedef enum GameStatesEnum {
	STATE_SPLASH = 0,
	STATE_MAINMENU,
	STATE_GAME,
	STATE_EXIT,
	STATE_POKEMON
} GameState;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Set the next state to update
@param state - The next game state
*/
void GameState_SetNext(GameState state);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief To be called inside every update function at the end, renders the transition (fade to black)
*/
void GameState_Update(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief If it is currently transitioning to the next game state
@returns true if currently in transitioning
*/
bool GameState_IsTransitioning(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Transition to the next state
*/
void GameState_GoNext(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Set the current state
@param state - The current game state
*/
void GameState_SetCurrent(GameState state);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the next state that is going to be transitioning to in the next frame
@return The next game state
*/
GameState GameState_GetNext(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the current game state in this frame
@return The current game state
*/
GameState GameState_GetCurrent(void);

#endif