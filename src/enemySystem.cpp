#include "enemySystem.hpp"

#include <cstddef>
#include <cstdint>
#include <variant>

#include "raylib.h"
#include "raymath.h"
#include "enemyData.hpp"
#include "gameHelpers.hpp"
#include "assetManager.hpp"

#include "signals.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"

EnemySystem::EnemySystem()
{
	this->EnemyHealth.reserve(1024);
	this->EnemySpeed.reserve(1024);
	this->EnemyIsVisible.reserve(1024);
	this->EnemyDirection.reserve(1024);
	this->EnemyRect.reserve(1024);
	this->EnemyAttackComponents.reserve(1024);
	this->EnemyTextureKey.reserve(1024);
	this->EnemyAnimationComponents.reserve(1024);
	this->EnemyTypes.reserve(1024);
}

void EnemySystem::Reset() noexcept
{
	this->EnemyHealth.clear();
	this->EnemySpeed.clear();
	this->EnemyIsVisible.clear();
	this->EnemyDirection.clear();
	this->EnemyRect.clear();
	this->EnemyAttackComponents.clear();
	this->EnemyTextureKey.clear();
	this->EnemyAnimationComponents.clear();
	this->EnemyTypes.clear();


	this->FutureEnemyTypes.clear();
	this->FutureSpawnLocations.clear();
}

void EnemySystem::PollSignals(MessageSystem& message_system, const AssetManager& assets, const size_t level, const size_t ticks) noexcept
{
	for (size_t i = 0; i < message_system.EnemySystemSignals.size(); i++)
	{
		if (message_system.EnemySystemSignals[i])
		{
			auto handler_function = this->SignalHandlers[i];
			(this->*handler_function)(ticks, level, message_system);

			message_system.EnemySystemSignals[i] = false;
		}
	}
}

void EnemySystem::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto const& command : message_system.EnemySystemCommands)
	{
		auto handler = this->CommandHandlers[command.index()];
		(this->*handler)(command);
	}

	message_system.EnemySystemCommands.clear();
}

void EnemySystem::UpdateEnemies(
		const size_t ticks, const Rectangle update_area, const Vector2 player_centre, const float map_width, const float map_height,
		const bool is_stinky, const size_t level, MessageSystem& message_system, const TimerSystem& timer_system
		) noexcept
{
	if (this->EnemyHealth.size() < 5 && !timer_system.GetTimerStatus(Timer::SpawnEnemies))
	{
		message_system.TimerSystemCommands.emplace_back(
				std::in_place_type<struct RegisterTimer>, TICK_RATE / 2,
				true, Timer::EmitLocationParticles
				);
		message_system.TimerSystemCommands.emplace_back(
				std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(3),
				false, Timer::SpawnEnemies
				);
		this->PrepareSpawnEnemies(level, map_width, map_height);
	}

	this->VisibilityCheck(update_area);

	this->MoveEnemies(map_width, map_height);
	this->EnemiesSetDirection(player_centre, is_stinky);
	this->AnimateEnemies(ticks);
	this->EnemiesUpdateTimers(ticks);

	this->KillEnemies(message_system);
}

