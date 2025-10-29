#pragma once

#include "raylib.h"
#include "assetManager.hpp"
#include "game.hpp"

namespace GameHelper
{
	void LoopOverMap(AssetManager& assets, Rectangle& m_obj) noexcept;
	void LevelUp(Game& game) noexcept;
	void SpawnEnemies(Game& game) noexcept;
}
