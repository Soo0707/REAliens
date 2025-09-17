#pragma once

#include <map>
#include <vector>
#include <string>

#include "raylib.h"

enum class EntityTextureKey
{
	None,
	PlayerNorth,
	PlayerSouth,
	PlayerEast,
	PlayerWest,
	AustralianFlash,
	AustralianNormal,
	BigManFlash,
	BigManNormal,
	Beer,
	BomberFlash,
	BomberNormal,
	BomberExplosion,
	DrunkardFlash,
	DrunkardNormal,
	PlebFlash,
	PlebNormal,
	PoisonFlash,
	PoisonNormal,
	TrapperFlash,
	TrapperNormal
};

enum class StaticTextureKey
{
	None,
	Circle,
	Lazer,
	Projectile,
	Xp
};

class Game
{
	public:
		Game();
		~Game();
		void draw();
		void update();
		void handleInput();

	private:
		void loadEntityTextures();
		void unloadEntityTextures();
		EntityTextureKey GetEntityTextureKeyFromString(std::string folder_name);
		std::map<EntityTextureKey, std::vector<Texture2D>> EntityTextures;

		void loadStaticTextures();
		void unloadStaticTextures();
		StaticTextureKey GetStaticTextureKeyFromString(std::string filename);
		std::map<StaticTextureKey, Texture2D> StaticTextures;
};


