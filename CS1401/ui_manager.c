/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 12 November 2025
@course CSD1401
@brief Definition file for UI rendering inside the game
*/

#include <stdlib.h>
#include <string.h>
#include "ui_manager.h"

#define MAX_UI_OBJECTS 16

typedef struct ButtonStruct {
	float x, y, w, h;
	CP_Color background, highlight;
	CP_Image back_image, hover_image;
	char const* text;
} ButtonStruct;

typedef struct TextStruct {
	float x, y, w, h;
	CP_Color color;
	char const* text;
} TextStruct;

typedef struct SliderStruct {
	float x, y, w, h;
	CP_Color background, highlight;
	CP_Image back_image, slider_image;
	float value;
} SliderStruct;

typedef struct UIArrayStruct {
	ButtonStruct buttons[MAX_UI_OBJECTS]; // max buttons
	SliderStruct sliders[MAX_UI_OBJECTS]; // max sliders
	TextStruct texts[MAX_UI_OBJECTS]; // max texts

	int buttons_length, sliders_length, texts_length;
} UIElementsArray;

static int IsButtonHighlight(ButtonStruct* button, float click_x, float click_y) {
	return (click_x <= button->x + (button->w * 0.5f)) && (click_x >= button->x - (button->w * 0.5f)) && (click_y <= button->y + (button->h * 0.5f)) && (click_y >= button->y - (button->h * 0.5f));
}

static int IsSliderHighlight(SliderStruct* button, float click_x, float click_y) {
	return (click_x <= button->x + (button->w * 0.5f)) && (click_x >= button->x - (button->w * 0.5f)) && (click_y <= button->y + (button->h * 0.5f)) && (click_y >= button->y - (button->h * 0.5f));
}

static UIElementsArray _ui_elements;

bool UI_Button(float x, float y, float w, float h, CP_Color background, CP_Color highlight) {
	return UI_ButtonText(x, y, w, h, background, highlight, NULL);
}

bool UI_ButtonImage(float x, float y, float w, float h, CP_Image background, CP_Image highlight) {
	return UI_ButtonImageText(x, y, w, h, background, highlight, NULL);
}

void UI_Image(float x, float y, float w, float h, CP_Image image) {
	UI_ButtonImage(x, y, w, h, image, image);
}

void UI_Quad(float x, float y, float w, float h, CP_Color color) {
	UI_Button(x, y, w, h, color, color);
}

bool UI_ButtonImageText(float x, float y, float w, float h, CP_Image background, CP_Image highlight, char const* text) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	// stop if array maxed out
	if (_ui_elements.buttons_length == MAX_UI_OBJECTS)
		return false;

	// append a button image text
	++_ui_elements.buttons_length;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].w = w;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].h = h;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].x = x;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].y = y;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].background = (CP_Color){ 0, 0, 0, 255 };
	_ui_elements.buttons[_ui_elements.buttons_length - 1].highlight = (CP_Color){ 0, 0, 0, 255 };
	_ui_elements.buttons[_ui_elements.buttons_length - 1].text = text;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].back_image = background;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].hover_image = highlight;

	return CP_Input_MouseTriggered(MOUSE_BUTTON_1) && IsButtonHighlight(&_ui_elements.buttons[_ui_elements.buttons_length - 1], mouse_x, mouse_y);
}

bool UI_ButtonText(float x, float y, float w, float h, CP_Color background, CP_Color highlight, char const* text) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	// stop if array maxed out
	if (_ui_elements.buttons_length == MAX_UI_OBJECTS)
		return false;

	// append button with text
	++_ui_elements.buttons_length;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].w = w;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].h = h;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].x = x;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].y = y;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].background = background;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].highlight = highlight;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].text = text;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].back_image = NULL;
	_ui_elements.buttons[_ui_elements.buttons_length - 1].hover_image = NULL;

	return CP_Input_MouseTriggered(MOUSE_BUTTON_1) && IsButtonHighlight(&_ui_elements.buttons[_ui_elements.buttons_length - 1], mouse_x, mouse_y);
}

void UI_Text(float x, float y, float w, float h, CP_Color color, char const* text) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	// stop if array maxed out
	if (_ui_elements.texts_length == MAX_UI_OBJECTS)
		return;

	// append text
	++_ui_elements.texts_length;
	_ui_elements.texts[_ui_elements.texts_length - 1].w = w;
	_ui_elements.texts[_ui_elements.texts_length - 1].h = h;
	_ui_elements.texts[_ui_elements.texts_length - 1].x = x;
	_ui_elements.texts[_ui_elements.texts_length - 1].y = y;
	_ui_elements.texts[_ui_elements.texts_length - 1].color = color;
	_ui_elements.texts[_ui_elements.texts_length - 1].text = text;
}

float UI_SliderHorizontal(float x, float y, float w, float h, CP_Color background, CP_Color highlight, float value, bool dragable) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	// stop if array maxed out
	if (_ui_elements.sliders_length == MAX_UI_OBJECTS)
		return false;

	// append horizontal slider
	++_ui_elements.sliders_length;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].w = w;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].h = h;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].x = x;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].y = y;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].background = background;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].highlight = highlight;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].value = value;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].back_image = NULL;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].slider_image = NULL;

	// edit slider value if user drag
	if (dragable && CP_Input_MouseDragged(MOUSE_BUTTON_1) && IsSliderHighlight(&_ui_elements.sliders[_ui_elements.sliders_length - 1], mouse_x, mouse_y)) {
		float new_val = (mouse_x - (x - (w * 0.5f))) / w;
		_ui_elements.sliders[_ui_elements.sliders_length - 1].value = new_val;
	}
	return _ui_elements.sliders[_ui_elements.sliders_length - 1].value;
}

