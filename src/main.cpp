#include "raylib.h"
#include "game.hpp"
#include "globalDataWrapper.hpp"
#include "powerupOverlay.hpp"

int main(void)
{
	int CurrentMonitor = GetCurrentMonitor();

	SetTargetFPS(GetMonitorRefreshRate(CurrentMonitor));
	InitWindow(1280, 720, "RE::Aliens");

	std::shared_ptr<GlobalDataWrapper> global_data = std::make_shared<GlobalDataWrapper>();

	Game game = Game(global_data);
	PowerupOverlay powerup_overlay = PowerupOverlay(global_data);
	
	while(!WindowShouldClose())
	{
		game.Update();
		game.HandleInput();
		powerup_overlay.HandleInput();

		BeginDrawing();
		ClearBackground(BLACK);
		game.Draw();
		powerup_overlay.Draw();
		EndDrawing();
	}
	CloseWindow();

	return 0;
}
