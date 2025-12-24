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

void GameHelper::LevelUp(GameState& game_state, SettingsManager& settings, GlobalDataWrapper& global_data) noexcept
{
	if (!settings.Data.at(SettingKey::UnlimitedPowerups))
	{
		game_state.UnclaimedPowerups++;
		global_data.CachedStrings[CachedString::UnclaimedPowerups] = "Unclaimed Powerups: " + std::to_string(game_state.UnclaimedPowerups);
	}

	game_state.Level++;
	global_data.CachedStrings[CachedString::LevelText] = "Level: " + std::to_string(game_state.Level);

	game_state.CollectedXp = 0;
	game_state.LevelUpTreshold += 5;
	
	if (settings.Data.at(SettingKey::PowerupMenuInterrupt))
		global_data.ActiveState = State::PowerupMenu;

	if (game_state.Level % 5 == 0 && !settings.Data.at(SettingKey::DisableLevelDebuffs))
		game_state.InsertLevelDebuff(global_data);
	else
		game_state.RemoveLevelDebuff(global_data);
}


void GameHelper::SpawnEnemies(GameState& game_state, const AssetManager& assets) noexcept
{
	const size_t level = game_state.Level;
	const size_t spawn_count = level * 15;

	game_state.Enemies.reserve(game_state.Enemies.size() + spawn_count);

	static std::vector<Vector2> locations;
	locations.reserve(spawn_count);

	if (level > 100 && !(level % 5))
	{
		Rectangle update_area = game_state.UpdateArea;
		GameHelper::ScreenLocation(update_area, spawn_count, locations);
	}
	else if (!(level % 5))
	{
		const Vector2 player_location = (Vector2) { game_state.m_Player->Rect.x, game_state.m_Player->Rect.y };

		GameHelper::NearPlayerLocation(spawn_count, player_location, locations);
	}
	else
	{
		const float ground_width = assets.Ground.width;
		const float ground_height = assets.Ground.height;

		GameHelper::RandomLocation(spawn_count, ground_width, ground_height, locations);
	}

	static std::vector<EnemyType> types;
	types.reserve(spawn_count);

	GameHelper::SpawnTypeFunctions[GetRandomValue(0, 1)](spawn_count, types);

	static std::vector<BehaviourModifier> modifiers;
	modifiers.reserve(spawn_count);

	GameHelper::GenerateModifierFunctions[GetRandomValue(0, 2)](spawn_count, level, modifiers);

	const float level_scale = 1 + static_cast<float>(level) / 10.0f;

	for (size_t i = 0; i < spawn_count; i++)
		game_state.Enemies.emplace_back(locations[i].x, locations[i].y, level_scale, assets, types[i], modifiers[i]);

	locations.clear();
	types.clear();
	modifiers.clear();
}


void GameHelper::ScreenLocation(Rectangle update_area, size_t spawn_count, std::vector<Vector2>& locations) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
	{
		locations.emplace_back(
				(Vector2) {
				static_cast<float>( GetRandomValue(update_area.x, update_area.x + REFERENCE_WIDTH) ),
				static_cast<float>( GetRandomValue(update_area.y, update_area.y + REFERENCE_HEIGHT) )
				});
	}
}

void GameHelper::NearPlayerLocation(size_t spawn_count, Vector2 player_location, std::vector<Vector2>& locations) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
	{
		unsigned int rand_x = GetRandomValue(32, 64);
		unsigned int rand_y = GetRandomValue(32, 64);

		rand_x = (GetRandomValue(-6, 18) <= 0) ? (rand_x * -1) : (rand_x);
		rand_y = (GetRandomValue(-12, 12) <= 0) ? (rand_y * -1) : (rand_y);

		locations.emplace_back(
				(Vector2) {
					static_cast<float>(player_location.x + rand_x * 32),
					static_cast<float>(player_location.y + rand_y * 32)
				});
	}
}

void GameHelper::RandomLocation(size_t spawn_count, float ground_width, float ground_height, std::vector<Vector2>& locations) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
	{
		locations.emplace_back(
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
		
		unsigned int random = GetRandomValue(0, 100);

		if (random > 10 && random < 50)
			modifier = modifier | BehaviourModifier::IncreasedSpeed;

		if (random <= 20 && level > 10)
			modifier = modifier | BehaviourModifier::Big;

		modifiers.emplace_back(modifier);
	}
}


void GameHelper::SingleType(size_t spawn_count, std::vector<EnemyType>& types) noexcept
{
	EnemyType type = static_cast<EnemyType>(GetRandomValue(0, static_cast<int>(EnemyType::COUNT) - 1));

	for (size_t i = 0; i < spawn_count; i++)
		types.emplace_back(type);
}

void GameHelper::RandomType(size_t spawn_count, std::vector<EnemyType>& types) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
		types.emplace_back(static_cast<EnemyType>(GetRandomValue(0, static_cast<int>(EnemyType::COUNT) - 1)));
}