void EnemySystem::Draw(const AssetManager& assets) const noexcept
{
	for (size_t i = 0, n = this->EnemyTextureKey.size(); i < n; i++)
	{
		if (this->EnemyIsVisible[i])
		{
			const Texture2D texture = assets.GetTexture(this->EnemyTextureKey[i]);

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
					WHITE
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
	}
}

const std::vector<Rectangle>& EnemySystem::GetEnemyRect() const noexcept
{
	return this->EnemyRect;
}

const std::vector<EnemyAttackComponent>& EnemySystem::GetEnemyAttackComponents() const noexcept
{
	return this->EnemyAttackComponents;
}

const std::vector<EnemyType>& EnemySystem::GetEnemyType() const noexcept
{
	return this->EnemyTypes;
}

const std::vector<float>& EnemySystem::GetEnemyHealth() const noexcept
{
	return this->EnemyHealth;
}

void EnemySystem::CreateEnemy(const float x, const float y, const float level_scale, const EnemyType type) noexcept
{
	const size_t type_index = static_cast<size_t>(type);

	this->EnemyHealth.emplace_back(this->EnemyAttributes[type_index].Health * level_scale);
	this->EnemySpeed.emplace_back(this->EnemyAttributes[type_index].Speed);
	this->EnemyIsVisible.emplace_back(false);
	this->EnemyDirection.emplace_back(0.0f, 0.0f);
	this->EnemyRect.emplace_back(x, y, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE);
	this->EnemyAttackComponents.emplace_back(0, this->EnemyAttributes[type_index].Damage * level_scale, true);
	this->EnemyTextureKey.emplace_back(this->EnemyAttributes[type_index].Texture);
	this->EnemyAnimationComponents.emplace_back(0, 0, this->EnemyAttributes[type_index].AnimationInterval, this->EnemyAttributes[type_index].AnimationFrames);
	this->EnemyTypes.emplace_back(type);
}

void EnemySystem::KillEnemies(MessageSystem& message_system) noexcept
{
	uint32_t killed = 0;

	for (size_t i = 0; i < this->EnemyHealth.size(); )
	{
		if (this->EnemyHealth[i] <= 0.0f)
		{
			const float x = this->EnemyRect[i].x;
			const float y = this->EnemyRect[i].y;
			const EnemyType type = this->EnemyTypes[i];

			this->EnemyHealth[i] = this->EnemyHealth.back();
			this->EnemySpeed[i] = this->EnemySpeed.back();
			this->EnemyIsVisible[i] = this->EnemyIsVisible.back();
			this->EnemyDirection[i] = this->EnemyDirection.back();
			this->EnemyRect[i] = this->EnemyRect.back();
			this->EnemyAttackComponents[i] = this->EnemyAttackComponents.back();
			this->EnemyTextureKey[i] = this->EnemyTextureKey.back();
			this->EnemyAnimationComponents[i] = this->EnemyAnimationComponents.back();
			this->EnemyTypes[i] = this->EnemyTypes.back();

			this->EnemyHealth.pop_back();
			this->EnemySpeed.pop_back();
			this->EnemyIsVisible.pop_back();
			this->EnemyDirection.pop_back();
			this->EnemyRect.pop_back();
			this->EnemyAttackComponents.pop_back();
			this->EnemyTextureKey.pop_back();
			this->EnemyAnimationComponents.pop_back();
			this->EnemyTypes.pop_back();

			const uint8_t value = this->EnemyXpValues[static_cast<size_t>(type)];
		
			message_system.XpSystemCommands.emplace_back(std::in_place_type<struct CreateXp>, x, y, value);
			killed++;
		}
		else
			i++;
	}

	message_system.StatSystemCommands.emplace_back(Stat::Kills, killed);
}


void EnemySystem::DamageEnemyHandler(const EnemySystemCommand& command) noexcept
{
	const DamageEnemy& data = std::get<struct DamageEnemy>(command);
	this->EnemyHealth[data.EnemyIndex] -= data.DamageAmount;
}

void EnemySystem::EnemyLeAttackedHandler(const EnemySystemCommand& command) noexcept
{
	const EnemyLeAttacked& data = std::get<struct EnemyLeAttacked>(command);

	this->EnemyAttackComponents[data.EnemyIndex].CanLeAttack = false;
	this->EnemyAttackComponents[data.EnemyIndex].LastLeAttack = data.Ticks;
}

void EnemySystem::EmitParticlesFromLocations(
		const size_t ticks, const size_t level, MessageSystem& message_system) noexcept
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

void EnemySystem::SpawnEnemies(
		const size_t ticks, const size_t level, MessageSystem& message_system) noexcept
{
	const size_t spawn_count = level * 15;
	const float level_scale = 1 + static_cast<float>(level) / 10.0f;

	const size_t new_size = this->EnemyHealth.size() + spawn_count;

	this->EnemyHealth.reserve(new_size);
	this->EnemySpeed.reserve(new_size);
	this->EnemyIsVisible.reserve(new_size);
	this->EnemyDirection.reserve(new_size);
	this->EnemyRect.reserve(new_size);
	this->EnemyAttackComponents.reserve(new_size);
	this->EnemyTextureKey.reserve(new_size);
	this->EnemyAnimationComponents.reserve(new_size);
	this->EnemyTypes.reserve(new_size);

	for (size_t i = 0; i < spawn_count; i++)
	{
		const Vector2 location = this->FutureSpawnLocations[i];
		const EnemyType type = this->FutureEnemyTypes[i];

		this->CreateEnemy(location.x, location.y, level_scale, type);
	}

	this->FutureSpawnLocations.clear();
	this->FutureEnemyTypes.clear();
}

void EnemySystem::PrepareSpawnEnemies(const size_t level, const float map_width, const float map_height) noexcept
{
	const size_t spawn_count = level * 15;
	
	EnemySystem::GenerateLocations(spawn_count, map_width, map_height);
	EnemySystem::GenerateTypes(spawn_count);
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
