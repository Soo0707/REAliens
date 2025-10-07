#include "raylib.h"
#include "game.hpp"
#include "globalDataWrapper.hpp"
#include "powerupMenu.hpp"

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
	
	while (!WindowShouldClose())
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
				/*
			case State::PauseMenu:
				break;
				*/
		}
	}
	CloseWindow();

	return 0;
}
