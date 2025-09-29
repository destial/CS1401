/*
*/

#pragma once
#ifndef SCORE_H
#define SCORE_H
#include "cprocessing.h"

void draw_hole(CP_Vector hole, float diameter, float world_width, float world_height, float animation);

void add_stroke(void);

void reset_strokes(void);

int get_strokes(void);

void draw_scoreboard(CP_Vector sBoard, float world_width, float world_height);


//
//void add_stroke();
//
//char is_win(void);
//
//char set_win(char win);
//
//void draw_scoreboard(void);
//
//void draw_hole(void);
//
#endif