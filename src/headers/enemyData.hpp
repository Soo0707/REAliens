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
};

struct EnemyData
{
	const TextureKey Texture;
	const float Damage;
	const float Speed;
	const float Health;

	const uint8_t AnimationInterval;
};
