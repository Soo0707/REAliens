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

	int x5 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::LevelText).c_str(), 24) / 2.0f;
	int x6 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::TimePerLevel).c_str(), 24) / 2.0f;
	
	int x7 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::TotalDistance).c_str(), 24) / 2.0f;
	int x8 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::AverageSpeed).c_str(), 24) / 2.0f;

	int x9 = mid - MeasureText("[ENTER] Restart, [Esc] Quit", 21) / 2.0f;
	/*
	 24px + 10px padding per element vertically except at beginning of new section
	 24px + 20px padding per section
	*/
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("GAME OVER", x1, 80, 48, VIOLET);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::Duration).c_str(), x2, 160, 24, GOLD);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::TotalDamage).c_str(), x3, 204, 24, GOLD);
		DrawText(this->GlobalData->CachedStrings.at(CachedString::DamagePerSecond).c_str(), x4, 238, 24, GOLD);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::LevelText).c_str(), x5, 282, 24, GOLD);
		DrawText(this->GlobalData->CachedStrings.at(CachedString::TimePerLevel).c_str(), x6, 316, 24, GOLD);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::TotalDistance).c_str(), x7, 360, 24, GOLD);
		DrawText(this->GlobalData->CachedStrings.at(CachedString::AverageSpeed).c_str(), x8, 394, 24, GOLD);

		DrawText("[ENTER] Restart, [Esc] Quit", x9, 620, 21, LIGHTGRAY);
	EndTextureMode();
}

void GameOverMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_ENTER))
		this->GlobalData->ActiveState = State::GameReset;
	else if (IsKeyPressed(KEY_ESCAPE))
		this->GlobalData->Running = false;
}
