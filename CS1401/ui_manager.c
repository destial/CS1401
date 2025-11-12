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

typedef struct TextStruct {
	float x, y, w, h;
	CP_Color color;
	char const* text;
} TextStruct;

typedef struct SliderStruct {
	float x, y, w, h, value;
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

typedef struct TextStructArray {
	TextStruct texts[16]; // max 16 texts
	int length;
} TextArray;

static int IsButtonHighlight(ButtonStruct* button, float click_x, float click_y) {
	return (click_x <= button->x + (button->w * 0.5f)) && (click_x >= button->x - (button->w * 0.5f)) && (click_y <= button->y + (button->h * 0.5f)) && (click_y >= button->y - (button->h * 0.5f));
}

static int IsSliderHighlight(SliderStruct* button, float click_x, float click_y) {
	return (click_x <= button->x + (button->w * 0.5f)) && (click_x >= button->x - (button->w * 0.5f)) && (click_y <= button->y + (button->h * 0.5f)) && (click_y >= button->y - (button->h * 0.5f));
}

static ButtonArray _buttons;
static SliderArray _sliders;
static TextArray _texts;

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

void UI_Text(float x, float y, float w, float h, CP_Color color, char const* text) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	++_texts.length;
	_texts.texts[_texts.length - 1].w = w;
	_texts.texts[_texts.length - 1].h = h;
	_texts.texts[_texts.length - 1].x = x;
	_texts.texts[_texts.length - 1].y = y;
	_texts.texts[_texts.length - 1].color = color;
	_texts.texts[_texts.length - 1].text = text;
}

float UI_SliderHorizontal(float x, float y, float w, float h, CP_Color background, CP_Color highlight, float value, bool dragable) {
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

	if (dragable && CP_Input_MouseDragged(MOUSE_BUTTON_1) && IsSliderHighlight(&_sliders.sliders[_sliders.length - 1], mouse_x, mouse_y)) {
		float new_val = (mouse_x - (x - (w * 0.5f))) / w;
		_sliders.sliders[_sliders.length - 1].value = new_val;
	}
	return _sliders.sliders[_sliders.length - 1].value;
}

float UI_SliderHorizontalImage(float x, float y, float w, float h, CP_Image background, CP_Image highlight, float value, bool dragable) {
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

	if (dragable && CP_Input_MouseDragged(MOUSE_BUTTON_1) && IsSliderHighlight(&_sliders.sliders[_sliders.length - 1], mouse_x, mouse_y)) {
		float new_val = (mouse_x - (x - (w * 0.5f))) / w;
		_sliders.sliders[_sliders.length - 1].value = new_val;
	}
	return _sliders.sliders[_sliders.length - 1].value;
}

void UI_Start(void) {
	_buttons.length = 0;
	_sliders.length = 0;
	_texts.length = 0;
}

void UI_Render(void) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	CP_Settings_Save();
	if (_sliders.sliders != NULL) {
		for (int i = 0; i < _sliders.length; ++i) {
			float x = _sliders.sliders[i].x;
			float y = _sliders.sliders[i].y;
			float w = _sliders.sliders[i].w;
			float h = _sliders.sliders[i].h;

			if (_sliders.sliders[i].back_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Stroke(Get_Color(COLOR_BLACK));
				CP_Settings_Fill(_sliders.sliders[i].background);
				CP_Graphics_DrawRect(x, y, w, h);
			}
			else {
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(_sliders.sliders[i].back_image, x, y, w, h, 255);
			}

			if (_sliders.sliders[i].slider_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Stroke(Get_Color(COLOR_BLACK));
				CP_Settings_Fill(_sliders.sliders[i].highlight);
				CP_Graphics_DrawRect(x - (w * (1.f - _sliders.sliders[i].value) * 0.5f), y, w * (_sliders.sliders[i].value), h);
			}
			else {
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(_sliders.sliders[i].slider_image, x - (w * (1.f - _sliders.sliders[i].value) * 0.5f), y, w * (_sliders.sliders[i].value), h, 255);
			}
		}
	}

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

	if (_texts.texts != NULL) {
		for (int i = 0; i < _texts.length; ++i) {
			float x = _texts.texts[i].x;
			float y = _texts.texts[i].y;
			float w = _texts.texts[i].w;
			float h = _texts.texts[i].h;

			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Settings_TextSize((h + w) / strlen(_texts.texts[i].text));
			CP_Settings_Fill(_texts.texts[i].color);
			CP_Font_DrawTextBox(_texts.texts[i].text, x - (w * 0.5f), y, w);
		}
	}

	CP_Settings_Restore();

	UI_Start();
}