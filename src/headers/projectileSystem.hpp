#pragma once

#include <vector>
#include <cstddef>

#include "raylib.h"
#include "messageSystem.hpp"
#include "projectiles.hpp"
#include "assetManager.hpp"

class ProjectileSystem
{
	public:
		ProjectileSystem();
		~ProjectileSystem() = default;

		void Reset() noexcept;

		void ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept;

		void UpdateProjectiles(const size_t ticks, const Rectangle update_area, MessageSystem& message_system) noexcept;
		void Draw(const Rectangle update_area) const noexcept;
		void DrawLightmap(const Rectangle update_area) const noexcept;

	private:
		std::vector<Projectile> Projectiles;
};
