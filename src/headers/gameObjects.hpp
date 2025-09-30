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

class Wall
{
	public:
		Wall(float pos_x, float pos_y, Texture2D image);
		~Wall() = default;
		Rectangle Rect;

		void Draw() const;
	
	private:
		bool Destroyed = false;
		Texture2D Image;
};
