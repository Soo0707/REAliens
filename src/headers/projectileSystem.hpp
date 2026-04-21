/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <cstddef>

#include "raylib.h"
#include "messageSystem.hpp"
#include "projectileData.hpp"
#include "assetManager.hpp"
#include "modifierSystem.hpp"

class ProjectileSystem
{
	public:
		ProjectileSystem();
		~ProjectileSystem() = default;

		void Reset() noexcept;

		void ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept;

		void Update(
				MessageSystem& message_system, const AssetManager& assets, const ModifierSystem& modifier_system,
				const Rectangle& update_area, const size_t ticks
				) noexcept;

		void Draw(const AssetManager& assets) const noexcept;
		void DrawLightmap() const noexcept;

		const std::vector<Vector2>& GetProjectileCentres() const noexcept;
		const std::vector<ProjectileType>& GetProjectileTypes() const noexcept;
		const std::vector<Vector2>& GetProjectileDirection() const noexcept;
		const std::vector<float>& GetProjectileRotation() const noexcept;

	private:
		void CreateProjectile(
				const float x, const float y, const float speed, const Vector2 direction,
				const ProjectileType type, const float texture_width, const float texture_height
				) noexcept;

		void VisibilityCheck(const Rectangle& update_area) noexcept;
		void MoveProjectiles() noexcept;
		void RemoveProjectiles() noexcept;
		void SpawnParticles(MessageSystem& message_system, const size_t ticks) const noexcept;
		void EvaluateHitCount(const ModifierSystem& modifier_system) noexcept;

		bool CheckIndex(const size_t index) const noexcept;

		static inline constexpr std::array<ProjectileData, static_cast<size_t>(ProjectileType::COUNT)> ProjectileAttributes = 
		{
			(ProjectileData) { YELLOW, TextureKey::Bullet },
			(ProjectileData) { CYAN, TextureKey::Lazer },
			(ProjectileData) { TEAL, TextureKey::Ball }
		};

		void CreateProjectileHandler(const ProjectileSystemCommand& command, const AssetManager& assets) noexcept;
		void ProjectileHitHandler(const ProjectileSystemCommand& command, const AssetManager& assets) noexcept;

		static inline constexpr std::array<void(ProjectileSystem::*)(const ProjectileSystemCommand&, const AssetManager&) noexcept, 2> CommandHandlers = 
		{
			&ProjectileSystem::CreateProjectileHandler,
			&ProjectileSystem::ProjectileHitHandler
		};

		std::vector<uint8_t> ProjectileIsVisible;
		std::vector<uint8_t> ProjectileKill;
		std::vector<float> ProjectileSpeed;
		std::vector<float> ProjectileRotation;
		std::vector<uint16_t> ProjectileHitCount;
		std::vector<Rectangle> ProjectileRect;
		std::vector<Vector2> ProjectileCentre;
		std::vector<Vector2> ProjectileDirection;
		std::vector<Color> ProjectileColour;
		std::vector<ProjectileType> ProjectileTypes;
		std::vector<TextureKey> ProjectileTexture;
};
