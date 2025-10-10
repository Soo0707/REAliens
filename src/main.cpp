#include "raylib.h"
#include "globalDataWrapper.hpp"

#include "game.hpp"
#include "powerupMenu.hpp"
#include "gameOverMenu.hpp"
#include "pauseMenu.hpp"
#include <iostream>
static constexpr int REFERENCE_WIDTH = 1280;
static constexpr int REFERENCE_HEIGHT = 720;

int main(void)
{
	int CurrentMonitor = GetCurrentMonitor();

	SetTargetFPS(GetMonitorRefreshRate(CurrentMonitor));
	InitWindow(REFERENCE_WIDTH, REFERENCE_HEIGHT, "RE::Aliens");
	SetExitKey(KEY_NULL);

	std::shared_ptr<GlobalDataWrapper> global_data = std::make_shared<GlobalDataWrapper>();

	Game game = Game(global_data);
	PowerupMenu powerup_menu = PowerupMenu(global_data);
	GameOverMenu game_over = GameOverMenu(global_data);
	PauseMenu pause = PauseMenu(global_data);

	while (global_data->Running)
	{
		switch (global_data->ActiveState)
		{
			case State::Game:
				game.Update();

				BeginDrawing();
				ClearBackground(BLACK);

				game.Draw();
				EndDrawing();
				break;
			case State::PowerupMenu:
				powerup_menu.HandleInput();

				BeginDrawing();
				ClearBackground(BLACK);

				powerup_menu.Draw();

				EndDrawing();
				break;
			case State::GameOverMenu:
				game_over.HandleInput();

				BeginDrawing();
				ClearBackground(BLACK);
				game_over.Draw();
				EndDrawing();
				break;
			case State::PauseMenu:
				pause.HandleInput();

				BeginDrawing();
				ClearBackground(BLACK);
				pause.Draw();
				EndDrawing();
				break;
		}
	}
	CloseWindow();

	return 0;
}
