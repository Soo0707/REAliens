#pragma once

#include "game.hpp"
#include "assetManager.hpp"
#include "globalDataWrapper.hpp"
#include "modifierSystem.hpp"
#include "timerSystem.hpp"

namespace GameDrawSystem
{
	void DrawGame(const Game& game, const ModifierSystem& modifier_system, const AssetManager& assets) noexcept;
	void DrawLighting(const Game& game, const ModifierSystem& modifier_system) noexcept;
	void DrawScreenLayer(const Game& game) noexcept;
	void DrawOverlay(
			const Game& game, const TimerSystem& timer_system, const ModifierSystem& modifier_system,
			const GlobalDataWrapper& global_data, const AssetManager& assets
			) noexcept;

	void DrawGreenbull(const TimerSystem& timer_system, const AssetManager& assets, const size_t ticks) noexcept;
	void DrawMilk(const TimerSystem& timer_system, const AssetManager& assets, const size_t ticks) noexcept;
	void DrawDrunk(const TimerSystem& timer_system, const AssetManager& assets, const size_t ticks) noexcept;
	void DrawMagnetism(const TimerSystem& timer_system, const AssetManager& assets, const size_t ticks) noexcept;
}
