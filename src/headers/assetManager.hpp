#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>

#include "raylib.h"
#include "tmx.h"

constexpr unsigned int TILESIZE = 32;

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

		std::unordered_map<EntityTextureKey, std::vector<Texture2D>> EntityTextures;
		std::unordered_map<StaticTextureKey, Texture2D> StaticTextures;

		Texture2D Ground;
		std::unordered_map<int32_t, Texture2D> MapTextures;

	private:
		void LoadEntityTextures();
		void UnloadEntityTextures();
		EntityTextureKey GetEntityTextureKeyFromString(std::string folder_name);

		void LoadStaticTextures();
		void UnloadStaticTextures();
		StaticTextureKey GetStaticTextureKeyFromString(std::string filename);

		void LoadMapTextures();
		void UnloadMapTextures();
		void LoadMapTexturesHelper(tmx_map* map, tmx_layer* layer, Image* tileset);

};
