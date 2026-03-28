/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

#include "raylib.h"
#include "assetManager.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"
#include "itemData.hpp"

class ItemSystem
{
	public:
		ItemSystem();
		~ItemSystem() = default;

		void Update(
				MessageSystem& message_system, const ModifierSystem& modifier_system, const AssetManager& assets,
				const Rectangle& update_area, const size_t ticks
				) noexcept;

		void Reset() noexcept;

		void ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept;

		void Draw(const AssetManager& assets) const noexcept;
		void DrawLightmap() const noexcept;

		const std::vector<Vector2>& GetItemCentre() const noexcept;

	private:
		void RunUpdateHooks(MessageSystem& message_system, const ModifierSystem& modifier_system) noexcept;
		void VisibilityCheck(const Rectangle& update_area) noexcept;
		void CreateItem(const float x, const float y, const float texture_width, const float texture_height) noexcept;
		void RemoveItem() noexcept;
		void EmitParticles(MessageSystem& message_system, const size_t ticks) noexcept;

		static constexpr std::array<ItemData, static_cast<size_t>(Item::COUNT)> ItemAttributes = {
			(ItemData) { GREEN, TextureKey::Xp, true},
			(ItemData) { WHITE, TextureKey::Turret, false },
			(ItemData) { LIGHTGRAY, TextureKey::Glue, false },
		};
		
		void DoNothingHook(MessageSystem& message_system, const size_t item_index, const ModifierSystem& modifier_system) const noexcept;
		void DoNothingHook(MessageSystem& message_system, const size_t item_index) noexcept;
		void DoNothingHook(MessageSystem& message_system, const size_t item_index, const size_t enemy_index) noexcept;


		void TurretUpdateHook(MessageSystem& message_system, const size_t item_index, const ModifierSystem& modifier_system) const noexcept;

		using UpdateHook = void(ItemSystem::*)(MessageSystem&, const size_t, const ModifierSystem&) const noexcept;

		static constexpr std::array<UpdateHook, static_cast<size_t>(Item::COUNT)> UpdateHooks = {
			&DoNothingHook,
			&TurretUpdateHook,
			&DoNothingHook
		};


		void XpCollisionHook(MessageSystem& message_system, const size_t item_index) noexcept;

		using CollisionHook = void(ItemSystem::*)(MessageSystem&, const size_t) noexcept; 

		static constexpr std::array<CollisionHook, static_cast<size_t>(Item::COUNT)> CollisionHooks = {
			&XpCollisionHook,
			&DoNothingHook,
			&DoNothingHook
		};


		void GlueEnemyItemCollisionHook(MessageSystem& message_system, const size_t item_index, const size_t enemy_index) noexcept;

		using EnemyItemCollisionHook = void(ItemSystem::*)(MessageSystem&, const size_t, const size_t) noexcept;

		static constexpr std::array<EnemyItemCollisionHook, static_cast<size_t>(Item::COUNT)> EnemyItemCollisionHooks = {
			&DoNothingHook,
			&DoNothingHook,
			&GlueEnemyItemCollisionHook
		};


		void CreateItemHandler(MessageSystem& message_system, const ItemSystemCommand& command, const AssetManager& assets) noexcept;
		void CollidedWithItemHandler(MessageSystem& message_system, const ItemSystemCommand& command, const AssetManager& assets) noexcept;
		void EnemyItemCollisionHandler(MessageSystem& message_system, const ItemSystemCommand& command, const AssetManager& assets) noexcept;

		bool CheckIndex(const size_t index) const noexcept;

		using CommandHandler = void(ItemSystem::*)(MessageSystem&, const ItemSystemCommand&, const AssetManager&) noexcept;

		static constexpr std::array<CommandHandler, 3> CommandHandlers = {
			&CreateItemHandler,
			&CollidedWithItemHandler,
			&EnemyItemCollisionHandler
		};
		
		std::vector<Item> ItemType;
		std::vector<uint8_t> ItemIsVisible;
		std::vector<uint8_t> ItemKill;
		std::vector<Vector2> ItemCentre;
		std::vector<Rectangle> ItemRect;
};
