#include "gameHelpers.hpp"

#include "raylib.h"
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
	game.GlobalData->Level++;
	game.GlobalData->UnclaimedPowerups++;

	game.GlobalData->CachedStrings[CachedString::LevelText] = "Level: " + std::to_string(game.GlobalData->Level);
	game.GlobalData->CachedStrings[CachedString::UnclaimedPowerups] = "Unclaimed Powerups: " + std::to_string(game.GlobalData->UnclaimedPowerups);

	game.CollectedXp = 0;
	game.GlobalData->LevelUpTreshold += 5;
	
	if (game.GlobalData->Settings.at(Setting::ShowPowerupMenuOnLevelUp))
		game.GlobalData->ActiveState = State::PowerupMenu;

	PlaySound(game.Assets->Sounds.at(SoundKey::LevelUp));
}


void GameHelper::SpawnEnemies(Game& game) noexcept
{
	std::vector<Vector2> rand_nums;

	for (size_t i = 0; i < game.GlobalData->Level * 15; i++)
	{
		rand_nums.emplace_back(
				(Vector2) {
				static_cast<float>( GetRandomValue(0, game.Assets->Ground.width) ),
				static_cast<float>( GetRandomValue(0, game.Assets->Ground.height) )
				});
	}

	for (auto &location : rand_nums)
	{
		EnemyType type = (EnemyType) GetRandomValue(0, 4);
		BehaviourModifier modifier = BehaviourModifier::None;
		
		unsigned int random1 = GetRandomValue(0, 100);
		unsigned int random2 = GetRandomValue(0, 100);
		unsigned int random3 = GetRandomValue(0, 100);

		if (random1 > 30)
			modifier = modifier | BehaviourModifier::IncreasedSpeed;

		if (random2 > 50)
			modifier = modifier | BehaviourModifier::OverrideDirection;

		if (random3 > 95 && game.GlobalData->Level > 10)
			modifier = modifier | BehaviourModifier::Big;

		game.Enemies.emplace_back(location.x, location.y, game.GlobalData->CurrentLayer, *game.Assets, type, modifier);
	}
}
