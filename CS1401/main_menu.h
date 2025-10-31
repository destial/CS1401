#pragma once

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
void change_resolution(num);
void setting_button(void);
void exit_main_menu(void);
void main_menu_button(float mouse_x, float mouse_y);
void level_select_button(float mouse_x, float mouse_y);
void volume_bar(float mouse_x, float mouse_y);
void settings_button(float mouse_x, float mouse_y);
void back_buttons(float mouse_x, float mouse_y);
void is_shadow(void);
char get_shadow(void);
