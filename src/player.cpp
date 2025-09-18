#include "player.hpp"
#include "assetManager.hpp"
#include "raylib.h"

#include <iostream>
#include <cmath>

Player::Player(float pos_x, float pos_y, AssetManager &assets) :
	Assets(assets), UpdatableGameObject(pos_x, pos_y, assets.EntityTextures[EntityTextureKey::PlayerSouth][0])
{}

Player::~Player()
{}

void Player::Update()
{
	Player::MoveX();
	Player::MoveY();
	Player::SetCurrentTextures();
	Player::Animate();
}

void Player::Draw()
{
	// NOTE TO SELF, change the alpha value of the tint to flash white
	DrawTexture(this->Image, (int) this->Rect.x, (int) this->Rect.y, WHITE); 
}

void Player::Animate()
{
	if (this->Direction.x != 0.0f || this->Direction.y != 0.0f)
		this->ImageIndex +=  this->AnimationSpeed * GetFrameTime();
	else
		this->ImageIndex = 0;

	this->Image = this->Assets.EntityTextures[this->CurrentTextures][(int) this->ImageIndex % this->Assets.EntityTextures[this->CurrentTextures].size()];
}

void Player::MoveX()
{
	this->Rect.x += this->Speed * this->Direction.x * GetFrameTime();
}

void Player::MoveY()
{
	this->Rect.y += this->Speed * this->Direction.y * GetFrameTime();
}

void Player::SetCurrentTextures()
{
	if (this->Direction.x > 0)
		this->CurrentTextures = EntityTextureKey::PlayerEast;
	else if (this->Direction.x < 0)
		this->CurrentTextures = EntityTextureKey::PlayerWest;
	else if (this->Direction.y > 0)
		this->CurrentTextures = EntityTextureKey::PlayerSouth;
	else if (this->Direction.y < 0)
		this->CurrentTextures = EntityTextureKey::PlayerNorth;
}
