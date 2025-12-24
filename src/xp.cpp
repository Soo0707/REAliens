#include "xp.hpp"
#include "assetManager.hpp"
#include "raylib.h"

Xp::Xp(float pos_x, float pos_y, unsigned int value, const AssetManager &assets) noexcept :
	Image(assets.Textures.at(TextureKey::Xp)),
	Value(value),
	Rect( {
			pos_x,
			pos_y,
			static_cast<float>(assets.Textures.at(TextureKey::Xp).width),
			static_cast<float>(assets.Textures.at(TextureKey::Xp).height)
			} ),
	Centre( {
			this->Rect.x + this->Rect.width / 2.0f,
			this->Rect.y + this->Rect.height / 2.0f
			} )

{}

void Xp::Draw() const noexcept
{
	DrawTexture(this->Image, (int) this->Rect.x, (int) this->Rect.y, WHITE);
}

void Xp::DrawLightmap() const noexcept
{
	DrawCircleGradient(this->Centre.x, this->Centre.y, 24, GREEN, LIGHTGRAY);
}
