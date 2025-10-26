#pragma once

#include <unordered_map>
#include <string>

#include "raylib.h"

enum class TextureKey
{
	None,
	Ball,
	Lazer,
	Bullet,
	Xp,

	Australian,
	Beer,
	Drunkard,
	Pleb,
	Poison,
	Trapper,

	PlayerNorth,
	PlayerSouth,
	PlayerEast,
	PlayerWest,
};

class AssetManager
{
	public:
		AssetManager();
		~AssetManager();

		std::unordered_map<TextureKey, Texture2D> Textures;

		Texture2D Ground;
	private:
		void LoadTextures();
		void UnloadTextures();
		TextureKey GetTextureKeyFromString(std::string filename);
};
