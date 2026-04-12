/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <array>
#include <memory>
#include <cstdint>
#include <cstddef>

#include "raylib.h"

#include "game.hpp"
#include "systemsResetState.hpp"
#include "powerupMenu.hpp"
#include "gameOverMenu.hpp"
#include "pauseMenu.hpp"
#include "mainMenu.hpp"

#include "states.hpp"
#include "messageSystem.hpp"
#include "commands.hpp"

class StateManager
{
	public:
		StateManager(
				std::shared_ptr<class Game> game_state, std::shared_ptr<class SystemsResetState> systems_reset_state,
				std::shared_ptr<class PowerupMenu> powerup_menu_state, std::shared_ptr<class GameOverMenu> game_over_menu_state,
				std::shared_ptr<class PauseMenu> pause_menu_state, std::shared_ptr<class MainMenu> main_menu_state
				);
		~StateManager() = default;

		void BeforeTick() noexcept;
		void Tick(MessageSystem& message_system) noexcept;
		void AfterTick(const RenderTexture2D& canvas) const noexcept;

		size_t Ticks;
		bool Terminate;

	private:
		void Reset() noexcept;
		void ExecuteCommands(MessageSystem& message_system) noexcept;

		const std::shared_ptr<class Game> GameState;
		const std::shared_ptr<class SystemsResetState> SystemsResetState;
		const std::shared_ptr<class PowerupMenu> PowerupMenuState;
		const std::shared_ptr<class GameOverMenu> GameOverMenuState;
		const std::shared_ptr<class PauseMenu> PauseMenuState;
		const std::shared_ptr<class MainMenu> MainMenuState;


		void GameBeforeTick() noexcept;
		void GameTick(MessageSystem& message_system) noexcept;
		void GameAfterTick(const RenderTexture2D& canvas) const noexcept;

		void SystemsResetTick(MessageSystem& message_system) noexcept;

		void PowerupMenuBeforeTick() noexcept;
		void PowerupMenuAfterTick(const RenderTexture2D& canvas) const noexcept;

		void GameOverMenuBeforeTick() noexcept;
		void GameOverMenuAfterTick(const RenderTexture2D& canvas) const noexcept;

		void PauseMenuBeforeTick() noexcept;
		void PauseMenuAfterTick(const RenderTexture2D& canvas) const noexcept;

		void MainMenuBeforeTick() noexcept;
		void MainMenuAfterTick(const RenderTexture2D& canvas) const noexcept;

		void GenerateGameOverStatsTick(MessageSystem& message_system) noexcept;

		static constexpr std::array<void(StateManager::*)() noexcept, static_cast<size_t>(State::COUNT)> BeforeTickHooks = {
			&StateManager::GameBeforeTick,
			nullptr,
			&StateManager::PowerupMenuBeforeTick,
			&StateManager::GameOverMenuBeforeTick,
			&StateManager::PauseMenuBeforeTick,
			&StateManager::MainMenuBeforeTick,
			nullptr
		};

		static constexpr std::array<void(StateManager::*)(MessageSystem&) noexcept, static_cast<size_t>(State::COUNT)> TickHooks = {
			&StateManager::GameTick,
			&StateManager::SystemsResetTick,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			&StateManager::GenerateGameOverStatsTick
		};

		static constexpr std::array<void(StateManager::*)(const RenderTexture2D&) const noexcept, static_cast<size_t>(State::COUNT)> AfterTickHooks = {
			&StateManager::GameAfterTick,
			nullptr,
			&StateManager::PowerupMenuAfterTick,
			&StateManager::GameOverMenuAfterTick,
			&StateManager::PauseMenuAfterTick,
			&StateManager::MainMenuAfterTick,
			nullptr
		};


		void SetStateHandler(const StateManagerCommand& command) noexcept;
		void SetTerminateHandler(const StateManagerCommand& command) noexcept;

		static constexpr std::array<void(StateManager::*)(const StateManagerCommand&), 2> CommandHandlers = {
			&StateManager::SetStateHandler,
			&StateManager::SetTerminateHandler
		};

		uint8_t TickIncrement;
		State ActiveState;
};
