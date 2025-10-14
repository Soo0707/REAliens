#pragma once

#include <unordered_map>
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
	Beer,
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
	Bullet,
	Xp,

	Australian,
	Beer,
	Drunkard,
	Pleb,
	Poison,
	Trapper
};

class AssetManager
{
	public:
		AssetManager();
		~AssetManager();

		std::unordered_map<EntityTextureKey, std::vector<Texture2D>> EntityTextures;
		std::unordered_map<StaticTextureKey, Texture2D> StaticTextures;

		Texture2D Ground;

	private:
		void LoadEntityTextures();
		void UnloadEntityTextures();
		EntityTextureKey GetEntityTextureKeyFromString(std::string folder_name);

		void LoadStaticTextures();
		void UnloadStaticTextures();
		StaticTextureKey GetStaticTextureKeyFromString(std::string filename);
};
