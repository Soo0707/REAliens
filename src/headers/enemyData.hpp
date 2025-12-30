#pragma once

#include <array>

#include "assetManager.hpp"
#include "enemy.hpp"

struct EnemyData
{
	const TextureKey texture_key;
	const float damage;
	const float speed;
	const float health;

	const unsigned int animation_interval;
	/*
	animation_frames = number of actual frames -1

	case index = 0:
		0 * TEXTURE_TILE_SIZE = 0, first frame
	
	so everything is off by 1
	*/
	const unsigned int animation_frames;
};

inline static constexpr std::array<EnemyData, static_cast<size_t>(EnemyType::COUNT)> EnemyAttributes = 
{
	(EnemyData) { TextureKey::Australian, 5, 87.5, 75, 30, 2 },
	(EnemyData) { TextureKey::Drunkard, 5, 125, 50, 15,	4 },
	(EnemyData) { TextureKey::Pleb, 5, 75,	75,	30,	2 },
	(EnemyData) { TextureKey::Poison, 5, 62.5, 100, 30, 3 },
	(EnemyData) { TextureKey::Trapper, 10, 0, 100, 30, 3 }
};

inline static constexpr std::array<unsigned int, static_cast<size_t>(EnemyType::COUNT)> EnemyXpValues = 
{
	1, 2, 1, 2, 2
};
