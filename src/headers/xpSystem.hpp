#pragma once

#include <vector>
#include <cstddef>

#include "raylib.h"
#include "assetManager.hpp"
#include "messageSystem.hpp"
#include "xp.hpp"

class XpSystem
{
	public:
		XpSystem();
		~XpSystem() = default;

		void Reset() noexcept;

		void ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept;
		void UpdateXps(const size_t ticks, const Rectangle update_area, MessageSystem& message_system) noexcept;
		void Draw(const Rectangle update_area) const noexcept;
		void DrawLightmap(const Rectangle update_area) const noexcept;

	private:
		std::vector<Xp> Xps;
};
