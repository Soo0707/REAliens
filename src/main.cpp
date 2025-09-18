#include "raylib.h"
#include "game.hpp"

int main(void)
{
	SetTargetFPS(60);
	InitWindow(1280, 720, "RE::Aliens");

	Game game = Game();
	while(!WindowShouldClose())
	{
		game.Update();
		game.HandleInput();

		BeginDrawing();
		game.Draw();
		ClearBackground(RAYWHITE);
		EndDrawing();
	}
	CloseWindow();

	return 0;
}
