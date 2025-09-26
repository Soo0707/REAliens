#pragma once

#include "raylib.h"
#include "gameObjects.hpp"

class Projectile : public UpdatableGameObject
{
	public:
		Projectile(float pos_x, float pos_y, Texture2D initial_image, unsigned int speed, Vector2 Direction, float rotation, float Scale);
		~Projectile() = default;
		void Update() override;
		void Draw() const override;
	
	private:
		unsigned int Speed;
		Vector2 Direction;
		float Rotation;
		float Scale;
};
