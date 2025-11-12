/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 12 November 2025
@course CSD1401
@brief Declaration file for UI rendering inside the game
*/

#pragma once
#ifndef UI_MANAGER_H
#define UI_MANAGER_H
#include "cprocessing.h"
#include "utils.h"

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Create text at the given position with the given dimensions and color
@param x - The x position of the text center
@param y - The y position of the text center
@param w - The width of the text
@param h - The height of the text
@param color - The color of the text
@param text - The text to display
*/
void UI_Text(float x, float y, float w, float h, CP_Color color, char const* text);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Create a button at the given position with the given dimensions and colors
@param x - The x position of the button center
@param y - The y position of the button center
@param w - The width of the button
@param h - The height of the button
@param background - The background color of the button
@param highlight - The highlight color of the button when hovered
@return TRUE if the button is clicked, FALSE otherwise
*/
bool UI_Button(float x, float y, float w, float h, CP_Color background, CP_Color highlight);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Create a button at the given position with the given dimensions and colors and text
@param x - The x position of the button center
@param y - The y position of the button center
@param w - The width of the button
@param h - The height of the button
@param background - The background color of the button
@param highlight - The highlight color of the button when hovered
@param text - The text to display on the button
@return TRUE if the button is clicked, FALSE otherwise
*/
bool UI_ButtonText(float x, float y, float w, float h, CP_Color background, CP_Color highlight, char const* text);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Create a button at the given position with the given dimensions and images
@param x - The x position of the button center
@param y - The y position of the button center
@param w - The width of the button
@param h - The height of the button
@param background - The background image of the button
@param highlight - The highlight image of the button when hovered
@return TRUE if the button is clicked, FALSE otherwise
*/
bool UI_ButtonImage(float x, float y, float w, float h, CP_Image background, CP_Image highlight);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Create a button at the given position with the given dimensions and images and text
@param x - The x position of the button center
@param y - The y position of the button center
@param w - The width of the button
@param h - The height of the button
@param background - The background image of the button
@param highlight - The highlight image of the button when hovered
@param text - The text to display on the button
@return TRUE if the button is clicked, FALSE otherwise
*/
bool UI_ButtonImageText(float x, float y, float w, float h, CP_Image background, CP_Image highlight, char const* text);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Create a horizontal slider at the given position with the given dimensions and colors
@param x - The x position of the slider center
@param y - The y position of the slider center
@param w - The width of the slider
@param h - The height of the slider
@param background - The background color of the slider
@param highlight - The highlight color of the slider
@param value - The starting float value of the slider (0.0 to 1.0)
@param dragable - If this slider value can be modified by the user
*/
float UI_SliderHorizontal(float x, float y, float w, float h, CP_Color background, CP_Color highlight, float value, bool dragable);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Create a horizontal slider at the given position with the given dimensions and images
@param x - The x position of the slider center
@param y - The y position of the slider center
@param w - The width of the slider
@param h - The height of the slider
@param background - The background image of the slider
@param highlight - The highlight image of the slider
@param value - The starting float value of the slider (0.0 to 1.0)
@param dragable - If this slider value can be modified by the user
*/
float UI_SliderHorizontalImage(float x, float y, float w, float h, CP_Image background, CP_Image highlight, float value, bool dragable);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Initialize the UI manager
*/
void UI_Start(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Render all UI elements created since the last UI_Start call
*/
void UI_Render(void);

#endif