#pragma once

#include <unordered_map>

#include "assetManager.hpp"
#include "enemy.hpp"

struct EnemyData
{
	EntityTextureKey texture_key;
	float damage;
	float speed;
	float animation_speed;
	float health;
};

inline static const std::unordered_map<EnemyType, EnemyData> EnemyAttributes = 
{
	{ 
		EnemyType::Australian,
		{
			EntityTextureKey::Australian,
			5,
			175,
			10,
			75,
		}
	},
	{ 
		EnemyType::Bomber,
		{
			EntityTextureKey::Bomber,
			15,
			175,
			10,
			100,
		}
	},
	{ 
		EnemyType::Drunkard,
		{
			EntityTextureKey::Drunkard,
			5,
			250,
			60,
			50,
		}
	},
	{ 
		EnemyType::Pleb,
		{
			EntityTextureKey::Pleb,
			5,
			150,
			10,
			75,
		}
	},
	{ 
		EnemyType::Poison,
		{
			EntityTextureKey::Poison,
			5,
			125,
			60,
			100,
		}
	},
	{ 
		EnemyType::Trapper,
		{
			EntityTextureKey::Trapper,
			10,
			125,
			50,
			100,
		}
	}
};

inline static const std::unordered_map<EnemyType, unsigned int> EnemyXpValues = 
{
	{ EnemyType::Australian, 1 },
	{ EnemyType::Bomber, 3 },
	{ EnemyType::Drunkard, 2 },
	{ EnemyType::Pleb, 1 },
	{ EnemyType::Poison, 2 },
	{ EnemyType::Trapper, 2	}
};
