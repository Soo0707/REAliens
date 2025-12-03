#include "gameHelpers.hpp"

#include <vector>
#include <cstddef>

#include "raylib.h"
#include "settingsManager.hpp"
#include "assetManager.hpp"
#include "game.hpp"

void GameHelper::LoopOverMap(AssetManager& assets, Rectangle& m_obj) noexcept
{
	if (m_obj.x < 0)
		m_obj.x = assets.Ground.width - m_obj.width;
	else if (m_obj.x + m_obj.width > assets.Ground.width)
		m_obj.x = 0;

	if (m_obj.y < 0)
		m_obj.y = assets.Ground.height - m_obj.height;
	else if (m_obj.y + m_obj.height > assets.Ground.height)
		m_obj.y = 0;
}

void GameHelper::LevelUp(Game& game, SettingsManager& settings) noexcept
{
	if (!settings.Data.at(SettingKey::UnlimitedPowerups))
	{
		game.GlobalData->UnclaimedPowerups++;
		game.GlobalData->CachedStrings[CachedString::UnclaimedPowerups] = "Unclaimed Powerups: " + std::to_string(game.GlobalData->UnclaimedPowerups);
	}

	game.GlobalData->Level++;
	game.GlobalData->CachedStrings[CachedString::LevelText] = "Level: " + std::to_string(game.GlobalData->Level);

	game.CollectedXp = 0;
	game.LevelUpTreshold += 5;
	
	if (settings.Data.at(SettingKey::PowerupMenuInterrupt))
		game.GlobalData->ActiveState = State::PowerupMenu;

	if (game.GlobalData->Level % 5 == 0 && !settings.Data.at(SettingKey::DisableLevelDebuffs))
		game.GlobalData->InsertLevelDebuff();
	else
		game.GlobalData->RemoveLevelDebuff();
}


void GameHelper::SpawnEnemies(Game& game) noexcept
{
	size_t spawn_count = game.GlobalData->Level * 15;

	game.Enemies.reserve(game.Enemies.size() + spawn_count);

	static std::vector<Vector2> locations;
	locations.reserve(spawn_count);

	if (game.GlobalData->Level > 100 && !(game.GlobalData->Level % 5))
		GameHelper::ScreenLocation(spawn_count, game, locations);
	else if (!(game.GlobalData->Level % 5))
		GameHelper::NearPlayerLocation(spawn_count, game, locations);
	else
		GameHelper::RandomLocation(spawn_count, game, locations);

	static std::vector<EnemyType> types;
	types.reserve(spawn_count);

	GameHelper::SpawnTypeFunctions[GetRandomValue(0, 1)](spawn_count, types);

	static std::vector<BehaviourModifier> modifiers;
	modifiers.reserve(spawn_count);

	GameHelper::GenerateModifierFunctions[GetRandomValue(0, 2)](spawn_count, game.GlobalData->Level, modifiers);

	float level_scale = 1 + game.GlobalData->Level / 10.0f;

	for (size_t i = 0; i < spawn_count; i++)
		game.Enemies.emplace_back(locations[i].x, locations[i].y, level_scale, game.GlobalData->CurrentLayer, *game.Assets, types[i], modifiers[i]);


	locations.clear();
	types.clear();
	modifiers.clear();
}

void GameHelper::RandomLocation(size_t spawn_count, Game& game, std::vector<Vector2>& locations) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
	{
		locations.emplace_back(
				(Vector2) {
				static_cast<float>( GetRandomValue(0, game.Assets->Ground.width) ),
				static_cast<float>( GetRandomValue(0, game.Assets->Ground.height) )
				});
	}
}

void GameHelper::ScreenLocation(size_t spawn_count, Game& game, std::vector<Vector2>& locations) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
	{
		locations.emplace_back(
				(Vector2) {
				static_cast<float>( GetRandomValue(game.UpdateArea.x, game.UpdateArea.x + game.UpdateArea.width) ),
				static_cast<float>( GetRandomValue(game.UpdateArea.y, game.UpdateArea.y + game.UpdateArea.height) )
				});
	}
}

void GameHelper::NearPlayerLocation(size_t spawn_count, Game& game, std::vector<Vector2>& locations) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
	{
		unsigned int rand_x = GetRandomValue(32, 64);
		unsigned int rand_y = GetRandomValue(32, 64);

		rand_x = (rand_x % 2) ? (rand_x * -1) : (rand_x);
		rand_y = (rand_y % 2) ? (rand_y * -1) : (rand_y);

		locations.emplace_back(
				(Vector2) {
					static_cast<float>( game.PlayerInstance->Rect.x + rand_x ),
					static_cast<float>( game.PlayerInstance->Rect.y + rand_y )
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

	unsigned int random = GetRandomValue(0, 100);

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
