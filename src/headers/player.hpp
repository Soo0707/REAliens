#pragma once

#include "raylib.h"
#include "assetManager.hpp"
#include "constants.hpp"

#include <cstddef>

enum class Bearing : size_t
{
	North = 0 * PLAYER_TEXTURE_TILE_SIZE,
	South = 1 * PLAYER_TEXTURE_TILE_SIZE,
	East = 2 * PLAYER_TEXTURE_TILE_SIZE,
	West = 3 * PLAYER_TEXTURE_TILE_SIZE 
};

class Player
{
	public:
		Player(float pos_x, float pos_y, AssetManager &assets);
		~Player() = default; 
		
		void Move(size_t* total_distance_moved) noexcept;
		void Update(size_t ticks, size_t* total_distance_moved) noexcept;
		void Animate(size_t ticks) noexcept;

		void Draw() const noexcept;
		void DrawLightmap() const noexcept;

		void IncreaseHealth(float addition) noexcept;
		void Reset() noexcept;
		
		float Health = 100;
		float HealthMax = 100;

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle Rect;
		Rectangle Aura;

		Vector2 Centre;
		const float Radius = 10;

		float Speed = 300;
		bool Sliding = false;
		size_t SlideExpire = 0;

	private:
		Texture2D Image;

		Bearing Bearing = Bearing::South;

		size_t LastAnimationUpdate = 0;
		unsigned int AnimationFrames = 2;
		unsigned int ImageIndex = 0;
		unsigned int AnimationSpeed = 30;

		void SetBearing() noexcept;
};
