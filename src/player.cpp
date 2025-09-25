#include "player.hpp"
#include "assetManager.hpp"
#include "raylib.h"

#include <cmath>

Player::Player(float pos_x, float pos_y, AssetManager &assets) :
	Assets(assets), MovableGameObject(pos_x, pos_y, assets.EntityTextures[EntityTextureKey::PlayerSouth][0])
{
	this->Speed = 300;
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
