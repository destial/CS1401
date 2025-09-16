#pragma once
#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H
#include "cprocessing.h"

typedef struct Map {
    char* key;
    char* value;
    struct Map* next;
} Map;

typedef struct Level {
    float world_width, world_height;
    CP_Vector golf_pos_1, golf_pos_2;
    CP_Vector hole_1, hole_2;
    int walls_size;
    float* walls; // [0 = x, 1 = y, 2 = scale.x, 3 = scale.y, 4 = angle]
} Level;

void free_map(Map* map);
char* read_content(const char* filename);
char* get_value(Map* map, const char* key);
Level* get_level(int level);
CP_Vector get_vector(char* line);

#endif