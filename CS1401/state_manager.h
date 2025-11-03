/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 3 November 2025
@course CSD1401
@brief Declaration file for controlling the game states
*/

#pragma once
#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

typedef enum GameStatesEnum {
	STATE_SPLASH = 0,
	STATE_MAINMENU,
	STATE_GAME,
	STATE_EXIT
} GameState;

/*!
@brief Set the next state to update
@param state - The next game state
*/
void Set_Next_State(GameState state);

/*!
@brief Set the current state
@param state - The current game state
*/
void Set_Current_State(GameState state);

/*!
@brief Get the next state that is going to be transitioning to in the next frame
@return The next game state
*/
GameState Get_Next_State(void);

/*!
@brief Get the current game state in this frame
@return The current game state
*/
GameState Get_Current_State(void);

#endif