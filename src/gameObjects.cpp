#include "gameObjects.hpp"
#include "raylib.h"
#include <cstddef>

Prop::Prop(float pos_x, float pos_y, Texture2D image):
	Rect({pos_x, pos_y, (float) image.width, (float) image.height}), Image(image)
{}

void Prop::Draw() const
{
	DrawTexture(this->Image, (int) this->Rect.x, (int) this->Rect.y, WHITE);
}
