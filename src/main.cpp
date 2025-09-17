#include "raylib.h"
#include "game.hpp"

int main(void)
{
	InitWindow(1280, 720, "RE::Aliens");

	Game game = Game();
	while(!WindowShouldClose())
	{
		BeginDrawing();
		game.Draw();
		ClearBackground(RAYWHITE);
		EndDrawing();
	}
	CloseWindow();

	return 0;
}
