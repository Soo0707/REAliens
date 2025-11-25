#pragma once

#include "raylib.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
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

	AuraSize,
	AuraCooldown,
	AuraDamage,

	LifeStealMultiplier
};

// The ordering here matters as GameEventSystem uses a fixed array as a function jump table
enum class Event
{
	GreenbullExpire,
	MilkExpire,

	PoisonTick,
	PoisonExpire,

	DrunkExpire,
	AussieExpire,
	MagnetismExpire,

	IncreasePlayerSpeed,
	IncreasePlotArmour,

	AuraTick,
	COUNT
};

enum class State
{
	Game,
	GameReset,
	PowerupMenu,

	PauseMenu,
	MainMenu,
	GameOverMenu,

	GenerateGameOverStats
};

enum class Effect
{
	Greenbull,
	Milk,

	LifeSteal,
	Magnetism,

	Trapped,
	Poison,
	Drunk,

	Aussie,
	Stinky,

	Microscope,
	Earthquake
};

enum class CachedString
{
	LayerText,
	LevelText,
	UnclaimedPowerups,

	Duration,
	TotalDamage,
	DamagePerSecond,

	TimePerLevel,
	TotalDistance,
	AverageSpeed,

	EnemiesKilled,
	GameOverReason,
	LevelDebuff
};

struct GlobalDataWrapper
{
	GlobalDataWrapper() = default;
	~GlobalDataWrapper() = default;

	GlobalDataWrapper(const GlobalDataWrapper&) = delete;
	GlobalDataWrapper& operator=(const GlobalDataWrapper&) = delete;

	GlobalDataWrapper(const GlobalDataWrapper&&) = delete;
	GlobalDataWrapper& operator=(const GlobalDataWrapper&&) = delete;

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

	std::unordered_map<CachedString, std::string> CachedStrings =
	{
		{ CachedString::LayerText, "Current Layer: 0" },
		{ CachedString::LevelText, "Level: 1" },
		{ CachedString::Duration, "Duration: 0s" },

		{ CachedString::UnclaimedPowerups, "" }
	};

	State ActiveState = State::MainMenu;

	size_t Ticks = 0;
	size_t TotalDamage = 0;
	size_t TotalDistance = 0;

	size_t EnemiesKilled = 0;
	
	size_t Level = 1;

	int CurrentLayer = 0;

	unsigned int UnclaimedPowerups = 0;

	bool Running = true;

	void Reset() noexcept;
};
