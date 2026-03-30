/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "powerupMenu.hpp"

#include <memory>
#include <variant>
#include <set>

#include "raylib.h"

#include "constants.hpp"

#include "stringCache.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"

#include "modifierSystem.hpp"
#include "messageSystem.hpp"
#include "commands.hpp"
#include "states.hpp"
#include "timerSystem.hpp"

PowerupMenu::PowerupMenu(
		std::shared_ptr<struct StringCache> string_cache, std::shared_ptr<class AssetManager> assets, 
		std::shared_ptr<class SettingsManager> settings, std::shared_ptr<struct MessageSystem> message_system,
		std::shared_ptr<class ModifierSystem> modifier_system, std::shared_ptr<class TimerSystem> timer_system
		):
	StringCache(string_cache),
	Settings(settings),
	Assets(assets),
	MessageSystem(message_system),
	ModifierSystem(modifier_system),
	TimerSystem(timer_system)
{
	PowerupMenu::GenerateList();
}

void PowerupMenu::Draw(const RenderTexture2D& canvas) const noexcept
{
	static constexpr float mid = REFERENCE_WIDTH / 2.0f;

	int x1 = mid - MeasureText(this->SelectionList[0].DisplayName.c_str(), 24) / 2.0f;
	int x2 = mid - MeasureText(this->SelectionList[1].DisplayName.c_str(), 24) / 2.0f;
	int x3 = mid - MeasureText(this->SelectionList[2].DisplayName.c_str(), 24) / 2.0f;

	int x4 = mid - MeasureText(this->SelectionList[0].Description, 21) / 2.0f;
	int x5 = mid - MeasureText(this->SelectionList[1].Description, 21) / 2.0f;
	int x6 = mid - MeasureText(this->SelectionList[2].Description, 21) / 2.0f;

	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("Select A Powerup", 452.5, 80, 42, VIOLET);

		DrawText(this->SelectionList[0].DisplayName.c_str(), x1, 260, 24, GOLD);
		DrawText(this->SelectionList[0].Description, x4, 294, 21, GRAY);

		DrawText(this->SelectionList[1].DisplayName.c_str(), x2, 360, 24, GOLD);
		DrawText(this->SelectionList[1].Description, x5, 394, 21, GRAY);

		DrawText(this->SelectionList[2].DisplayName.c_str(), x3, 460, 24, GOLD);
		DrawText(this->SelectionList[2].Description, x6, 494, 21, GRAY);

		DrawText("[TAB] Toggle Powerup Menu, [Enter] LET'S GO GAMBLING!!!", 326, 580, 21, LIGHTGRAY);

		DrawText(this->StringCache->Data[static_cast<size_t>(GameString::UnclaimedPowerups)].c_str(), 50, 670, 21, LIGHTGRAY);
	EndTextureMode();
}

void PowerupMenu::GenerateList() noexcept
{
	std::set<Powerup> powerup_set = {};

	while (true)
	{
		Powerup powerup = static_cast<Powerup>( GetRandomValue(0, ( static_cast<int>(Powerup::COUNT) ) - 1) );
		
		if (!powerup_set.count(powerup))
			powerup_set.insert(powerup);

		if (powerup_set.size() == 3)
			break;
	}

	this->SelectionList.clear();
	
	for (auto const &powerup : powerup_set)
	{
		const int i = this->SelectionList.size();

		this->SelectionList.emplace_back(
				powerup,
				"[" + std::to_string(i + 1) + "] " + this->PowerupNames[static_cast<size_t>(powerup)],
				this->PowerupDescriptions[static_cast<size_t>(powerup)]
				);
	}
}

void PowerupMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_ONE) || this->Gamble)
		PowerupMenu::ApplyPowerup(this->SelectionList[0].Powerup);
	else if (IsKeyPressed(KEY_TWO))
		PowerupMenu::ApplyPowerup(this->SelectionList[1].Powerup);
	else if (IsKeyPressed(KEY_THREE))
		PowerupMenu::ApplyPowerup(this->SelectionList[2].Powerup);

	if (IsKeyPressed(KEY_TAB))
	{
		this->Gamble = false;
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<struct SetState>, State::Game);
	}

	if (IsKeyPressed(KEY_ENTER))
		this->Gamble = !this->Gamble;
}

