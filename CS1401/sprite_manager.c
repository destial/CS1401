/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 23 September 2025
@course CSD1401
@brief Definition file for implementing a sprite manager that will handle graphic animations
*/

#include "sprite_manager.h"

void DrawSprite(SpriteSheet* sheet, float x, float y, float w, float h, int alpha) {
	// just renders with 0 degrees of rotation
	DrawSpriteAdvanced(sheet, x, y, w, h, alpha, 0);
}

void DrawSpriteAdvanced(SpriteSheet* sheet, float x, float y, float w, float h, int alpha, float degrees) {

	// get the root image width & height
	int img_h = CP_Image_GetHeight(sheet->root_image);
	int img_w = CP_Image_GetWidth(sheet->root_image);

	// get each sub image width and height according to rows and columns
	int sub_h = img_h / sheet->rows;
	int sub_w = img_w / sheet->columns;

	// the current frame of this sprite being rendered
	int current_r = sheet->current_row;
	int current_c = sheet->current_column;

	// texture coordinates starting point
	int u0 = current_c * sub_w;
	int v0 = current_r * sub_h;

	// draw the sub image of this sprite
	CP_Image_DrawSubImageAdvanced(sheet->root_image, x, y, w, h, u0, v0, u0 + sub_w, v0 + sub_h, alpha, degrees);

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