#pragma once

#include "raylib.h"

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

class Spawner
{
	public:
		Spawner(float pos_x, float pos_y, Texture2D image);
		~Spawner() = default;
		Rectangle Rect;

		void Draw() const;
	
	private:
		Texture2D Image;
};
