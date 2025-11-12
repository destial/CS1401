/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 12 November 2025
@course CSD1401
@brief Definition file for UI rendering inside the game
*/

#include <stdlib.h>
#include <string.h>
#include "ui_manager.h"

typedef struct ButtonStruct {
	float x, y, w, h;
	CP_Color background, highlight;
	CP_Image back_image, hover_image;
	char const* text;
} ButtonStruct;

typedef struct SliderStruct {
	float x, y, w, h, *value;
	CP_Color background, highlight;
	CP_Image back_image, slider_image;
} SliderStruct;

typedef struct ButtonStructArray {
	ButtonStruct buttons[16]; // max 16 buttons
	int length;
} ButtonArray;

typedef struct SliderStructArray {
	SliderStruct sliders[16]; // max 16 sliders
	int length;
} SliderArray;

static int IsButtonHighlight(ButtonStruct* button, float click_x, float click_y) {
	return (click_x <= button->x + (button->w * 0.5f)) && (click_x >= button->x - (button->w * 0.5f)) && (click_y <= button->y + (button->h * 0.5f)) && (click_y >= button->y - (button->h * 0.5f));
}

static int IsSliderHighlight(SliderStruct* button, float click_x, float click_y) {
	return (click_x <= button->x + (button->w * 0.5f)) && (click_x >= button->x - (button->w * 0.5f)) && (click_y <= button->y + (button->h * 0.5f)) && (click_y >= button->y - (button->h * 0.5f));
}

static ButtonArray _buttons;
static SliderArray _sliders;

bool UI_Button(float x, float y, float w, float h, CP_Color background, CP_Color highlight) {
	return UI_ButtonText(x, y, w, h, background, highlight, NULL);
}

bool UI_ButtonImage(float x, float y, float w, float h, CP_Image background, CP_Image highlight) {
	return UI_ButtonImageText(x, y, w, h, background, highlight, NULL);
}

bool UI_ButtonImageText(float x, float y, float w, float h, CP_Image background, CP_Image highlight, char const* text) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	++_buttons.length;
	_buttons.buttons[_buttons.length - 1].w = w;
	_buttons.buttons[_buttons.length - 1].h = h;
	_buttons.buttons[_buttons.length - 1].x = x;
	_buttons.buttons[_buttons.length - 1].y = y;
	_buttons.buttons[_buttons.length - 1].background = (CP_Color){ 0, 0, 0, 255 };
	_buttons.buttons[_buttons.length - 1].highlight = (CP_Color){ 0, 0, 0, 255 };
	_buttons.buttons[_buttons.length - 1].text = text;
	_buttons.buttons[_buttons.length - 1].back_image = background;
	_buttons.buttons[_buttons.length - 1].hover_image = highlight;

	return CP_Input_MouseTriggered(MOUSE_BUTTON_1) && IsButtonHighlight(&_buttons.buttons[_buttons.length - 1], mouse_x, mouse_y);
}

bool UI_ButtonText(float x, float y, float w, float h, CP_Color background, CP_Color highlight, char const* text) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	++_buttons.length;
	_buttons.buttons[_buttons.length - 1].w = w;
	_buttons.buttons[_buttons.length - 1].h = h;
	_buttons.buttons[_buttons.length - 1].x = x;
	_buttons.buttons[_buttons.length - 1].y = y;
	_buttons.buttons[_buttons.length - 1].background = background;
	_buttons.buttons[_buttons.length - 1].highlight = highlight;
	_buttons.buttons[_buttons.length - 1].text = text;
	_buttons.buttons[_buttons.length - 1].back_image = NULL;
	_buttons.buttons[_buttons.length - 1].hover_image = NULL;

	return CP_Input_MouseTriggered(MOUSE_BUTTON_1) && IsButtonHighlight(&_buttons.buttons[_buttons.length - 1], mouse_x, mouse_y);
}

