#pragma once

#include <unordered_map>

#include "assetManager.hpp"
#include "enemy.hpp"

struct EnemyData
{
	TextureKey texture_key;
	float damage;
	float speed;
	float health;

	unsigned int animation_speed;
	/*
	animation_frames = number of actual frames -1

	case index = 0:
		0 * TEXTURE_TILE_SIZE = 0, first frame
	
	so everything is off by 1
	*/
	unsigned int animation_frames;
};

inline static const std::unordered_map<EnemyType, EnemyData> EnemyAttributes = 
{
	{ 
		EnemyType::Australian,
		{
			TextureKey::Australian,
			5,
			175,
			75,

			120,
			2

		}
	},
	{ 
		EnemyType::Drunkard,
		{
			TextureKey::Drunkard,
			5,
			250,
			50,

			120,
			4
		}
	},
	{ 
		EnemyType::Pleb,
		{
			TextureKey::Pleb,
			5,
			150,
			75,
		
			120,
			2
		}
	},
	{ 
		EnemyType::Poison,
		{
			TextureKey::Poison,
			5,
			125,
			100,

			120,
			3
		}
	},
	{ 
		EnemyType::Trapper,
		{
			TextureKey::Trapper,
			10,
			125,
			100,

			120,
			3
		}
	}
};

inline static const std::unordered_map<EnemyType, unsigned int> EnemyXpValues = 
{
	{ EnemyType::Australian, 1 },
	{ EnemyType::Drunkard, 2 },
	{ EnemyType::Pleb, 1 },
	{ EnemyType::Poison, 2 },
	{ EnemyType::Trapper, 2	}
};