void PowerupMenu::ApplyPowerup(Powerup powerup) noexcept
{
	auto handler_function = this->ApplyHandles[static_cast<size_t>(powerup)];

	(this->*handler_function)();

	PowerupMenu::GenerateList();

	if (!this->Settings->Get(SettingKey::UnlimitedPowerups))
	{
		const size_t unclaimed_powerups = this->ModifierSystem->GetUnclaimedPowerups();
		this->ModifierSystem->DecrementUnclaimedPowerups();

		this->StringCache->CacheString(
				"Unclaimed Powerups: " + std::to_string(unclaimed_powerups - 1),
				GameString::UnclaimedPowerups
				);
	}

	if (this->ModifierSystem->GetUnclaimedPowerups() == 0 && !this->Settings->Get(SettingKey::UnlimitedPowerups))
	{
		this->Gamble = false;
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<struct SetState>, State::Game);
	}
}

void PowerupMenu::ApplyMilk() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyMilk)]++;

	this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<RegisterTimer>, SECONDS_TO_TICKS(180), false, Timer::MilkExpire);

	this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<DisableTimer>, Timer::PoisonTick);
	this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<DisableTimer>, Timer::PoisonExpire);
}

void PowerupMenu::ApplyGreenbull() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyGreenbull)]++;
	
	this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<RegisterTimer>, SECONDS_TO_TICKS(120), false, Timer::GreenbullExpire);
}

void PowerupMenu::ApplyMagnetism() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyMagnetism)]++;

	this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<RegisterTimer>, SECONDS_TO_TICKS(240), false, Timer::MagnetismExpire);
}

void PowerupMenu::ApplyAura() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyAura)]++;

	if (!this->TimerSystem->GetTimerStatus(Timer::AuraTick))
		this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<EnableTimer>, true, Timer::AuraTick);
	else
	{
		this->MessageSystem->TimerSystemCommands.emplace_back(
				std::in_place_type<struct DecreaseTimerInterval>, 25,
				TICK_RATE / 4, Timer::AuraTick
				);
	}

	this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<struct TriggerNow>, Timer::AuraTick);
}

void PowerupMenu::ApplyBuckshot() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyBuckshot)]++;
}

void PowerupMenu::ApplyBullet() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyBullet)]++;
}

void PowerupMenu::ApplyLazer() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyLazer)]++;
}

void PowerupMenu::ApplyBall() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyBall)]++;
	const bool has_ball = this->TimerSystem->GetTimerStatus(Timer::BallCountdown);

	if (!has_ball)
		this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<EnableTimer>, true, Timer::BallCountdown);
	else
	{
		this->MessageSystem->TimerSystemCommands.emplace_back(
				std::in_place_type<struct DecreaseTimerInterval>, SECONDS_TO_TICKS(5),
				TICK_RATE / 2, Timer::BallCountdown
				);
	}

	this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<struct TriggerNow>, Timer::BallCountdown);
}

void PowerupMenu::ApplyLifeSteal() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyLifeSteal)]++;
}

void PowerupMenu::ApplyPlotArmour() noexcept
{
	this->MessageSystem->PlayerSignals[static_cast<size_t>(PlayerSignal::IncreasePlotArmour)]++;
}

void PowerupMenu::ApplySpeedBoots() noexcept
{
	this->MessageSystem->PlayerSignals[static_cast<size_t>(PlayerSignal::ApplySpeedBoots)]++;
}

void PowerupMenu::ApplyBabyOil() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyBabyOil)]++;
}

void PowerupMenu::ApplyLuck() noexcept
{
	this->MessageSystem->ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyLuck)]++;
}

void PowerupMenu::ApplyFastHands() noexcept
{
	this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<struct DecreaseTimerInterval>, 50, 25, Timer::LMB);
	this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<struct DecreaseTimerInterval>, 75, 25, Timer::RMB);
}

void PowerupMenu::ApplyDrippyGlue() noexcept
{
	if (!this->TimerSystem->GetTimerStatus(Timer::GlueCountdown))
		this->MessageSystem->TimerSystemCommands.emplace_back(std::in_place_type<struct EnableTimer>, true, Timer::GlueCountdown);
	else
		this->MessageSystem->TimerSystemCommands.emplace_back(
				std::in_place_type<struct DecreaseTimerInterval>,
				SECONDS_TO_TICKS(5), TICK_RATE / 3, Timer::GlueCountdown
				);
}
