/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>

#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"
#include "statSystem.hpp"
#include "messageSystem.hpp"

class GameOverMenu
{
	public:
		GameOverMenu(
				std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
				std::shared_ptr<class StatSystem> stat_system, std::shared_ptr<struct MessageSystem> message_system
				);

		~GameOverMenu() = default;

		void Draw(const RenderTexture2D& canvas) const noexcept;
		void HandleInput() const noexcept;

		void GenerateStats(const size_t ticks) noexcept;

	private:
		const std::shared_ptr<class GlobalDataWrapper> GlobalData;
		const std::shared_ptr<class AssetManager> Assets;
		const std::shared_ptr<class StatSystem> StatSystem;
		const std::shared_ptr<struct MessageSystem> MessageSystem;
};
