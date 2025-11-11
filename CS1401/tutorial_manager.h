/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 November 2025
@course CSD1401
@brief Declaration file for controlling the tutorial stages
*/

#pragma once
#ifndef TUTORIAL_H
#define TUTORIAL_H
#include "utils.h"

typedef enum TutorialPhaseEnum {
	TUT_NO_PHASE = -1,
	TUT_PHASE_DRAG_BALL = 0,
	TUT_PHASE_ITEM,
	TUT_PHASE_TERRAIN,
	//TUT_PHASE_PORTAL,
	TUT_PHASE_LAST,
} TutorialPhase;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load the tutorial instructions if this level has one, hard-coded.
	   Always called AFTER SetPhase()!!!
@param tut_mode - TRUE if this level has a tutorial mode.
*/
void Tutorial_Load(bool tut_mode);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Set the phase of the tutorial
@param phase - The next phase of the tutorial
*/
void Tutorial_SetPhase(TutorialPhase phase);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the current tutorial phase
@return The current tutorial phase
*/
TutorialPhase Tutorial_GetPhase(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the current tutorial mode
@return The current tutorial mode
*/
bool Tutorial_GetMode(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Update the tutorial to the next phase on click
*/
void Tutorial_NextPhase(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Tutorial update function inside the game to process inputs
*/
void Tutorial_Update(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Render the tutorial to the screen
*/
void Tutorial_Render(void);

#endif