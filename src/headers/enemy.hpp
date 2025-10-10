#pragma once

#include "raylib.h"
#include "assetManager.hpp"
#include <memory>
#include <cstddef>

static constexpr unsigned int FLASH_DURATION = 30;

enum class EnemyType
{
	Australian,
	Bomber,
	Drunkard,
	Pleb,
	Poison,
	Trapper
};

enum class BehaviourModifier
{
	None,
	BomberExplode,
	OverrideDirection
};

class Enemy
{
	public:
		Enemy(float pos_x, float pos_y, std::shared_ptr<AssetManager> assets, EnemyType type, BehaviourModifier modifier);
		~Enemy(); 
		
		void Move();
		void Animate();
		void SetDirection(Rectangle& player_rect);

		void Draw();
		void FlashSprite(size_t ticks);
		void Update(Rectangle& player_rect, size_t& ticks);
		
		float Health;

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle Rect;

		float Speed;
		bool Flash = false;
		size_t FlashTriggered = 0;

		bool CanLeAttack = true;
		size_t LastLeAttack = 0;
		unsigned int LeAttackCooldown = 5000;

		EnemyType Type;
		BehaviourModifier Modifier;

	private:
		std::shared_ptr<AssetManager> Assets;
		EntityTextureKey TextureKey;

		Texture2D Image;

		float ImageIndex = 0.0f;
		float AnimationSpeed;
};
