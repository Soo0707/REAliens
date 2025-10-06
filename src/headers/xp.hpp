#pragma once

#include "raylib.h"
#include "assetManager.hpp"

class Xp
{
	public:
		Xp(float pos_x, float pos_y, unsigned int value, AssetManager &assets);
		~Xp() = default;

		void Draw() const;

		Rectangle Rect;
		bool Kill = false;
		unsigned int Value;

	private:
		Texture2D Image;
};
