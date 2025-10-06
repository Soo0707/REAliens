#pragma once

#include "raylib.h"
#include "assetManager.hpp"

class Xp
{
	public:
		Xp(float pos_x, float pos_y, unsigned int value, AssetManager &assets);
		~Xp() = default; 

		Rectangle Rect;
	private:
		Texture2D Image;
		unsigned int value;
};
