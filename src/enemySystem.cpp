#include "enemySystem.hpp"

#include <cstddef>
#include <cstdint>

#include "raylib.h"
#include "raymath.h"
#include "enemyData.hpp"
#include "gameHelpers.hpp"
#include "assetManager.hpp"

#include "signals.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"

EnemySystem::EnemySystem()
{
	this->EnemyHealth.reserve(1024);
	this->EnemyScale.reserve(1024);
	this->EnemySpeed.reserve(1024);
	this->EnemyIsVisible.reserve(1024);
	this->EnemyDirection.reserve(1024);
	this->EnemyRect.reserve(1024);
	this->EnemyAttackComponents.reserve(1024);
	this->EnemyDisplayComponents.reserve(1024);
	this->EnemyAnimationComponents.reserve(1024);
	this->EnemyTypes.reserve(1024);
	this->EnemyBehaviourModifiers.reserve(1024);
}

void EnemySystem::Reset() noexcept
{
	this->EnemyHealth.clear();
	this->EnemyScale.clear();
	this->EnemySpeed.clear();
	this->EnemyIsVisible.clear();
	this->EnemyDirection.clear();
	this->EnemyRect.clear();
	this->EnemyAttackComponents.clear();
	this->EnemyDisplayComponents.clear();
	this->EnemyAnimationComponents.clear();
	this->EnemyTypes.clear();
	this->EnemyBehaviourModifiers.clear();


	this->FutureEnemyTypes.clear();
	this->FutureEnemyModifiers.clear();
	this->FutureSpawnLocations.clear();
}

void EnemySystem::PollSignals(MessageSystem& message_system, const AssetManager& assets, const size_t level) noexcept
{
	const float map_width = assets.Ground.width;
	const float map_height = assets.Ground.height;

	for (size_t i = 0; i < message_system.EnemySystemSignals.size(); i++)
	{
		if (message_system.EnemySystemSignals[i])
		{
			auto handler_function = this->SignalHandlers[i];
			(this->*handler_function)(level, map_width, map_height);

			message_system.EnemySystemSignals[i] = false;
		}
	}
}

void EnemySystem::UpdateEnemies(
		const size_t ticks, const Rectangle update_area, const Vector2 player_centre,
		const float map_width, const float map_height, const bool is_stinky, MessageSystem& message_system
		) noexcept
{
	/*
	long long slide_damage = 0;
	
	for (auto &enemy : this->Enemies)
	{
		if (CheckCollisionRecs(update_area, enemy.Rect))
		{
			enemy.Update(player_centre, ticks, is_stinky);

			if (!has_greenbull && !is_sliding)
				Collisions::LeAttack(*this->Player, enemy, ticks, this->Effects, this->Events);
			
			if (is_sliding)
				slide_damage = Collisions::SlideAttack(*this->Player, enemy);
		}
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

	this->VisibilityCheck(update_area);

	this->MoveEnemies(map_width, map_height);
	this->EnemiesSetDirection(player_centre, is_stinky);
	this->AnimateEnemies(ticks);
	this->EnemiesUpdateTimers(ticks);

	this->KillEnemies(message_system);
}

void EnemySystem::Draw(const AssetManager& assets) const noexcept
{
	for (size_t i = 0, n = this->EnemyDisplayComponents.size(); i < n; i++)
	{
		if (this->EnemyIsVisible[i])
		{
			const Texture2D texture = assets.Textures.at(this->EnemyDisplayComponents[i].Texture);
			const bool flash = this->EnemyDisplayComponents[i].Flash;

			const Rectangle source_rect = {
				static_cast<float>(this->EnemyAnimationComponents[i].ImageIndex) * TEXTURE_TILE_SIZE,
				0.0f,
				TEXTURE_TILE_SIZE,
				TEXTURE_TILE_SIZE
			};

			DrawTexturePro(
					texture,
					source_rect,
					this->EnemyRect[i],
					{ 0.0f, 0.0f },
					0.0f,
					(flash) ? YELLOW : WHITE
					);
		}
	}
}

void EnemySystem::VisibilityCheck(const Rectangle update_area) noexcept
{
	for (size_t i = 0, n = this->EnemyIsVisible.size(); i < n; i++)
		this->EnemyIsVisible[i] = static_cast<uint8_t>(CheckCollisionRecs(update_area, this->EnemyRect[i]));
}

void EnemySystem::MoveEnemies(const float map_width, const float map_height) noexcept
{
	for (size_t i = 0, n = this->EnemyRect.size(); i < n; i++)
	{
		this->EnemyRect[i].x += this->EnemySpeed[i] * this->EnemyDirection[i].x * TICK_TIME;
		this->EnemyRect[i].y += this->EnemySpeed[i] * this->EnemyDirection[i].y * TICK_TIME;

		GameHelper::LoopOverMap(map_width, map_height, this->EnemyRect[i]);
	}
}

void EnemySystem::AnimateEnemies(const size_t ticks) noexcept
{
	for (size_t i = 0, n = this->EnemyAnimationComponents.size(); i < n; i++)
	{
		if (this->EnemyIsVisible[i])
		{
			if (this->EnemyDirection[i].x == 0.0f && this->EnemyDirection[i].y == 0.0f)
			{
				this->EnemyAnimationComponents[i].ImageIndex = 0;
				this->EnemyAnimationComponents[i].LastAnimationUpdate = ticks;
				continue;
			}

			if (ticks - this->EnemyAnimationComponents[i].LastAnimationUpdate >= this->EnemyAnimationComponents[i].AnimationInterval)
			{
				this->EnemyAnimationComponents[i].ImageIndex++;
				this->EnemyAnimationComponents[i].LastAnimationUpdate = ticks;
				this->EnemyAnimationComponents[i].ImageIndex %= this->EnemyAnimationComponents[i].AnimationFrames;
			}
		}
	}
}

void EnemySystem::EnemiesSetDirection(const Vector2 player_centre, const bool is_stinky) noexcept
{
	for (size_t i = 0, n = this->EnemyDirection.size(); i < n; i++)
	{
		if (this->EnemyIsVisible[i])
		{
			this->EnemyDirection[i].x = player_centre.x - this->EnemyRect[i].x;
			this->EnemyDirection[i].y = player_centre.y - this->EnemyRect[i].y;

			if (is_stinky)
				this->EnemyDirection[i] = Vector2Scale(this->EnemyDirection[i], -1.0f);

			this->EnemyDirection[i] = Vector2Normalize(this->EnemyDirection[i]);
		}
	}
}

void EnemySystem::EnemiesUpdateTimers(const size_t ticks) noexcept
{
	for (size_t i = 0, n = this->EnemyAttackComponents.size(); i < n; i++)
	{
		if (ticks - this->EnemyAttackComponents[i].LastLeAttack >= SECONDS_TO_TICKS(3))
			this->EnemyAttackComponents[i].CanLeAttack = true;

		if (ticks - this->EnemyDisplayComponents[i].LastFlash >= 60)
			this->EnemyDisplayComponents[i].Flash = false;
	}
}

void EnemySystem::FlashSprite(const size_t ticks) noexcept
{
	/*
	this->Flash = true;
	this->FlashTriggered = ticks;
	*/
}

