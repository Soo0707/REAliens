#include "enemy.hpp"
#include "enemyData.hpp"

#include "assetManager.hpp"
#include "raylib.h"
#include "raymath.h"
#include "game.hpp"
#include "assetManager.hpp"

#include <cstddef>

Enemy::Enemy(float pos_x, float pos_y, std::shared_ptr<AssetManager> assets, EnemyType type, BehaviourModifier modifier) :
	Assets(assets), Type(type), Modifier(modifier)
{
	this->TextureKey = EnemyAttributes.at(this->Type).texture_key;
	this->Speed = EnemyAttributes.at(this->Type).speed;
	this->AnimationSpeed = EnemyAttributes.at(this->Type).animation_speed;
	this->Health = EnemyAttributes.at(this->Type).health;

	this->CanSecondary = EnemyAttributes.at(this->Type).can_secondary;
	this->LastSecondary = EnemyAttributes.at(this->Type).last_secondary;
	this->SecondaryCooldown = EnemyAttributes.at(this->Type).secondary_cooldown;

	this->Image = this->Assets->EntityTextures.at(this->TextureKey)[0];
	this->Rect = { pos_x, pos_y, (float) this->Image.width, (float) this->Image.height };
}

Enemy::~Enemy()
{}

void Enemy::Update(Rectangle& player_rect, size_t& ticks)
{
	Enemy::Move();

	if (this->Modifier != BehaviourModifier::OverrideDirection)
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
	DrawTextureV(this->Image, (Vector2) { this->Rect.x, this->Rect.y }, WHITE);

	if (this->Flash)
	{
		BeginBlendMode(BLEND_ADDITIVE);
		DrawTextureV(this->Image, (Vector2) { this->Rect.x, this->Rect.y }, WHITE);
		EndBlendMode();
	}
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
	this->Rect.x += this->Speed * this->Direction.x * TICK_TIME;
	this->Rect.y += this->Speed * this->Direction.y * TICK_TIME;
	this->Rect.width = this->Image.width;
	this->Rect.height = this->Image.height;
}

void Enemy::SecondaryAttack()
{}

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
