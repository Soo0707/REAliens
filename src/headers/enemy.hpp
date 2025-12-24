#pragma once

#include "raylib.h"
#include "assetManager.hpp"
#include "constants.hpp"

#include <cstddef>

static constexpr unsigned int FLASH_DURATION = 60;

enum class EnemyType : size_t
{
	Australian,
	Drunkard,
	Pleb,

	Poison,
	Trapper,
	COUNT
};

enum class BehaviourModifier : size_t
{
	None = 0,
	Big = 1 << 0,
	IncreasedSpeed = 1 << 1
};

inline BehaviourModifier operator| (BehaviourModifier l, BehaviourModifier r) noexcept
{
	return static_cast<BehaviourModifier>(static_cast<size_t>(l) | static_cast<size_t>(r));
}

inline BehaviourModifier operator& (BehaviourModifier l, BehaviourModifier r) noexcept
{
	return static_cast<BehaviourModifier>(static_cast<size_t>(l) & static_cast<size_t>(r));
}


class Enemy
{
	public:
		Enemy(float pos_x, float pos_y, float level_scale, const AssetManager& assets, EnemyType type, BehaviourModifier modifier) noexcept;
		~Enemy() = default; 
		
		void Draw() const noexcept;
		void FlashSprite(size_t ticks) noexcept;
		void Update(Vector2 player_centre, size_t ticks, bool is_stinky) noexcept;
		
		float Health;
		float Speed;

		float Scale;
		float Damage;

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle Rect;

		bool Flash = false;
		size_t FlashTriggered = 0;

		bool CanLeAttack = true;
		size_t LastLeAttack = 0;
		unsigned int LeAttackCooldown = 3 * TICK_RATE;

		EnemyType Type;
		BehaviourModifier Modifiers;

	private:
		void Move() noexcept;
		void Animate(size_t ticks) noexcept;
		void SetDirection(Vector2 player_centre, bool is_stinky) noexcept;

		Texture2D Image;

		size_t LastAnimationUpdate = 0;
		unsigned int ImageIndex = 0;
		unsigned int AnimationSpeed;
		unsigned int AnimationFrames;
};
