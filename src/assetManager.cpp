#include "assetManager.hpp"

#include <unordered_map>
#include <filesystem>
#include <string>

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

void AssetManager::LoadTextures() noexcept
{
	std::filesystem::path path = "assets/textures";

	if (!std::filesystem::exists(path))
		exit(1);

	for (auto const &item : std::filesystem::directory_iterator(path))
	{
		TextureKey current_texture = AssetManager::GetTextureKeyFromString(std::filesystem::relative(item, path).string());

		if (current_texture != TextureKey::None)
		{
			this->Textures[current_texture] = LoadTexture(item.path().string().c_str());
		}
	}
}

void AssetManager::UnloadTextures() noexcept
{
	for (auto const &pair : this->Textures)
	{
		UnloadTexture(this->Textures[pair.first]);
	}
}


TextureKey AssetManager::GetTextureKeyFromString(std::string filename) const noexcept
{
	static const std::unordered_map<std::string, TextureKey> lookup =
	{
		{ "lazer.png", TextureKey::Lazer },
		{ "bullet.png", TextureKey::Bullet },
		{ "xp.png", TextureKey::Xp },
		{ "ball.png", TextureKey::Ball },

		{ "australian.png", TextureKey::Australian },
		{ "drunkard.png", TextureKey::Drunkard },
		{ "pleb.png" , TextureKey::Pleb },

		{ "poison.png", TextureKey::Poison },
		{ "trapper.png", TextureKey::Trapper },
		{ "player.png", TextureKey::Player },

		{ "milk_icon.png", TextureKey::MilkIcon },
		{ "magnetism_icon.png", TextureKey::MagnetismIcon },
		{ "greenbull_icon.png", TextureKey::GreenbullIcon },
		{ "drunk_icon.png", TextureKey::DrunkIcon },

		{ "white_pixel.png", TextureKey::WhitePixel },
		{ "health_bar_background.png", TextureKey::HealthBarBackground },
		{ "xp_bar_background.png", TextureKey::XpBarBackground }
	};

	return (lookup.count(filename)) ? lookup.at(filename) : TextureKey::None;
}
