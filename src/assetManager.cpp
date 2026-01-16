#include "assetManager.hpp"

#include <unordered_map>
#include <filesystem>
#include <string>

#include "raylib.h"

AssetManager::AssetManager()
{
	this->TextureLoaded = { 0 };
	this->Ground = LoadTexture("assets/map/ground.png");

	AssetManager::LoadTextures();
}

AssetManager::~AssetManager()
{
	AssetManager::UnloadTextures();
}

const Texture2D AssetManager::GetTexture(const TextureKey texture_key) const noexcept
{
	const size_t index = static_cast<size_t>(texture_key);

	if (!this->TextureLoaded[index])
		exit(1);

	return this->Textures[index];
}

void AssetManager::LoadTextures() noexcept
{
	std::filesystem::path path = "assets/textures";

	if (!std::filesystem::exists(path))
		exit(1);

	for (auto const &item : std::filesystem::directory_iterator(path))
	{
		const TextureKey current_texture = AssetManager::GetTextureKeyFromString(std::filesystem::relative(item, path).string());
		const size_t index = static_cast<size_t>(current_texture);

		if (current_texture != TextureKey::None)
		{
			this->Textures[index] = LoadTexture(item.path().string().c_str());
			this->TextureLoaded[index] = true;
		}
	}
}

void AssetManager::UnloadTextures() noexcept
{
	for (size_t i = 0, n = static_cast<size_t>(TextureKey::COUNT); i < n; i++)
	{
		if (this->TextureLoaded[i])
			UnloadTexture(this->Textures[i]);
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
