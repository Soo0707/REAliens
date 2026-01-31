#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <cstddef>

#include "raylib.h"
#include "messageSystem.hpp"
#include "projectileData.hpp"
#include "assetManager.hpp"

class ProjectileSystem
{
	public:
		ProjectileSystem();
		~ProjectileSystem() = default;

		void Reset() noexcept;

		void ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept;

		void UpdateProjectiles(const size_t ticks, const Rectangle update_area, MessageSystem& message_system) noexcept;
		void Draw(const AssetManager& assets) const noexcept;
		void DrawLightmap() const noexcept;

		const std::vector<Rectangle>& GetProjectileRect() const noexcept;
		const std::vector<ProjectileType>& GetProjectileType() const noexcept;
		const std::vector<Vector2>& GetProjectileDirection() const noexcept;

	private:
		void CreateProjectile(
				const float x, const float y, const float speed, const float scale, const Vector2 direction,
				const ProjectileType type, const float texture_width, const float texture_height
				) noexcept;
		void VisibilityCheck(const Rectangle update_area) noexcept;
		void MoveProjectiles() noexcept;
		void RemoveProjectiles() noexcept;
		void SpawnParticles(MessageSystem& message_system, const size_t ticks) const noexcept;

		static inline constexpr std::array<ProjectileData, static_cast<size_t>(ProjectileType::COUNT)> ProjectileAttributes = 
		{
			(ProjectileData) { YELLOW, TextureKey::Bullet },
			(ProjectileData) { CYAN, TextureKey::Lazer },
			(ProjectileData) { GREEN, TextureKey::Ball }
		};

		void CreateProjectileHandler(const ProjectileSystemCommand& command, const AssetManager& assets) noexcept;
		void ProjectileHitHandler(const ProjectileSystemCommand& command, const AssetManager& assets) noexcept;

		static inline constexpr std::array<void(ProjectileSystem::*)(const ProjectileSystemCommand&, const AssetManager&) noexcept, 2> CommandHandlers = 
		{
			&CreateProjectileHandler,
			&ProjectileHitHandler
		};

		std::vector<uint8_t> ProjectileIsVisible;
		std::vector<uint8_t> ProjectileKill;

		std::vector<float> ProjectileSpeed;

		std::vector<float> ProjectileRotation;
		std::vector<float> ProjectileScale;

		std::vector<uint16_t> ProjectileKillCount;

		std::vector<Rectangle> ProjectileRect;
		std::vector<Vector2> ProjectileDirection;
		std::vector<Color> ProjectileColour;

		std::vector<ProjectileType> ProjectileTypes;

		std::vector<TextureKey> ProjectileTexture;
};
