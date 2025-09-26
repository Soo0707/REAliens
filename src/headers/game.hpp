#pragma once

#include <vector>
#include <memory>

#include "raylib.h"
#include "tmx.h"

#include "player.hpp"
#include "assetManager.hpp"
#include "gameObjects.hpp"
#include "projectiles.hpp"

#define WALLS_LAYER 1
#define PROPS_LAYER 2
#define SPAWNERS_LAYER 3

class Game
{
	public:
		Game();
		~Game();
		void Draw();
		void Update();
		void HandleInput();

	private:
		void InitialiseMapObjects(tmx_map* map, tmx_layer* layer, const unsigned int type);

		void ResolveCollisionsX();
		void ResolveCollisionsY();

		//std::vector<std::shared_ptr<BasicGameObject>> AllObjects;
		//std::vector<std::shared_ptr<UpdatableGameObject>> UpdatableObjects;
		//std::vector<std::shared_ptr<MovableGameObject>> MovableObjects;

		std::vector<Wall> Walls;
		std::vector<BasicGameObject> Props;
		std::vector<Spawner> Spawners;
		std::vector<MovableGameObject> Enemies;
		std::vector<Projectile> Projectiles;

		std::unique_ptr<AssetManager> AssetManagerInstance;
		std::unique_ptr<Player> PlayerInstance;
		

		Camera2D Camera;
		


	//std::vector<std::unique_ptr> EnemyProjectiles;
	//std::vector<std::unique_ptr> PlayerProjectiles;
};


