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

		std::shared_ptr<AssetManager> Assets;
		std::unique_ptr<Player> PlayerInstance;

		std::shared_ptr<AttributeManager> Attributes;

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
