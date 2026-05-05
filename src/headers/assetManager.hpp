/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
	Turret,
	Glue,
	Ball,
	Australian,
	Drunkard,
	Pleb,
	Poison,
	Player,
	MilkIcon,
	MagnetismIcon,
	GreenbullIcon,
	DrunkIcon,
	TariffIcon,
	WeaknessIcon,
	WhitePixel,
	HealthBarBackground,
	XpBarBackground,
	Orange,
	Tyrone,
	AlcoholismIcon,
	Plebifier,
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
