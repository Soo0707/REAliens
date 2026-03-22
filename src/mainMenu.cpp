/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "mainMenu.hpp"

#include <memory>
#include <variant>

#include "raylib.h"

#include "assetManager.hpp"
#include "messageSystem.hpp"

MainMenu::MainMenu(
		std::shared_ptr<class AssetManager> assets,
		std::shared_ptr<struct MessageSystem> message_system
		) :
	Assets(assets),
	MessageSystem(message_system)
{}

void MainMenu::Draw(const RenderTexture2D& canvas) const noexcept
{
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("RE::ALIENS", 292, 100, 128, VIOLET);
		DrawText("No Sounds Included", 407, 250, 48, GOLD);
		DrawText("[SPACE] Start, [Esc] Quit, [F11] Toggle Fullscreen", 240.5, 620, 32, LIGHTGRAY);
	EndTextureMode();
}

void MainMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_SPACE))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<SetState>, State::SystemsReset);

	if (IsKeyPressed(KEY_ESCAPE))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<SetTerminate>, true);
}
