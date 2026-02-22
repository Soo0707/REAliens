#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

#include "raylib.h"
#include "assetManager.hpp"
#include "constants.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"

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
		Player();
		~Player() = default; 
		
		void PollSignals(MessageSystem& message_system, const ModifierSystem& modifier_system) noexcept;
		void ExecuteCommands(MessageSystem& message_system, const ModifierSystem& modifier_system) noexcept;
		
		void Move(MessageSystem& message_system, const float slide_speed) noexcept;
		void Update(MessageSystem& message_system, const size_t ticks, const float slide_speed) noexcept;
		void Animate(const size_t ticks) noexcept;

		void Draw(const AssetManager& assets) const noexcept;
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
		Bearing Bearing;

		void IncreaseHealth(const PlayerCommand& command, const ModifierSystem& modifier_system) noexcept;
		void TakeDamage(const PlayerCommand& command, const ModifierSystem& modifier_system) noexcept;
		void SetDirection(const PlayerCommand& command, const ModifierSystem& modifier_system) noexcept;

		static constexpr std::array<void(Player::*)(const PlayerCommand&, const ModifierSystem&) noexcept, 3> CommandHandlers = 
		{
			&TakeDamage,
			&IncreaseHealth,
			&SetDirection
		};

		void IncreasePlotArmour(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system)noexcept;
		void ApplySpeedBoots(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept;
		void ApplySlide(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept;
		void RemoveSlide(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept;
		void SpawnBall(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept;

		static constexpr std::array<void(Player::*)(MessageSystem&, const uint16_t, const ModifierSystem&) noexcept, static_cast<size_t>(PlayerSignal::COUNT)> SignalHandlers = 
		{
			&IncreasePlotArmour,
			&ApplySpeedBoots,
			&ApplySlide,
			&RemoveSlide,
			&SpawnBall
		};

		size_t LastAnimationUpdate;
		uint8_t ImageIndex;
		static constexpr uint8_t AnimationFrames = 2;
		static constexpr uint8_t AnimationInterval = 20;

		void SetBearing() noexcept;
};
