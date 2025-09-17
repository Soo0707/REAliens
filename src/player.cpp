#include "player.hpp"

#include "raylib.h"

Player::Player(float pos_x, float pos_y, Texture2D initial_image) : UpdatableGameObject(pos_x, pos_y, initial_image)
{

}

Player::~Player()
{

}

void Player::Update()
{

}

void Player::Draw()
{
	// NOTE TO SELF, change the alpha value of the tint to flash white
	DrawTexture(this->image, (int) this->rect.x, (int) this->rect.y, WHITE); 
}

void Player::Animate()
{

}