void EnemySystem::CreateEnemy(const float x, const float y, const float level_scale, const EnemyType type, const BehaviourModifier modifier) noexcept
{
	const size_t type_index = static_cast<size_t>(type);

	this->EnemyHealth.emplace_back(this->EnemyAttributes[type_index].Health * level_scale);

	this->EnemyScale.emplace_back(1.0f);

	this->EnemySpeed.emplace_back(this->EnemyAttributes[type_index].Speed);
	this->EnemyIsVisible.emplace_back(false);

	this->EnemyDirection.emplace_back(0.0f, 0.0f);
	this->EnemyRect.emplace_back(x, y, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE);

	this->EnemyAttackComponents.emplace_back(0, this->EnemyAttributes[type_index].Damage * level_scale, true);

	this->EnemyDisplayComponents.emplace_back(0, this->EnemyAttributes[type_index].Texture, false);

	this->EnemyAnimationComponents.emplace_back(0, 0, this->EnemyAttributes[type_index].AnimationInterval, this->EnemyAttributes[type_index].AnimationFrames);

	this->EnemyTypes.emplace_back(type);
	this->EnemyBehaviourModifiers.emplace_back(modifier);
}

void EnemySystem::KillEnemies(MessageSystem& message_system) noexcept
{
	uint32_t killed = 0;

	for (size_t i = 0; i < this->EnemyHealth.size(); )
	{
		if (this->EnemyHealth[i] <= 0)
		{
			const float x = this->EnemyRect[i].x;
			const float y = this->EnemyRect[i].y;

			this->EnemyHealth[i] = this->EnemyHealth.back();
			this->EnemyScale[i] = this->EnemyScale.back();
			this->EnemySpeed[i] = this->EnemySpeed.back();
			this->EnemyIsVisible[i] = this->EnemyIsVisible.back();
			this->EnemyDirection[i] = this->EnemyDirection.back();
			this->EnemyRect[i] = this->EnemyRect.back();
			this->EnemyAttackComponents[i] = this->EnemyAttackComponents.back();
			this->EnemyDisplayComponents[i] = this->EnemyDisplayComponents.back();
			this->EnemyAnimationComponents[i] = this->EnemyAnimationComponents.back();
			this->EnemyTypes[i] = this->EnemyTypes.back();
			this->EnemyBehaviourModifiers[i] = this->EnemyBehaviourModifiers.back();

			this->EnemyHealth.pop_back();
			this->EnemyScale.pop_back();
			this->EnemySpeed.pop_back();
			this->EnemyIsVisible.pop_back();
			this->EnemyDirection.pop_back();
			this->EnemyRect.pop_back();
			this->EnemyAttackComponents.pop_back();
			this->EnemyDisplayComponents.pop_back();
			this->EnemyAnimationComponents.pop_back();
			this->EnemyTypes.pop_back();
			this->EnemyBehaviourModifiers.pop_back();

			const unsigned int value = this->EnemyXpValues[static_cast<size_t>(this->EnemyTypes[i])];
		
			message_system.XpSystemCommands.emplace_back(x, y, value * static_cast<int>(this->EnemyScale[i]));
			killed++;
		}
		else
			i++;
	}

	message_system.StatSystemCommands.emplace_back(Stat::Kills, killed);
}

