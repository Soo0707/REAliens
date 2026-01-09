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
			(ProjectileData) { CYAN, TextureKey::Lazer }
		};

		std::vector<uint8_t> ProjectileIsVisible;

		std::vector<float> ProjectileSpeed;

		std::vector<float> ProjectileRotation;
		std::vector<float> ProjectileScale;

		std::vector<Rectangle> ProjectileRect;
		std::vector<Vector2> ProjectileDirection;
		std::vector<Color> ProjectileColour;

		std::vector<ProjectileType> ProjectileTypes;

		std::vector<TextureKey> ProjectileTexture;
};
