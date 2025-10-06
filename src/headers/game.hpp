#pragma once

#include <vector>
#include <memory>
#include <shared_mutex>

#include "raylib.h"

#include "player.hpp"
#include "assetManager.hpp"
#include "globalDataWrapper.hpp"

#include "projectiles.hpp"
#include "enemy.hpp"
#include "xp.hpp"

static constexpr float TICK_TIME = (1.0/240.0);
static constexpr float MAX_TICK_TIME = (1.0/5.0);

class Game
{
	public:
		Game(std::shared_ptr<GlobalDataWrapper> global_data);
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
		std::vector<Xp> Xps;

		std::shared_ptr<AssetManager> Assets;
		std::unique_ptr<Player> PlayerInstance;

		std::shared_ptr<GlobalDataWrapper> GlobalData;

		unsigned int LastLMB = 0;
		bool CanLMB = true;

		unsigned int LastRMB = 0;
		bool CanRMB = true;

		float Accumulator = 0.0f;
		size_t Ticks = 0;
		
		size_t LastSpawn = 0;
		size_t SpawnTimeout = 1000;

		Camera2D Camera;
		Rectangle UpdateArea;
};
