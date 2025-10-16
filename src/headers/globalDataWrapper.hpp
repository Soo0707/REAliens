#pragma once

#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <mutex>

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
	AuraDamage
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
	Regeneration,
	Magnetism,

	Trapped,
	Poison,
	Drunk,

	Aussie
};

class GlobalDataWrapper
{
	public:
		GlobalDataWrapper() = default;

		GlobalDataWrapper(const GlobalDataWrapper&) = delete;
		GlobalDataWrapper& operator=(const GlobalDataWrapper&) = delete;

		std::mutex AttributesMutex;
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
			{ Attribute::CircleScale, 1.0f },
			{ Attribute::CircleAngularSpeed, 3.142 / 2 }
		};

		std::mutex EventsMutex;
		std::unordered_map<Event, size_t> Events;

		std::mutex EffectsMutex;
		std::unordered_set<Effect> Effects;

		State ActiveState = State::Game;

		size_t Ticks = 0;
		
		size_t Level = 1;
		size_t LevelUpTreshold = 2;

		bool Running = true;
};
