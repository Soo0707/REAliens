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
	ResetState(reset_state),
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

void StateManager::Update(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept
{
	this->ExecuteCommands(message_system);
	
	this->PollGlobalInput();

	auto hook = this->StateHooks[static_cast<size_t>(this->ActiveState)];
	(this->*hook)(message_system, canvas);

	this->Ticks += this->TickIncrement
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
void StateManager::RunGame(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept
{
	this->TickIncrement = 1;

	this->GameState->Update();
	this->GameState->TickedUpdate();
	this->GameState->Draw(canvas);
}

void StateManager::RunSystemsReset(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept
{
	this->TickIncrement = 0;

	this->SystemsResetState->Reset();
	this->GameState->Reset();
	this->Reset();
	
	message_system.StateManagerCommands.emplace_back(std::in_place_type<struct StateManagerCommand>, State::Game);
}

void StateManager::RunPowerupMenu(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept
{
	this->TickIncrement = 0;

	this->PowerupMenuState->HandleInput();
	this->PowerupMenuState->Draw(canvas);
}

void StateManager::RunGameOverMenu(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept
{
	this->TickIncrement = 0;

	this->GameOverMenuState->HandleInput();
	this->GameOverMenuState->Draw(canvas);
}

void StateManager::RunPauseMenu(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept
{
	this->TickIncrement = 0;

	this->PauseMenuState->HandleInput();
	this->PauseMenuState->Draw(canvas);
}

void StateManager::RunMainMenu(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept
{
	this->TickIncrement = 0;

	this->MainMenuState->HandleInput();
	this->MainMenuState->Draw(canvas);
}
/*
void StateManager::RunGenerateGameOverStats() noexcept
{
	this->TickIncrement = 0;
	game_over.GenerateStats(game, *stat_system);
	global_data->ActiveState = State::GameOverMenu;
}
*/


// commands
void StateManager::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto const& command : message_system.StateSystemCommands)
	{
		auto handler = this->CommandHandlers[command.index()];

		(this->*handler)(command);
	}
}

void StateManager::SetStateHandler(const StateSystemCommand& command) noexcept
{
	const SetState& data = std::get<struct SetState>(command);
	this->ActiveState = data.NextState;
}
