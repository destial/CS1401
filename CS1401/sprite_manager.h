/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 23 September 2025
@course CSD1401
@brief Header file for implementing a sprite manager that will handle graphic animations
*/

#pragma once
#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H
#include "cprocessing.h"

typedef struct SpriteSheetStruct {
	CP_Image root_image;
	int img_h, img_w, sub_h, sub_w;
	int rows, columns, current_row, current_column;
	float duration, elapsedDuration;
} SpriteSheet;

/*!
@brief Draw a sprite to the screen
@param
spritesheet - The spritesheet
x - Screen x coordinates
y - Screen y coordinates
w - Width of the sprite in screen space
h - Height of the sprite in screen space
alpha - Transparency of sprite
*/
void DrawSprite(SpriteSheet* spritesheet, float x, float y, float w, float h, int alpha);

/*!
@brief Draw a sprite to the screen
@param
spritesheet - The spritesheet
x - Screen x coordinates
y - Screen y coordinates
w - Width of the sprite in screen space
h - Height of the sprite in screen space
alpha - Transparency of sprite
degrees - Rotation of sprite
*/
void DrawSpriteAdvanced(SpriteSheet* spritesheet, float x, float y, float w, float h, int alpha, float degrees);

#endif