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
#define BORDER_WALL_ASSET "Assets/Images/stone.png"
#define GOLF_BALL_ASSET "Assets/Images/golf_ball.png"
#define GOLF_BALL_GOLD_ASSET "Assets/Images/golf_ball_gold.png"
#define GOLF_BALL_POOL_ASSET "Assets/Images/golf_ball_pool.png"
#define GAME_BACKGROUND_ASSET "Assets/Images/Game Background.png"
#define SETTING_BACKGROUND_ASSET "Assets/Images/setting_background.png"
#define BUTTON_YELLOW_ASSET "Assets/Images/button_yellow.png"
#define SMALL_BUTTON_YELLOW_ASSET "Assets/Images/small_button_yellow.png"
#define BUTTON_BLUE_ASSET "Assets/Images/button_blue.png"
#define BUTTON_GREY_ASSET "Assets/Images/button_grey.png"
#define SMALL_BUTTON_GREY_ASSET "Assets/Images/small_button_grey.png"
#define BUTTON_GREEN_ASSET "Assets/Images/button_green.png"
#define SPLASH_SCREEN_ASSET "Assets/Images/splash_screen.png"
#define TITLE_SPRITE_ASSET "Assets/Images/title_sprite.png"
#define TEAM_CREDITS_ASSET "Assets/Images/team_credits.png"
#define BOOST_ITEM_ASSET "Assets/Images/boost_item.png"
#define BIG_ITEM_ASSET "Assets/Images/big_item.png"
#define SMALL_ITEM_ASSET "Assets/Images/small_item.png"
#define REVERSE_ITEM_ASSET "Assets/Images/reverse_item.png"
#define COIN_ITEM_SPRITE_ASSET "Assets/Images/coin_sprite.png"
#define BLUE_PORTAL_SPRITE_ASSET "Assets/Images/blue_portal_sprite.png"
#define ORANGE_PORTAL_SPRITE_ASSET "Assets/Images/orange_portal_sprite.png"
#define FLAME_PARTICLE_SPRITE_ASSET "Assets/Images/flame_particle_sheet.png"
#define SPARKLE_SPRITE_ASSET "Assets/Images/sparkle_sheet.png"
#define RAINBOW_SPARKLE_SPRITE_ASSET "Assets/Images/rainbow_sparkle_sheet.png"
#define HALO_SPRITE_ASSET "Assets/Images/halo_sheet.png"
#define PUTT_ARROW_ASSET "Assets/Images/putt_arrow.png"

//Scoreboard
#define RED_CROSS "Assets/Images/icon_outline_cross.png"
#define DARK_PLAY_BUTTON "Assets/Images/icon_play_dark.png" 
#define RED_ARROW "Assets/Images/arrow_basic_w.png"
#define DARK_REPLAY_BUTTON "Assets/Images/icon_repeat_dark.png"
#define GREY_STAR "Assets/Images/star_outline.png"
#define YELLOW_STAR "Assets/Images/star.png"
#define SCOREBOARD_BG "Assets/Images/button_square_flat.png"
#define STROKE_BG "Assets/Images/button_rectangle_depth_line.png"
#define SCOREBOARD_HEADER "Assets/Images/button_rectangle_line.png"
#define BANNER "Assets/Images/banner_classic_curtain.png"
#define HANGING_BANNER "Assets/Images/banner_hanging.png"
#define SB_BUTTON_BLUE "Assets/Images/button_square_flat.png"
#define SB_BUTTON_YELLOW "Assets/Images/yellow_button_square_flat.png"
#define SB_BUTTON_GREEN "Assets/Images/green_button_square_flat.png"
#define SB_BUTTON_GREY "Assets/Images/grey_button_square_flat.png"
// Sounds / Music / Audio

#define GOLF_HIT_WALL_ASSET "Assets/Audio/golf_hit_wall.wav"
#define GOLF_HIT_STROKE_ASSET "Assets/Audio/Golf_hit.wav"
#define VICTORY_ASSET "Assets/Audio/victory.wav"
#define BACKGROUND_MUSIC_ASSET "Assets/Audio/background_music.wav"

// Fonts

#define ESL_FONT_ASSET "Assets/Fonts/esl.ttf"
#define ARIAL_FONT_ASSET "Assets/Fonts/arial.ttf"
//New pixel font
#define CARTER_FONT_ASSET "Assets/Fonts/Jersey25-Regular.ttf"

// Level file

#define LEVEL_FILEPATH "Assets/Levels/level_%d.lvl"

// Tutorial phase gifs
#define TUTORIAL_PHASE_1_ASSET "Assets/Images/tut_phase1.png"
#define TUTORIAL_PHASE_2_ASSET "Assets/Images/tut_phase2.png"
#define TUTORIAL_PHASE_3_ASSET "Assets/Images/tut_phase3.png"

typedef enum AssetBlendEnum {
	BLEND_ADD,
	BLEND_ADD_ALPHA,
	BLEND_MULTIPLY,
	BLEND_SET
} AssetBlendMode;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Preload some assets to reduce loading time
*/
void Asset_Preload(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the image at this file path
@param filepath - The image filepath
*/
CP_Image Asset_GetImage(const char* filepath);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Blend the image at this filepath with the given color parameter
@param filepath - The image filepath
@param color - The color to blend each pixel by
@param method - The blend mode to use for each pixel
*/
CP_Image Asset_GetBlendedImage(const char* filepath, CP_Color color, AssetBlendMode mode);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Get the font at this file path
@param filepath - The font filepath
*/
CP_Font Asset_GetFont(const char* filepath);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu
@brief Get the audio from this file path
@param filepath - The audio filepath
*/
CP_Sound Asset_GetAudio(const char* filepath);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu
@brief Get the spritesheet from this file path
@param filepath - The spritesheet filepath
@param rows - The number of rows in the spritesheet
@param columns - The number of columns in the spritesheet
@param duration - The duration of the entire animation in seconds
*/
SpriteSheet Asset_GetSprite(const char* filepath, int rows, int columns, float duration);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu
@brief Reset all sprite frame updates. Called at the start of Render
*/
void Asset_SpriteStartFrame(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free up all the loaded assets from the asset map. Do not call this in main()!
*/
void Assets_Free(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free up a specific node from the asset map according to its stored pointer.
@param value - The stored pointer to the image/audio/sprite/font asset
*/
void Asset_Free_Node(void* value);

#endif