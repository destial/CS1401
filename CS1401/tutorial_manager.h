#pragma once
#ifndef TUTORIAL_H
#define TUTORIAL_H
#include "utils.h"

typedef enum TutorialPhaseEnum {
	TUT_NO_PHASE = -1,
	TUT_PHASE_DRAG_BALL = 0,
	TUT_PHASE_ITEM,
	//TUT_PHASE_TERRAIN,
	//TUT_PHASE_PORTAL,
	TUT_PHASE_LAST,
} TutorialPhase;

void Tutorial_Load(bool tut_mode);
void Tutorial_SetPhase(TutorialPhase phase);
TutorialPhase Tutorial_GetPhase(void);
void Tutorial_NextPhase(void);
void Tutorial_Update(void);
void Tutorial_Render(void);
bool Tutorial_GetMode(void);

#endif