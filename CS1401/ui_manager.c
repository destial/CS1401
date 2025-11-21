/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 12 November 2025
@course CSD1401
@brief Definition file for UI rendering inside the game
*/

#include <string.h>
#include <stdlib.h>
#include "ui_manager.h"
#include "game_settings.h"

#ifndef MAX_UI_OBJECTS
#define MAX_UI_OBJECTS 16
#endif

// declare different types of UI
typedef struct VideoStruct {
	float x, y, w, h;
	SpriteSheet spritesheet;
	int priority;
} VideoStruct;

typedef struct ButtonStruct {
	float x, y, w, h;
	CP_Color background, highlight;
	CP_Image back_image, hover_image;
	char const* text;
	int priority;
} ButtonStruct;

typedef struct TextStruct {
	float x, y, w, h, size;
	CP_Color color;
	char const* text;
	int priority;
	CP_POSITION_MODE mode;
} TextStruct;

typedef struct SliderStruct {
	float x, y, w, h;
	CP_Color background, highlight;
	CP_Image back_image, slider_image;
	float value;
	int priority;
} SliderStruct;

// these is used to sort each UI element by its rendering priority order
typedef enum UITypeEnum {
	UI_BUTTON,
	UI_TEXT,
	UI_VIDEO,
	UI_SLIDER
} UIType;

typedef struct UIElement {
	union {
		TextStruct* text;
		ButtonStruct* button;
		SliderStruct* slider;
		VideoStruct* video;
		void* ptr;
	};
	UIType type;
	int priority;
} UIElement;

typedef struct UIArrayStruct {
	ButtonStruct buttons[MAX_UI_OBJECTS]; // max buttons
	SliderStruct sliders[MAX_UI_OBJECTS]; // max sliders
	TextStruct texts[MAX_UI_OBJECTS]; // max texts
	VideoStruct videos[MAX_UI_OBJECTS]; // max videos

	UIElement sorted[MAX_UI_OBJECTS * 4]; // sorted each UI element by render priority

	int elements_length, buttons_length, sliders_length, texts_length, videos_length;
} UIElementsArray;

// static cuz i dont want it exposed anywhere else
static UIElementsArray _ui_elements;
static int current_priority = 0;

static int IsButtonHighlight(ButtonStruct* button, float click_x, float click_y) {
	return (click_x <= button->x + (button->w * 0.5f)) && (click_x >= button->x - (button->w * 0.5f)) && (click_y <= button->y + (button->h * 0.5f)) && (click_y >= button->y - (button->h * 0.5f));
}

static int IsSliderHighlight(SliderStruct* button, float click_x, float click_y) {
	return (click_x <= button->x + (button->w * 0.5f)) && (click_x >= button->x - (button->w * 0.5f)) && (click_y <= button->y + (button->h * 0.5f)) && (click_y >= button->y - (button->h * 0.5f));
}

// to pass as function ptr into qsort()
static int UI_CompPriority(const UIElement* a, const UIElement* b) {
	return a->priority - b->priority;
}

