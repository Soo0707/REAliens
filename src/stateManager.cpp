/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "stateManager.hpp"

#include <variant>
#include <cstddef>
#include <cstdint>
#include <memory>

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
#include "constants.hpp"

StateManager::StateManager(
		std::shared_ptr<class Game> game_state, std::shared_ptr<class SystemsResetState> systems_reset_state,
		std::shared_ptr<class PowerupMenu> powerup_menu_state, std::shared_ptr<class GameOverMenu> game_over_menu_state,
		std::shared_ptr<class PauseMenu> pause_menu_state, std::shared_ptr<class MainMenu> main_menu_state
		) :
	Terminate(false),
	ActiveState(State::MainMenu),
	GameState(game_state),
	SystemsResetState(systems_reset_state),
	PowerupMenuState(powerup_menu_state),
	GameOverMenuState(game_over_menu_state),
	PauseMenuState(pause_menu_state),
	MainMenuState(main_menu_state)
{
	this->Reset();
}

void StateManager::Reset() noexcept
{
	this->Ticks = 0;
	this->TickIncrement = 0;
}

void StateManager::BeforeTick() noexcept
{
	this->PollGlobalInput();

	auto hook = this->BeforeTickHooks[static_cast<size_t>(this->ActiveState)];
	if (hook)
		(this->*hook)();
}

void StateManager::Tick(MessageSystem& message_system) noexcept
{
	this->ExecuteCommands(message_system);

	auto hook = this->TickHooks[static_cast<size_t>(this->ActiveState)];
	if (hook)
		(this->*hook)(message_system);

	this->Ticks += this->TickIncrement;
}

void StateManager::AfterTick(const RenderTexture2D& canvas) const noexcept
{
	auto hook = this->AfterTickHooks[static_cast<size_t>(this->ActiveState)];
	if (hook)
		(this->*hook)(canvas);
}


void StateManager::PollGlobalInput() const noexcept
{
	if (IsKeyPressed(KEY_F11))
	{
		ToggleBorderlessWindowed();
		EnableCursor();
	}

	SetMouseScale(
			REFERENCE_WIDTH / static_cast<float>(GetScreenWidth()),
			REFERENCE_HEIGHT / static_cast<float>(GetScreenHeight())
			);
}


// states
void StateManager::GameBeforeTick() noexcept
{
	this->GameState->UntickedInput();
}

void StateManager::GameTick(MessageSystem& message_system) noexcept
{
	this->TickIncrement = 1;

	this->GameState->TickedInput();
	this->GameState->Update(this->Ticks);
}

void StateManager::GameAfterTick(const RenderTexture2D& canvas) const noexcept
{
	this->GameState->Draw(canvas);
}


void StateManager::SystemsResetTick(MessageSystem& message_system) noexcept
{
	this->TickIncrement = 0;

	this->SystemsResetState->Reset();
	this->GameState->Reset();
	this->Reset();
	
	message_system.StateManagerCommands.emplace_back(std::in_place_type<SetState>, State::Game);
}


void StateManager::PowerupMenuBeforeTick() noexcept
{
	this->TickIncrement = 0;
	this->PowerupMenuState->Input();
}

void StateManager::PowerupMenuAfterTick(const RenderTexture2D& canvas) const noexcept
{
	this->PowerupMenuState->Draw(canvas);
}


void StateManager::GameOverMenuBeforeTick() noexcept
{
	this->TickIncrement = 0;
	this->GameOverMenuState->Input();
}

void StateManager::GameOverMenuAfterTick(const RenderTexture2D& canvas) const noexcept
{
	this->GameOverMenuState->Draw(canvas);
}


void StateManager::PauseMenuBeforeTick() noexcept
{
	this->TickIncrement = 0;
	this->PauseMenuState->Input();
}

void StateManager::PauseMenuAfterTick(const RenderTexture2D& canvas) const noexcept
{
	this->PauseMenuState->Draw(canvas);
}

void StateManager::MainMenuBeforeTick() noexcept
{
	this->TickIncrement = 0;
	this->MainMenuState->Input();
}

void StateManager::MainMenuAfterTick(const RenderTexture2D& canvas) const noexcept
{
	this->MainMenuState->Draw(canvas);
}


void StateManager::GenerateGameOverStatsTick(MessageSystem& message_system) noexcept
{
	this->TickIncrement = 0;

	this->GameOverMenuState->GenerateStats(this->Ticks);
	message_system.StateManagerCommands.emplace_back(std::in_place_type<SetState>, State::GameOverMenu);
}



// commands
void StateManager::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto const& command : message_system.StateManagerCommands)
	{
		auto handler = this->CommandHandlers[command.index()];

		(this->*handler)(command);
	}
}

void StateManager::SetStateHandler(const StateManagerCommand& command) noexcept
{
	const SetState& data = std::get<struct SetState>(command);
	this->ActiveState = data.NextState;
}

void StateManager::SetTerminateHandler(const StateManagerCommand& command) noexcept
{
	const SetTerminate& data = std::get<struct SetTerminate>(command);
	this->Terminate = data.Terminate;
}
