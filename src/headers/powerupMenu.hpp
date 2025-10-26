#pragma once

#include <memory>
#include <unordered_map>
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
		void Draw(RenderTexture2D& canvas);
		void HandleInput();

		std::string UnacquiredPowerupCache;
	private:
		void GenerateList();

		void ApplyPowerup(Powerup powerup);
		void ApplyEffect(const Effect effect, const Event event, const unsigned int duration);
		void ApplyMilk();
		void ApplyAura();

		void ApplyBall();
		void ApplyBuckshot();
		void ApplyProjectile();
		void ApplyLazer();
		void ApplyLifeSteal();

		std::shared_ptr<GlobalDataWrapper> GlobalData;
		std::vector<PowerupWrapper> SelectionList;

		static const inline std::unordered_map<Powerup, std::string> PowerupNames =
		{
			{ Powerup::Aura, "Aura" },
			{ Powerup::Buckshot, "Buckshot" },
			{ Powerup::Projectile, "Projectile" },
			{ Powerup::Lazer, "Lazer" },
			{ Powerup::Ball, "Ball" },
			{ Powerup::Greenbull, "Greenbull" },
			{ Powerup::Milk, "Milk" },
			{ Powerup::LifeSteal, "Life Steal" },
			{ Powerup::PlotArmour, "Plot Armour" },
			{ Powerup::Magnetism, "Magnetism" },
			{ Powerup::SpeedBoots, "Speed Boots"}
		};
};
