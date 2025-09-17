#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "raylib.h"
#include "player.hpp"


enum class EntityTextureKey
{
	None,
	PlayerNorth,
	PlayerSouth,
	PlayerEast,
	PlayerWest,
	Australian,
	BigMan,
	Beer,
	Bomber,
	BomberExplosion,
	Drunkard,
	Pleb,
	Poison,
	Trapper
};

enum class StaticTextureKey
{
	None,
	Circle,
	Lazer,
	Projectile,
	Xp
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
		void LoadEntityTextures();
		void UnloadEntityTextures();
		EntityTextureKey GetEntityTextureKeyFromString(std::string folder_name);
		std::map<EntityTextureKey, std::vector<Texture2D>> EntityTextures;

		void LoadStaticTextures();
		void UnloadStaticTextures();
		StaticTextureKey GetStaticTextureKeyFromString(std::string filename);
		std::map<StaticTextureKey, Texture2D> StaticTextures;

		std::vector<std::shared_ptr<BasicGameObject>> AllObjects;
		std::vector<std::shared_ptr<UpdatableGameObject>> UpdatableObjects;

	//std::vector<std::unique_ptr> Collidables;
	//std::vector<std::unique_ptr> Enemies;

	//std::vector<std::unique_ptr> EnemyProjectiles;
	//std::vector<std::unique_ptr> PlayerProjectiles;
};


