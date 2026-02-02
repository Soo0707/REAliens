#pragma once

#include <memory>
#include <array>
#include <string>
#include <vector>

#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"

#include "messageSystem.hpp"
#include "timerSystem.hpp"

enum class Powerup
{
	Aura,
	Buckshot,
	Projectile,

	Lazer,
	Ball,
	Greenbull,
	Milk,

	LifeSteal,
	PlotArmour,
	Magnetism,

	SpeedBoots,
	BabyOil,
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
				std::shared_ptr<class TimerSystem> timer_system
				);

		~PowerupMenu() = default;

		void Draw(RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;

	private:
		void GenerateList() noexcept;

		void ApplyPowerup(Powerup powerup) noexcept;

		void ApplyMilk() noexcept;
		void ApplyAura() noexcept;

		void ApplyBuckshot() noexcept;
		void ApplyProjectile() noexcept;
		void ApplyLazer() noexcept;
		void ApplyBall() noexcept;

		void ApplyLifeSteal() noexcept;
		void ApplyGreenbull() noexcept;
		void ApplyMagnetism() noexcept;

		void ApplyPlotArmour() noexcept;
		void ApplySpeedBoots() noexcept;
		void ApplyBabyOil() noexcept;

		std::vector<PowerupWrapper> SelectionList;

		const std::shared_ptr<GlobalDataWrapper> GlobalData;
		const std::shared_ptr<SettingsManager> Settings;
		const std::shared_ptr<AssetManager> Assets;

		const std::shared_ptr<struct MessageSystem> MessageSystem;
		const std::shared_ptr<class TimerSystem> TimerSystem;

		static inline constexpr std::array<std::string, static_cast<size_t>(Powerup::COUNT)> PowerupNames =
		{
			"Aura",
			"Buckshot",
			"Projectile",

			"Lazer",
			"Ball",
			"Greenbull",

			"Milk",
			"Life Steal",
			"Plot Armour",

			"Magnetism",
			"Speed Boots",
			"Baby Oil"
		};

		static inline constexpr std::array<const char*, static_cast<size_t>(Powerup::COUNT)> PowerupDescriptions =
		{
			"+Aura Size, -Aura Timeout",
			"Increase The Number Of Projectiles Shot",
			"+Projectile Damage, -LMB Timeout",

			"+Lazer Damage, -RMB Timeout, +Penetration Power",
			"Periodically Spawn Balls Which Explode Into Smaller Balls",
			"Temporary Invincibility",

			"Removes And Prevents Enemy Effects",
			"Regain Health By Dealing Damage",
			"Increase Max Health",

			"Auto Collects XP",
			"Increase Player Speed",
			"Increase Slide Speed" 
		};

		static inline constexpr std::array<void(PowerupMenu::*)(), static_cast<size_t>(Powerup::COUNT)> ApplyHandles = 
		{
			&ApplyAura,
			&ApplyBuckshot,
			&ApplyProjectile,

			&ApplyLazer,
			&ApplyBall,
			&ApplyGreenbull,

			&ApplyMilk,
			&ApplyLifeSteal,
			&ApplyPlotArmour,

			&ApplyMagnetism,
			&ApplySpeedBoots,
			&ApplyBabyOil
		};

		bool Gamble = false;
};
