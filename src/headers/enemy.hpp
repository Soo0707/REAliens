#pragma once

#include "raylib.h"
#include "assetManager.hpp"
#include "constants.hpp"

#include <cstddef>




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
		//unsigned int LeAttackCooldown = 3 * TICK_RATE;

		EnemyType Type;
		BehaviourModifier Modifiers;

	private:
		void Move() noexcept;
		void Animate(size_t ticks) noexcept;
		void SetDirection(Vector2 player_centre, bool is_stinky) noexcept;

		Texture2D Image;

		size_t LastAnimationUpdate = 0;
		unsigned int ImageIndex = 0;
		unsigned int AnimationInterval;
		unsigned int AnimationFrames;
};
