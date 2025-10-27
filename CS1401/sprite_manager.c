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

	// texture coordinates starting point
	int u0 = sheet->current_column * sheet->sub_w;
	int v0 = sheet->current_row * sheet->sub_h;

	// draw the sub image of this sprite
	CP_Image_DrawSubImageAdvanced(sheet->root_image, x, y, w, h, (float) u0, (float) v0, (float) (u0 + sheet->sub_w), (float) (v0 + sheet->sub_h), alpha, degrees);

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