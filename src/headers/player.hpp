#pragma once
#include <raylib.h>
#include <map>
#include <vector>

#include "gameObject.hpp"

enum class Bearing
{
	North,
	South,
	East,
	West
};

class Player : public gameObject
{
	public:
		Player() overide;
		~Player() overide;
		void draw() overide;
		void update() overide;

	private:
		Texture2D image;
		unsigned int image_index = 0;

		Bearing bearing;
		std::map<Bearing, std::vector<Texture2D>> images = 
		{
			{ Bearing::North, {} },
			{ Bearing::South, {} },
			{ Bearing::East, {} },
			{ Bearing::West, {} }
		};

		Rectangle rect;
		Vector2 direction;

		int health;
		unsigned int speed;

		void animate();
		void load_textures();
		void move_x();
		void move_y();
		void update_bearing();
};
