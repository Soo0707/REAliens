#include "mainMenu.hpp"

#include <memory>

#include "raylib.h"
#include "globalDataWrapper.hpp"
#include "assetManager.hpp"

MainMenu::MainMenu(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets) :
	GlobalData(global_data),
	Assets(assets)
{}

void MainMenu::Draw(RenderTexture2D& canvas) const noexcept
{
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("RE::ALIENS", 292, 100, 128, VIOLET);
		DrawText("No Sounds Included", 407, 250, 48, GOLD);
		DrawText("[SPACE] Start, [Esc] Quit, [F11] Toggle Fullscreen", 240.5, 620, 32, LIGHTGRAY);
	EndTextureMode();
}

void MainMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_SPACE))
		this->GlobalData->ActiveState = State::GameReset;

	if (IsKeyPressed(KEY_ESCAPE))
		this->GlobalData->Running = false;
}
