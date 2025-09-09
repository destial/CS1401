#pragma once
#ifndef GAME_H
#define GAME_H
#include "cprocessing.h"

typedef struct Wall {
	CP_Vector pos;
	CP_Vector scale;
	CP_Vector normal;
	CP_Vector left;
	float angle;

} Wall;

void game_init(void);
void game_update(void);
void game_exit(void);
void update_wall(Wall* wall);

#endif