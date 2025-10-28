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

// Define asset file paths here (e.g.)
// #define IMAGE_ASSET "Assets/Images/image.png" (png only)
// #define AUDIO_ASSET "Assets/Audio/audio.wav" (wav only)
// #define FONT_ASSET "Assets/Fonts/font.ttf" (ttf only)
// #define SPRITE_ASSET "Assets/Images/sprite.png" (same as image)

// Images / Sprites

#define WALL_LEFT_ASSET "Assets/Images/wall_left.png"
#define WALL_RIGHT_ASSET "Assets/Images/wall_right.png"
#define GOLF_BALL_ASSET "Assets/Images/golf_ball.png"
#define GAME_BACKGROUND_ASSET "Assets/Images/Game Background.png"
#define SETTING_BACKGROUND_ASSET "Assets/Images/setting_background.png"
#define BUTTON_YELLOW_ASSET "Assets/Images/button_yellow.png"
#define BUTTON_BLUE_ASSET "Assets/Images/button_blue.png"
#define BUTTON_GREY_ASSET "Assets/Images/button_grey.png"
#define LEVEL_BUTTON_ASSET "Assets/Images/level button.png"
#define SPLASH_SCREEN_ASSET "Assets/Images/splash_screen.png"
#define TITLE_SPRITE_ASSET "Assets/Images/title_sprite.png"
#define BOOST_ITEM_ASSET "Assets/Images/boost_item.png"
#define BIG_ITEM_ASSET "Assets/Images/big_item.png"
#define SMALL_ITEM_ASSET "Assets/Images/small_item.png"
#define REVERSE_ITEM_ASSET "Assets/Images/reverse_item.png"
#define BLUE_PORTAL_SPRITE_ASSET "Assets/Images/blue_portal_sprite.png"
#define ORANGE_PORTAL_SPRITE_ASSET "Assets/Images/orange_portal_sprite.png"

//Scoreboard
#define RED_CROSS "Assets/Images/icon_cross.png"
#define DARK_PLAY_BUTTON "Assets/Images/icon_play_dark.png" 
#define RED_PLAY_BUTTON "Assets/Images/arrow_basic_w.png"
#define DARK_REPLAY_BUTTON "Assets/Images/icon_repeat_dark.png"
#define GREY_STAR "Assets/Images/star_outline.png"
#define YELLOW_STAR "Assets/Images/star.png"

// Sounds / Music / Audio

#define GOLF_HIT_WALL_ASSET "Assets/Audio/golf_hit_wall.wav"
#define GOLF_HIT_STROKE_ASSET "Assets/Audio/Golf_hit.wav"
#define VICTORY_ASSET "Assets/Audio/victory.wav"
#define BACKGROUND_MUSIC_ASSET "Assets/Audio/background_music.wav"

// Fonts

#define ESL_FONT_ASSET "Assets/Fonts/esl.ttf"
#define ARIAL_FONT_ASSET "Assets/Fonts/arial.ttf"

// Level file

#define LEVEL_FILEPATH "Assets/Levels/level_%d.lvl"

typedef struct AssetStruct Asset;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the image at this file path
@param filepath - The image filepath
*/
CP_Image get_image(const char* filepath);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the font at this file path
@param filepath - The font filepath
*/
CP_Font get_font(const char* filepath);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu
@brief Get the audio from this file path
@param filepath - The audio filepath
*/
CP_Sound get_audio(const char* filepath);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu
@brief Get the spritesheet from this file path
@param filepath - The spritesheet filepath
@param rows - The number of rows in the spritesheet
@param columns - The number of columns in the spritesheet
@param duration - The duration of the entire animation in seconds
*/
SpriteSheet* get_sprite(const char* filepath, int rows, int columns, float duration);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free up all the loaded assets from the asset map. Do not call this in main()!
*/
void free_assets(void);

#endif