#pragma once

#include "raylib.h"

enum class ProjectileType
{
	Bullet,
	Lazer,
	Circle
};

class Projectile
{
	public:
		Projectile(float pos_x, float pos_y, Texture2D image, unsigned int speed, Vector2 direction, float rotation, float scale, ProjectileType type, float damage);
		~Projectile() = default;

		void Update();
		void Draw() const;

		bool Kill = false;
		ProjectileType Type;

		Rectangle Rect;
		float Damage;
	
	private:
		Texture2D Image;
		unsigned int Speed;
		Vector2 Direction;
		float Rotation;
		float Scale;
};
