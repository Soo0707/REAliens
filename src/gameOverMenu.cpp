#include "gameOverMenu.hpp"

#include "raylib.h"
#include "globalDataWrapper.hpp"

GameOverMenu::GameOverMenu(std::shared_ptr<GlobalDataWrapper> global_data) :
	GlobalData(global_data)
{}

GameOverMenu::~GameOverMenu()
{}

void GameOverMenu::Draw()
{
	float mid = 1280 / 2.0f;

	int x1 = mid - MeasureText("GAME OVER", 48) / 2.0f;
	int x2 = mid - MeasureText("[Space] Restart, [Esc] Quit", 24) / 2.0f;

	DrawText("GAME OVER", x1, 260, 48, VIOLET);
	DrawText("[Space] Restart, [Esc] Quit", x2, 360, 24, VIOLET);
}

void GameOverMenu::HandleInput()
{
	if (IsKeyPressed(KEY_SPACE))
		this->GlobalData->ActiveState = State::Game;
	//else if (IsKeyPressed(KEY_ESC))
}
