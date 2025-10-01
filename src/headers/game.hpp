#pragma once

#include <vector>
#include <memory>
#include <shared_mutex>

#include "raylib.h"

#include "player.hpp"
#include "assetManager.hpp"
#include "gameObjects.hpp"
#include "projectiles.hpp"
#include "enemy.hpp"

static constexpr float TICK_TIME = (1.0/60.0);
static constexpr float MAX_TICK_TIME = (1.0/5.0);


enum class EffectKey
{
	BulletCooldown,
	BulletDamage,
	BulletSpeed,

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

		std::shared_mutex EnemiesMutex;
		std::vector<Enemy> Enemies;

		std::vector<Projectile> Projectiles;

		std::shared_ptr<AssetManager> AssetManagerInstance;
		std::unique_ptr<Player> PlayerInstance;

		std::unordered_map<EffectKey, float> Effects = 
		{
			{ EffectKey::BulletCooldown, 2500 },
			{ EffectKey::BulletDamage, 25.0f },
			{ EffectKey::BulletSpeed, 1000.0f },

			{ EffectKey::LazerCooldown, 7500 },
			{ EffectKey::LazerDamage, 25.0f },
			{ EffectKey::LazerScale, 1.0f },
			{ EffectKey::LazerSpeed, 3000.0f }
		};

		std::unordered_map<EffectKey, size_t> EffectTimeouts;

		unsigned int LastLMB = 0;
		bool CanLMB = true;

		unsigned int LastRMB = 0;
		bool CanRMB = true;

		float Accumulator = 0.0f;
		size_t Ticks = 0;
		
		size_t LastSpawn = 0;
		size_t SpawnTimeout = 40000;

		unsigned int Level = 1;

		Camera2D Camera;
		Rectangle UpdateArea;
};
