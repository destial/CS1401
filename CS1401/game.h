#pragma once
#ifndef GAME_H
#define GAME_H
#include "cprocessing.h"

typedef struct Wall Wall;
typedef struct GolfBall GolfBall;

void game_init(void);
void game_update(void);
void game_exit(void);
void update_wall(Wall* wall);
char check_collision(Wall* wall, GolfBall* ball);
void collision_response(Wall* wall, GolfBall* ball, CP_Vector normal);
void debug_draw(void);

#endif