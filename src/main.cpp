#include "raylib.h"
#include "globalDataWrapper.hpp"

#include "game.hpp"
#include "powerupMenu.hpp"
#include "gameOverMenu.hpp"
#include "pauseMenu.hpp"
#include "constants.hpp"

int main(void)
{
	int current_monitor = GetCurrentMonitor();
	unsigned int max_refresh_rate = GetMonitorRefreshRate(current_monitor);

	InitWindow(REFERENCE_WIDTH, REFERENCE_HEIGHT, "RE::Aliens");

	SetExitKey(KEY_NULL);

	std::shared_ptr<GlobalDataWrapper> global_data = std::make_shared<GlobalDataWrapper>();

	Game game = Game(global_data);
	PowerupMenu powerup_menu = PowerupMenu(global_data);
	GameOverMenu game_over = GameOverMenu(global_data);
	PauseMenu pause = PauseMenu(global_data);

	State prev_state = global_data->ActiveState;

	while (!WindowShouldClose() && global_data->Running)
	{
		if (global_data->ActiveState != prev_state)
		{
			(global_data->ActiveState == State::Game) ? ( SetTargetFPS(max_refresh_rate) ) : ( SetTargetFPS(15) );
			prev_state = global_data->ActiveState;
		}

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
