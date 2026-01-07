#include "enemySystem.hpp"

#include <algorithm>
#include <cstddef>

#include "enemy.hpp"
#include "enemyData.hpp"
#include "gameHelpers.hpp"
#include "assetManager.hpp"

#include "signals.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"

EnemySystem::EnemySystem()
{
	this->Enemies.reserve(1024);
}

void EnemySystem::Reset() noexcept
{
	this->Enemies.clear(); 
	this->EnemyTypes.clear();
	this->EnemyModifiers.clear();
	this->SpawnLocations.clear();
}

void EnemySystem::PollSignals(MessageSystem& message_system, const AssetManager& assets, const size_t level) noexcept
{
	for (size_t i = 0; i < message_system.EnemySystemSignals.size(); i++)
	{
		if (message_system.EnemySystemSignals[i])
		{
			auto handler_function = this->SignalHandlers[i];
			(this->*handler_function)(level, assets);

			message_system.EnemySystemSignals[i] = false;
		}
	}
}

void EnemySystem::UpdateEnemies(
		const size_t ticks, const Rectangle update_area, const Vector2 player_centre,
		const float map_width, const float map_height, const bool is_stinky, MessageSystem& message_system
		) noexcept
{
	//long long slide_damage = 0;
	
	for (auto &enemy : this->Enemies)
	{
		if (CheckCollisionRecs(update_area, enemy.Rect))
		{
			enemy.Update(player_centre, ticks, is_stinky);

			GameHelper::LoopOverMap(map_width, map_height, enemy.Rect);
/*
			if (!has_greenbull && !is_sliding)
				Collisions::LeAttack(*this->Player, enemy, ticks, this->Effects, this->Events);
			
			if (is_sliding)
				slide_damage = Collisions::SlideAttack(*this->Player, enemy);
				*/
		}
/*
		if (slide_damage > 0)
		{
			this->Stats[static_cast<size_t>(Stat::TotalDamage)] += slide_damage;

			this->GameTexts.emplace_back( 
					enemy.Rect.x, enemy.Rect.y, 64.0f, std::to_string(slide_damage),
					52,	ORANGE, ticks, ticks + TICK_RATE / 4
					);

			GameState::EmitParticles(
					50, enemy.Rect.x, enemy.Rect.y, 10, 25,
					60, TICK_RATE, this->Player->Direction, 512, ORANGE, RED, assets);
		}
		*/

		if (enemy.Health <= 0)
		{
			const unsigned int value = EnemyXpValues[static_cast<size_t>(enemy.Type)];
		
			message_system.XpSystemCommands.emplace_back(enemy.Rect.x, enemy.Rect.y, value * static_cast<int>(enemy.Scale));

			//this->Stats[static_cast<size_t>(Stat::Kills)]++;
		}
	}

	std::erase_if(this->Enemies, [](const Enemy& enemy) { return (enemy.Health <= 0); });
}

void EnemySystem::Draw(const Rectangle update_area) const noexcept
{
	for (auto const &enemy : this->Enemies)
	{
		if (CheckCollisionRecs(update_area, enemy.Rect))
			enemy.Draw();
	}
}

void EnemySystem::EmitParticlesFromLocations(const size_t ticks, MessageSystem& message_system) noexcept
{
	for (auto const& location : this->SpawnLocations)
	{
		const Vector2 velocity = (Vector2) { 
			static_cast<float>(GetRandomValue(-8, 8)),
			static_cast<float>(GetRandomValue(-96, -64))
		};

		message_system.ParticleSystemCommands.emplace_back(ticks, 20, velocity, location.x, location.y, 5, 20, 120, TICK_RATE, 32, RED, MAGENTA);
	}
}

void EnemySystem::SpawnEnemies(const size_t level, const AssetManager& assets) noexcept
{
	const size_t spawn_count = level * 15;
	const float level_scale = 1 + static_cast<float>(level) / 10.0f;

	this->Enemies.reserve(this->Enemies.size() + spawn_count);

	for (size_t i = 0; i < spawn_count; i++)
	{
		const Vector2 location = this->SpawnLocations[i];
		const EnemyType type = this->EnemyTypes[i];
		const BehaviourModifier modifier = this->EnemyModifiers[i];

		this->Enemies.emplace_back(location.x, location.y, level_scale, assets, type, modifier);
	}

	this->SpawnLocations.clear();
	this->EnemyTypes.clear();
	this->EnemyModifiers.clear();
}

void EnemySystem::PrepareSpawnEnemies(const size_t level, const AssetManager& assets) noexcept
{
	const size_t spawn_count = level * 15;
	
	const float map_width = assets.Ground.width;
	const float map_height = assets.Ground.height;

	EnemySystem::GenerateLocations(spawn_count, map_width, map_height);
	EnemySystem::GenerateTypes(spawn_count);
	EnemySystem::GenerateModifiers(spawn_count, level);
}

void EnemySystem::GenerateLocations(const size_t spawn_count, const float map_width, const float map_height) noexcept
{
	this->SpawnLocations.reserve(spawn_count);

	for (size_t i = 0; i < spawn_count; i++)
	{
		this->SpawnLocations.emplace_back(
				static_cast<float>(GetRandomValue(0, map_width)),
				static_cast<float>(GetRandomValue(0, map_height))
				);
	}
}

void EnemySystem::GenerateTypes(const size_t spawn_count) noexcept
{
	this->EnemyTypes.reserve(spawn_count);

	for (size_t i = 0; i < spawn_count; i++)
		this->EnemyTypes.emplace_back(static_cast<EnemyType>(GetRandomValue(0, static_cast<int>(EnemyType::COUNT) - 1)));
}

void EnemySystem::GenerateModifiers(const size_t spawn_count, const size_t level) noexcept
{
	this->EnemyModifiers.reserve(spawn_count);

	auto RandomModifierFunction = this->GenerateModifierFunctions[GetRandomValue(0, this->GenerateModifierFunctions.size() - 1)];

	(this->*RandomModifierFunction)(spawn_count, level);
}

void EnemySystem::NoModifiers(const size_t spawn_count, const size_t level) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
		this->EnemyModifiers.emplace_back(BehaviourModifier::None);
}

void EnemySystem::SameModifiers(const size_t spawn_count, const size_t level) noexcept
{
	BehaviourModifier modifier = BehaviourModifier::None;

	const unsigned int random = GetRandomValue(1, 100);

	if (random >= 10 && random < 30)
		modifier = modifier | BehaviourModifier::IncreasedSpeed;

	if (random <= 5 && level > 20)
		modifier = modifier | BehaviourModifier::Big;

	for (size_t i = 0; i < spawn_count; i++)
		this->EnemyModifiers.emplace_back(modifier);
}

void EnemySystem::RandomModifiers(const size_t spawn_count, const size_t level) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
	{
		BehaviourModifier modifier = BehaviourModifier::None;
		
		const unsigned int random = GetRandomValue(1, 100);

		if (random >= 10 && random < 50)
			modifier = modifier | BehaviourModifier::IncreasedSpeed;

		if (random <= 20 && level > 10)
			modifier = modifier | BehaviourModifier::Big;

		this->EnemyModifiers.emplace_back(modifier);
	}
}
