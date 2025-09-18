#pragma once

#include <map>
#include <vector>
#include <string>

#include "raylib.h"

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

class AssetManager
{
	public:
		AssetManager();
		~AssetManager();

		std::map<EntityTextureKey, std::vector<Texture2D>> EntityTextures;
		std::map<StaticTextureKey, Texture2D> StaticTextures;
	private:
		void LoadEntityTextures();
		void UnloadEntityTextures();
		EntityTextureKey GetEntityTextureKeyFromString(std::string folder_name);

		void LoadStaticTextures();
		void UnloadStaticTextures();
		StaticTextureKey GetStaticTextureKeyFromString(std::string filename);
};
