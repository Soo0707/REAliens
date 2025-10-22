#include "raylib.h"
#include "globalDataWrapper.hpp"

#include "game.hpp"
#include "powerupMenu.hpp"
#include "gameOverMenu.hpp"
#include "pauseMenu.hpp"
#include "constants.hpp"


int main(void)
{
	SetConfigFlags(FLAG_FULLSCREEN_MODE);

	InitWindow(GetScreenWidth(), GetScreenHeight(), "RE::Aliens");

	RenderTexture2D virtual_canvas = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);
	SetTextureFilter(virtual_canvas.texture, TEXTURE_FILTER_POINT);

	SetWindowMinSize(REFERENCE_WIDTH, REFERENCE_HEIGHT);

	unsigned int max_refresh_rate = 2 * GetMonitorRefreshRate(GetCurrentMonitor());
	SetTargetFPS(max_refresh_rate);

	SetExitKey(KEY_NULL);

	std::shared_ptr<GlobalDataWrapper> global_data = std::make_shared<GlobalDataWrapper>();

	Game game = Game(global_data);
	PowerupMenu powerup_menu = PowerupMenu(global_data);
	GameOverMenu game_over = GameOverMenu(global_data);
	PauseMenu pause = PauseMenu(global_data);

	State prev_state = global_data->ActiveState;

	while (!WindowShouldClose() && global_data->Running)
	{
		Vector2 scale_factors = { GetScreenWidth() / REFERENCE_WIDTH, GetScreenHeight() / REFERENCE_HEIGHT };

		if (global_data->ActiveState != prev_state)
		{
			(global_data->ActiveState == State::Game) ? ( SetTargetFPS(max_refresh_rate) ) : ( SetTargetFPS(15) );
			prev_state = global_data->ActiveState;
		}

		switch (global_data->ActiveState)
		{
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
					(Rectangle) {
						0,
						0,
						GetScreenWidth(),
						GetScreenHeight()
						},
					(Vector2) { 0, 0 },
					0.0f,
					WHITE
					);

		EndDrawing();
	}

	UnloadRenderTexture(virtual_canvas);
	CloseWindow();

	return 0;
}
