#include "gameOverMenu.hpp"

#include "raylib.h"

#include "stats.hpp"
#include "statSystem.hpp"
#include "globalDataWrapper.hpp"
#include "assetManager.hpp"
#include "constants.hpp"

GameOverMenu::GameOverMenu(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets) :
	GlobalData(global_data),
	Assets(assets)
{}

void GameOverMenu::Draw(RenderTexture2D& canvas) const noexcept
{
	const float mid = 1280 / 2.0f;

	const int x1 = mid - MeasureText("GAME OVER", 48) / 2.0f;

	const int x2 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::Duration).c_str(), 24) / 2.0f;

	const int x3 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::EnemiesKilled).c_str(), 24) / 2.0f;

	const int x5 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::DamagePerSecond).c_str(), 24) / 2.0f;

	const int x6 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::LevelText).c_str(), 24) / 2.0f;
	const int x9 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::AverageSpeed).c_str(), 24) / 2.0f;

	const int x10 = mid - MeasureText(this->GlobalData->CachedStrings.at(CachedString::GameOverReason).c_str(), 24) / 2.0f;

	const int x11 = mid - MeasureText("[ENTER] Restart, [Esc] Quit", 21) / 2.0f;
	/*
	 24px + 10px padding per element vertically except at beginning of new section
	 24px + 20px padding per section
	*/
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("GAME OVER", x1, 80, 48, VIOLET);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::Duration).c_str(), x2, 160, 24, GOLD);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::EnemiesKilled).c_str(), x3, 204, 24, GOLD);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::DamagePerSecond).c_str(), x5, 282, 24, GOLD);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::LevelText).c_str(), x6, 326, 24, GOLD);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::AverageSpeed).c_str(), x9, 438, 24, GOLD);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::GameOverReason).c_str(), x10, 482, 24, GOLD);

		DrawText("[ENTER] Restart, [Esc] Quit", x11, 620, 21, LIGHTGRAY);
	EndTextureMode();
}

void GameOverMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_ENTER))
		this->GlobalData->ActiveState = State::GameReset;

	if (IsKeyPressed(KEY_ESCAPE))
		this->GlobalData->Running = false;
}

void GameOverMenu::GenerateStats(const Game& game, const StatSystem& stat_system) noexcept
{
	const size_t ticks = game.Ticks;
	size_t damage_per_second = 0;
	size_t average_speed = 0;

	if (TICKS_TO_SECONDS(ticks) != 0)
	{
		damage_per_second = stat_system.GetStat(Stat::TotalDamage) / TICKS_TO_SECONDS(ticks);
		average_speed = stat_system.GetStat(Stat::TotalDistance) / TICKS_TO_SECONDS(ticks);
	}

	this->GlobalData->CachedStrings[CachedString::DamagePerSecond] = "Damage / Second: " + std::to_string(damage_per_second);
	this->GlobalData->CachedStrings[CachedString::EnemiesKilled] = "Enemies Killed: " + std::to_string(stat_system.GetStat(Stat::Kills));
	this->GlobalData->CachedStrings[CachedString::AverageSpeed] = "Average Speed: " + std::to_string(average_speed) + "px/s";
}
