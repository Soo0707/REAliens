#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

#include "raylib.h"
#include "assetManager.hpp"
#include "messageSystem.hpp"

class XpSystem
{
	public:
		XpSystem();
		~XpSystem() = default;

		void Reset() noexcept;

		void ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept;
		void UpdateXps(MessageSystem& message_system, const Rectangle update_area, const size_t ticks) noexcept;

		void Draw(const AssetManager& assets) const noexcept;
		void DrawLightmap() const noexcept;

	private:
		void VisibilityCheck(const Rectangle update_area) noexcept;
		void CreateXp(const float x, const float y, const float texture_width, const float texture_height, const unsigned int value) noexcept;
		void RemoveXp() noexcept;
		void EmitParticles(MessageSystem& message_system, const size_t ticks) noexcept;

		std::vector<uint8_t> XpIsVisible;
		std::vector<uint8_t> XpKill;
		std::vector<unsigned int> XpValue;
		std::vector<Vector2> XpCentre;
		std::vector<Rectangle> XpRect;
};
