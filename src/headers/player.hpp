#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

#include "raylib.h"
#include "assetManager.hpp"
#include "constants.hpp"
#include "messageSystem.hpp"

enum class Bearing : uint16_t
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
		void ExecuteCommands(MessageSystem& message_system) noexcept;
		
		void Move(MessageSystem& message_system, const float slide_speed) noexcept;
		void Update(MessageSystem& message_system, const size_t ticks, const float slide_speed) noexcept;
		void Animate(const size_t ticks) noexcept;

		void Draw() const noexcept;
		void DrawLightmap() const noexcept;

		void Reset() noexcept;
		
		float Health;
		float HealthMax;

		Vector2 Direction;
		Rectangle Rect;

		Vector2 Centre;
		float Radius;

		float Speed;
		bool Sliding;

	private:
		Texture2D Image;

		Bearing Bearing;

		void IncreaseHealth(const PlayerCommand& command) noexcept;
		void TakeDamage(const PlayerCommand& command) noexcept;
		void SetDirection(const PlayerCommand& command) noexcept;

		static constexpr std::array<void(Player::*)(const PlayerCommand&) noexcept, 4> CommandHandlers = 
		{
			&TakeDamage,
			&IncreaseHealth,
			&SetDirection
		};

		void IncreasePlotArmour(const uint16_t times) noexcept;
		void ApplySpeedBoots(const uint16_t times) noexcept;
		void ApplySlide(const uint16_t times) noexcept;
		void RemoveSlide(const uint16_t times) noexcept;

		static constexpr std::array<void(Player::*)(const uint16_t) noexcept, static_cast<size_t>(PlayerSignal::COUNT)> SignalHandlers = 
		{
			&IncreasePlotArmour,
			&ApplySpeedBoots,
			&ApplySlide,
			&RemoveSlide
		};

		size_t LastAnimationUpdate;
		uint8_t ImageIndex;
		static constexpr uint8_t AnimationFrames = 2;
		static constexpr uint8_t AnimationInterval = 20;

		void SetBearing() noexcept;
};
