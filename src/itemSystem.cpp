/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "itemSystem.hpp"

#include <cstddef>
#include <cstdint>
#include <variant>

#include "raylib.h"
#include "raymath.h"
#include "constants.hpp"
#include "assetManager.hpp"
#include "projectileData.hpp"
#include "enemyData.hpp"
#include "commands.hpp"
#include "signals.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"
#include "modifiers.hpp"

ItemSystem::ItemSystem()
{
	this->ItemType.reserve(128);
	this->ItemIsVisible.reserve(128);
	this->ItemHitsLeft.reserve(128);
	this->ItemCentre.reserve(128);
	this->ItemRect.reserve(128);
}

void ItemSystem::Update(
		MessageSystem& message_system, const ModifierSystem& modifier_system, const AssetManager& assets,
		const Rectangle& update_area, const size_t ticks
		) noexcept
{
	this->ExecuteCommands(message_system, assets);

	this->VisibilityCheck(update_area);
	this->RunUpdateHooks(message_system, modifier_system);
	this->EmitParticles(message_system, ticks);
	this->KillItems();
}

void ItemSystem::Reset() noexcept
{
	this->ItemType.clear();
	this->ItemIsVisible.clear();
	this->ItemHitsLeft.clear();
	this->ItemCentre.clear();
	this->ItemRect.clear();
}

void ItemSystem::ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept
{
	for (auto const& command : message_system.ItemSystemCommands)
	{
		const size_t handler_index = command.index();
		auto handler = this->CommandHandlers[handler_index];
		(this->*handler)(message_system, command, assets);
	}

	message_system.ItemSystemCommands.clear();
}

void ItemSystem::Draw(const AssetManager& assets) const noexcept
{
	for (size_t i = 0, n = this->ItemIsVisible.size(); i < n; i++)
	{
		if (this->ItemIsVisible[i])
		{
			const size_t type_index = static_cast<size_t>(this->ItemType[i]);

			DrawTexture(
					assets.GetTexture(this->ItemAttributes[type_index].Texture),
					static_cast<int>(this->ItemRect[i].x),
					static_cast<int>(this->ItemRect[i].y),
					WHITE
					);
		}
	}
}

void ItemSystem::DrawLightmap() const noexcept
{
	for (size_t i = 0, n = this->ItemIsVisible.size(); i < n; i++)
	{
		if (this->ItemIsVisible[i])
		{
			const size_t type_index = static_cast<size_t>(this->ItemType[i]);

			DrawCircleGradient(
					this->ItemCentre[i].x, this->ItemCentre[i].y, 24,
					this->ItemAttributes[type_index].Colour, LIGHTGRAY
					);
		}
	}
}

void ItemSystem::VisibilityCheck(const Rectangle& update_area) noexcept
{
	for (size_t i = 0, n = this->ItemIsVisible.size(); i < n; i++)
		this->ItemIsVisible[i] = static_cast<uint8_t>(CheckCollisionRecs(update_area, this->ItemRect[i]));
}

void ItemSystem::RunUpdateHooks(MessageSystem& message_system, const ModifierSystem& modifier_system) noexcept
{
	for (size_t i = 0, n = this->ItemType.size(); i < n; i++)
	{
		const size_t type_index = static_cast<size_t>(this->ItemType[i]);

		auto hook = this->UpdateHooks[type_index];

		if (hook)
			(this->*hook)(message_system, i, modifier_system);
	}
}


void ItemSystem::EmitParticles(MessageSystem& message_system, const size_t ticks) noexcept
{
	for (size_t i = 0, n = this->ItemIsVisible.size(); i < n; i++)
	{
		const size_t type_index = static_cast<size_t>(this->ItemType[i]);

		if (this->ItemAttributes[type_index].EmitParticles && this->ItemIsVisible[i] && GetRandomValue(1, 100) < 3)
		{
			const Vector2 velocity = { static_cast<float>(GetRandomValue(-64, 64)), static_cast<float>(GetRandomValue(-96, -32)) };

			message_system.ParticleSystemCommands.emplace_back(
					ticks, 1, velocity, this->ItemCentre[i].x, this->ItemCentre[i].y, 
					10, 25, 120, TICK_RATE, 0, this->ItemAttributes[type_index].Colour, GRAY
					);
		}
	}
}

void ItemSystem::KillItems() noexcept
{
	for (size_t i = 0; i < this->ItemHitsLeft.size(); )
	{
		if (this->ItemHitsLeft[i] == 0)
		{
			this->ItemType[i] = this->ItemType.back();
			this->ItemIsVisible[i] = this->ItemIsVisible.back();
			this->ItemHitsLeft[i] = this->ItemHitsLeft.back();
			this->ItemCentre[i] = this->ItemCentre.back();
			this->ItemRect[i] = this->ItemRect.back();

			this->ItemType.pop_back();
			this->ItemIsVisible.pop_back();
			this->ItemHitsLeft.pop_back();
			this->ItemCentre.pop_back();
			this->ItemRect.pop_back();
		}
		else
			i++;
	}
}

