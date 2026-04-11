/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gameOverMenu.hpp"

#include <memory>

#include "raylib.h"

#include "stats.hpp"
#include "statSystem.hpp"
#include "stringCache.hpp"
#include "assetManager.hpp"
#include "states.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"
#include "constants.hpp"

GameOverMenu::GameOverMenu(
		std::shared_ptr<struct StringCache> string_cache, std::shared_ptr<class AssetManager> assets,
		std::shared_ptr<class StatSystem> stat_system, std::shared_ptr<struct MessageSystem> message_system
		) :
	StringCache(string_cache),
	Assets(assets),
	StatSystem(stat_system),
	MessageSystem(message_system)
{}

void GameOverMenu::Draw(const RenderTexture2D& canvas) const noexcept
{
	const float mid = 1280 / 2.0f;

	const int x1 = mid - MeasureText("GAME OVER", 48) / 2.0f;

	const int x2 = mid - MeasureText(this->StringCache->Data[static_cast<size_t>(GameString::Duration)].c_str(), 24) / 2.0f;

	const int x3 = mid - MeasureText(this->StringCache->Data[static_cast<size_t>(GameString::EnemiesKilled)].c_str(), 24) / 2.0f;

	const int x5 = mid - MeasureText(this->StringCache->Data[static_cast<size_t>(GameString::DamagePerSecond)].c_str(), 24) / 2.0f;

	const int x6 = mid - MeasureText(this->StringCache->Data[static_cast<size_t>(GameString::LevelText)].c_str(), 24) / 2.0f;
	const int x9 = mid - MeasureText(this->StringCache->Data[static_cast<size_t>(GameString::AverageSpeed)].c_str(), 24) / 2.0f;

	const int x11 = mid - MeasureText("[ENTER] Restart, [Esc] Quit", 21) / 2.0f;
	/*
	 24px + 10px padding per element vertically except at beginning of new section
	 24px + 20px padding per section
	*/
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("GAME OVER", x1, 80, 48, VIOLET);

		DrawText(this->StringCache->Data[static_cast<size_t>(GameString::Duration)].c_str(), x2, 160, 24, GOLD);

		DrawText(this->StringCache->Data[static_cast<size_t>(GameString::EnemiesKilled)].c_str(), x3, 204, 24, GOLD);

		DrawText(this->StringCache->Data[static_cast<size_t>(GameString::DamagePerSecond)].c_str(), x5, 282, 24, GOLD);

		DrawText(this->StringCache->Data[static_cast<size_t>(GameString::LevelText)].c_str(), x6, 326, 24, GOLD);

		DrawText(this->StringCache->Data[static_cast<size_t>(GameString::AverageSpeed)].c_str(), x9, 438, 24, GOLD);

		DrawText("[ENTER] Restart, [Esc] Quit", x11, 620, 21, LIGHTGRAY);
	EndTextureMode();
}

void GameOverMenu::Input() const noexcept
{
	if (IsKeyPressed(KEY_ENTER))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<SetState>, State::SystemsReset);

	if (IsKeyPressed(KEY_ESCAPE))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<SetTerminate>, true);
}

void GameOverMenu::GenerateStats(const size_t ticks) noexcept
{
	size_t damage_per_second = 0;
	size_t average_speed = 0;

	if (TICKS_TO_SECONDS(ticks) != 0)
	{
		damage_per_second = this->StatSystem->GetStat(Stat::TotalDamage) / TICKS_TO_SECONDS(ticks);
		average_speed = this->StatSystem->GetStat(Stat::TotalDistance) / TICKS_TO_SECONDS(ticks);
	}

	this->StringCache->CacheString("Damage / Second: " + std::to_string(damage_per_second), GameString::DamagePerSecond);
	this->StringCache->CacheString("Enemies Killed: " + std::to_string(this->StatSystem->GetStat(Stat::Kills)), GameString::EnemiesKilled);
	this->StringCache->CacheString("Average Speed: " + std::to_string(average_speed) + "px/s", GameString::AverageSpeed);
}
