#include "assetManager.hpp"

#include <map>
#include <filesystem>
#include <string>
#include <algorithm>

#include "raylib.h"

AssetManager::AssetManager()
{
	AssetManager::LoadStaticTextures();
	AssetManager::LoadEntityTextures();
}

AssetManager::~AssetManager()
{
	AssetManager::UnloadStaticTextures();
	AssetManager::UnloadEntityTextures();
}

void AssetManager::LoadStaticTextures()
{
	std::filesystem::path path = "assets/static_textures";

	for (auto const &item : std::filesystem::directory_iterator(path))
	{
		StaticTextureKey CurrentTexture = GetStaticTextureKeyFromString(std::filesystem::relative(item, path).string());

		if (CurrentTexture != StaticTextureKey::None)
		{
			this->StaticTextures[CurrentTexture] = LoadTexture(item.path().string().c_str());
		}
	}
}

void AssetManager::UnloadStaticTextures()
{
	for (auto const &pair : this->StaticTextures)
	{
		UnloadTexture(this->StaticTextures[pair.first]);
	}
}


StaticTextureKey AssetManager::GetStaticTextureKeyFromString(std::string filename)
{
	static const std::map<std::string, StaticTextureKey> Lookup =
	{
		{ "circle.png", StaticTextureKey::Circle },
		{ "lazer.png", StaticTextureKey::Lazer },
		{ "projectile.png", StaticTextureKey::Projectile },
		{ "xp.png", StaticTextureKey::Xp }
	};

	return (Lookup.count(filename)) ? Lookup.at(filename) : StaticTextureKey::None;
}

void AssetManager::LoadEntityTextures()
{
	std::filesystem::path base_path = "assets/entity_textures";

	for (auto const &directory : std::filesystem::directory_iterator(base_path))
	{
		EntityTextureKey Entity = GetEntityTextureKeyFromString(
				std::filesystem::relative(directory, base_path).string()
		);

		if (Entity != EntityTextureKey::None)
		{
			this->EntityTextures[Entity] = std::vector<Texture2D>{};

			std::vector<std::filesystem::path> tmp;
			
			for (auto const &item : std::filesystem::directory_iterator(directory))
			{
				tmp.push_back(item.path());
			}
			
			std::sort(tmp.begin(), tmp.end());
			
			for (auto const &item : tmp)
			{
				this->EntityTextures[Entity].push_back(LoadTexture(item.string().c_str()));
			}
		}
	}
}

void AssetManager::UnloadEntityTextures()
{
	for (auto const &pair : this->EntityTextures)
	{
		for (auto const &item : this->EntityTextures[pair.first])
		{
			UnloadTexture(item);
		}
	}
}

EntityTextureKey AssetManager::GetEntityTextureKeyFromString(std::string folder_name)
{
	static const std::map<std::string, EntityTextureKey> Lookup =
	{
		{ "player_north", EntityTextureKey::PlayerNorth },
		{ "player_south", EntityTextureKey::PlayerSouth },
		{ "player_east", EntityTextureKey::PlayerEast },
		{ "player_west", EntityTextureKey::PlayerWest },
		{ "australian", EntityTextureKey::Australian },
		{ "big_man", EntityTextureKey::BigMan },
		{ "beer", EntityTextureKey::Beer },
		{ "bomber", EntityTextureKey::Bomber },
		{ "bomber_explosion", EntityTextureKey::BomberExplosion },
		{ "drunkard", EntityTextureKey::Drunkard },
		{ "pleb", EntityTextureKey::Pleb },
		{ "poison", EntityTextureKey::Poison},
		{ "trapper", EntityTextureKey::Trapper}
	};

	return (Lookup.count(folder_name)) ? Lookup.at(folder_name) : EntityTextureKey::None;
}