void ItemSystem::CreateItemHandler(MessageSystem& message_system, const ItemSystemCommand& command, const AssetManager& assets) noexcept
{
	const struct CreateItem& data = std::get<struct CreateItem>(command);

	const size_t type_index = static_cast<size_t>(data.Type);
	const TextureKey texture_key = this->ItemAttributes[type_index].Texture;
	
	const float texture_width = assets.GetTexture(texture_key).width;
	const float texture_height = assets.GetTexture(texture_key).height;

	const uint16_t hits_left = this->ItemAttributes[type_index].MaxCollisionHits;

	this->ItemType.emplace_back(data.Type);
	this->ItemIsVisible.emplace_back(static_cast<bool>(false));
	this->ItemHitsLeft.emplace_back(hits_left);
	this->ItemCentre.emplace_back(data.X + texture_width / 2.0f, data.Y + texture_height / 2.0f);
	this->ItemRect.emplace_back(data.X, data.Y, texture_width, texture_height);
}

void ItemSystem::CollidedWithItemHandler(MessageSystem& message_system, const ItemSystemCommand& command, const AssetManager& assets) noexcept
{
	const struct CollidedWithItem& data = std::get<struct CollidedWithItem>(command);
	const uint32_t index = data.ItemIndex;
	
	if (!this->CheckIndex(index))
		return;

	if (this->ItemHitsLeft[index] == 0)
		return;
	
	const size_t type_index = static_cast<size_t>(this->ItemType[index]);

	auto hook = this->CollisionHooks[type_index];

	if (hook)
		(this->*hook)(message_system, index);
}

void ItemSystem::EnemyItemCollisionHandler(MessageSystem& message_system, const ItemSystemCommand& command, const AssetManager& assets) noexcept
{
	const struct EnemyItemCollision& data = std::get<struct EnemyItemCollision>(command);
	const uint32_t index = data.ItemIndex;

	if (!this->CheckIndex(index))
		return;

	if (this->ItemHitsLeft[index] == 0)
		return;

	const size_t type_index = static_cast<size_t>(this->ItemType[index]);

	auto hook = this->EnemyItemCollisionHooks[type_index];

	if (hook)
		(this->*hook)(message_system, data);
}

bool ItemSystem::CheckIndex(const uint32_t index) const noexcept
{
	return (index < this->ItemType.size());
}

const std::vector<Vector2>& ItemSystem::GetItemCentre() const noexcept
{
	return this->ItemCentre;
}

void ItemSystem::TurretUpdateHook(MessageSystem& message_system, const uint32_t item_index, const ModifierSystem& modifier_system) const noexcept
{
	if (GetRandomValue(1, 100) < 2)
	{
		const Vector2 direction = Vector2Rotate({ 1.0f, 0.0f }, static_cast<float>(GetRandomValue(0, 360) * TO_RAD));
		const Vector2 item_centre = this->ItemCentre[item_index];
		const float speed = modifier_system.GetAttribute(Attribute::BulletSpeed);

		message_system.ProjectileSystemCommands.emplace_back(
			std::in_place_type<CreateProjectile>, ProjectileType::Bullet, direction,
			item_centre.x, item_centre.y, speed
			);
	}
}


void ItemSystem::XpCollisionHook(MessageSystem& message_system, const uint32_t item_index) noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::IncrementCollectedXp)]++;
	this->ItemHitsLeft[item_index]--;

}

void ItemSystem::XpEnemyItemCollisionHook(MessageSystem& message_system, const EnemyItemCollision& data) noexcept
{
	const uint32_t item_index = data.ItemIndex;
	const EnemyType enemy_type = data.EnemyType;
	
	if (enemy_type == EnemyType::Orange)
		this->ItemHitsLeft[item_index]--;
}

void ItemSystem::GlueEnemyItemCollisionHook(MessageSystem& message_system, const EnemyItemCollision& data) noexcept
{
	const uint32_t item_index = data.ItemIndex;
	const uint32_t enemy_index = data.EnemyIndex;

	message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct EnemyGotGlued>, enemy_index);
	this->ItemHitsLeft[item_index]--;
}

void ItemSystem::TurretEnemyItemCollisionHook(MessageSystem& message_system, const EnemyItemCollision& data) noexcept
{
	const uint32_t item_index = data.ItemIndex;
	this->ItemHitsLeft[item_index]--;
}
