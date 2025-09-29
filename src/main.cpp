#include "raylib.h"
#include "game.hpp"

int main(void)
{
	int CurrentMonitor = GetCurrentMonitor();

	SetTargetFPS(GetMonitorRefreshRate(CurrentMonitor));
	InitWindow(1280, 720, "RE::Aliens");

	Game game = Game();
	
	while(!WindowShouldClose())
	{
		game.Update();
		game.HandleInput();
		game.Draw();
	}
	CloseWindow();

	return 0;
}
