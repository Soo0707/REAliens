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
#include "commands.hpp"
#include "modifiers.hpp"
#include "modifierSystem.hpp"

class ProjectileSystem
{
	public:
		ProjectileSystem();
		~ProjectileSystem() = default;

		void Reset() noexcept;

		void Update(
				MessageSystem& message_system, const AssetManager& assets, const ModifierSystem& modifier_system,
				const Rectangle& update_area, const float map_width, const float map_height, const size_t ticks
				) noexcept;

		void Draw(const AssetManager& assets) const noexcept;
		void DrawLightmap() const noexcept;

		const std::vector<Vector2>& GetProjectileCentres() const noexcept;
		const std::vector<ProjectileType>& GetProjectileTypes() const noexcept;
		const std::vector<Vector2>& GetProjectileDirection() const noexcept;
		const std::vector<float>& GetProjectileRotation() const noexcept;
		size_t GetEntityCount() const noexcept;

	private:
		void CreateProjectile(
				const float x, const float y, const Vector2 direction, const ProjectileType type,
				const ModifierSystem& modifier_system, const AssetManager& assets
				) noexcept;

		void ExecuteCommands(MessageSystem& message_system, const ModifierSystem& modifier_system, const AssetManager& assets) noexcept;

		void VisibilityCheck(const Rectangle& update_area) noexcept;
		void MoveProjectiles(const float map_width, const float map_height) noexcept;
		void KillProjectiles() noexcept;
		void SpawnParticles(MessageSystem& message_system, const size_t ticks) const noexcept;

		bool CheckIndex(const uint32_t index) const noexcept;

		static inline constexpr std::array<ProjectileData, static_cast<size_t>(ProjectileType::COUNT)> ProjectileAttributes = 
		{
			(ProjectileData) { YELLOW, TextureKey::Bullet, Attribute::BulletSpeed, Attribute::BulletPenetration },
			(ProjectileData) { CYAN, TextureKey::Lazer, Attribute::LazerSpeed, Attribute::LazerPenetration },
			(ProjectileData) { TEAL, TextureKey::Ball, Attribute::BallSpeed, Attribute::BallPenetration },
			(ProjectileData) { LIGHTGRAY, TextureKey::Glue, Attribute::GlueSpeed, Attribute::GluePenetration },
			(ProjectileData) { LIGHTRED, TextureKey::Plebifier, Attribute::PlebifierSpeed, Attribute::PlebifierPenetration }
		};

		void CreateProjectileHandler(
				MessageSystem& message_system, const ModifierSystem& modifier_system,
				const AssetManager& assets, const ProjectileSystemCommand& command 
				) noexcept;

		void ProjectileHitHandler(
				MessageSystem& message_system, const ModifierSystem& modifier_system,
				const AssetManager& assets, const ProjectileSystemCommand& command 
				) noexcept;

		using CommandHandler = void(ProjectileSystem::*)(MessageSystem&, const ModifierSystem&, const AssetManager&, const ProjectileSystemCommand&) noexcept;
		static inline constexpr std::array<CommandHandler, 2> CommandHandlers = 
		{
			&ProjectileSystem::CreateProjectileHandler,
			&ProjectileSystem::ProjectileHitHandler
		};

		void BallHitHandler(MessageSystem&, const ModifierSystem&, const AssetManager& assets, const ProjectileHit&) noexcept;
		void GlueHitHandler(MessageSystem&, const ModifierSystem&, const AssetManager& assets, const ProjectileHit&) noexcept;
		void PlebifierHitHandler(MessageSystem&, const ModifierSystem&, const AssetManager& assets, const ProjectileHit&) noexcept;

		using AfterProjectileHit = void(ProjectileSystem::*)(MessageSystem&, const ModifierSystem&, const AssetManager&, const ProjectileHit&) noexcept;

		static inline constexpr std::array<AfterProjectileHit, static_cast<size_t>(ProjectileType::COUNT)> AfterProjectileHitHandlers =
		{
			nullptr,
			nullptr,
			&ProjectileSystem::BallHitHandler,
			&ProjectileSystem::GlueHitHandler,
			&ProjectileSystem::PlebifierHitHandler
		};

		std::vector<uint8_t> ProjectileIsVisible;
		std::vector<float> ProjectileSpeed;
		std::vector<float> ProjectileRotation;
		std::vector<uint16_t> ProjectileHitsLeft;
		std::vector<Rectangle> ProjectileRect;
		std::vector<Vector2> ProjectileCentre;
		std::vector<Vector2> ProjectileDirection;
		std::vector<ProjectileType> ProjectileTypes;
};
