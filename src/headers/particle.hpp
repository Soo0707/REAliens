#pragma once

#include <cstddef>

#include "raylib.h"
#include "assetManager.hpp"

class Particle
{
	public:
		Particle(
				float x, float y, float scale, float rotation, size_t creation, size_t expiry,
				Vector2 direction, Color begin_colour, Color end_colour, AssetManager& assets
				) noexcept;

		~Particle() = default;

		void Update(size_t ticks) noexcept;

		void Draw(size_t ticks) const noexcept;
		void DrawLightmap() const noexcept;

		size_t Expiry;
		Rectangle Rect;

	private:
		float Rotation;
		float Scale;

		size_t Creation;
		Vector2 Direction;

		Color BeginColour;
		Color EndColour;

		Texture2D Image;
};
