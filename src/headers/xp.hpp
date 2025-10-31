#pragma once

#include "raylib.h"
#include "assetManager.hpp"

class Xp
{
	public:
		Xp(float pos_x, float pos_y, unsigned int value, AssetManager &assets) noexcept;
		~Xp() = default;

		void Draw() const noexcept;
		void DrawLightmap() const noexcept;

		Rectangle Rect;
		bool Kill = false;
		unsigned int Value;

	private:
		Texture2D Image;
		Vector2 Centre;
};
