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
#include <cstdint>

#include "raylib.h"

#include "stringCache.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"

#include "modifierSystem.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"

enum class Powerup : uint8_t
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
	Luck,

	FastHands,
	Greed,
	GlueGun,
	Plebifier,

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
				std::shared_ptr<struct StringCache> string_cache, std::shared_ptr<class AssetManager> assets,
				std::shared_ptr<class SettingsManager> settings, std::shared_ptr<struct MessageSystem> message_system,
				std::shared_ptr<class ModifierSystem> modifier_system, std::shared_ptr<class TimerSystem> timer_system
				);

		~PowerupMenu() = default;

		void Draw(const RenderTexture2D& canvas) const noexcept;
		void Input() noexcept;

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
		void ApplyLuck() noexcept;
		void ApplyFastHands() noexcept;
		void ApplyGreed() noexcept;
		void ApplyGlueGun() noexcept;
		void ApplyPlebifier() noexcept;

		std::vector<PowerupWrapper> SelectionList;

		const std::shared_ptr<struct StringCache> StringCache;
		const std::shared_ptr<class SettingsManager> Settings;
		const std::shared_ptr<class AssetManager> Assets;

		const std::shared_ptr<struct MessageSystem> MessageSystem;
		const std::shared_ptr<class ModifierSystem> ModifierSystem;
		const std::shared_ptr<class TimerSystem> TimerSystem;

		std::array<std::string, static_cast<size_t>(Powerup::COUNT)> PowerupNames =
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
			"Luck",
			"Fast Hands",
			"Greed",

			"Glue",
			"Plebifier"
		};

		static inline constexpr std::array<const char*, static_cast<size_t>(Powerup::COUNT)> PowerupDescriptions =
		{
			"Increase Aura Size And Decrease Aura Timeout",
			"Increase The Number Of Bullets Shot By LMB",
			"Increase Bullet Speed And Damage",

			"Increase Lazer Damage, Penetration Power, And Speed",
			"Periodically Spawn A Ball Which Splits On Impact Into Bullets Based On Luck",
			"Temporary Invincibility For 2 Minutes",

			"Removes And Prevents Enemy Effects For 3 Minutes",
			"Regain Health By Dealing Damage With Projectiles",
			"Increase To Max Health And Recover Health",

			"Automatically Collect XP For 4 Minutes",
			"Increase Player Speed",
			"Increase Slide Speed",
			"Increase Luck For Luck Based Events",
			"Decrease LMB And RMB Timeout",
			"Gives Bonus XP Depending On Luck",

			"Increase Glue Speed And Damage",
			"Increase Plebifier Speed And Damage"
		};

		static inline constexpr std::array<void(PowerupMenu::*)(), static_cast<size_t>(Powerup::COUNT)> ApplyHandles = 
		{
			&PowerupMenu::ApplyAura,
			&PowerupMenu::ApplyBuckshot,
			&PowerupMenu::ApplyBullet,

			&PowerupMenu::ApplyLazer,
			&PowerupMenu::ApplyBall,
			&PowerupMenu::ApplyGreenbull,

			&PowerupMenu::ApplyMilk,
			&PowerupMenu::ApplyLifeSteal,
			&PowerupMenu::ApplyPlotArmour,

			&PowerupMenu::ApplyMagnetism,
			&PowerupMenu::ApplySpeedBoots,
			&PowerupMenu::ApplyBabyOil,
			&PowerupMenu::ApplyLuck,
			&PowerupMenu::ApplyFastHands,
			&PowerupMenu::ApplyGreed,
			&PowerupMenu::ApplyGlueGun,
			&PowerupMenu::ApplyPlebifier
		};

		bool Gamble = false;
};
