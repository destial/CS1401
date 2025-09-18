/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 11 September 2025
@course CSD1401
@brief Header file for implementing the main game functions
*/

#pragma once
#ifndef GAME_H
#define GAME_H
#include "cprocessing.h"

typedef struct Wall Wall;
typedef struct GolfBall GolfBall;

typedef enum ColorCode;

/*!
@brief Game scene init
*/
void game_init(void);

/*!
@brief Game scene processing
*/
void game_update(void);

/*!
@brief Game scene rendering
*/
void game_render(void);

/*!
@brief Game scene deallocation
*/
void game_exit(void);

/*!
@brief Update wall normals and rotation
*/
void update_wall(Wall* wall);

/*!
@brief Check the collision between a wall and a golf ball
@param
wall - The wall to check, ball - The golf ball to check
@return
1 if collided, 0 if not
*/
char check_collision(Wall* wall, GolfBall* ball);

/*!
@brief Respond to the collision between a wall and a ball according to the wall's normal
@param
wall - The wall it hit, ball - The golf ball that hit, normal - The normal of the wall to the ball
*/
void collision_response(Wall* wall, GolfBall* ball, CP_Vector normal);

/*!
@brief Save the current level data to the text file under "Assets/Levels/level_{level}.lvl"
*/
void save_level(void);

/*!
@brief Load the current level from text file under "Assets/Levels/level_{current_level}.lvl"
*/
void load(void);

/*!
@brief Set the current game level. To be called before load(). Does nothing if not before game init
*/
void set_game_level(char level);

/*!
@brief Get the current game level that is being played on
*/
char get_game_level(void);

/*!
@brief Render debug information such as FPS, mouse coords, ball positions & velocity and collision markers
*/
void debug_draw(void);

#endif