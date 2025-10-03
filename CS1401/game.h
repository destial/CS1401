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
	CP_Vector pos; // wall position (world space)
	CP_Vector scale; // wall scale (world space)
	CP_Vector normal; // wall normal vector (calculated based on angle, always updated before collision testing)
	CP_Vector right; // wall right vector (calculated based on normal)
	CP_Color color; // wall color if no base image
	CP_Image image; // wall image
	float angle; // wall rotation angle in degrees
	float angle_radians; // wall angle in radians (touching this is useless)
	float rotation_rate; // wall rotation rate in degrees per second
	CP_BOOL destroyed; // if this wall is disabled
} Wall;

typedef struct GolfBall {
	CP_Vector pos; // golfball position (world space)
	CP_Vector prev_pos; // golfball position in previous physics update
	CP_Vector vel; // golfball velocity (world space)
	float diameter; // golfball diameter (world space)
	float angle; // golfball rotation angle
	CP_BOOL dragging; // player currently striking
	CP_BOOL halt; // if one ball is currently striking, halt the other ball from moving
	CP_BOOL scored; // if this ball has scored
} GolfBall;

typedef struct Hole {
	CP_Vector pos; // hole position (world space)
	float diameter; // hole diameter (world space)
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
@param wall - The wall to check
@param ball - The golf ball to check
@return TRUE if collided, FALSE if not
*/
CP_BOOL check_collision(Wall* wall, GolfBall* ball);

/*!
@brief Respond to the collision between a wall and a ball according to the wall's normal
@param wall - The wall it hit
@param ball - The golf ball that hit
@param normal - The normal of the wall to the ball
*/
void collision_response(Wall* wall, GolfBall* ball, CP_Vector normal);

/*!
@brief Save the current level data to the text file under "Assets/Levels/level_{current_level}.lvl"
*/
void save_level(void);

/*!
@brief Load the current level from text file under "Assets/Levels/level_{current_level}.lvl"
*/
void load(void);

/*!
@brief Set the current game level. To be called before load(). Does nothing if not before game init
@param level - The level to load from "Assets/Levels/level_{level}.lvl"
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

/*!
@brief Get the world width and height ratios according to the display width and height (e.g. displayWidth / worldWidth)
*/
CP_Vector get_world_ratios(void);

/*!
@brief Add a wall to the level
*/
Wall* add_wall(CP_Vector pos);

/*!
@brief Remove a wall from the level
*/
void remove_wall(Wall* wall);

#endif