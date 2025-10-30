#include "assetManager.hpp"

#include <unordered_map>
#include <filesystem>
#include <string>

#include "raylib.h"

AssetManager::AssetManager()
{
	AssetManager::LoadTextures();
	AssetManager::LoadSounds();

	this->Ground = LoadTexture("assets/map/ground.png");
}

AssetManager::~AssetManager()
{
	AssetManager::UnloadTextures();
	AssetManager::UnloadSounds();
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
		{ "ball.png", TextureKey::Ball },
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

	return (lookup.count(filename)) ? lookup.at(filename) : TextureKey::None;
}

void AssetManager::LoadSounds() noexcept
{
	std::filesystem::path path = "assets/sounds";

	if (!std::filesystem::exists(path))
		exit(1);

	for (auto const &item : std::filesystem::directory_iterator(path))
	{
		SoundKey current_sound = AssetManager::GetSoundKeyFromString(std::filesystem::relative(item, path).string());

		if (current_sound != SoundKey::None)
		{
			this->Sounds[current_sound] = LoadSound(item.path().string().c_str());
		}
	}
}

void AssetManager::UnloadSounds() noexcept
{
	for (auto const &pair : this->Sounds)
	{
		UnloadSound(this->Sounds[pair.first]);
	}
}

SoundKey AssetManager::GetSoundKeyFromString(std::string filename) const noexcept
{
	static const std::unordered_map<std::string, SoundKey> lookup =
	{
		{ "player_damage.wav", SoundKey::PlayerDamage },
		{ "level_up.wav", SoundKey::LevelUp },

		{ "bullets.wav", SoundKey::Bullets },
		{ "lazer.wav", SoundKey::Lazer },
		{ "xp.wav" , SoundKey::Xp },

		{ "pause.wav", SoundKey::Pause },
		{ "unpause.wav", SoundKey::Unpause },
		{ "select.wav", SoundKey::Select },

		{ "poison.wav", SoundKey::Poison }
	};

	return (lookup.count(filename)) ? lookup.at(filename) : SoundKey::None;
}


