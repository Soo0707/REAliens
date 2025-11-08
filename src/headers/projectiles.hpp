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
	COUNT
};

class Projectile
{
	public:
		Projectile(float x, float y, const Vector2& direction, ProjectileType type, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept;
		~Projectile() = default;

		void Update() noexcept;
		void Draw() const noexcept;
		void DrawLightmap() const noexcept;

		bool Kill = false;
		ProjectileType Type;

		float Rotation;
		float Scale;

		float Speed;
		Vector2 Direction;

		Rectangle Rect;

	private:
		void BulletConstructor(float x, float y, const Vector2& direction, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept;
		void LazerConstructor(float x, float y, const Vector2& direction, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept;

		void BulletUpdate() noexcept;
		void LazerUpdate() noexcept;
		
		void BulletDrawLightmap() const noexcept;
		void LazerDrawLightmap() const noexcept;

		using ConstructorHook = void(Projectile::*)(float, float, const Vector2&, const GlobalDataWrapper&, const AssetManager&) noexcept;
		static inline constexpr std::array<ConstructorHook, static_cast<size_t>(ProjectileType::COUNT)> ConstructorHooks =
		{
			&BulletConstructor,
			&LazerConstructor
		};

		static inline constexpr std::array<void(Projectile::*)(), static_cast<size_t>(ProjectileType::COUNT)> UpdateHooks = 
		{
			&BulletUpdate,
			&LazerUpdate
		};

		static inline constexpr std::array<void(Projectile::*)() const noexcept, static_cast<size_t>(ProjectileType::COUNT)> DrawLightmapHooks =
		{
			&BulletDrawLightmap,
			&LazerDrawLightmap
		};

		Texture2D Image;
};
