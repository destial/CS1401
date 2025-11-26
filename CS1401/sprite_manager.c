/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 23 September 2025
@course CSD1401
@brief Definition file for implementing a sprite manager that will handle graphic animations
*/

#include "sprite_manager.h"
#include "utils.h"

typedef struct SpriteSheetStruct {
	CP_Image root_image;
	int img_h, img_w, sub_h, sub_w,
		rows, columns, current_row, current_column;
	float duration, elapsedDuration;
	bool updated_frame;
} *SpriteSheet;

void Sprite_ResetFrame(SpriteSheet sheet) {
	sheet->updated_frame = false;
}

void Sprite_Draw(SpriteSheet sheet, float x, float y, float w, float h, int alpha) {
	// just renders with 0 degrees of rotation
	Sprite_DrawAdvanced(sheet, x, y, w, h, alpha, 0);
}

void Sprite_DrawAdvanced(SpriteSheet sheet, float x, float y, float w, float h, int alpha, float degrees) {
	// draw the sprite at this frame
	Sprite_DrawFrame(sheet, sheet->current_row, sheet->current_column, x, y, w, h, alpha, degrees);

	// update the sprite frame
	if (sheet->updated_frame)
		return;

	sheet->updated_frame = true;
	// increment the animation frame based on the time elapsed
	int frameCount = sheet->rows * sheet->columns;
	if ((sheet->elapsedDuration += CP_System_GetDt()) > sheet->duration / frameCount) {
		if (++sheet->current_column >= sheet->columns) {
			sheet->current_column = 0;

			if (++sheet->current_row >= sheet->rows) {
				sheet->current_row = 0;
			}
		}
		sheet->elapsedDuration = 0.f;
	}
}

void Sprite_DrawFrame(SpriteSheet sheet, int row, int col, float x, float y, float w, float h, int alpha, float degrees) {
	int u0 = col * sheet->sub_w;
	int v0 = row * sheet->sub_h;
	CP_Image_DrawSubImageAdvanced(sheet->root_image, x, y, w, h, (float)u0, (float)v0, (float)(u0 + sheet->sub_w), (float)(v0 + sheet->sub_h), alpha, degrees);
}

void Sprite_DrawCropped(SpriteSheet sheet, int row, int col, float x, float y, float w, float h, float y_bot) {
	int u0 = col * sheet->sub_w;
	int v0 = row * sheet->sub_h;
	CP_Image_DrawSubImageAdvanced(sheet->root_image, x, y, w, h - y_bot, (float)u0, (float)v0, (float)(u0 + sheet->sub_w), (float)(v0 + sheet->sub_h) - y_bot, 255, 0.f);
}

void Sprite_Free(SpriteSheet spritesheet) {
	CP_Image_Free(spritesheet->root_image);
	spritesheet->root_image = NULL;
	FREE_MEMORY(spritesheet);
}

SpriteSheet Sprite_Create(CP_Image image, int rows, int columns, float duration) {
	SpriteSheet sheet = gc_malloc(sizeof(struct SpriteSheetStruct));
	if (sheet == NULL)
		return NULL;

	// initialize the sprite sheet values
	sheet->root_image = image;
	sheet->current_row = 0;
	sheet->current_column = 0;
	sheet->rows = rows;
	sheet->columns = columns;
	sheet->duration = duration;
	sheet->elapsedDuration = 0.f;
	sheet->updated_frame = false;

	sheet->img_h = CP_Image_GetHeight(sheet->root_image);
	sheet->img_w = CP_Image_GetWidth(sheet->root_image);
	sheet->sub_h = sheet->img_h / rows;
	sheet->sub_w = sheet->img_w / columns;
	return sheet;
}

int Sprite_GetWidth(SpriteSheet spritesheet) {
	return spritesheet->sub_w;
}

int Sprite_GetHeight(SpriteSheet spritesheet) {
	return spritesheet->sub_h;
}