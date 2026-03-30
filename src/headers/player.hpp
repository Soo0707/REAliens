/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

#include "raylib.h"
#include "constants.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"

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
		
		void Update(
				MessageSystem& message_system, const ModifierSystem& modifier_system,
				const SettingsManager& settings, const float map_width, const float map_height,
				const size_t ticks
				) noexcept;

		void Move(MessageSystem& message_system, const ModifierSystem& modifier_system, const float map_width, const float map_height) noexcept;
		void Animate(const size_t ticks) noexcept;

		void Draw(const AssetManager& assets) const noexcept;
		void DrawLightmap() const noexcept;

		void Reset() noexcept;
		
		float Health;
		float HealthMax;

		Vector2 Direction;
		Rectangle Rect;

		Vector2 Centre;

		float Speed;
		bool Sliding;

	private:
		void IncreaseHealth(const PlayerCommand& command, const ModifierSystem& modifier_system) noexcept;
		void TakeDamage(const PlayerCommand& command, const ModifierSystem& modifier_system) noexcept;
		void SetDirection(const PlayerCommand& command, const ModifierSystem& modifier_system) noexcept;

		static constexpr std::array<void(Player::*)(const PlayerCommand&, const ModifierSystem&) noexcept, 3> CommandHandlers = 
		{
			&Player::TakeDamage,
			&Player::IncreaseHealth,
			&Player::SetDirection
		};

		void IncreasePlotArmour(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system)noexcept;
		void ApplySpeedBoots(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept;
		void ApplySlide(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept;
		void RemoveSlide(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept;
		void SpawnBall(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept;
		void DripGlue(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept;

		static constexpr std::array<void(Player::*)(MessageSystem&, const uint16_t, const ModifierSystem&) noexcept, static_cast<size_t>(PlayerSignal::COUNT)> SignalHandlers = 
		{
			&Player::IncreasePlotArmour,
			&Player::ApplySpeedBoots,
			&Player::ApplySlide,
			&Player::RemoveSlide,
			&Player::SpawnBall,
			&Player::DripGlue
		};

		size_t LastAnimationUpdate;
		
		Bearing Bearing;
		
		uint8_t ImageIndex;
		static constexpr uint8_t AnimationFrames = 2;
		static constexpr uint8_t AnimationInterval = 20;

		void SetBearing() noexcept;
};
