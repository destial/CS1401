#pragma once
#ifndef GAME_H
#define GAME_H
#include "cprocessing.h"

typedef struct Wall Wall;

void game_init(void);
void game_update(void);
void game_exit(void);
void update_wall(Wall* wall);
char check_collision(Wall* wall);
void collision_response(Wall* wall, CP_Vector normal);

#endif