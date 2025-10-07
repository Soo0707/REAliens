#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

#include "raylib.h"

#include "globalDataWrapper.hpp"

constexpr unsigned int PADDING = 32;

enum class Powerup
{
	Aura,
	Buckshot,
	Projectile,
	Circle,
	Lazer,
	Greenbull,
	Milk,
	LifeSteal,
	Regeneration,
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
		void Draw();
		void GenerateList();
		void HandleInput();
		void ApplyPowerup(Powerup powerup);

	private:
		std::shared_ptr<GlobalDataWrapper> GlobalData;
		std::vector<PowerupWrapper> SelectionList = {};

		static const inline std::unordered_map<Powerup, std::string> PowerupNames =
		{
			{ Powerup::Aura, "Aura" },
			{ Powerup::Buckshot, "Buckshot" },
			{ Powerup::Projectile, "Projectile" },
			{ Powerup::Lazer, "Lazer" },
			{ Powerup::Circle, "Circle" },
			{ Powerup::Greenbull, "Greenbull" },
			{ Powerup::Milk, "Milk" },
			{ Powerup::LifeSteal, "Life Steal" },
			{ Powerup::Regeneration, "Regeneration" },
			{ Powerup::Magnetism, "Magnetism" },
			{ Powerup::SpeedBoots, "Speed Boots"}
		};

		Rectangle Rect;
		Vector2 Left;
		Vector2 Right;
		Vector2 Middle;
};
