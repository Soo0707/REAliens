#pragma once

#include <array>
#include <vector>
#include <cstddef>

#include "raylib.h"
#include "settingsManager.hpp"
#include "assetManager.hpp"
#include "game.hpp"

namespace GameHelper
{
	void LoopOverMap(AssetManager& assets, Rectangle& m_obj) noexcept;
	void LevelUp(Game& game, SettingsManager& settings) noexcept;
	void SpawnEnemies(Game& game) noexcept;

	void RandomLocation(size_t spawn_count, Game& game, std::vector<Vector2>& locations) noexcept;
	void ScreenLocation(size_t spawn_count, Game& game, std::vector<Vector2>& locations) noexcept;
	void NearPlayerLocation(size_t spawn_count, Game& game, std::vector<Vector2>& locations) noexcept;

	void NoModifiers(size_t spawn_count, size_t level, std::vector<BehaviourModifier>& modifiers) noexcept;
	void SameModifiers(size_t spawn_count, size_t level, std::vector<BehaviourModifier>& modifiers) noexcept;
	void RandomModifiers(size_t spawn_count, size_t level, std::vector<BehaviourModifier>& modifiers) noexcept;

	static inline constexpr std::array<void(*)(size_t, size_t, std::vector<BehaviourModifier>&) noexcept, 3> GenerateModifierFunctions = {
		&NoModifiers,
		&SameModifiers,
		&RandomModifiers
	};

	void SingleType(size_t spawn_count, std::vector<EnemyType>& types) noexcept;
	void RandomType(size_t spawn_count, std::vector<EnemyType>& types) noexcept;

	static inline constexpr std::array<void(*)(size_t, std::vector<EnemyType>&) noexcept, 2> SpawnTypeFunctions = {
		&SingleType,
		&RandomType
	};
}
