#pragma once

#include <unordered_map>
#include <string>

#include "raylib.h"

enum class TextureKey
{
	None,
	Lazer,
	Bullet,
	Xp,

	Australian,
	Drunkard,
	Pleb,
	Poison,
	Trapper,

	Player,

	MilkIcon,
	MagnetismIcon,
	GreenbullIcon,
	DrunkIcon,

	WhitePixel,
	HealthBarBackground,
	XpBarBackground
};

class AssetManager
{
	public:
		AssetManager();
		~AssetManager();

		std::unordered_map<TextureKey, Texture2D> Textures;

		Texture2D Ground;

	private:
		void LoadTextures() noexcept;
		void UnloadTextures() noexcept;
		TextureKey GetTextureKeyFromString(std::string filename) const noexcept;
};
