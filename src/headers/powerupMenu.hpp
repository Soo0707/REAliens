/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <array>
#include <string>
#include <vector>

#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"

#include "modifierSystem.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"

enum class Powerup
{
	Aura,
	Buckshot,
	Bullet,

	Lazer,
	Ball,
	Greenbull,
	Milk,

	LifeSteal,
	PlotArmour,
	Magnetism,

	SpeedBoots,
	BabyOil,
	FastHands,
	COUNT
};

struct PowerupWrapper
{
	Powerup Powerup;
	std::string DisplayName;
	const char* Description;
};

class PowerupMenu
{
	public:
		PowerupMenu(
				std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
				std::shared_ptr<SettingsManager> settings, std::shared_ptr<struct MessageSystem> message_system,
				std::shared_ptr<class ModifierSystem> modifier_system, std::shared_ptr<class TimerSystem> timer_system
				);

		~PowerupMenu() = default;

		void Draw(const RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;

	private:
		void GenerateList() noexcept;

		void ApplyPowerup(Powerup powerup) noexcept;

		void ApplyMilk() noexcept;
		void ApplyAura() noexcept;

		void ApplyBuckshot() noexcept;
		void ApplyBullet() noexcept;
		void ApplyLazer() noexcept;
		void ApplyBall() noexcept;

		void ApplyLifeSteal() noexcept;
		void ApplyGreenbull() noexcept;
		void ApplyMagnetism() noexcept;

		void ApplyPlotArmour() noexcept;
		void ApplySpeedBoots() noexcept;
		void ApplyBabyOil() noexcept;
		void ApplyFastHands() noexcept;

		std::vector<PowerupWrapper> SelectionList;

		const std::shared_ptr<GlobalDataWrapper> GlobalData;
		const std::shared_ptr<SettingsManager> Settings;
		const std::shared_ptr<AssetManager> Assets;

		const std::shared_ptr<struct MessageSystem> MessageSystem;
		const std::shared_ptr<class ModifierSystem> ModifierSystem;
		const std::shared_ptr<class TimerSystem> TimerSystem;

		static inline constexpr std::array<std::string, static_cast<size_t>(Powerup::COUNT)> PowerupNames =
		{
			"Aura",
			"Buckshot",
			"Bullet",

			"Lazer",
			"Ball",
			"Greenbull",

			"Milk",
			"Life Steal",
			"Plot Armour",

			"Magnetism",
			"Speed Boots",
			"Baby Oil",
			"Fast Hands"
		};

		static inline constexpr std::array<const char*, static_cast<size_t>(Powerup::COUNT)> PowerupDescriptions =
		{
			"+Aura Size, -Aura Timeout",
			"Increase The Number Of Bullets Shot",
			"Increase Bullet Damage",

			"Increase Lazer Damage & Penetration Power",
			"Periodically Spawn A Ball",
			"Temporary Invincibility",

			"Removes And Prevents Enemy Effects",
			"Regain Health By Dealing Damage With Bullet Or Lazer",
			"Increase Max Health",

			"Auto Collects XP",
			"Increase Player Speed",
			"Increase Slide Speed",
			"Decrease LMB & RMB Timeout"
		};

		static inline constexpr std::array<void(PowerupMenu::*)(), static_cast<size_t>(Powerup::COUNT)> ApplyHandles = 
		{
			&ApplyAura,
			&ApplyBuckshot,
			&ApplyBullet,

			&ApplyLazer,
			&ApplyBall,
			&ApplyGreenbull,

			&ApplyMilk,
			&ApplyLifeSteal,
			&ApplyPlotArmour,

			&ApplyMagnetism,
			&ApplySpeedBoots,
			&ApplyBabyOil,
			&ApplyFastHands
		};

		bool Gamble = false;
};
