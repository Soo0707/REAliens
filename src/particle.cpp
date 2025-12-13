#include "particle.hpp"

#include <cstddef>

#include "raylib.h"
#include "constants.hpp"
#include "assetManager.hpp"

Particle::Particle(
		float x, float y, float speed, float scale, float rotation, size_t creation, size_t expiry,
		Vector2 direction, Color begin_colour, Color end_colour, AssetManager& assets) noexcept :
	Rect(x, y, scale, scale),
	Speed(speed),
	Scale(scale),
	Rotation(rotation),
	Creation(creation),
	Expiry(expiry),
	Direction(direction),
	BeginColour(begin_colour),
	EndColour(end_colour),
	Image(assets.Textures.at(TextureKey::WhitePixel))
{}

void Particle::Update(size_t ticks) noexcept
{
	this->Rect.x += this->Speed * this->Direction.x * TICK_TIME;
	this->Rect.y += this->Speed * this->Direction.y * TICK_TIME;

	float lerp_factor = static_cast<float>(this->Expiry - ticks) / static_cast<float>(this->Expiry - this->Creation);

	this->Rect.width = this->Scale * lerp_factor;
	this->Rect.height = this->Scale * lerp_factor;
}

void Particle::Draw(size_t ticks) const noexcept
{
	float lerp_factor = static_cast<float>(this->Expiry - ticks) / static_cast<float>(this->Expiry - this->Creation);
	Color colour = ColorLerp(this->BeginColour, this->EndColour, lerp_factor);

	DrawTexturePro(
			this->Image,
			(Rectangle) { 0, 0, 1, 1 },
			this->Rect,
			(Vector2) {0, 0},
			this->Rotation,
			colour
			);

}

void Particle::DrawLightmap() const noexcept
{}
