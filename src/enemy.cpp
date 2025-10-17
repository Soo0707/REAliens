#include "enemy.hpp"
#include "enemyData.hpp"

#include "assetManager.hpp"
#include "raylib.h"
#include "raymath.h"
#include "assetManager.hpp"
#include "constants.hpp"

#include <cstddef>

Enemy::Enemy(float pos_x, float pos_y, std::shared_ptr<AssetManager> assets, EnemyType type, BehaviourModifier modifier) :
	Assets(assets), Type(type), Modifier(modifier)
{
	this->TextureKey = EnemyAttributes.at(this->Type).texture_key;
	this->Speed = EnemyAttributes.at(this->Type).speed;

	this->AnimationSpeed = EnemyAttributes.at(this->Type).animation_speed;
	this->AnimationFrames = EnemyAttributes.at(this->Type).animation_frames;

	this->Health = EnemyAttributes.at(this->Type).health;
	this->Image = this->Assets->Textures.at(this->TextureKey);

	this->Rect = { 
		pos_x,
		pos_y,
		TEXTURE_TILE_SIZE,
		TEXTURE_TILE_SIZE
	};
}

Enemy::~Enemy()
{}

void Enemy::Update(Rectangle& player_rect, size_t ticks)
{
	Enemy::Animate(ticks);

	if (this->Modifier != BehaviourModifier::NoMovement)
		Enemy::Move();

	if (this->Modifier != BehaviourModifier::OverrideDirection)
		Enemy::SetDirection(player_rect);

	if (ticks - this->LastLeAttack >= this->LeAttackCooldown)
		this->CanLeAttack = true;

	if (ticks - this->FlashTriggered >= FLASH_DURATION)
		this->Flash = false;
}

void Enemy::Draw()
{
	DrawTextureRec(
			this->Image,
			(Rectangle) { (float) this->ImageIndex * TEXTURE_TILE_SIZE, 0, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE },
			(Vector2) { this->Rect.x, this->Rect.y },
			(this->Flash) ? YELLOW : WHITE
			);
}

void Enemy::Animate(size_t ticks)
{
	if (this->Direction.x != 0.0f || this->Direction.y != 0.0f)
	{
		if (ticks - this->LastAnimationUpdate >= this->AnimationSpeed)
		{
			this->ImageIndex++;
			this->LastAnimationUpdate = ticks;
		}
	}
	else
	{
		this->ImageIndex = 0;
		this->LastAnimationUpdate = ticks;
	}

	this->ImageIndex %= this->AnimationFrames;
}

void Enemy::Move()
{
	this->Rect.x += this->Speed * this->Direction.x * TICK_TIME;
	this->Rect.y += this->Speed * this->Direction.y * TICK_TIME;
}

void Enemy::SetDirection(Rectangle& player_rect)
{
	this->Direction.x = player_rect.x - this->Rect.x;
	this->Direction.y = player_rect.y - this->Rect.y;

	if ((this->Direction.x != 0.0f || this->Direction.y != 0.0f))
		this->Direction = Vector2Normalize(this->Direction);
	else
		this->Direction = Vector2 { 0.0f, 0.0f };
}

void Enemy::FlashSprite(size_t ticks)
{
	this->Flash = true;
	this->FlashTriggered = ticks;
}
