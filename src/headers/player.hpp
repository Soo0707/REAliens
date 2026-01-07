#pragma once

#include <cstddef>
#include <array>

#include "raylib.h"
#include "assetManager.hpp"
#include "constants.hpp"
#include "messageSystem.hpp"

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
		
		void PollSignals(MessageSystem& message_system) noexcept;

		void Move(MessageSystem& message_system, const float slide_speed) noexcept;
		void Update(MessageSystem& message_system, const size_t ticks, const float slide_speed) noexcept;
		void Animate(const size_t ticks) noexcept;

		void Draw() const noexcept;
		void DrawLightmap() const noexcept;

		void IncreaseHealth(const float addition) noexcept;
		void Reset() noexcept;
		
		float Health = 100;
		float HealthMax = 100;

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle Rect;

		Vector2 Centre;
		const float Radius = 10;

		float Speed = 300;
		bool Sliding = false;
		size_t SlideExpire = 0;

	private:
		Texture2D Image;

		Bearing Bearing = Bearing::South;

		void IncreasePlotArmour(const unsigned int times) noexcept;
		void ApplySpeedBoots(const unsigned int times) noexcept;
		void PoisonTick(const unsigned int times) noexcept;

		std::array<void(Player::*)(const unsigned int) noexcept, static_cast<size_t>(PlayerSignal::COUNT)> SignalHandlers = 
		{
			&IncreasePlotArmour,
			&ApplySpeedBoots,
			&PoisonTick
		};

		size_t LastAnimationUpdate = 0;
		unsigned int AnimationFrames = 2;
		unsigned int ImageIndex = 0;
		unsigned int AnimationInterval = 20;

		void SetBearing() noexcept;
};
