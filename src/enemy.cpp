#include "enemy.hpp"
#include "enemyData.hpp"

#include "assetManager.hpp"
#include "raylib.h"
#include "raymath.h"
#include "assetManager.hpp"
#include "constants.hpp"

#include <cstddef>

Enemy::Enemy(float pos_x, float pos_y, std::shared_ptr<AssetManager> assets, EnemyType type, BehaviourModifier modifier) :
	Assets(assets),
	Type(type),
	Modifiers(modifier),
	LeAttackCooldown(SECONDS_TO_TICKS(3)),
	Rect({ pos_x, pos_y, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE })
{
	this->TextureKey = EnemyAttributes.at(this->Type).texture_key;
	this->Speed = EnemyAttributes.at(this->Type).speed;

	this->AnimationSpeed = EnemyAttributes.at(this->Type).animation_speed;
	this->AnimationFrames = EnemyAttributes.at(this->Type).animation_frames;

	this->Health = EnemyAttributes.at(this->Type).health;
	this->Image = this->Assets->Textures.at(this->TextureKey);


	if (type == EnemyType::Trapper)
		this->Modifiers = this->Modifiers | BehaviourModifier::NoMovement;


	if ((modifier & BehaviourModifier::OverrideDirection) == BehaviourModifier::OverrideDirection)
	{
		Vector2 random_direction = { 1, 1 };

		if (GetRandomValue(0, 50) % 2)
			random_direction.x *= -1;
		else
			random_direction.x = 0;

		if (GetRandomValue(0, 50) % 2)
			random_direction.y *= -1;
		else if (random_direction.x != 0)
			random_direction.y = 0;

		this->Direction = Vector2Normalize(random_direction);
	}

	if ((this->Modifiers & BehaviourModifier::Big) == BehaviourModifier::Big)
	{
		float scale = static_cast<float>(GetRandomValue(2, 5));

		this->Rect.width *= scale;
		this->Rect.height *= scale;
		this->Speed *= (1 / scale);
		this->Health *= scale;
		this->AnimationSpeed *= scale;

		this->Scale = scale;
	}
	else
		this->Scale = 1.0f;

	if ((this->Modifiers & BehaviourModifier::IncreasedSpeed) == BehaviourModifier::IncreasedSpeed)
	{
		float scale = static_cast<float>(GetRandomValue(150, 200)) / 100.0f;

		this->Speed *= scale;
		this->AnimationSpeed *= scale;
	}
}

Enemy::~Enemy()
{}

void Enemy::Update(Rectangle& player_rect, size_t ticks)
{
	Enemy::Animate(ticks);

	if ((this->Modifiers & BehaviourModifier::NoMovement) == BehaviourModifier::None)
		Enemy::Move();

	if ((this->Modifiers & BehaviourModifier::OverrideDirection) == BehaviourModifier::None)
		Enemy::SetDirection(player_rect);


	if (ticks - this->LastLeAttack >= this->LeAttackCooldown)
		this->CanLeAttack = true;

	if (ticks - this->FlashTriggered >= FLASH_DURATION)
		this->Flash = false;
}

void Enemy::Draw()
{
	DrawTexturePro(
			this->Image,
			(Rectangle) { (float) this->ImageIndex * TEXTURE_TILE_SIZE, 0, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE },
			this->Rect,
			(Vector2) { 0.0f, 0.0f },
			0.0f,
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
