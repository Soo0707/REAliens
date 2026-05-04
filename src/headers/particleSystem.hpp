/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

#include "raylib.h" 
#include "messageSystem.hpp"
#include "assetManager.hpp"

class ParticleSystem
{
	public:
		ParticleSystem();
		~ParticleSystem() = default;
		
		void Update(MessageSystem& message_system, const AssetManager& assets, const Rectangle& update_area, const size_t ticks) noexcept;
		void Reset() noexcept;
		void Draw(const AssetManager& assets) const noexcept;

		size_t GetEntityCount() const noexcept;

	private:
		void ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept;
		void VisibilityCheck(const Rectangle& update_area) noexcept;

		void CreateParticle(
				const float x, const float y, const float scale, const float rotation, const size_t creation,
				const size_t expiry, const Vector2 velocity, const Color start_colour, const Color end_colour
				) noexcept;

		void RemoveParticles(const size_t ticks) noexcept;
		void Clear() noexcept;
		void MoveAndScaleParticles(const size_t ticks) noexcept;
		void ColourParticles(const size_t ticks) noexcept;

		std::vector<uint8_t> ParticleIsVisible;
		std::vector<size_t> ParticleCreation;
		std::vector<size_t> ParticleExpiry;
		std::vector<float> ParticleRotation;
		std::vector<float> ParticleScale;
		std::vector<Vector2> ParticleVelocity;
		std::vector<Color> ParticleStartColour;
		std::vector<Color> ParticleCurrentColour;
		std::vector<Color> ParticleEndColour;
		std::vector<Rectangle> ParticleRect;
};

