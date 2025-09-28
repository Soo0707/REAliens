#pragma once

#include <vector>
#include <memory>

#include "raylib.h"
#include "tmx.h"

#include "player.hpp"
#include "assetManager.hpp"
#include "gameObjects.hpp"
#include "projectiles.hpp"
#include "enemy.hpp"

#define WALLS_LAYER 1
#define PROPS_LAYER 2
#define SPAWNERS_LAYER 3

enum class EffectKey
{
	BulletTimeout,
	BulletDamage,
	BulletSpeed,

	LazerTimeout,
	LazerDamage,
	LazerScale,
	LazerSpeed
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
		void InitialiseMapObjects(tmx_map* map, tmx_layer* layer, const unsigned int type);

		std::vector<Wall> Walls;
		std::vector<Prop> Props;
		std::vector<Spawner> Spawners;
		std::vector<Enemy> Enemies;
		std::vector<Projectile> Projectiles;

		std::unique_ptr<AssetManager> AssetManagerInstance;
		std::unique_ptr<Player> PlayerInstance;

		std::unordered_map<EffectKey, float> Effects = 
		{
			{ EffectKey::BulletTimeout, 250/60 },
			{ EffectKey::BulletDamage, 25.0f },
			{ EffectKey::BulletSpeed, 1000.0f },

			{ EffectKey::LazerTimeout, 750/60 },
			{ EffectKey::LazerDamage, 25.0f },
			{ EffectKey::LazerScale, 1.0f },
			{ EffectKey::LazerSpeed, 3000.0f }
		};
		
		Camera2D Camera;
		Rectangle UpdateArea;
};


