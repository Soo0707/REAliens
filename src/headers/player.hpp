#pragma once

#include "gameObjects.hpp"
#include "raylib.h"

enum class Bearing
{
	North,
	South,
	East,
	West
};

class Player : public UpdatableGameObject
{
	public:
		Player(float pos_x, float pos_y, Texture2D initial_image);
		~Player(); //override;
		void Draw(); //override;
		void Update(); //override;

	private:
		unsigned int ImageIndex;
		Vector2 Direction;

		Bearing Bearing;

		int Health;
		unsigned int Speed;

		void Animate();
		void MoveX();
		void MoveY();
		void UpdateBearing();
};
