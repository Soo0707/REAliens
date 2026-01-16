#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "raylib.h"

enum class TextureKey : uint8_t
{
	None,
	Lazer,
	Bullet,
	Xp,
	Ball,
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
	XpBarBackground,
	COUNT
};

class AssetManager
{
	public:
		AssetManager();
		~AssetManager();

		const Texture2D GetTexture(const TextureKey texture_key) const noexcept;
		Texture2D Ground;

	private:
		void LoadTextures() noexcept;
		void UnloadTextures() noexcept;
		TextureKey GetTextureKeyFromString(std::string filename) const noexcept;

		std::array<Texture2D, static_cast<size_t>(TextureKey::COUNT)> Textures;
		std::array<bool, static_cast<size_t>(TextureKey::COUNT)> TextureLoaded;
};
