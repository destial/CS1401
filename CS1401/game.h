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

void game_init(void);
void game_update(void);
void game_exit(void);

void update_wall(Wall* wall);
char check_collision(Wall* wall, GolfBall* ball);
void collision_response(Wall* wall, GolfBall* ball, CP_Vector normal);
void debug_draw(void);

void save_level(void);
void load(void);

void set_game_level(char level);
char get_game_level(void);

#endif