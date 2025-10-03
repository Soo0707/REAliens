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

	bool can_secondary;
	size_t last_secondary;
	unsigned int secondary_cooldown;

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
			false,
			0,
			0
		}
	},
	{ 
		EnemyType::BigMan,
		{
			EntityTextureKey::BigMan,
			50,
			175,
			10,
			500,
			false,
			0,
			0
		}
	},
	{ 
		EnemyType::Bomber,
		{
			EntityTextureKey::Bomber,
			15,
			350,
			10,
			100,
			false,
			0,
			0
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
			true,
			0,
			5000
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
			false,
			0,
			0
		}
	},
	{ 
		EnemyType::Poison,
		{
			EntityTextureKey::Poison,
			5,
			225,
			60,
			100,
			false,
			0,
			0
		}
	},
	{ 
		EnemyType::Trapper,
		{
			EntityTextureKey::Trapper,
			10,
			430,
			50,
			100,
			false,
			0,
			0
		}
	}
};