// to be called before render
// sorts each UI element by priority
static void UI_SortPriority(void) {
	if (_ui_elements.sliders != NULL) {
		for (int i = 0; i < _ui_elements.sliders_length; ++i) {
			_ui_elements.sorted[_ui_elements.elements_length].slider = &_ui_elements.sliders[i];
			_ui_elements.sorted[_ui_elements.elements_length].type = UI_SLIDER;
			_ui_elements.sorted[_ui_elements.elements_length].priority = _ui_elements.sliders[i].priority;
			++_ui_elements.elements_length;
		}
	}

	if (_ui_elements.buttons != NULL) {
		for (int i = 0; i < _ui_elements.buttons_length; ++i) {
			_ui_elements.sorted[_ui_elements.elements_length].button = &_ui_elements.buttons[i];
			_ui_elements.sorted[_ui_elements.elements_length].type = UI_BUTTON;
			_ui_elements.sorted[_ui_elements.elements_length].priority = _ui_elements.buttons[i].priority;
			++_ui_elements.elements_length;
		}
	}

	if (_ui_elements.texts != NULL) {
		for (int i = 0; i < _ui_elements.texts_length; ++i) {
			_ui_elements.sorted[_ui_elements.elements_length].text = &_ui_elements.texts[i];
			_ui_elements.sorted[_ui_elements.elements_length].type = UI_TEXT;
			_ui_elements.sorted[_ui_elements.elements_length].priority = _ui_elements.texts[i].priority;
			++_ui_elements.elements_length;
		}
	}

	if (_ui_elements.videos != NULL) {
		for (int i = 0; i < _ui_elements.videos_length; ++i) {
			_ui_elements.sorted[_ui_elements.elements_length].video = &_ui_elements.videos[i];
			_ui_elements.sorted[_ui_elements.elements_length].type = UI_VIDEO;
			_ui_elements.sorted[_ui_elements.elements_length].priority = _ui_elements.videos[i].priority;
			++_ui_elements.elements_length;
		}
	}

	// thanks qsort!
	qsort(_ui_elements.sorted, _ui_elements.elements_length, sizeof(UIElement), UI_CompPriority);
}

void UI_SetPriority(int priority) {
	current_priority = priority;
}

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

void UI_Video(float x, float y, float w, float h, SpriteSheet spritesheet) {
	// stop if array maxed out
	if (_ui_elements.videos_length == MAX_UI_OBJECTS)
		return;

	// append a button image text
	++_ui_elements.videos_length;
	_ui_elements.videos[_ui_elements.videos_length - 1].w = w;
	_ui_elements.videos[_ui_elements.videos_length - 1].h = h;
	_ui_elements.videos[_ui_elements.videos_length - 1].x = x;
	_ui_elements.videos[_ui_elements.videos_length - 1].y = y;
	_ui_elements.videos[_ui_elements.videos_length - 1].spritesheet = spritesheet;
	_ui_elements.videos[_ui_elements.videos_length - 1].priority = current_priority;
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
	_ui_elements.buttons[_ui_elements.buttons_length - 1].priority = current_priority;

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
	_ui_elements.buttons[_ui_elements.buttons_length - 1].priority = current_priority;

	return CP_Input_MouseTriggered(MOUSE_BUTTON_1) && IsButtonHighlight(&_ui_elements.buttons[_ui_elements.buttons_length - 1], mouse_x, mouse_y);
}

void UI_Text(float x, float y, float w, float h, CP_Color color, char const* text, float size) {
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
	_ui_elements.texts[_ui_elements.texts_length - 1].size = size;
	_ui_elements.texts[_ui_elements.texts_length - 1].priority = current_priority;
	_ui_elements.texts[_ui_elements.texts_length - 1].mode = CP_POSITION_CENTER;
}

