#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cstddef>
#include <array>

#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"
#include "enemy.hpp"
#include "projectiles.hpp"
#include "xp.hpp"
#include "gameText.hpp"
#include "particle.hpp"
#include "player.hpp"

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

	LifeStealMultiplier,
	SlideSpeed
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
	Earthquake,
	Invisible
};


struct GameState
{
	GameState(AssetManager& assets) noexcept;
	~GameState() = default;

	const std::unique_ptr<class Player> Player;

	std::vector<Enemy> Enemies;
	std::vector<Projectile> Projectiles;
	std::vector<Xp> Xps;
	std::vector<GameText> GameTexts;
	std::vector<Particle> Particles;

	Camera2D Camera;
	Rectangle UpdateArea;

	std::unordered_set<Effect> Effects;

	std::unordered_map<Attribute, float> Attributes; 

	std::unordered_map<Event, size_t> Events;

	size_t Ticks = 0;
	size_t TotalDamage = 0;
	size_t TotalDistance = 0;

	size_t EnemiesKilled = 0;
	
	size_t Level = 1;

	size_t CollectedXp = 0;
	size_t LevelUpTreshold = 5;
	
	size_t LastLMB = 0;
	bool CanLMB = true;

	size_t LastRMB = 0;
	bool CanRMB = true;

	size_t LastSlide = 0;
	bool CanSlide = true;

	size_t LastSpawn = 0;

	unsigned int UnclaimedPowerups;
	void Reset() noexcept;

	void UpdateTimeouts() noexcept;

	void UpdatePlayer(GlobalDataWrapper& global_data, const SettingsManager& settings, const AssetManager& assets) noexcept;
	void UpdateCamera() noexcept;

	void UpdateEnemies(const AssetManager& assets) noexcept;
	void UpdateProjectiles(const AssetManager& assets) noexcept;
	void UpdateXps(const AssetManager& assets) noexcept;

	void UpdateGameTexts() noexcept;
	void UpdateParticles() noexcept;

	void LevelUp(const SettingsManager& settings, GlobalDataWrapper& global_data) noexcept;
	void InsertLevelDebuff(GlobalDataWrapper& global_data) noexcept;
	void RemoveLevelDebuff(GlobalDataWrapper& global_data) noexcept;
	
	private:
		void EmitParticles(
				size_t number, float x, float y, unsigned int min_size, unsigned int max_size, unsigned int min_lifetime, unsigned int max_lifetime,
				Vector2 velocity, unsigned int max_speed, Color start_colour, Color end_colour, const AssetManager& assets
				) noexcept;

		static constexpr std::array<Effect, 4> DebuffList = { 
			Effect::Microscope,
			Effect::Earthquake,
			Effect::Stinky,
			Effect::Invisible
		};

		static constexpr std::array<const char *, 4> DebuffNames = {
			"Level Debuff: Microscope",
			"Level Debuff: Earthquake",
			"Level Debuff: Stinky",
			"Level Debuff: Invisible"
		};
};
