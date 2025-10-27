#pragma once

#include <memory>
#include <array>
#include <string>
#include <vector>

#include "raylib.h"

#include "globalDataWrapper.hpp"


enum class Powerup
{
	Aura,
	Buckshot,
	Projectile,

	Ball,
	Lazer,
	Greenbull,

	Milk,
	LifeSteal,
	PlotArmour,

	Magnetism,
	SpeedBoots,
	COUNT
};

struct PowerupWrapper
{
	Powerup Powerup;
	std::string DisplayName;
};

class PowerupMenu
{
	public:
		PowerupMenu(std::shared_ptr<GlobalDataWrapper> global_data);
		~PowerupMenu();
		void Draw(RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;

	private:
		void GenerateList() noexcept;

		void ApplyPowerup(Powerup powerup) noexcept;
		void ApplyEffect(const Effect effect, const Event event, const unsigned int duration) noexcept;

		void ApplyMilk() noexcept;
		void ApplyAura() noexcept;
		void ApplyBall() noexcept;

		void ApplyBuckshot() noexcept;
		void ApplyProjectile() noexcept;
		void ApplyLazer() noexcept;

		void ApplyLifeSteal() noexcept;
		void ApplyGreenbull() noexcept;
		void ApplyMagnetism() noexcept;

		void ApplyPlotArmour() noexcept;
		void ApplySpeedBoots() noexcept;

		std::shared_ptr<GlobalDataWrapper> GlobalData;
		std::vector<PowerupWrapper> SelectionList;

		static inline constexpr std::array<std::string, static_cast<size_t>(Powerup::COUNT)> PowerupNames =
		{
			"Aura",
			"Buckshot",
			"Projectile",

			"Ball",
			"Lazer",
			"Greenbull",

			"Milk",
			"Life Steal",
			"Plot Armour",

			"Magnetism",
			"Speed Boots"
		};

		static inline constexpr std::array<void(PowerupMenu::*)(), static_cast<size_t>(Powerup::COUNT)> ApplyHandles = 
		{
			&PowerupMenu::ApplyAura,
			&PowerupMenu::ApplyBuckshot,
			&PowerupMenu::ApplyProjectile,

			&PowerupMenu::ApplyBall,
			&PowerupMenu::ApplyLazer,
			&PowerupMenu::ApplyGreenbull,

			&PowerupMenu::ApplyMilk,
			&PowerupMenu::ApplyLifeSteal,
			&PowerupMenu::ApplyPlotArmour,

			&PowerupMenu::ApplyMagnetism,
			&PowerupMenu::ApplySpeedBoots
		};
};
