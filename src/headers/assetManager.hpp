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
	Drunkard,
	Pleb,
	Poison,
	Trapper,

	PlayerNorth,
	PlayerSouth,
	PlayerEast,
	PlayerWest,
};

enum class SoundKey
{
	None,
	PlayerDamage,
	LevelUp,

	Bullets,
	Lazer,
	Xp,

	Pause,
	Unpause,
	Select,

	Poison
};

class AssetManager
{
	public:
		AssetManager();
		~AssetManager();

		std::unordered_map<TextureKey, Texture2D> Textures;
		std::unordered_map<SoundKey, Sound> Sounds;

		Texture2D Ground;
	private:
		void LoadTextures() noexcept;
		void UnloadTextures() noexcept;
		TextureKey GetTextureKeyFromString(std::string filename) const noexcept;

		void LoadSounds() noexcept;
		void UnloadSounds() noexcept;
		SoundKey GetSoundKeyFromString(std::string filename) const noexcept;
};
