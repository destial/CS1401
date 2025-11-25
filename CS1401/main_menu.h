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

/*
@brief sets the enum to STATE_EXIT and exits the game
*/
void exit_main_menu(void);
void main_menu_init(void);
void main_menu_update(void);

/*
@brief This function is called when rendering and animating the main menu buttons play button, setting button, quit button
@param takes in the position of the mouse (x and y axis)
*/
void main_menu_button(float mouse_x, float mouse_y);

/*
@brief This function is called when rendering and animating the square level select buttons from 1 - 10
@param takes in the position of the mouse (x and y axis)
*/
void level_select_button(float mouse_x, float mouse_y);

/*
@brief This function is called when rendering and animating the two volume bar in the setting page, music and sfx volume
@param takes in the position of the mouse (x and y axis)
*/
void volume_bar(float mouse_x, float mouse_y);

/*
@brief This function is called when rendering and animating the 3 resolution buttons, is_shadow button, and is_inverse button
@param takes in the position of the mouse (x and y axis)
*/
void settings_button(float mouse_x, float mouse_y);

/*
@brief This function is called when rendering and animating the shop button at the main menu screen
@param takes in the position of the mouse (x and y axis)
*/
void shops_button(float mouse_x, float mouse_y);

/*
@brief This function is called when rendering and animating the back buttons for setting, level select and shop
@param takes in the position of the mouse (x and y axis)
*/
void back_buttons(float mouse_x, float mouse_y);

/*
@brief This function is called to define the position of the various UI elements drawn onto the screen from world to screen space
@param takes in the position of the mouse (x and y axis)
*/
CP_Vector defining_position(float pos_x, float pos_y);

/*
@brief This function is called to define the size/scale of the various UI elements drawn onto the screen from world to screen space
@param takes in the scale factor of the object (x and y)
*/
CP_Vector defining_coords(float scale_x, float scale_y);

/*
@brief This function is called to check if the users mouse is hovering over a button
@param takes in the mouse position, the buttons position and scale
*/
bool check_mouse_pos(float mouse_x, float mouse_y, float pos_x, float pos_y, float scale_x, float scale_y);

/*
@brief This function is called to check if the users mouse is hovering over the volume button
@param takes in the mouse position, the volume position and scale
*/
bool check_mouse_pos_volume(float mouse_x, float mouse_y, float pos_x, float pos_y, float scale_x, float scale_y);

/*
@brief This function is called to draw text anywhere specified by the pos
@param takes in the text size, text, and the position you want the text to draw at (x and y)
*/
void draw_text(float text_size, char* text, float pos_x, float pos_y);

/*
@brief This function is called to draw text onto the two volume bars and adjust the text according to the scale of the volume bar
@param takes in the text size, the position, and the limit
*/
void draw_volume(float text_size, float pos_x, float pos_y, float limit);

/*
@brief This function is called to lerp in the level select buttons and lerp out the main menu buttons
@param nothing
*/
void play_button(void);

/*
@brief This function is called to open the setting menu
@param nothing
*/
void setting_button(void);

/*
@brief This function is called to open the shop menu
@param nothing
*/
void shop_button(void);

/*
@brief This function is called for the shop buy buttons to check if the player have bought the skin
	   and deduct the relevant amount of coins
@param takes in the button number
*/
void buy_button(int num);

/*
@brief This function is called for the shop buy buttons to check if the player have equipped the skin
@param takes in the button number
*/
void equip_button(int num);

/*
@brief This function is called to draw the GIF logo at the main menu, and to lerp out when play button is clicked
@param nothing
*/
void draw_logo_image(void);

/*
@brief This function is called to change the game state to STATE_GAME which are the various levels.
@param nothing
*/
void change_screen(void);

/*
@brief This function is called to change the resolution.
@param resolution button number
*/
void change_resolution(int num);

/*
@brief This function is called to activate or deactivate the shadow feature for the golf ball.
@param nothing
*/
void is_shadow(void);

/*
@brief This function is called to activate or deactivate the inverse contols for the golf ball.
@param nothing
*/
void is_inverse(void);

/*
@brief This function is called to return the value of is_shadowed (true or false).
@param nothing
*/
bool get_shadow_setting(void);

/*
@brief This function is called to return the value of is_inverse (true or false).
@param nothing
*/
bool get_inverse_setting(void);

/*
@brief This function is called to free the images when exiting the application.
@param nothing
*/
void main_menu_exit(void);

#endif