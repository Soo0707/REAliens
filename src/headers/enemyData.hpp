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
	Trapper,
	COUNT
};

enum class BehaviourModifier : uint8_t
{
	None = 0,
	Big = 1 << 0,
	IncreasedSpeed = 1 << 1
};

struct EnemyAttackComponent
{
	size_t LastLeAttack;
	float Damage;
	bool CanLeAttack;
};

struct EnemyDisplayComponent
{
	size_t LastFlash;
	TextureKey Texture;
	bool Flash;
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

inline BehaviourModifier operator| (BehaviourModifier l, BehaviourModifier r) noexcept
{
	return static_cast<BehaviourModifier>(static_cast<size_t>(l) | static_cast<size_t>(r));
}

inline BehaviourModifier operator& (BehaviourModifier l, BehaviourModifier r) noexcept
{
	return static_cast<BehaviourModifier>(static_cast<size_t>(l) & static_cast<size_t>(r));
}
