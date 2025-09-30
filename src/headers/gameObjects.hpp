#pragma once

#include "raylib.h"
#include <cstddef>

class Prop
{
	public:
		Prop(float pos_x, float pos_y, Texture2D image);
		~Prop() = default;
		void Draw() const;
		Rectangle Rect;

	private:
		Texture2D Image;
};
