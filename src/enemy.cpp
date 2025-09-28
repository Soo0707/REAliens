#include "enemy.hpp"
#include "assetManager.hpp"
#include "raylib.h"
#include "raymath.h"

Enemy::Enemy(float pos_x, float pos_y, AssetManager &assets, EntityTextureKey texture_key, EnemyType type) :
	Assets(assets), Image(assets.EntityTextures[texture_key][0]), TextureKey(texture_key), Type(type), UniqueState(UniqueStates::None)
{
	this->Rect = {pos_x, pos_y, (float) this->Image.width, (float) this->Image.height};

	switch (type)
	{
		case EnemyType::Australian:
		{
			this->Damage = 5;
			this->Speed = 350;
			this->AnimationSpeed = 10.0f;
			this->Health = 75;
			break;
		}
		case EnemyType::BigMan:
		{
			this->Damage = 50;
			this->Speed = 150;
			this->AnimationSpeed = 10.0f;
			this->Health = 500;
			break;
		}
		case EnemyType::Bomber:
		{
			this->Damage = 15;
			this->Speed = 700;
			this->AnimationSpeed = 10.0f;
			this->Health = 100;
			break;
		}
		case EnemyType::Drunkard:
		{
			this->Damage = 5;
			this->Speed = 500;
			this->AnimationSpeed = 60.0f;
			this->Health = 50;
			break;
		}
		case EnemyType::Pleb:
		{
			this->Damage = 5;
			this->Speed = 300;
			this->AnimationSpeed = 10.0f;
			this->Health = 75;
			break;
		}
		case EnemyType::Poison:
		{
			this->Damage = 5;
			this->Speed = 450;
			this->AnimationSpeed = 60;
			this->Health = 100;
			break;
		}
		case EnemyType::Trapper:
		{
			this->Damage = 10;
			this->Speed = 430;
			this->AnimationSpeed = 50.0f;
			this->Health = 100;
			break;
		}
	}
}

Enemy::~Enemy()
{}

void Enemy::Update()
{
	Enemy::Animate();
}

void Enemy::Draw()
{
	if (this->Flash)
	{
		DrawTexture(this->Image, (int) this->Rect.x, (int) this->Rect.y, {255, 255, 255, 0});
		this->Flash = false;
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

	this->Image = this->Assets.EntityTextures[this->TextureKey][(int) this->ImageIndex % this->Assets.EntityTextures[this->TextureKey].size()];
}

void Enemy::MoveX()
{
	this->NextRect.x = this->Rect.x + this->Speed * this->Direction.x * GetFrameTime();
	this->NextRect.width = this->Rect.width;
}

void Enemy::MoveY()
{
	this->NextRect.y = this->Rect.y + this->Speed * this->Direction.y * GetFrameTime();
	this->NextRect.height = this->Rect.height;
}

void Enemy::SecondaryAttack()
{}

void Enemy::SetDirection(Rectangle& player_rect)
{
	this->Direction.x = player_rect.x - this->Rect.x;
	this->Direction.y = player_rect.y - this->Rect.y;

	if (this->Direction.x != 0.0f || this->Direction.y != 0.0f)
		this->Direction = Vector2Normalize(this->Direction);
}
