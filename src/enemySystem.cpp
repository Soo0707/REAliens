/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "enemySystem.hpp"

#include <cstddef>
#include <cstdint>
#include <variant>

#include "raylib.h"
#include "raymath.h"

#include "enemyData.hpp"
#include "assetManager.hpp"

#include "signals.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"
#include "timers.hpp"
#include "modifierSystem.hpp"
#include "modifiers.hpp"
#include "itemData.hpp"
#include "projectileData.hpp"

EnemySystem::EnemySystem()
{
	this->EnemyHealth.reserve(1024);
	this->EnemySpeed.reserve(1024);
	this->EnemyIsVisible.reserve(1024);
	this->EnemyDirection.reserve(1024);
	this->EnemyRect.reserve(1024);
	this->EnemyCentre.reserve(1024);
	this->EnemyTypes.reserve(1024);
	this->EnemyAttackComponents.reserve(1024);
	this->EnemyAnimationComponents.reserve(1024);
}

void EnemySystem::Reset() noexcept
{
	this->EnemyHealth.clear();
	this->EnemySpeed.clear();
	this->EnemyIsVisible.clear();
	this->EnemyDirection.clear();
	this->EnemyRect.clear();
	this->EnemyCentre.clear();
	this->EnemyTypes.clear();
	this->EnemyAttackComponents.clear();
	this->EnemyAnimationComponents.clear();

	this->FutureEnemyTypes.clear();
	this->FutureSpawnLocations.clear();
}

void EnemySystem::PollSignals(MessageSystem& message_system, const AssetManager& assets, const size_t level, const size_t ticks) noexcept
{
	for (size_t i = 0; i < message_system.EnemySystemSignals.size(); i++)
	{
		const uint16_t times = message_system.EnemySystemSignals[i];

		if (times)
		{
			auto handler_function = this->SignalHandlers[i];
			(this->*handler_function)(ticks, level, message_system);
		}
		message_system.EnemySystemSignals[i] = 0;
	}
}

void EnemySystem::ExecuteCommands(MessageSystem& message_system, const ModifierSystem& modifier_system) noexcept
{
	for (auto const& command : message_system.EnemySystemCommands)
	{
		auto handler = this->CommandHandlers[command.index()];
		(this->*handler)(message_system, modifier_system, command);
	}

	message_system.EnemySystemCommands.clear();
}

void EnemySystem::Update(
		MessageSystem& message_system, const AssetManager& assets, const ModifierSystem& modifier_system,
		const TimerSystem& timer_system, const Rectangle& update_area,
		const Vector2 player_centre, const size_t ticks, const size_t level
		) noexcept
{
	this->PollSignals(message_system, assets, level, ticks);
	this->ExecuteCommands(message_system, modifier_system);
	
	const float map_width = assets.Ground.width;
	const float map_height = assets.Ground.height;

	if (this->EnemyHealth.size() < 15 && !timer_system.GetTimerStatus(Timer::SpawnEnemies))
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

	const bool is_stinky = modifier_system.EffectStatus(Effect::Stinky);

	this->EnemiesSetDirection(player_centre, is_stinky);

	this->MoveEnemies(map_width, map_height);
	
	this->AnimateEnemies(ticks);
	this->EnemiesUpdateTimers(ticks);

	const bool has_magnetism = modifier_system.EffectStatus(Effect::Magnetism);

	this->KillEnemies(message_system, has_magnetism);
}

