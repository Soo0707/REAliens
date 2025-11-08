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

	BallDamage,
	BallScale,
	BallAngularSpeed,
	BallRadius,

	PoisonDamage,

	AuraSize,
	AuraCooldown,
	AuraDamage,

	LifeStealMultiplier
};

// The ordering here matters as GameEventSystem uses a fixed array as a function jump table
enum class Event
{
	SpawnAndUpgradeBall,
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
	Stinky,
	Blind,

	Microscope
};

enum class Setting
{
	ShowPowerupMenuOnLevelUp,
	AutoClick,
	DisableHealthCheck,
	UnlimitedPowerups
};

enum class CachedString
{
	LayerText,
	LevelText,
	UnclaimedPowerups,

	Duration,
	TotalDamage,
	DamagePerSecond
};

class GlobalDataWrapper
{
	public:
		GlobalDataWrapper() = default;
		~GlobalDataWrapper() = default;

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
			{ Setting::ShowPowerupMenuOnLevelUp, 0 },
			{ Setting::AutoClick, 0 },
			{ Setting::DisableHealthCheck, 0 },
			{ Setting::UnlimitedPowerups, 0 }
		};

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
		
		size_t Level = 1;
		size_t LevelUpTreshold = 5;

		int CurrentLayer = 0;

		unsigned int UnclaimedPowerups = 0;

		bool Running = true;

		void Reset() noexcept
		{
			this->Attributes = 
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

			this->CachedStrings =
			{
				{ CachedString::LayerText, "Current Layer: 0" },
				{ CachedString::LevelText, "Level: 1" },
				{ CachedString::Duration, "Duration: 0s" },

				{ CachedString::UnclaimedPowerups, "" }
			};
			
			this->Events.clear();
			this->Effects.clear();

			this->Ticks = 0;
			this->TotalDamage = 0;
			
			this->Level = 1;
			this->LevelUpTreshold = 5;

			this->CurrentLayer = 0;

			this->UnclaimedPowerups = 0;
		}
};