float UI_SliderHorizontalImage(float x, float y, float w, float h, CP_Image background, CP_Image highlight, float value, bool dragable) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	// stop if array maxed out
	if (_ui_elements.sliders_length == MAX_UI_OBJECTS)
		return false;

	// append horizontal image slider
	++_ui_elements.sliders_length;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].w = w;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].h = h;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].x = x;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].y = y;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].background = (CP_Color){ 0, 0, 0, 255 };
	_ui_elements.sliders[_ui_elements.sliders_length - 1].highlight = (CP_Color){ 0, 0, 0, 255 };
	_ui_elements.sliders[_ui_elements.sliders_length - 1].value = value;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].back_image = background;
	_ui_elements.sliders[_ui_elements.sliders_length - 1].slider_image = highlight;

	// edit slider value if user drag
	if (dragable && CP_Input_MouseDragged(MOUSE_BUTTON_1) && IsSliderHighlight(&_ui_elements.sliders[_ui_elements.sliders_length - 1], mouse_x, mouse_y)) {
		float new_val = (mouse_x - (x - (w * 0.5f))) / w;
		_ui_elements.sliders[_ui_elements.sliders_length - 1].value = new_val;
	}
	return _ui_elements.sliders[_ui_elements.sliders_length - 1].value;
}

void UI_Start(void) {
	// reset ui buffer lengths to 0
	_ui_elements.buttons_length = 0;
	_ui_elements.sliders_length = 0;
	_ui_elements.texts_length = 0;
}

void UI_Render(void) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	CP_Settings_Save();

	// render sliders first
	if (_ui_elements.sliders != NULL) {
		for (int i = 0; i < _ui_elements.sliders_length; ++i) {
			float x = _ui_elements.sliders[i].x;
			float y = _ui_elements.sliders[i].y;
			float w = _ui_elements.sliders[i].w;
			float h = _ui_elements.sliders[i].h;

			// if slider color
			if (_ui_elements.sliders[i].back_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Stroke(Get_Color(COLOR_BLACK));
				CP_Settings_Fill(_ui_elements.sliders[i].background);
				CP_Graphics_DrawRect(x, y, w, h);
			}
			else { // if slider has image
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(_ui_elements.sliders[i].back_image, x, y, w, h, 255);
			}
			
			// if slider color
			if (_ui_elements.sliders[i].slider_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Stroke(Get_Color(COLOR_BLACK));
				CP_Settings_Fill(_ui_elements.sliders[i].highlight);
				CP_Graphics_DrawRect(x - (w * (1.f - _ui_elements.sliders[i].value) * 0.5f), y, w * (_ui_elements.sliders[i].value), h);
			}
			else { // if slider has image
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(_ui_elements.sliders[i].slider_image, x - (w * (1.f - _ui_elements.sliders[i].value) * 0.5f), y, w * (_ui_elements.sliders[i].value), h, 255);
			}
		}
	}

	// then render buttons
	if (_ui_elements.buttons != NULL) {
		for (int i = 0; i < _ui_elements.buttons_length; ++i) {
			float x = _ui_elements.buttons[i].x;
			float y = _ui_elements.buttons[i].y;
			float w = _ui_elements.buttons[i].w;
			float h = _ui_elements.buttons[i].h;

			CP_BOOL c = IsButtonHighlight(&_ui_elements.buttons[i], mouse_x, mouse_y);

			if (_ui_elements.buttons[i].back_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Fill(c ? _ui_elements.buttons[i].highlight : _ui_elements.buttons[i].background);
				CP_Graphics_DrawRect(x, y, w, h);
			}
			else {
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(c ? _ui_elements.buttons[i].hover_image : _ui_elements.buttons[i].back_image, x, y, w, h, 255);
			}
			if (_ui_elements.buttons[i].text != NULL) {
				CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
				CP_Settings_TextSize((h + w) / strlen(_ui_elements.buttons[i].text));
				CP_Settings_Fill(c ? _ui_elements.buttons[i].background : _ui_elements.buttons[i].highlight);
				CP_Font_DrawTextBox(_ui_elements.buttons[i].text, x - (w * 0.5f), y, w);
			}
		}
	}

	// then render texts
	if (_ui_elements.texts != NULL) {
		for (int i = 0; i < _ui_elements.texts_length; ++i) {
			float x = _ui_elements.texts[i].x;
			float y = _ui_elements.texts[i].y;
			float w = _ui_elements.texts[i].w;
			float h = _ui_elements.texts[i].h;

			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Settings_TextSize((h + w) / strlen(_ui_elements.texts[i].text));
			CP_Settings_Fill(_ui_elements.texts[i].color);
			CP_Font_DrawTextBox(_ui_elements.texts[i].text, x - (w * 0.5f), y, w);
		}
	}

	CP_Settings_Restore();

	// reset ui buffer
	UI_Start();
}