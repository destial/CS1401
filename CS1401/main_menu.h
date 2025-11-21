#pragma once
#ifndef MAIN_MENU_H
#define MAIN_MENU_H
#include "utils.h"

/*!
@author Nathan Ng (n.ng@digipen.edu)
@date 8 September 2025
@course CSD1401
@brief this file contains header functions for the main_menu
*/

void main_menu_init(void);
void main_menu_update(void);
void change_screen(void);
void main_menu_exit(void);
void play_button(void);
void change_resolution(int num);
void setting_button(void);
void shop_button(void);
void exit_main_menu(void);
void main_menu_button(float mouse_x, float mouse_y);
void level_select_button(float mouse_x, float mouse_y);
void volume_bar(float mouse_x, float mouse_y);
void settings_button(float mouse_x, float mouse_y);
void shops_button(float mouse_x, float mouse_y);
void back_buttons(float mouse_x, float mouse_y);
CP_Vector defining_position(float pos_x, float pos_y);
CP_Vector defining_coords(float scale_x, float scale_y);
bool check_mouse_pos(float mouse_x, float mouse_y, float pos_x, float pos_y, float scale_x, float scale_y);
bool check_mouse_pos_volume(float mouse_x, float mouse_y, float pos_x, float pos_y, float scale_x, float scale_y);
void draw_text(float text_size, char* text, float pos_x, float pos_y);
void draw_volume(float text_size, float pos_x, float pos_y, float limit);
void buy_button(int num);
void equip_button(int num);
void is_shadow(void);
void is_inverse(void);
bool get_shadow_setting(void);
void draw_logo_image(void);
int equipped_index;

#endif