void EnemySystem::EmitParticlesFromLocations(const size_t ticks, MessageSystem& message_system) noexcept
{
	for (auto const& location : this->FutureSpawnLocations)
	{
		const Vector2 velocity = (Vector2) { 
			static_cast<float>(GetRandomValue(-8, 8)),
			static_cast<float>(GetRandomValue(-96, -64))
		};

		message_system.ParticleSystemCommands.emplace_back(ticks, 20, velocity, location.x, location.y, 5, 20, 120, TICK_RATE, 32, RED, MAGENTA);
	}
}

void EnemySystem::SpawnEnemies(const size_t level, const float map_width, const float map_height) noexcept
{
	const size_t spawn_count = level * 15;
	const float level_scale = 1 + static_cast<float>(level) / 10.0f;

	const size_t new_size = this->EnemyHealth.size() + spawn_count;

	this->EnemyHealth.reserve(new_size);
	this->EnemyScale.reserve(new_size);
	this->EnemySpeed.reserve(new_size);
	this->EnemyIsVisible.reserve(new_size);
	this->EnemyDirection.reserve(new_size);
	this->EnemyRect.reserve(new_size);
	this->EnemyAttackComponents.reserve(new_size);
	this->EnemyDisplayComponents.reserve(new_size);
	this->EnemyAnimationComponents.reserve(new_size);
	this->EnemyTypes.reserve(new_size);
	this->EnemyBehaviourModifiers.reserve(new_size);

	for (size_t i = 0; i < spawn_count; i++)
	{
		const Vector2 location = this->FutureSpawnLocations[i];
		const EnemyType type = this->FutureEnemyTypes[i];
		const BehaviourModifier modifier = this->FutureEnemyModifiers[i];

		this->CreateEnemy(location.x, location.y, level_scale, type, modifier);
	}

	this->FutureSpawnLocations.clear();
	this->FutureEnemyTypes.clear();
	this->FutureEnemyModifiers.clear();
}

void EnemySystem::PrepareSpawnEnemies(const size_t level, const float map_width, const float map_height) noexcept
{
	const size_t spawn_count = level * 15;
	
	EnemySystem::GenerateLocations(spawn_count, map_width, map_height);
	EnemySystem::GenerateTypes(spawn_count);
	EnemySystem::GenerateModifiers(spawn_count, level);
}

void EnemySystem::GenerateLocations(const size_t spawn_count, const float map_width, const float map_height) noexcept
{
	this->FutureSpawnLocations.reserve(spawn_count);

	for (size_t i = 0; i < spawn_count; i++)
	{
		this->FutureSpawnLocations.emplace_back(
				static_cast<float>(GetRandomValue(0, map_width)),
				static_cast<float>(GetRandomValue(0, map_height))
				);
	}
}

void EnemySystem::GenerateTypes(const size_t spawn_count) noexcept
{
	this->FutureEnemyTypes.reserve(spawn_count);

	for (size_t i = 0; i < spawn_count; i++)
		this->FutureEnemyTypes.emplace_back(static_cast<EnemyType>(GetRandomValue(0, static_cast<int>(EnemyType::COUNT) - 1)));
}

void EnemySystem::GenerateModifiers(const size_t spawn_count, const size_t level) noexcept
{
	this->FutureEnemyModifiers.reserve(spawn_count);

	auto RandomModifierFunction = this->GenerateModifierFunctions[GetRandomValue(0, this->GenerateModifierFunctions.size() - 1)];

	(this->*RandomModifierFunction)(spawn_count, level);
}

void EnemySystem::NoModifiers(const size_t spawn_count, const size_t level) noexcept
{
	for (size_t i = 0; i < spawn_count; i++)
		this->FutureEnemyModifiers.emplace_back(BehaviourModifier::None);
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
		this->FutureEnemyModifiers.emplace_back(modifier);
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

		this->FutureEnemyModifiers.emplace_back(modifier);
	}
}
