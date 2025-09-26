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
@brief Define asset file paths here
*/
#define WALL_LEFT_ASSET "Assets/Images/wall_left.png"
#define WALL_RIGHT_ASSET "Assets/Images/wall_right.png"
#define GOLF_BALL_ASSET "Assets/Images/golf_ball.png"
#define GOLF_HIT_WALL_ASSET "Assets/Audio/golf_hit_wall.wav"
#define VICTORY_ASSET "Assets/Audio/victory.wav"
#define GAME_BACKGROUND_ASSET "Assets/Images/game_background.png"
#define ESL_FONT_ASSET "Assets/Fonts/esl.ttf"
#define ARIAL_FONT_ASSET "Assets/Fonts/arial.ttf"

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