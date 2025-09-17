#include <string>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <memory>

#include "game.hpp"
#include "player.hpp"
#include "tmx.h"

Game::Game()
{
	Game::LoadStaticTextures();
	Game::LoadEntityTextures();
	
	std::shared_ptr PlayerInstance = std::make_shared<Player>(500, 500, this->EntityTextures[EntityTextureKey::PlayerSouth][0]);

	this->AllObjects.push_back(PlayerInstance);
	this->UpdatableObjects.push_back(PlayerInstance);
}

Game::~Game()
{
	Game::UnloadStaticTextures();
	Game::UnloadEntityTextures();
}

void Game::Draw()
{
	for (auto const &obj : this->AllObjects)
		obj->Draw();
}

void Game::Update()
{
	for (auto const &obj : this->UpdatableObjects)
		obj->Update();
}

void Game::HandleInput()
{

}

StaticTextureKey Game::GetStaticTextureKeyFromString(std::string filename)
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

EntityTextureKey Game::GetEntityTextureKeyFromString(std::string folder_name)
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

void Game::LoadEntityTextures()
{
	std::filesystem::path base_path = "assets/entity_textures";

	for (auto const &directory : std::filesystem::directory_iterator(base_path))
	{
		std::cout << std::filesystem::relative(directory, base_path) << std::endl;

		EntityTextureKey Entity = Game::GetEntityTextureKeyFromString(
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

void Game::LoadStaticTextures()
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

void Game::UnloadEntityTextures()
{
	for (auto const &pair : this->EntityTextures)
	{
		for (auto const &item : this->EntityTextures[pair.first])
		{
			UnloadTexture(item);
		}
	}
}

void Game::UnloadStaticTextures()
{
	for (auto const &pair : this->StaticTextures)
	{
		UnloadTexture(this->StaticTextures[pair.first]);
	}
}
