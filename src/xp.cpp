#include "xp.hpp"
#include "assetManager.hpp"
#include "raylib.h"

Xp::Xp(float pos_x, float pos_y, unsigned int value, AssetManager &assets) :
	Image(assets.StaticTextures[StaticTextureKey::Xp]),
	Value(value),
	Rect( {pos_x, pos_y, (float) this->Image.width, (float) this->Image.height} )
{}

void Xp::Draw() const
{
	DrawTexture(this->Image, (int) this->Rect.x, (int) this->Rect.y, WHITE);
}
