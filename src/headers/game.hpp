#pragma once

#include <vector>
#include <memory>
#include <shared_mutex>

#include "raylib.h"

#include "player.hpp"
#include "assetManager.hpp"
#include "projectiles.hpp"
#include "enemy.hpp"

static constexpr float TICK_TIME = (1.0/240.0);
static constexpr float MAX_TICK_TIME = (1.0/5.0);


enum class EffectKey
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
	CircleSize,
	CircleVelocity,

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

class Game
{
	public:
		Game();
		~Game();
		void Draw();
		void Update();
		void HandleInput();

	private:
		void SpawnEnemies();
		void LoopOverMap(Rectangle& m_obj);

		std::shared_mutex EnemiesMutex;
		std::vector<Enemy> Enemies;

		std::vector<Projectile> Projectiles;

		std::shared_ptr<AssetManager> AssetManagerInstance;
		std::unique_ptr<Player> PlayerInstance;

		std::unordered_map<EffectKey, float> Effects = 
		{
			{ EffectKey::BulletCooldown, 150 },
			{ EffectKey::BulletDamage, 25.0f },
			{ EffectKey::BulletSpeed, 1000.0f },
			{ EffectKey::Buckshot, 3 },
			{ EffectKey::BuckshotSpread, 3.142 / 4 },

			{ EffectKey::LazerCooldown, 450 },
			{ EffectKey::LazerDamage, 25.0f },
			{ EffectKey::LazerScale, 1.0f },
			{ EffectKey::LazerSpeed, 3000.0f }
		};

		std::unordered_map<EventKey, size_t> Events;

		unsigned int LastLMB = 0;
		bool CanLMB = true;

		unsigned int LastRMB = 0;
		bool CanRMB = true;

		float Accumulator = 0.0f;
		size_t Ticks = 0;
		
		size_t LastSpawn = 0;
		size_t SpawnTimeout = 1000;

		unsigned int Level = 1;

		Camera2D Camera;
		Rectangle UpdateArea;
};
