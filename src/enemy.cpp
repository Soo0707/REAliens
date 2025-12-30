#include "enemy.hpp"
#include "enemyData.hpp"

#include "assetManager.hpp"
#include "raylib.h"
#include "raymath.h"
#include "constants.hpp"

#include <cstddef>
#include <array>

Enemy::Enemy(float pos_x, float pos_y, float level_scale, const AssetManager& assets, EnemyType type, BehaviourModifier modifier) noexcept :
	Type(type),
	Modifiers(modifier),
	Rect({ pos_x, pos_y, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE })
{
	const size_t type_index = static_cast<size_t>(type);

	this->Speed = EnemyAttributes[type_index].speed;

	this->Damage = EnemyAttributes[type_index].damage * level_scale;

	this->AnimationInterval = EnemyAttributes[type_index].animation_interval;
	this->AnimationFrames = EnemyAttributes[type_index].animation_frames;

	this->Health = EnemyAttributes[type_index].health * level_scale;
	this->Image = assets.Textures.at(EnemyAttributes[type_index].texture_key);

	if ((this->Modifiers & BehaviourModifier::Big) == BehaviourModifier::Big)
	{
		this->Rect.width *= 2;
		this->Rect.height *= 2;

		this->Speed *= (1 / 2);
		this->Health *= 2;
		this->Damage *= level_scale * 2;

		this->AnimationInterval *= 2;
		this->Scale = 2;
	}
	else
		this->Scale = 1;

	if ((this->Modifiers & BehaviourModifier::IncreasedSpeed) == BehaviourModifier::IncreasedSpeed)
	{
		float scale = static_cast<float>(GetRandomValue(125, 150)) / 100.0f;

		this->Speed *= scale;
		this->AnimationInterval *= (1 / scale);
	}
}

void Enemy::Update(Vector2 player_centre, size_t ticks, bool is_stinky) noexcept
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
	if (this->Direction.x == 0.0f && this->Direction.y == 0.0f)
	{
		this->ImageIndex = 0;
		this->LastAnimationUpdate = ticks;
		return;
	}

	if (ticks - this->LastAnimationUpdate >= this->AnimationInterval)
	{
		this->ImageIndex++;
		this->LastAnimationUpdate = ticks;
		this->ImageIndex %= this->AnimationFrames;
	}
}

void Enemy::Move() noexcept
{
	this->Rect.x += this->Speed * this->Direction.x * TICK_TIME;
	this->Rect.y += this->Speed * this->Direction.y * TICK_TIME;
}

void Enemy::SetDirection(Vector2 player_centre, bool is_stinky) noexcept
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