void UI_Slider(float x, float y, float w, float h, CP_Color background, CP_Color highlight, float* value) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	++_sliders.length;
	_sliders.sliders[_sliders.length - 1].w = w;
	_sliders.sliders[_sliders.length - 1].h = h;
	_sliders.sliders[_sliders.length - 1].x = x;
	_sliders.sliders[_sliders.length - 1].y = y;
	_sliders.sliders[_sliders.length - 1].background = background;
	_sliders.sliders[_sliders.length - 1].highlight = highlight;
	_sliders.sliders[_sliders.length - 1].value = value;
	_sliders.sliders[_sliders.length - 1].back_image = NULL;
	_sliders.sliders[_sliders.length - 1].slider_image = NULL;

	if (CP_Input_MouseDragged(MOUSE_BUTTON_1) && IsSliderHighlight(&_sliders.sliders[_sliders.length - 1], mouse_x, mouse_y)) {
		float new_val = (mouse_x - (x - (w * 0.5f))) / w;
		*_sliders.sliders[_sliders.length - 1].value = new_val;
	}
}

void UI_SliderImage(float x, float y, float w, float h, CP_Image background, CP_Image highlight, float* value) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	++_sliders.length;
	_sliders.sliders[_sliders.length - 1].w = w;
	_sliders.sliders[_sliders.length - 1].h = h;
	_sliders.sliders[_sliders.length - 1].x = x;
	_sliders.sliders[_sliders.length - 1].y = y;
	_sliders.sliders[_sliders.length - 1].background = (CP_Color){ 0, 0, 0, 255 };
	_sliders.sliders[_sliders.length - 1].highlight = (CP_Color){ 0, 0, 0, 255 };
	_sliders.sliders[_sliders.length - 1].value = value;
	_sliders.sliders[_sliders.length - 1].back_image = background;
	_sliders.sliders[_sliders.length - 1].slider_image = highlight;

	if (CP_Input_MouseDragged(MOUSE_BUTTON_1) && IsSliderHighlight(&_sliders.sliders[_sliders.length - 1], mouse_x, mouse_y)) {
		float new_val = (mouse_x - (x - (w * 0.5f))) / w;
		*_sliders.sliders[_sliders.length - 1].value = new_val;
	}
}

void UI_Start(void) {
	_buttons.length = 0;
	_sliders.length = 0;
}

void UI_Render(void) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	CP_Settings_Save();
	if (_buttons.buttons != NULL) {
		for (int i = 0; i < _buttons.length; ++i) {
			float x = _buttons.buttons[i].x;
			float y = _buttons.buttons[i].y;
			float w = _buttons.buttons[i].w;
			float h = _buttons.buttons[i].h;

			CP_BOOL c = IsButtonHighlight(&_buttons.buttons[i], mouse_x, mouse_y);

			if (_buttons.buttons[i].back_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Fill(c ? _buttons.buttons[i].highlight : _buttons.buttons[i].background);
				CP_Graphics_DrawRect(x, y, w, h);
			}
			else {
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(c ? _buttons.buttons[i].hover_image : _buttons.buttons[i].back_image, x, y, w, h, 255);
			}
			if (_buttons.buttons[i].text != NULL) {
				CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
				CP_Settings_TextSize((h + w) / strlen(_buttons.buttons[i].text));
				CP_Settings_Fill(c ? _buttons.buttons[i].background : _buttons.buttons[i].highlight);
				CP_Font_DrawTextBox(_buttons.buttons[i].text, x - (w * 0.5f), y, w);
			}
		}
	}

	if (_sliders.sliders != NULL) {
		for (int i = 0; i < _sliders.length; ++i) {
			float x = _sliders.sliders[i].x;
			float y = _sliders.sliders[i].y;
			float w = _sliders.sliders[i].w;
			float h = _sliders.sliders[i].h;

			if (_sliders.sliders[i].back_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Fill(_sliders.sliders[i].background);
				CP_Graphics_DrawRect(x, y, w, h);
			}
			else {
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(_sliders.sliders[i].back_image, x, y, w, h, 255);
			}

			if (_sliders.sliders[i].slider_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Fill(_sliders.sliders[i].highlight);
				CP_Graphics_DrawRect(x - (w * (1.f - *_sliders.sliders[i].value)), y, w * (*_sliders.sliders[i].value), h);
			}
			else {
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(_sliders.sliders[i].slider_image, x - (w * (1.f - *_sliders.sliders[i].value)), y, w * (*_sliders.sliders[i].value), h, 255);
			}
		}
	}

	CP_Settings_Restore();

	UI_Start();
}