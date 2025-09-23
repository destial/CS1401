/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 22 September 2025
@course CSD1401
@brief Header file for implementing an asset manager that will handle loading and freeing of assets
*/

#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H
#include "cprocessing.h"
#include "sprite_manager.h"

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the image at this file path
@param
filepath - The image filepath
*/
CP_Image* get_image(const char* filepath);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the font at this file path
@param
filepath - The font filepath
*/
CP_Font* get_font(const char* filepath);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu
@brief Get the audio from this file path
@param
filepath - The audio filepath
*/
CP_Sound* get_audio(const char* filepath);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu
@brief Get the spritesheet from this file path
@param
filepath - The spritesheet filepath
*/
SpriteSheet* get_sprite(const char* filepath, int rows, int columns, float duration);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free up all the loaded assets from the asset map. Do not call this in main()!
*/
void free_assets(void);

#endif