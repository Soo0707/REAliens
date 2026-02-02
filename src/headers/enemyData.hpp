#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "assetManager.hpp"

enum class EnemyType : uint8_t
{
	Australian,
	Drunkard,
	Pleb,
	Poison,
	COUNT
};

struct EnemyAttackComponent
{
	size_t LastLeAttack;
	float Damage;
	bool CanLeAttack;
};

struct EnemyAnimationComponent
{
	size_t LastAnimationUpdate;
	uint8_t ImageIndex;
	uint8_t AnimationInterval;
	uint8_t AnimationFrames;
};

struct EnemyData
{
	const TextureKey Texture;
	const float Damage;
	const float Speed;
	const float Health;

	const uint8_t AnimationInterval;
	/*
	animation_frames = number of actual frames -1

	case index = 0:
		0 * TEXTURE_TILE_SIZE = 0, first frame
	
	so everything is off by 1
	*/
	const uint8_t AnimationFrames;
};
