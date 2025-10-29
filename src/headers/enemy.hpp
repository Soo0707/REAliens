#pragma once

#include "raylib.h"
#include "assetManager.hpp"
#include <cstddef>

static constexpr unsigned int FLASH_DURATION = 60;

enum class EnemyType
{
	Australian,
	Drunkard,
	Pleb,
	Poison,
	Trapper
};

enum class BehaviourModifier : size_t
{
	None = 0,
	OverrideDirection = 1 << 0,
	NoMovement = 1 << 1,

	Big = 1 << 2,
	IncreasedSpeed = 1 << 4,
	Invisible = 1 << 5
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
		Enemy(float pos_x, float pos_y, int layer, AssetManager& assets, EnemyType type, BehaviourModifier modifier = BehaviourModifier::None) noexcept;
		~Enemy() = default; 
		
		void Draw() const noexcept;
		void FlashSprite(size_t ticks) noexcept;
		void Update(Rectangle& player_rect, size_t ticks) noexcept;
		
		float Health;

		float Speed;

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle Rect;

		bool Flash = false;
		size_t FlashTriggered = 0;

		bool CanLeAttack = true;
		size_t LastLeAttack = 0;
		unsigned int LeAttackCooldown;

		float Scale;
		int Layer;

		EnemyType Type;
		BehaviourModifier Modifiers;

	private:
		void Move() noexcept;
		void Animate(size_t ticks) noexcept;
		void SetDirection(Rectangle& player_rect) noexcept;

		Texture2D Image;

		size_t LastAnimationUpdate = 0;
		unsigned int ImageIndex = 0;
		unsigned int AnimationSpeed;
		unsigned int AnimationFrames;
};
