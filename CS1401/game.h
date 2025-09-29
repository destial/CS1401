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

typedef struct Wall {
	CP_Vector pos;
	CP_Vector scale;
	CP_Vector normal;
	CP_Vector right;
	CP_Color color;
	CP_Image* image;
	float angle;
	float angle_radians;
	float rotation_rate;
	char destroyed;
} Wall;

typedef struct GolfBall {
	CP_Vector pos;
	CP_Vector prev_pos;
	CP_Vector vel;
	float diameter;
	float angle;
	char dragging;
	char halt;
	char scored;
} GolfBall;

typedef struct Hole {
	CP_Vector pos;
	float diameter;
} Hole;

/*!
@brief Game scene init
*/
void game_init(void);

/*!
@brief Game scene processing
*/
void game_update(void);

/*!
@brief Game physics processing
*/
void game_physics_update(float dt);

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
void update_wall(Wall* wall, float dt);

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
@brief Load the current level from text file under "Assets/Levels/level_{current_level+1}.lvl"
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

/*!
@brief Get the mouse position in world space
*/
CP_Vector get_mouse_world(void);

/*!
@brief Get the world width and height
*/
CP_Vector get_world_limits(void);

/*
@brief Get the world width and height ratios according to the display width and height (e.g. displayWidth / worldWidth)
*/
CP_Vector get_world_ratios(void);

Wall* add_wall(CP_Vector pos);

void remove_wall(Wall* wall);

#endif