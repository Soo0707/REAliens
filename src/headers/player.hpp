#pragma once
#include <raylib.h>

#include "gameObject.hpp"

enum class Bearing
{
	North,
	South,
	East,
	West
};

class Player : public GameObject
{
	public:
		Player();
		~Player(); //override;
		void draw(); //override;
		void update(); //override;

	private:
		Texture2D image;
		unsigned int image_index = 0;

		Bearing bearing;

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
