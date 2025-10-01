#include "enemy.hpp"
#include "assetManager.hpp"
#include "raylib.h"
#include "raymath.h"

#include <cstddef>

Enemy::Enemy(float pos_x, float pos_y, std::shared_ptr<AssetManager> assets, EnemyType type, UniqueStates state) :
	Assets(assets), Type(type), UniqueState(state)
{
	this->Rect = { pos_x, pos_y, (float) this->Image.width, (float) this->Image.height };

	switch (type)
	{
		case EnemyType::Australian:
		{
			this->TextureKey = EntityTextureKey::Australian;
			this->Damage = 5;
			this->Speed = 175;
			this->AnimationSpeed = 10.0f;
			this->Health = 75;

			this->CanSecondary = false;
			this->LastSecondary = -1;
			this->SecondaryCooldown = -1;

			break;
		}
		case EnemyType::BigMan:
		{
			this->TextureKey = EntityTextureKey::BigMan;
			this->Damage = 50;
			this->Speed = 175;
			this->AnimationSpeed = 10.0f;
			this->Health = 500;

			this->CanSecondary = false;
			this->LastSecondary = -1;
			this->SecondaryCooldown = -1;
			break;
		}
		case EnemyType::Bomber:
		{
			this->TextureKey = EntityTextureKey::Bomber;
			this->Damage = 15;
			this->Speed = 350;
			this->AnimationSpeed = 10.0f;
			this->Health = 100;

			this->CanSecondary = false;
			this->LastSecondary = -1;
			this->SecondaryCooldown = -1;

			break;
		}
		case EnemyType::Drunkard:
		{
			this->TextureKey = EntityTextureKey::Drunkard;
			this->Damage = 5;
			this->Speed = 250;
			this->AnimationSpeed = 60.0f;
			this->Health = 50;

			this->CanSecondary = true;
			this->LastSecondary = 0;
			this->SecondaryCooldown = 5000;

			break;
		}
		case EnemyType::Pleb:
		{
			this->TextureKey = EntityTextureKey::Pleb;

			this->Damage = 5;
			this->Speed = 150;
			this->AnimationSpeed = 10.0f;
			this->Health = 75;

			this->CanSecondary = false;
			this->LastSecondary = -1;
			this->SecondaryCooldown = -1;

			break;
		}
		case EnemyType::Poison:
		{
			this->TextureKey = EntityTextureKey::Poison;
			this->Damage = 5;
			this->Speed = 225;
			this->AnimationSpeed = 60;
			this->Health = 100;

			this->CanSecondary = false;
			this->LastSecondary = -1;
			this->SecondaryCooldown = -1;

			break;
		}
		case EnemyType::Trapper:
		{
			this->TextureKey = EntityTextureKey::Trapper;
			this->Damage = 10;
			this->Speed = 430;
			this->AnimationSpeed = 50.0f;
			this->Health = 100;

			this->CanSecondary = false;
			this->LastSecondary = -1;
			this->SecondaryCooldown = -1;

			break;
		}
	}

	this->Image = this->Assets->EntityTextures[this->TextureKey][0];
}

Enemy::~Enemy()
{}

void Enemy::Update(Rectangle& player_rect, size_t& ticks)
{
	Enemy::Animate();
	Enemy::Move();

	if (this->UniqueState != UniqueStates::OverrideDirection)
		Enemy::SetDirection(player_rect);

	if (ticks - this->LastLeAttack >= this->LeAttackCooldown)
		this->CanLeAttack = true;

	if (this->SecondaryCooldown > 0 && (ticks - this->LastSecondary >= this->SecondaryCooldown))
		this->CanSecondary = true;

	if (ticks - this->FlashTriggered >= FLASH_DURATION)
		this->Flash = false;
}

void Enemy::Draw()
{
	if (this->Flash)
	{
		BeginBlendMode(BLEND_ADDITIVE);
		DrawTexture(this->Image, (int) this->Rect.x, (int) this->Rect.y, WHITE);
		EndBlendMode();
	}
	else
		DrawTexture(this->Image, (int) this->Rect.x, (int) this->Rect.y, WHITE); 
}

void Enemy::Animate()
{
	if (this->Direction.x != 0.0f || this->Direction.y != 0.0f)
		this->ImageIndex +=  this->AnimationSpeed * GetFrameTime();
	else
		this->ImageIndex = 0;

	this->Image = this->Assets->EntityTextures[this->TextureKey][(int) this->ImageIndex % this->Assets->EntityTextures[this->TextureKey].size()];
}

void Enemy::Move()
{
	this->Rect.x = this->Rect.x + this->Speed * this->Direction.x * GetFrameTime();
	this->Rect.y = this->Rect.y + this->Speed * this->Direction.y * GetFrameTime();
	this->Rect.width = this->Image.width;
	this->Rect.height = this->Image.height;
}

void Enemy::SecondaryAttack()
{}

void Enemy::SetDirection(Rectangle& player_rect)
{
	this->Direction.x = player_rect.x - this->Rect.x;
	this->Direction.y = player_rect.y - this->Rect.y;

	float distance = Vector2Length(this->Direction);

	if ((this->Direction.x != 0.0f || this->Direction.y != 0.0f) && distance >= 32)
		this->Direction = Vector2Normalize(this->Direction);
	else
		this->Direction = Vector2 { 0.0f, 0.0f };
}
