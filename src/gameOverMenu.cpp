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
	int x2 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::Duration).c_str(), 24) / 2.0f;
	int x3 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::TotalDamage).c_str(), 24) / 2.0f;

	int x4 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::DamagePerSecond).c_str(), 24) / 2.0f;
	int x5 = mid - MeasureText("[Space] Restart, [Esc] Quit", 21) / 2.0f;
	
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("GAME OVER", x1, 80, 48, VIOLET);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::Duration).c_str(), x2, 260, 24, GOLD);
		DrawText(this->GlobalData->CachedStrings.at(CachedString::TotalDamage).c_str(), x3, 360, 24, GOLD);
		DrawText(this->GlobalData->CachedStrings.at(CachedString::DamagePerSecond).c_str(), x4, 460, 24, GOLD);

		DrawText("[Space] Restart, [Esc] Quit", x5, 620, 21, LIGHTGRAY);

	EndTextureMode();
}

void GameOverMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_SPACE))
		this->GlobalData->ActiveState = State::GameReset;
	else if (IsKeyPressed(KEY_ESCAPE))
		this->GlobalData->Running = false;
}
