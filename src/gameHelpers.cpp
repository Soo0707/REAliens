#include "gameHelpers.hpp"

#include <vector>
#include <cstddef>

#include "raylib.h"

#include "constants.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"
#include "gameState.hpp"

void GameHelper::LoopOverMap(float ground_width, float ground_height, Rectangle& m_obj) noexcept
{
	if (m_obj.x < 0)
		m_obj.x = ground_width - m_obj.width;
	else if (m_obj.x + m_obj.width > ground_width)
		m_obj.x = 0;

	if (m_obj.y < 0)
		m_obj.y = ground_height - m_obj.height;
	else if (m_obj.y + m_obj.height > ground_height)
		m_obj.y = 0;
}



void GameHelper::SpawnEnemies(GameState& game_state, const AssetManager& assets) noexcept
{
	const size_t level = game_state.Level;
	const size_t spawn_count = level * 15;

	game_state.Enemies.reserve(game_state.Enemies.size() + spawn_count);

	static std::vector<EnemyType> types;
	types.reserve(spawn_count);

	GameHelper::SpawnTypeFunctions[GetRandomValue(0, 1)](spawn_count, types);

	static std::vector<BehaviourModifier> modifiers;
	modifiers.reserve(spawn_count);

	GameHelper::GenerateModifierFunctions[GetRandomValue(0, 2)](spawn_count, level, modifiers);

	const float level_scale = 1 + static_cast<float>(level) / 10.0f;

	for (size_t i = 0; i < spawn_count; i++)
		game_state.Enemies.emplace_back(game_state.SpawnLocations[i].x, game_state.SpawnLocations[i].y, level_scale, assets, types[i], modifiers[i]);

	types.clear();
	modifiers.clear();
	game_state.SpawnLocations.clear();
}


void GameHelper::RandomLocation(GameState& game_state, const AssetManager& assets) noexcept
{
	const float ground_width = assets.Ground.width;
	const float ground_height = assets.Ground.height;

	const size_t level = game_state.Level;
	const size_t spawn_count = level * 15;

	game_state.SpawnLocations.reserve(spawn_count);

	for (size_t i = 0; i < spawn_count; i++)
	{
		game_state.SpawnLocations.emplace_back(
				(Vector2) {
				static_cast<float>( GetRandomValue(0, ground_width) ),
				static_cast<float>( GetRandomValue(0, ground_height) )
				});
	}
}


void GameHelper::NoModifiers(size_t spawn_count, size_t level, std::vector<BehaviourModifier>& modifiers) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
		modifiers.emplace_back(BehaviourModifier::None);
}

void GameHelper::SameModifiers(size_t spawn_count, size_t level, std::vector<BehaviourModifier>& modifiers) noexcept
{
	BehaviourModifier modifier = BehaviourModifier::None;

	const unsigned int random = GetRandomValue(0, 100);

	if (random > 10 && random < 30)
		modifier = modifier | BehaviourModifier::IncreasedSpeed;

	if (random <= 5 && level > 50)
		modifier = modifier | BehaviourModifier::Big;

	for (size_t i = 0; i < spawn_count; i++)
		modifiers.emplace_back(modifier);
}

void GameHelper::RandomModifiers(size_t spawn_count, size_t level, std::vector<BehaviourModifier>& modifiers) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
	{
		BehaviourModifier modifier = BehaviourModifier::None;
		
		const unsigned int random = GetRandomValue(0, 100);

		if (random > 10 && random < 50)
			modifier = modifier | BehaviourModifier::IncreasedSpeed;

		if (random <= 20 && level > 10)
			modifier = modifier | BehaviourModifier::Big;

		modifiers.emplace_back(modifier);
	}
}


void GameHelper::SingleType(size_t spawn_count, std::vector<EnemyType>& types) noexcept
{
	const EnemyType type = static_cast<EnemyType>(GetRandomValue(0, static_cast<int>(EnemyType::COUNT) - 1));

	for (size_t i = 0; i < spawn_count; i++)
		types.emplace_back(type);
}

void GameHelper::RandomType(size_t spawn_count, std::vector<EnemyType>& types) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
		types.emplace_back(static_cast<EnemyType>(GetRandomValue(0, static_cast<int>(EnemyType::COUNT) - 1)));
}
