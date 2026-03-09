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

		void Update(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept;

		size_t Ticks;
		bool Terminate;

	private:
		void Reset() noexcept;
		void PollGlobalInput() const noexcept;
		void ExecuteCommands(MessageSystem& message_system) noexcept;

		const std::shared_ptr<class Game> GameState;
		const std::shared_ptr<class SystemsResetState> SystemsResetState;
		const std::shared_ptr<class PowerupMenu> PowerupMenuState;
		const std::shared_ptr<class GameOverMenu> GameOverMenuState;
		const std::shared_ptr<class PauseMenu> PauseMenuState;
		const std::shared_ptr<class MainMenu> MainMenuState;


		void RunGame(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept;
		void RunSystemsReset(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept;
		void RunPowerupMenu(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept;
		void RunGameOverMenu(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept;
		void RunPauseMenu(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept;
		void RunMainMenu(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept;
		void RunGenerateGameOverStats(MessageSystem& message_system, const RenderTexture2D& canvas) noexcept;

		static constexpr std::array<void(StateManager::*)(MessageSystem&, const RenderTexture2D&), static_cast<size_t>(State::COUNT)> StateHooks = {
			&RunGame,
			&RunSystemsReset,
			&RunPowerupMenu,
			&RunGameOverMenu,
			&RunPauseMenu,
			&RunMainMenu,
			&RunGenerateGameOverStats
		};

		void SetStateHandler(const StateManagerCommand& command) noexcept;
		void SetTerminateHandler(const StateManagerCommand& command) noexcept;

		static constexpr std::array<void(StateManager::*)(const StateManagerCommand&), 2> CommandHandlers = {
			&SetStateHandler,
			&SetTerminateHandler
		};

		uint8_t TickIncrement;
		State ActiveState;
};
