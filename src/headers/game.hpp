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
			{ EffectKey::BulletCooldown, 250 },
			{ EffectKey::BulletDamage, 25.0f },
			{ EffectKey::BulletSpeed, 1000.0f },

			{ EffectKey::LazerCooldown, 750 },
			{ EffectKey::LazerDamage, 25.0f },
			{ EffectKey::LazerScale, 1.0f },
			{ EffectKey::LazerSpeed, 3000.0f }
		};

		unsigned int LastLMB = 0;
		bool CanLMB = true;

		unsigned int LastRMB = 0;
		bool CanRMB = true;

		float Accumulator = 0.0f;
		size_t Ticks = 0;
		
		Camera2D Camera;
		Rectangle UpdateArea;
};