void EnemySystem::Draw(const AssetManager& assets) const noexcept
{
	for (size_t i = 0, n = this->EnemyIsVisible.size(); i < n; i++)
	{
		if (this->EnemyIsVisible[i])
		{
			const size_t enemy_type_index = static_cast<size_t>(this->EnemyTypes[i]);
			const Texture2D texture = assets.GetTexture(this->EnemyAttributes[enemy_type_index].Texture);

			const Rectangle source_rect = {
				static_cast<float>(this->EnemyAnimationComponents[i].ImageIndex) * ENEMY_TEXTURE_TILE_SIZE,
				0.0f,
				ENEMY_TEXTURE_TILE_SIZE,
				ENEMY_TEXTURE_TILE_SIZE
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

void EnemySystem::VisibilityCheck(const Rectangle& update_area) noexcept
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

		if (this->EnemyRect[i].x < 0.0f)
			this->EnemyRect[i].x = map_width - this->EnemyRect[i].width;
		else if (this->EnemyRect[i].x + this->EnemyRect[i].width > map_width)
			this->EnemyRect[i].x = 0.0f;

		if (this->EnemyRect[i].y < 0.0f)
			this->EnemyRect[i].y = map_height - this->EnemyRect[i].height;
		else if (this->EnemyRect[i].y + this->EnemyRect[i].height > map_height)
			this->EnemyRect[i].y = 0.0f;

		this->EnemyCentre[i].x = this->EnemyRect[i].x + this->EnemyRect[i].width / 2.0f;
		this->EnemyCentre[i].y = this->EnemyRect[i].y + this->EnemyRect[i].height / 2.0f;
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
				this->EnemyAnimationComponents[i].ImageIndex %= this->AnimationFrames;
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
			this->EnemyDirection[i].x = player_centre.x - this->EnemyCentre[i].x;
			this->EnemyDirection[i].y = player_centre.y - this->EnemyCentre[i].y;

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

const std::vector<Vector2>& EnemySystem::GetEnemyCentre() const noexcept
{
	return this->EnemyCentre;
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
	this->EnemyRect.emplace_back(x, y, ENEMY_TEXTURE_TILE_SIZE, ENEMY_TEXTURE_TILE_SIZE);
	this->EnemyCentre.emplace_back(
			x + static_cast<float>(ENEMY_TEXTURE_TILE_SIZE) / 2.0f,
			y + static_cast<float>(ENEMY_TEXTURE_TILE_SIZE) / 2.0f
			);
	this->EnemyTypes.emplace_back(type);
	this->EnemyAttackComponents.emplace_back(0, this->EnemyAttributes[type_index].Damage * level_scale, true);
	this->EnemyAnimationComponents.emplace_back(0, 0, this->EnemyAttributes[type_index].AnimationInterval);
}

void EnemySystem::KillEnemies(MessageSystem& message_system, const bool has_magnetism) noexcept
{
	uint32_t killed = 0;

	for (size_t i = 0; i < this->EnemyHealth.size(); )
	{
		if (this->EnemyHealth[i] <= 0.0f)
		{
			const float x = this->EnemyRect[i].x;
			const float y = this->EnemyRect[i].y;

			this->EnemyHealth[i] = this->EnemyHealth.back();
			this->EnemySpeed[i] = this->EnemySpeed.back();
			this->EnemyIsVisible[i] = this->EnemyIsVisible.back();
			this->EnemyDirection[i] = this->EnemyDirection.back();
			this->EnemyRect[i] = this->EnemyRect.back();
			this->EnemyCentre[i] = this->EnemyCentre.back();
			this->EnemyTypes[i] = this->EnemyTypes.back();
			this->EnemyAttackComponents[i] = this->EnemyAttackComponents.back();
			this->EnemyAnimationComponents[i] = this->EnemyAnimationComponents.back();

			this->EnemyHealth.pop_back();
			this->EnemySpeed.pop_back();
			this->EnemyIsVisible.pop_back();
			this->EnemyDirection.pop_back();
			this->EnemyRect.pop_back();
			this->EnemyCentre.pop_back();
			this->EnemyTypes.pop_back();
			this->EnemyAttackComponents.pop_back();
			this->EnemyAnimationComponents.pop_back();

			if (has_magnetism)
				message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::IncrementCollectedXp)]++;
			else
				message_system.ItemSystemCommands.emplace_back(std::in_place_type<struct CreateItem>, x, y, Item::Xp);

			killed++;
		}
		else
			i++;
	}

	message_system.StatSystemCommands.emplace_back(killed, Stat::Kills);
}


void EnemySystem::DamageEnemyHandler(MessageSystem& message_system, const ModifierSystem& modifier_system, const EnemySystemCommand& command) noexcept
{
	const DamageEnemy& data = std::get<struct DamageEnemy>(command);
	const uint32_t index = data.EnemyIndex;

	if (!this->CheckIndex(index))
		return;

	const float weakness_factor = modifier_system.EffectStatus(Effect::Weakness) ? 0.67f : 1.0f;
	this->EnemyHealth[index] -= data.DamageAmount * weakness_factor;
}

void EnemySystem::EnemyLeAttackedHandler(MessageSystem& message_system, const ModifierSystem& modifier_system, const EnemySystemCommand& command) noexcept
{
	const EnemyLeAttacked& data = std::get<struct EnemyLeAttacked>(command);
	const uint32_t index = data.EnemyIndex;

	if (!this->CheckIndex(index))
		return;

	this->EnemyAttackComponents[index].CanLeAttack = false;
	this->EnemyAttackComponents[index].LastLeAttack = data.Ticks;
	
	const size_t type_index = static_cast<size_t>(this->EnemyTypes[index]);
	const bool has_milk = modifier_system.EffectStatus(Effect::Milk);

	auto hook = this->LeAttackHooks[type_index];

	if (!has_milk && hook)
		(this->*hook)(message_system);
}

void EnemySystem::EnemyGotGluedHandler(MessageSystem& message_system, const ModifierSystem& modifier_system, const EnemySystemCommand& command) noexcept
{
	const EnemyGotGlued& data = std::get<struct EnemyGotGlued>(command);
	const uint32_t index = data.EnemyIndex;
	
	if (!this->CheckIndex(index))
		return;

	this->EnemySpeed[index] = 0.0f;
}

void EnemySystem::PlebifyEnemyHandler(MessageSystem& message_system, const ModifierSystem& modifier_system, const EnemySystemCommand& command) noexcept
{
	const PlebifyEnemy& data = std::get<struct PlebifyEnemy>(command);
	const uint32_t index = data.EnemyIndex;

	if (!this->CheckIndex(index))
		return;

	this->EnemyTypes[index] = EnemyType::Pleb;
}

void EnemySystem::EmitParticlesFromLocations(
		const size_t ticks, const size_t level, MessageSystem& message_system
		) noexcept
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
		const size_t ticks, const size_t level, MessageSystem& message_system
		) noexcept
{
	const size_t spawn_count = level * 15;
	const float level_scale = 1 + static_cast<float>(level) / 10.0f;

	const size_t new_size = this->EnemyHealth.size() + spawn_count;

	this->EnemyHealth.reserve(new_size);
	this->EnemySpeed.reserve(new_size);
	this->EnemyIsVisible.reserve(new_size);
	this->EnemyDirection.reserve(new_size);
	this->EnemyRect.reserve(new_size);
	this->EnemyCentre.reserve(new_size);
	this->EnemyTypes.reserve(new_size);
	this->EnemyAttackComponents.reserve(new_size);
	this->EnemyAnimationComponents.reserve(new_size);

	for (size_t i = 0, n = this->FutureSpawnLocations.size(); i < n; i++)
	{
		const Vector2 location = this->FutureSpawnLocations[i];
		const EnemyType type = this->FutureEnemyTypes[i];

		this->CreateEnemy(location.x, location.y, level_scale, type);
	}
}

void EnemySystem::PrepareSpawnEnemies(const size_t level, const float map_width, const float map_height) noexcept
{
	const size_t spawn_count = level * 15;
	
	EnemySystem::GenerateLocations(spawn_count, map_width, map_height);
	EnemySystem::GenerateTypes(spawn_count);
}

void EnemySystem::GenerateLocations(const size_t spawn_count, const float map_width, const float map_height) noexcept
{
	this->FutureSpawnLocations.clear();
	this->FutureSpawnLocations.reserve(spawn_count);

	for (size_t i = 0; i < spawn_count; i++)
	{
		this->FutureSpawnLocations.emplace_back(
				static_cast<float>(GetRandomValue(100, map_width - 100)),
				static_cast<float>(GetRandomValue(100, map_height - 100))
				);
	}
}

void EnemySystem::GenerateTypes(const size_t spawn_count) noexcept
{
	this->FutureEnemyTypes.clear();
	this->FutureEnemyTypes.reserve(spawn_count);

	for (size_t i = 0; i < spawn_count; i++)
		this->FutureEnemyTypes.emplace_back(static_cast<EnemyType>(GetRandomValue(0, static_cast<int>(EnemyType::COUNT) - 1)));
}

bool EnemySystem::CheckIndex(const uint32_t index) const noexcept
{
	return (index < this->EnemyHealth.size());
}

void EnemySystem::ApplyAussie(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyAussie)]++;

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(1), false, Timer::AussieExpire);
}

void EnemySystem::ApplyDrunk(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyDrunk)]++;
	
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(5), false, Timer::DrunkExpire);
}

void EnemySystem::ApplyPoison(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyPoison)]++;

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(1), true, Timer::PoisonTick);
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(5), false, Timer::PoisonExpire);
}

void EnemySystem::ApplyTariffs(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyTariffs)]++;

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(20), false, Timer::TariffsExpire);
}

void EnemySystem::ApplyWeakness(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyWeakness)]++;

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(25), false, Timer::WeaknessExpire);
}
