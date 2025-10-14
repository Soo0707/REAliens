#include "assetManager.hpp"

#include <unordered_map>
#include <filesystem>
#include <string>
#include <algorithm>

#include "raylib.h"

AssetManager::AssetManager()
{
	AssetManager::LoadTextures();

	this->Ground = LoadTexture("assets/map/ground.png");
}

AssetManager::~AssetManager()
{
	AssetManager::UnloadTextures();
}

void AssetManager::LoadTextures()
{
	std::filesystem::path path = "assets/textures";

	if (!std::filesystem::exists(path))
		exit(1);

	for (auto const &item : std::filesystem::directory_iterator(path))
	{
		TextureKey CurrentTexture = GetTextureKeyFromString(std::filesystem::relative(item, path).string());

		if (CurrentTexture != TextureKey::None)
		{
			this->Textures[CurrentTexture] = LoadTexture(item.path().string().c_str());
		}
	}
}

void AssetManager::UnloadTextures()
{
	for (auto const &pair : this->Textures)
	{
		UnloadTexture(this->Textures[pair.first]);
	}
}


TextureKey AssetManager::GetTextureKeyFromString(std::string filename)
{
	static const std::unordered_map<std::string, TextureKey> Lookup =
	{
		{ "circle.png", TextureKey::Circle },
		{ "lazer.png", TextureKey::Lazer },
		{ "bullet.png", TextureKey::Bullet },
		{ "xp.png", TextureKey::Xp },

		{ "australian.png", TextureKey::Australian },
		{ "drunkard.png", TextureKey::Drunkard },
		{ "pleb.png" , TextureKey::Pleb },
		{ "poison.png", TextureKey::Poison },
		{ "trapper.png", TextureKey::Trapper },

		{ "player_east.png", TextureKey::PlayerEast },
		{ "player_west.png", TextureKey::PlayerWest },
		{ "player_north.png", TextureKey::PlayerNorth },
		{ "player_south.png", TextureKey::PlayerSouth }
	};

	return (Lookup.count(filename)) ? Lookup.at(filename) : TextureKey::None;
}
