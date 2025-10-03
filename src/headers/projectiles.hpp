#pragma once

#include <unordered_map>
#include <memory>

#include "assetManager.hpp"
#include "attributes.hpp"
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
		Projectile(float x, float y, Vector2 direction, ProjectileType type, std::shared_ptr<AttributeManager> attributes, AssetManager& assets);
		~Projectile() = default;

		void Update(Rectangle& player_rect);
		void Draw() const;

		bool Kill = false;
		ProjectileType Type;

		Rectangle Rect;
	
	private:
		// used for all types
		std::shared_ptr<AttributeManager> Attributes;

		Texture2D Image;
		float Rotation;
		float Scale;

		// used by bullet and lazer
		float Speed;
		Vector2 Direction;

		// used by circle
		float CurrentAngle;
};
