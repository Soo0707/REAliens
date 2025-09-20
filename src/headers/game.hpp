#pragma once

#include <vector>
#include <memory>

#include "raylib.h"
#include "tmx.h"

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
		void InitialiseMapObjects(tmx_map* map, tmx_layer* layer);

		std::vector<std::shared_ptr<BasicGameObject>> AllObjects;
		std::vector<std::shared_ptr<UpdatableGameObject>> UpdatableObjects;

		std::unique_ptr<AssetManager> AssetManagerInstance;
		std::shared_ptr<Player> PlayerInstance;

		Camera2D Camera;
		
		//Texture2D Ground;

	//std::vector<std::unique_ptr> Collidables;
	//std::vector<std::unique_ptr> Enemies;

	//std::vector<std::unique_ptr> EnemyProjectiles;
	//std::vector<std::unique_ptr> PlayerProjectiles;
};


