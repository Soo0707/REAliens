#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"

#include "game.hpp"
#include "powerupMenu.hpp"
#include "gameOverMenu.hpp"
#include "pauseMenu.hpp"
#include "mainMenu.hpp"

#include "constants.hpp"

int main(void)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(REFERENCE_WIDTH, REFERENCE_HEIGHT, "RE::Aliens");
	InitAudioDevice();

	RenderTexture2D virtual_canvas = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);
	SetTextureFilter(virtual_canvas.texture, TEXTURE_FILTER_POINT);

	SetWindowMinSize(REFERENCE_WIDTH, REFERENCE_HEIGHT);

	unsigned int max_refresh_rate = 3 * GetMonitorRefreshRate(GetCurrentMonitor());
	SetTargetFPS(max_refresh_rate);

	SetExitKey(KEY_NULL);

	std::shared_ptr<AssetManager> assets = std::make_shared<AssetManager>();
	std::shared_ptr<GlobalDataWrapper> global_data = std::make_shared<GlobalDataWrapper>();

	Game game = Game(global_data, assets);
	PowerupMenu powerup_menu = PowerupMenu(global_data, assets);
	GameOverMenu game_over = GameOverMenu(global_data, assets);
	PauseMenu pause = PauseMenu(global_data, assets);
	MainMenu main_menu = MainMenu(global_data, assets);

	State prev_state = global_data->ActiveState;

	while (!WindowShouldClose() && global_data->Running)
	{
		if (global_data->ActiveState != prev_state)
		{
			(global_data->ActiveState == State::Game) ? SetTargetFPS(max_refresh_rate) : SetTargetFPS(15);
			prev_state = global_data->ActiveState;
		}

		if (IsKeyPressed(KEY_F11))
		{
			ToggleBorderlessWindowed();
			EnableCursor();
		}

		switch (global_data->ActiveState)
		{
			case State::GameReset:
				game.Reset();
				global_data->ActiveState = State::Game;
				break;
			case State::MainMenu:
				main_menu.HandleInput();
				main_menu.Draw(virtual_canvas);
				break;
			case State::Game:
				game.HandleEssentialInput();
				game.Update();
				game.Draw(virtual_canvas);
				break;
			case State::PowerupMenu:
				powerup_menu.HandleInput();
				powerup_menu.Draw(virtual_canvas);
				break;
			case State::GameOverMenu:
				game_over.HandleInput();
				game_over.Draw(virtual_canvas);
				break;
			case State::PauseMenu:
				pause.HandleInput();
				pause.Draw(virtual_canvas);
				break;
		}

		BeginDrawing();

			ClearBackground(BLACK);

			DrawTexturePro(
					virtual_canvas.texture,
					(Rectangle) { 0, 0, REFERENCE_WIDTH, -static_cast<float>(REFERENCE_HEIGHT) },
					(Rectangle) { 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
					(Vector2) { 0, 0 },
					0.0f,
					WHITE
					);

		EndDrawing();
	}

	UnloadRenderTexture(virtual_canvas);

	CloseAudioDevice();

	CloseWindow();

	return 0;
}
