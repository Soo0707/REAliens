#pragma once

#include <vector>
#include <memory>
#include <thread>

#include <barrier>
#include <atomic>
#include <mutex>

#include "raylib.h"

#include "player.hpp"
#include "assetManager.hpp"
#include "globalDataWrapper.hpp"

#include "projectiles.hpp"
#include "enemy.hpp"
#include "xp.hpp"

class Game
{
	public:
		Game(std::shared_ptr<GlobalDataWrapper> global_data);
		~Game();
		void Draw();
		void Update();

	private:
		void SpawnEnemies();
		void LoopOverMap(Rectangle& m_obj);
		void HandleEvents();

		void HandleInput();
		void HandleLeftClick();
		void HandleRightClick();

		std::atomic<bool> RunThreads = true;

		std::vector<std::thread> Threads;

		std::barrier<void(*)()> StartWorkers;
		std::barrier<void(*)()> StopWorkers;
		void UpdateThread1();
		void UpdateThread2();


		std::mutex EnemiesMutex;
		std::vector<Enemy> Enemies;

		std::mutex ProjectilesMutex;
		std::vector<Projectile> Projectiles;
		
		std::mutex XpsMutex;
		std::vector<Xp> Xps;

		std::shared_ptr<AssetManager> Assets;
		std::unique_ptr<Player> PlayerInstance;

		std::shared_ptr<GlobalDataWrapper> GlobalData;


		unsigned int LastLMB = 0;
		bool CanLMB = true;

		unsigned int LastRMB = 0;
		bool CanRMB = true;

		float Accumulator = 0.0f;
		size_t CollectedXp = 0;

		size_t LastSpawn = 0;
		size_t SpawnTimeout = 1000;

		Camera2D Camera;
		Rectangle UpdateArea;
};
