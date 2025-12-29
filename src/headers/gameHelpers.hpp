#pragma once

#include <array>
#include <vector>
#include <cstddef>

#include "raylib.h"
#include "globalDataWrapper.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"
#include "gameState.hpp"


namespace GameHelper
{
	void LoopOverMap(float ground_width, float ground_height, Rectangle& m_obj) noexcept;
	void LevelUp(GameState& game_state, const SettingsManager& settings, GlobalDataWrapper& global_data) noexcept;

	void SpawnEnemies(GameState& game_state, const AssetManager& assets) noexcept;

	void RandomLocation(GameState& game_state, const AssetManager& assets) noexcept;

	void NoModifiers(size_t spawn_count, size_t level, std::vector<BehaviourModifier>& modifiers) noexcept;
	void SameModifiers(size_t spawn_count, size_t level, std::vector<BehaviourModifier>& modifiers) noexcept;
	void RandomModifiers(size_t spawn_count, size_t level, std::vector<BehaviourModifier>& modifiers) noexcept;

	void SingleType(size_t spawn_count, std::vector<EnemyType>& types) noexcept;
	void RandomType(size_t spawn_count, std::vector<EnemyType>& types) noexcept;


	static inline constexpr std::array<void(*)(size_t, size_t, std::vector<BehaviourModifier>&) noexcept, 3> GenerateModifierFunctions = {
		&NoModifiers,
		&SameModifiers,
		&RandomModifiers
	};

	static inline constexpr std::array<void(*)(size_t, std::vector<EnemyType>&) noexcept, 2> SpawnTypeFunctions = {
		&SingleType,
		&RandomType
	};
}
