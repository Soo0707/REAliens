#include "gameOverMenu.hpp"

#include "raylib.h"
#include "globalDataWrapper.hpp"
#include "assetManager.hpp"

GameOverMenu::GameOverMenu(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets) :
	GlobalData(global_data),
	Assets(assets)
{}

void GameOverMenu::Draw(RenderTexture2D& canvas) const noexcept
{
	float mid = 1280 / 2.0f;

	int x1 = mid - MeasureText("GAME OVER", 48) / 2.0f;
	int x2 = mid - MeasureText("[Space] Restart, [Esc] Quit", 24) / 2.0f;
	
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("GAME OVER", x1, 260, 48, VIOLET);
		DrawText("[Space] Restart, [Esc] Quit", x2, 360, 24, VIOLET);

	EndTextureMode();
}

void GameOverMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_SPACE))
		this->GlobalData->ActiveState = State::GameReset;
	else if (IsKeyPressed(KEY_ESCAPE))
		this->GlobalData->Running = false;
}
