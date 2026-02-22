#pragma once

#include <cstdint>

enum class Attribute : uint32_t
{
	BulletDamage,
	BulletSpeed,

	Buckshot,
	BuckshotSpread,

	LazerDamage,
	LazerScale,
	LazerSpeed,
	LazerMaxHit,

	BallSpeed,
	BallScale,
	BallDamage,

	AuraSize,
	AuraDamage,

	LifeStealMultiplier,
	SlideSpeedMultiplier,

	LuckBottomLimit,
	LuckBoundary,
	LuckUpperLimit,

	COUNT
};

enum class Effect : uint32_t
{
	Greenbull = 1 << 0,
	Milk = 1 << 1,

	LifeSteal = 1 << 2,
	Magnetism = 1 << 3,

	Trapped = 1 << 4,
	Poison = 1 << 5,
	Drunk = 1 << 6,

	Aussie = 1 << 7,
	Stinky = 1 << 8,

	Microscope = 1 << 9,
	Earthquake = 1 << 10,
	Invisible = 1 << 11,

	Weakness = 1 << 12,
	DiddySlide = 1 << 13
};

constexpr Effect operator&(const Effect l, const Effect r) noexcept
{ 
	return static_cast<Effect>(static_cast<uint32_t>(l) & static_cast<uint32_t>(r));
}

constexpr Effect operator|(const Effect l, const Effect r) noexcept
{
	return static_cast<Effect>(static_cast<uint32_t>(l) | static_cast<uint32_t>(r));
}

constexpr Effect operator~(const Effect target) noexcept
{
	return static_cast<Effect>(~static_cast<uint32_t>(target));
}
