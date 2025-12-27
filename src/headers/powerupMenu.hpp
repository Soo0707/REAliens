#pragma once

#include <memory>
#include <array>
#include <string>
#include <vector>

#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "settingsManager.hpp"
#include "gameState.hpp"
#include "assetManager.hpp"

enum class Powerup
{
	Aura,
	Buckshot,
	Projectile,

	Lazer,
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
		PowerupMenu(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
				std::shared_ptr<SettingsManager> settings, std::shared_ptr<struct GameState> game_state);
		~PowerupMenu() = default;

		void Draw(RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;

	private:
		void GenerateList() noexcept;

		void ApplyPowerup(Powerup powerup) noexcept;
		void ApplyEffect(const Effect effect, const Event event, const unsigned int duration) noexcept;

		void ApplyMilk() noexcept;
		void ApplyAura() noexcept;

		void ApplyBuckshot() noexcept;
		void ApplyProjectile() noexcept;
		void ApplyLazer() noexcept;

		void ApplyLifeSteal() noexcept;
		void ApplyGreenbull() noexcept;
		void ApplyMagnetism() noexcept;

		void ApplyPlotArmour() noexcept;
		void ApplySpeedBoots() noexcept;
		void ApplyBabyOil() noexcept;

		const std::shared_ptr<GlobalDataWrapper> GlobalData;
		const std::shared_ptr<SettingsManager> Settings;
		const std::shared_ptr<AssetManager> Assets;
		const std::shared_ptr<struct GameState> GameState;

		std::vector<PowerupWrapper> SelectionList;

		bool Gamble = false;

		static inline constexpr std::array<std::string, static_cast<size_t>(Powerup::COUNT)> PowerupNames =
		{
			"Aura",
			"Buckshot",
			"Projectile",

			"Lazer",
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

			"+Lazer Damage, -RMB Timeout",
			"Temporary Invincibility",

			"Removes and Prevents Enemy Effects",
			"Regain Health By Dealing Damage",
			"Increase Max Health",

			"Auto Collects XP",
			"Increase Player Speed",
			"Increase Slide Duration" 
		};

		static inline constexpr std::array<void(PowerupMenu::*)(), static_cast<size_t>(Powerup::COUNT)> ApplyHandles = 
		{
			&PowerupMenu::ApplyAura,
			&PowerupMenu::ApplyBuckshot,
			&PowerupMenu::ApplyProjectile,

			&PowerupMenu::ApplyLazer,
			&PowerupMenu::ApplyGreenbull,

			&PowerupMenu::ApplyMilk,
			&PowerupMenu::ApplyLifeSteal,
			&PowerupMenu::ApplyPlotArmour,

			&PowerupMenu::ApplyMagnetism,
			&PowerupMenu::ApplySpeedBoots,
			&PowerupMenu::ApplyBabyOil
		};
};
