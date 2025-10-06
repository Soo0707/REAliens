#pragma once

#include <unordered_map>
#include <cstddef>

enum class Attribute
{
	BulletCooldown,
	BulletDamage,
	BulletSpeed,
	Buckshot,
	BuckshotSpread,

	LazerCooldown,
	LazerDamage,
	LazerScale,
	LazerSpeed,

	CircleDamage,
	CircleScale,
	CircleAngularSpeed,
	CircleRadius,

	PlayerSpeed,
	Drunk,
	Poisoned,
	Trapped,
	Aussie,

	Greenbull,
	Milk
};

enum class EventKey
{
	GreenbullExpire,
	MilkExpire,
	PoisonedExpire,
	DrunkExpire,
	AussieExpire
};

class GlobalDataWrapper
{
	public:
		GlobalDataWrapper() = default;

		GlobalDataWrapper(const GlobalDataWrapper&) = delete;
		GlobalDataWrapper& operator=(const GlobalDataWrapper&) = delete;

		std::unordered_map<Attribute, float> Attributes = 
		{
			{ Attribute::BulletCooldown, 150 },
			{ Attribute::BulletDamage, 25.0f },
			{ Attribute::BulletSpeed, 1000.0f },
			{ Attribute::Buckshot, 3 },
			{ Attribute::BuckshotSpread, 3.142 / 8 },

			{ Attribute::LazerCooldown, 450 },
			{ Attribute::LazerDamage, 25.0f },
			{ Attribute::LazerScale, 1.0f },
			{ Attribute::LazerSpeed, 3000.0f },

			{ Attribute::CircleRadius, 64 },
			{ Attribute::CircleDamage, 5.0f },
			{ Attribute::CircleScale, 2.0f },
			{ Attribute::CircleAngularSpeed, 3.142 / 2 }
		};

		std::unordered_map<EventKey, size_t> Events;

		bool ShowPowerupOverlay = false;
		unsigned int Level = 1;
		unsigned int XpLevelUpTreshold = 2;
		unsigned int CollectedXp = 0;
};
