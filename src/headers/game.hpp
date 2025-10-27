#pragma once

#include <vector>
#include <memory>

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

		void HandleEssentialInput() noexcept;
		void Draw(RenderTexture2D& virtual_canvas) const noexcept;
		void Update() noexcept;

		std::shared_ptr<AssetManager> Assets;
		std::shared_ptr<GlobalDataWrapper> GlobalData;


		std::vector<Enemy> Enemies;
		std::vector<Projectile> Projectiles;
		std::vector<Xp> Xps;

		std::unique_ptr<Player> PlayerInstance;

	private:
		void SpawnEnemies() noexcept;
		void LoopOverMap(Rectangle& m_obj) noexcept;

		void DrawOverlay() const noexcept;

		void LevelUp() noexcept;

		void UpdateEnemies() noexcept;
		void UpdateProjectiles() noexcept;
		void UpdateXps() noexcept;

		void HandleTickedInput() noexcept;
		void HandleLeftClick() noexcept;
		void HandleRightClick() noexcept;

		size_t LastLMB = 0;
		bool CanLMB = true;

		size_t LastRMB = 0;
		bool CanRMB = true;

		size_t LastLayerDown = 0;
		bool CanLayerDown = true;

		size_t LastLayerUp = 0;
		bool CanLayerUp = true;

		float Accumulator = 0.0f;
		size_t CollectedXp = 0;

		size_t LastSpawn = 0;
		size_t SpawnTimeout;

		Camera2D Camera;
		Rectangle UpdateArea;
};
