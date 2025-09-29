#include "player.hpp"
#include "assetManager.hpp"
#include "raylib.h"

#include <cmath>

Player::Player(float pos_x, float pos_y, AssetManager &assets) :
	Assets(assets), Image(assets.EntityTextures[EntityTextureKey::PlayerSouth][0]), Speed(300)
{
	this->Rect = {pos_x, pos_y, (float) this->Image.width, (float) this->Image.height};
}

Player::~Player()
{}

void Player::Update()
{
	Player::SetCurrentTextures();
	Player::Animate();

	this->Rect.width = this->Image.width;
	this->Rect.height = this->Image.height;
}

void Player::Draw() const
{
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

void Player::MoveX()
{
	this->Rect.x += this->Speed * this->Direction.x * GetFrameTime();
}

void Player::MoveY()
{
	this->Rect.y += this->Speed * this->Direction.y * GetFrameTime();
}