void UI_TextLeft(float x, float y, float w, float h, CP_Color color, char const* text, float size) {
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
	_ui_elements.texts[_ui_elements.texts_length - 1].size = size;
	_ui_elements.texts[_ui_elements.texts_length - 1].priority = current_priority;
	_ui_elements.texts[_ui_elements.texts_length - 1].mode = CP_POSITION_CORNER;
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
	_ui_elements.sliders[_ui_elements.sliders_length - 1].priority = current_priority;

	// edit slider value if user drag
	if (dragable && CP_Input_MouseDragged(MOUSE_BUTTON_1) && IsSliderHighlight(&_ui_elements.sliders[_ui_elements.sliders_length - 1], mouse_x, mouse_y)) {
		float new_val = (mouse_x - (x - (w * 0.5f))) / w;
		_ui_elements.sliders[_ui_elements.sliders_length - 1].value = new_val;
		GameSetting_SetCursor(CURSOR_GRAB);
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
	_ui_elements.sliders[_ui_elements.sliders_length - 1].priority = current_priority;

	// edit slider value if user drag
	if (dragable && CP_Input_MouseDragged(MOUSE_BUTTON_1) && IsSliderHighlight(&_ui_elements.sliders[_ui_elements.sliders_length - 1], mouse_x, mouse_y)) {
		float new_val = (mouse_x - (x - (w * 0.5f))) / w;
		_ui_elements.sliders[_ui_elements.sliders_length - 1].value = new_val;
		GameSetting_SetCursor(CURSOR_GRAB);
	}
	return _ui_elements.sliders[_ui_elements.sliders_length - 1].value;
}

void UI_Start(void) {
	// reset ui buffer lengths to 0
	_ui_elements.buttons_length = 0;
	_ui_elements.sliders_length = 0;
	_ui_elements.texts_length = 0;
	_ui_elements.videos_length = 0;
	_ui_elements.elements_length = 0;

	current_priority = 0;
}

void UI_Render(void) {
	float mouse_x = CP_Input_GetMouseX();
	float mouse_y = CP_Input_GetMouseY();

	// sort ui elements by priority
	UI_SortPriority();

	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
	for (int i = 0; i < _ui_elements.elements_length; ++i) {
		UIType type = _ui_elements.sorted[i].type;
		CP_Settings_Save();
		if (type == UI_SLIDER) {
			SliderStruct* slider = _ui_elements.sorted[i].slider;
			float x = slider->x;
			float y = slider->y;
			float w = slider->w;
			float h = slider->h;

			// if slider color
			if (slider->back_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Stroke(Get_Color(COLOR_BLACK));
				CP_Settings_Fill(slider->background);
				CP_Graphics_DrawRect(x, y, w, h);
			}
			else { // if slider has image
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(slider->back_image, x, y, w, h, 255);
			}

			// if slider color
			if (slider->slider_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Stroke(Get_Color(COLOR_BLACK));
				CP_Settings_Fill(slider->highlight);
				CP_Graphics_DrawRect(x - (w * (1.f - slider->value) * 0.5f), y, w * (slider->value), h);
			}
			else { // if slider has image
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(slider->slider_image, x - (w * (1.f - slider->value) * 0.5f), y, w * (slider->value), h, 255);
			}
		}
		else if (type == UI_BUTTON) {
			ButtonStruct* button = _ui_elements.sorted[i].button;
			float x = button->x;
			float y = button->y;
			float w = button->w;
			float h = button->h;

			CP_BOOL c = IsButtonHighlight(button, mouse_x, mouse_y);
			if (c && (button->hover_image != button->back_image)) {
				GameSetting_SetCursor(CURSOR_POINT);
			}
			// if button color
			if (button->back_image == NULL) {
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_Fill(c ? button->highlight : button->background);
				CP_Graphics_DrawRect(x, y, w, h);
			}
			else { // if button has image
				CP_Settings_ImageMode(CP_POSITION_CENTER);
				CP_Image_Draw(c ? button->hover_image : button->back_image, x, y, w, h, 255);
			}

			// if button has text
			if (button->text != NULL) {
				CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
				CP_Settings_TextSize((h + w) / strlen(button->text));
				CP_Settings_Fill(c ? button->background : button->highlight);
				CP_Font_DrawTextBox(button->text, x - (w * 0.5f), y, w);
			}
		}
		else if (type == UI_VIDEO) {
			VideoStruct* video = _ui_elements.sorted[i].video;
			float x = video->x;
			float y = video->y;
			float w = video->w;
			float h = video->h;

			// render video sprite
			Sprite_Draw(video->spritesheet, x, y, w, h, 255);
		}
		else if (type == UI_TEXT) {
			TextStruct* text = _ui_elements.sorted[i].text;
			float x = text->x;
			float y = text->y;
			float w = text->w;
			float h = text->h;

			// render text
			if (text->mode == CP_POSITION_CENTER) {
				CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
			}
			else {
				CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
			}
			CP_Settings_TextSize(text->size);
			CP_Settings_Fill(text->color);
			CP_Font_DrawTextBox(text->text, text->mode == CP_POSITION_CENTER ? x - (w * 0.5f) : x, y, w);
		}

		CP_Settings_Restore();
	}

	// reset ui buffer
	UI_Start();
}