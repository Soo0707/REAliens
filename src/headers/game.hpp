#pragma once

#include <vector>
#include <memory>

#include "player.hpp"
#include "assetManager.hpp"
#include "gameObjects.hpp"

class Game
{
	public:
		Game();
		~Game();
		void Draw();
		void Update();
		void HandleInput();

	private:
		std::vector<std::shared_ptr<BasicGameObject>> AllObjects;
		std::vector<std::shared_ptr<UpdatableGameObject>> UpdatableObjects;

		std::unique_ptr<AssetManager> AssetManagerInstance;
		std::shared_ptr<Player> PlayerInstance;

	//std::vector<std::unique_ptr> Collidables;
	//std::vector<std::unique_ptr> Enemies;

	//std::vector<std::unique_ptr> EnemyProjectiles;
	//std::vector<std::unique_ptr> PlayerProjectiles;
};


