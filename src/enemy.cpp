#include "enemy.hpp"
#include "enemyData.hpp"

#include "assetManager.hpp"
#include "raylib.h"
#include "raymath.h"
#include "constants.hpp"

#include <cstddef>

Enemy::Enemy(float pos_x, float pos_y, int layer, AssetManager& assets, EnemyType type, BehaviourModifier modifier) noexcept :
	Type(type),
	Modifiers(modifier),
	LeAttackCooldown(SECONDS_TO_TICKS(3)),
	Rect({ pos_x, pos_y, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE }),
	Layer(layer)
{
	this->Speed = EnemyAttributes.at(this->Type).speed;

	this->AnimationSpeed = EnemyAttributes.at(this->Type).animation_speed;
	this->AnimationFrames = EnemyAttributes.at(this->Type).animation_frames;

	this->Health = EnemyAttributes.at(this->Type).health;

	this->Image = assets.Textures.at(EnemyAttributes.at(this->Type).texture_key);


	if ((modifier & BehaviourModifier::OverrideDirection) == BehaviourModifier::OverrideDirection)
	{
		Vector2 random_direction;

		switch (GetRandomValue(0, 4))
		{
			case 0:
				{
					random_direction = {1, 0};
				}
				break;
			case 1:
				{
					random_direction = {0, 1};
				}
				break;
			case 2:
				{
					random_direction = {-1, 0};
				}
				break;
			case 3:
				{
					random_direction = {0, -1};
				}
				break;
		}

		this->Direction = random_direction;
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
		float scale = static_cast<float>(GetRandomValue(200, 300)) / 100.0f;

		this->Speed *= scale;
		this->AnimationSpeed *= (1 / scale);
	}
}

void Enemy::Update(Vector2& player_centre, size_t ticks) noexcept
{
	Enemy::Animate(ticks);

	Enemy::Move();

	if ((this->Modifiers & BehaviourModifier::OverrideDirection) == BehaviourModifier::None)
		Enemy::SetDirection(player_centre);

	if (ticks - this->LastLeAttack >= this->LeAttackCooldown)
		this->CanLeAttack = true;

	if (ticks - this->FlashTriggered >= FLASH_DURATION)
		this->Flash = false;
}

void Enemy::Draw() const noexcept
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

void Enemy::Animate(size_t ticks) noexcept
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

void Enemy::Move() noexcept
{
	this->Rect.x += this->Speed * this->Direction.x * TICK_TIME;
	this->Rect.y += this->Speed * this->Direction.y * TICK_TIME;
}

void Enemy::SetDirection(Vector2& player_centre) noexcept
{
	this->Direction.x = player_centre.x - this->Rect.x;
	this->Direction.y = player_centre.y - this->Rect.y;

	if ((this->Direction.x != 0.0f || this->Direction.y != 0.0f))
		this->Direction = Vector2Normalize(this->Direction);
	else
		this->Direction = Vector2 { 0.0f, 0.0f };
}

void Enemy::FlashSprite(size_t ticks) noexcept
{
	this->Flash = true;
	this->FlashTriggered = ticks;
}
