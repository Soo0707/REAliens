#pragma once

#include "raylib.h"

#include <unordered_map>
#include <unordered_set>
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

	PoisonDamage,

	AuraSize,
	AuraCooldown,
	AuraDamage,

	LifeStealMultiplier
};

enum class Event
{
	UpgradeCircle,
	SpawnCircle,
	GreenbullExpire,
	MilkExpire,

	PoisonTick,
	PoisonExpire,

	DrunkExpire,
	AussieExpire,
	MagnetismExpire,

	IncreasePlayerSpeed,
	IncreasePlotArmour,

	IncreaseAura,
	AuraTick
};

enum class State
{
	Game,
	PowerupMenu,
	PauseMenu,
	StartMenu,
	GameOverMenu
};

enum class Effect
{
	Aura,
	Greenbull,
	Milk,

	LifeSteal,
	Magnetism,

	Trapped,
	Poison,
	Drunk,

	Aussie,
	Stinky
};

enum class Setting
{
	ShowPowerupMenuOnLevelUp,
	AutoClick,
	DisableHealthCheck
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
			{ Attribute::BuckshotSpread, PI / 8 },

			{ Attribute::LazerCooldown, 450 },
			{ Attribute::LazerDamage, 25.0f },
			{ Attribute::LazerScale, 1.0f },
			{ Attribute::LazerSpeed, 3000.0f }		
		};

		std::unordered_map<Event, size_t> Events;

		std::unordered_set<Effect> Effects;

		std::unordered_map<Setting, int> Settings =
		{
			{ Setting::ShowPowerupMenuOnLevelUp, 1 },
			{ Setting::AutoClick, 0 },
			{ Setting::DisableHealthCheck, 1 }
		};

		State ActiveState = State::Game;

		size_t Ticks = 0;
		
		size_t Level = 1;
		size_t LevelUpTreshold = 2;

		unsigned int UnclaimedPowerups = 0;

		bool Running = true;
};
