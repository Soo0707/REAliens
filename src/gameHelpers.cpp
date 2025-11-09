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
	else if (m_obj.x > assets.Ground.width)
		m_obj.x = 0;

	if (m_obj.y < 0)
		m_obj.y = assets.Ground.height - m_obj.height;
	else if (m_obj.y > assets.Ground.height)
		m_obj.y = 0;
}

void GameHelper::LevelUp(Game& game) noexcept
{
	if (!game.Settings->Data.at(SettingKey::UnlimitedPowerups))
	{
		game.GlobalData->UnclaimedPowerups++;
		game.GlobalData->CachedStrings[CachedString::UnclaimedPowerups] = "Unclaimed Powerups: " + std::to_string(game.GlobalData->UnclaimedPowerups);
	}

	game.GlobalData->Level++;
	game.GlobalData->CachedStrings[CachedString::LevelText] = "Level: " + std::to_string(game.GlobalData->Level);

	game.CollectedXp = 0;
	game.LevelUpTreshold += 5;
	
	if (game.Settings->Data.at(SettingKey::PowerupMenuInterrupt))
		game.GlobalData->ActiveState = State::PowerupMenu;
}


void GameHelper::SpawnEnemies(Game& game) noexcept
{
	size_t spawn_count = game.GlobalData->Level * 15;

	game.Enemies.reserve(game.Enemies.size() + spawn_count);

	static std::vector<Vector2> locations;
	locations.reserve(spawn_count);

	switch (GetRandomValue(0, 1))
	{
		case 0:
			GameHelper::RandomLocation(spawn_count, game, locations);
			break;
		case 1:
			GameHelper::ScreenLocation(spawn_count, game, locations);
			break;
	}


	static std::vector<EnemyType> types;
	types.reserve(spawn_count);

	switch (GetRandomValue(0, 1))
	{
		case 0:
			GameHelper::SingleType(spawn_count, types);
			break;
		case 1:
			GameHelper::RandomType(spawn_count, types);
			break;
	}

	static std::vector<BehaviourModifier> modifiers;
	modifiers.reserve(spawn_count);

	switch (GetRandomValue(0, 2))
	{
		case 0:
			GameHelper::NoModifiers(spawn_count, modifiers);
			break;
		case 1:
			GameHelper::SameModifiers(spawn_count, game.GlobalData->Level, modifiers);
			break;
		case 2:
			GameHelper::RandomModifiers(spawn_count, game.GlobalData->Level, modifiers);
			break;
	}

	for (size_t i = 0; i < spawn_count; i++)
		game.Enemies.emplace_back(locations[i].x, locations[i].y, game.GlobalData->CurrentLayer, *game.Assets, types[i], modifiers[i]);


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


void GameHelper::NoModifiers(size_t spawn_count, std::vector<BehaviourModifier>& modifiers) noexcept
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

	if (random >= 50 && random < 65)
		modifier = modifier | BehaviourModifier::OverrideDirection;

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

		if (random >= 50 && random < 55)
			modifier = modifier | BehaviourModifier::OverrideDirection;
		
		if (random <= 10 && level > 10)
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
