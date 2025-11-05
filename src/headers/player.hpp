#pragma once

#include "raylib.h"
#include "assetManager.hpp"
#include <cstddef>

enum class Bearing : size_t
{
	North = 0,
	South = 45,
	East = 90,
	West = 135
};

class Player
{
	public:
		Player(float pos_x, float pos_y, AssetManager &assets);
		~Player() = default; 
		
		void Move() noexcept;
		void Update(size_t ticks) noexcept;
		void Animate(size_t ticks) noexcept;

		void Draw() const noexcept;
		void DrawLightmap() const noexcept;

		void IncreaseHealth(float addition) noexcept;
		
		float Health = 100;
		float HealthMax = 100;

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle Rect;
		Rectangle Aura;
		Vector2 Centre;

		float Speed = 300;
		bool Sliding = false;
		size_t SlideExpire = 0;

	private:
		AssetManager& Assets;

		Texture2D Image;

		Bearing Bearing = Bearing::South;

		size_t LastAnimationUpdate = 0;
		unsigned int AnimationFrames = 2;
		unsigned int ImageIndex = 0;
		unsigned int AnimationSpeed = 30;

		void SetBearing() noexcept;
};
