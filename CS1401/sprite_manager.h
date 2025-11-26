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

typedef struct SpriteSheetStruct* SpriteSheet;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Create a spritesheet from the given image, rows, columns and duration
@param image - The root image
@param rows - How many rows in this spritesheet
@param columns - How many columns in this spritesheet
@param duration - How long is the total spritesheet animation (in seconds)
@return The created spritesheet
*/
SpriteSheet Sprite_Create(CP_Image image, int rows, int columns, float duration);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Draw a sprite to the screen
@param spritesheet - The spritesheet
@param x - Screen x coordinates
@param y - Screen y coordinates
@param w - Width of the sprite in screen space
@param h - Height of the sprite in screen space
@param alpha - Transparency of sprite
*/
void Sprite_Draw(SpriteSheet spritesheet, float x, float y, float w, float h, int alpha);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Draw a sprite to the screen
@param spritesheet - The spritesheet
@param x - Screen x coordinates
@param y - Screen y coordinates
@param w - Width of the sprite in screen space
@param h - Height of the sprite in screen space
@param alpha - Transparency of sprite
@param degrees - Rotation of sprite
*/
void Sprite_DrawAdvanced(SpriteSheet spritesheet, float x, float y, float w, float h, int alpha, float degrees);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Draw a sprite frame to the screen
@param spritesheet - The spritesheet
@param row - The frame row
@param col - The frame column
@param x - Screen x coordinates
@param y - Screen y coordinates
@param w - Width of the sprite in screen space
@param h - Height of the sprite in screen space
@param alpha - Transparency of sprite
@param degrees - Rotation of sprite
*/
void Sprite_DrawFrame(SpriteSheet spritesheet, int row, int col, float x, float y, float w, float h, int alpha, float degrees);

void Sprite_DrawCropped(SpriteSheet spritesheet, int row, int col, float x, float y, float w, float h, float y_bot);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the memory allocated for this spritesheeet
@param spritesheet - The spritesheet to free
*/
void Sprite_Free(SpriteSheet spritesheet);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the subdivided width of this spritesheet
@param spritesheet - The spritesheet to get
@return The width of each frame
*/
int Sprite_GetWidth(SpriteSheet spritesheet);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the subdivided height of this spritesheet
@param spritesheet - The spritesheet to get
@return The height of each frame
*/
int Sprite_GetHeight(SpriteSheet spritesheet);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Reset the frame update for this sheet.
@param spritesheet - The spritesheet to update
*/
void Sprite_ResetFrame(SpriteSheet sheet);

#endif