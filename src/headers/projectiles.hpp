#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include "assetManager.hpp"
#include "globalDataWrapper.hpp"
#include "raylib.h"


enum class ProjectileType
{
	Bullet,
	Lazer,
	Ball
};

class Projectile
{
	public:
		Projectile(float x, float y, Vector2 direction, ProjectileType type, GlobalDataWrapper& global_data, AssetManager& assets) noexcept;
		~Projectile() = default;

		void Update(const Vector2 player_centre) noexcept;
		void Draw() const noexcept;

		bool Kill = false;
		ProjectileType Type;

		float Rotation;
		float Scale;

		// used by bullet and lazer
		float Speed;
		Vector2 Direction;

		// used by circle
		float CurrentAngle;
		float Radius;

		Rectangle Rect;

		int Layer;
	private:
		Texture2D Image;
};
