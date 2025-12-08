#include "enemy.hpp"
#include "enemyData.hpp"

#include "assetManager.hpp"
#include "raylib.h"
#include "raymath.h"
#include "constants.hpp"

#include <cstddef>
#include <array>

Enemy::Enemy(float pos_x, float pos_y, float level_scale, int layer, AssetManager& assets, EnemyType type, BehaviourModifier modifier) noexcept :
	Type(type),
	Modifiers(modifier),
	Rect({ pos_x, pos_y, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE }),
	Layer(layer)
{
	size_t type_index = static_cast<size_t>(type);

	this->Speed = EnemyAttributes[type_index].speed;

	this->AnimationSpeed = EnemyAttributes[type_index].animation_speed;
	this->AnimationFrames = EnemyAttributes[type_index].animation_frames;

	this->Health = EnemyAttributes[type_index].health * level_scale;

	this->Image = assets.Textures.at(EnemyAttributes[type_index].texture_key);


	if ((this->Modifiers & BehaviourModifier::Big) == BehaviourModifier::Big)
	{
		int scale = GetRandomValue(2, 5);

		this->Rect.width *= scale;
		this->Rect.height *= scale;

		this->Speed *= (1 / scale);
		this->Health *= scale;

		this->AnimationSpeed *= scale;
		this->Scale = scale;
	}
	else
		this->Scale = 1;

	if ((this->Modifiers & BehaviourModifier::IncreasedSpeed) == BehaviourModifier::IncreasedSpeed)
	{
		float scale = static_cast<float>(GetRandomValue(200, 300)) / 100.0f;

		this->Speed *= scale;
		this->AnimationSpeed *= (1 / scale);
	}
}

void Enemy::Update(Vector2& player_centre, size_t ticks, bool is_stinky) noexcept
{
	Enemy::Animate(ticks);

	Enemy::Move();

	Enemy::SetDirection(player_centre, is_stinky);

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

void Enemy::SetDirection(Vector2& player_centre, bool is_stinky) noexcept
{
	this->Direction.x = player_centre.x - this->Rect.x;
	this->Direction.y = player_centre.y - this->Rect.y;

	if (is_stinky)
		this->Direction = Vector2Scale(this->Direction, -1.0f);

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
