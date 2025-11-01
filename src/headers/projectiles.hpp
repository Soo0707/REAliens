#pragma once

#include <cstddef>
#include <array>

#include "assetManager.hpp"
#include "globalDataWrapper.hpp"
#include "raylib.h"


enum class ProjectileType : size_t
{
	Bullet,
	Lazer,
	Ball,
	COUNT
};

class Projectile
{
	public:
		Projectile(float x, float y, const Vector2& direction, ProjectileType type, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept;
		~Projectile() = default;

		void Update(const Vector2& player_centre) noexcept;
		void Draw() const noexcept;
		void DrawLightmap() const noexcept;

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
	private:
		void BulletConstructor(float x, float y, const Vector2& direction, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept;
		void LazerConstructor(float x, float y, const Vector2& direction, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept;
		void BallConstructor(float x, float y, const Vector2& direction, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept;

		void BulletUpdate(const Vector2& player_centre) noexcept;
		void LazerUpdate(const Vector2& player_centre) noexcept;
		void BallUpdate(const Vector2& player_centre) noexcept;
		
		void BulletDrawLightmap() const noexcept;
		void LazerDrawLightmap() const noexcept;
		void BallDrawLightmap() const noexcept;

		using ConstructorHook = void(Projectile::*)(float, float, const Vector2&, const GlobalDataWrapper&, const AssetManager&) noexcept;
		static inline constexpr std::array<ConstructorHook, static_cast<size_t>(ProjectileType::COUNT)> ConstructorHooks =
		{
			&BulletConstructor,
			&LazerConstructor,
			&BallConstructor
		};

		static inline constexpr std::array<void(Projectile::*)(const Vector2&), static_cast<size_t>(ProjectileType::COUNT)> UpdateHooks = 
		{
			&BulletUpdate,
			&LazerUpdate,
			&BallUpdate
		};

		static inline constexpr std::array<void(Projectile::*)() const noexcept, static_cast<size_t>(ProjectileType::COUNT)> DrawLightmapHooks =
		{
			&BulletDrawLightmap,
			&LazerDrawLightmap,
			&BallDrawLightmap
		};

		Texture2D Image;
};
