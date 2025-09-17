#include <string>
#include <filesystem>
#include <iostream>
#include <algorithm>

#include "game.hpp"

Game::Game()
{
	Game::loadStaticTextures();
	Game::loadEntityTextures();
}

Game::~Game()
{
	Game::unloadStaticTextures();
	Game::unloadEntityTextures();
}

void Game::draw()
{

}

void Game::update()
{

}

void Game::handleInput()
{


}

StaticTextureKey Game::GetStaticTextureKeyFromString(std::string filename)
{
	if (filename == "circle.png")
		return StaticTextureKey::Circle;
	else if (filename == "lazer.png")
		return StaticTextureKey::Lazer;
	else if (filename == "projectile.png")
		return StaticTextureKey::Projectile;
	else if (filename == "xp.png")
		return StaticTextureKey::Xp;
	else
		return StaticTextureKey::None;
}

EntityTextureKey Game::GetEntityTextureKeyFromString(std::string folder_name)
{
	if (folder_name == "player_north")
		return EntityTextureKey::PlayerNorth;
	else if (folder_name == "player_south")
		return EntityTextureKey::PlayerSouth;
	else if (folder_name == "player_east")
		return EntityTextureKey::PlayerEast;
	else if (folder_name == "player_west")
		return EntityTextureKey::PlayerWest;
	else if (folder_name == "australian_flash")
		return EntityTextureKey::AustralianFlash;
	else if (folder_name == "australian_normal")
		return EntityTextureKey::AustralianNormal;
	else if (folder_name == "big_man_normal")
		return EntityTextureKey::BigManNormal;
	else if (folder_name == "big_man_flash")
		return EntityTextureKey::BigManFlash;
	else if (folder_name == "beer")
		return EntityTextureKey::Beer;
	else if (folder_name == "bomber_flash")
		return EntityTextureKey::BomberFlash;
	else if (folder_name == "bomber_normal")
		return EntityTextureKey::BomberNormal;
	else if (folder_name == "bomber_explosion")
		return EntityTextureKey::BomberExplosion;
	else if (folder_name == "drunkard_flash")
		return EntityTextureKey::DrunkardFlash;
	else if (folder_name == "drunkard_normal")
		return EntityTextureKey::DrunkardNormal;
	else if (folder_name == "pleb_flash")
		return EntityTextureKey::PlebFlash;
	else if (folder_name == "pleb_normal")
		return EntityTextureKey::PlebNormal;
	else if (folder_name == "poison_flash")
		return EntityTextureKey::PoisonFlash;
	else if (folder_name == "poison_normal")
		return EntityTextureKey::PoisonNormal;
	else if (folder_name == "trapper_flash")
		return EntityTextureKey::TrapperFlash;
	else if (folder_name == "trapper_normal")
		return EntityTextureKey::TrapperNormal;
	else
		return EntityTextureKey::None;

}

void Game::loadEntityTextures()
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

void Game::loadStaticTextures()
{
	std::filesystem::path path = "assets/static_textures";

	for (auto const &item : std::filesystem::directory_iterator(path))
	{
		StaticTextureKey CurrentTexture = GetStaticTextureKeyFromString(std::filesystem::relative(item, path).string());

		if (CurrentTexture != StaticTextureKey::None)
		{
			this -> StaticTextures[CurrentTexture] = LoadTexture(item.path().string().c_str());
		}
	}
}

void Game::unloadEntityTextures()
{
	for (auto const &pair : this->EntityTextures)
	{
		for (auto const &item : this->EntityTextures[pair.first])
		{
			UnloadTexture(item);
		}
	}
}

void Game::unloadStaticTextures()
{
	for (auto const &pair : this->StaticTextures)
	{
		UnloadTexture(this->StaticTextures[pair.first]);
	}
}
