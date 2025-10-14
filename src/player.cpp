#include "player.hpp"
#include "assetManager.hpp"
#include "raylib.h"
#include "constants.hpp"


Player::Player(float pos_x, float pos_y, AssetManager &assets) :
	Assets(assets), Image(assets.StaticTextures[StaticTextureKey::PlayerSouth])
{
	this->Rect = { pos_x, pos_y, PLAYER_TEXTURE_TILE_WIDTH, PLAYER_TEXTURE_TILE_HEIGHT };
	this->Centre = { this->Rect.x + PLAYER_TEXTURE_TILE_WIDTH / 2.0f, this->Rect.y + PLAYER_TEXTURE_TILE_HEIGHT / 2.0f };
	this->Aura = { 0, 0, 0, 0 };
}

Player::~Player()
{}

void Player::Update(size_t ticks)
{
	Player::SetCurrentTextures();
	Player::Animate(ticks);
	Player::Move();
}

void Player::Draw() const
{
	DrawTextureRec(
			this->Image,
			(Rectangle) { (float) this->ImageIndex * PLAYER_TEXTURE_TILE_WIDTH, 0, PLAYER_TEXTURE_TILE_WIDTH, PLAYER_TEXTURE_TILE_HEIGHT },
			(Vector2) { this->Rect.x, this->Rect.y },
			WHITE
			);
}

void Player::Animate(size_t ticks)
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

void Player::SetCurrentTextures()
{
	if (this->Direction.x > 0)
		this->CurrentTextures = StaticTextureKey::PlayerEast;
	else if (this->Direction.x < 0)
		this->CurrentTextures = StaticTextureKey::PlayerWest;
	else if (this->Direction.y > 0)
		this->CurrentTextures = StaticTextureKey::PlayerSouth;
	else if (this->Direction.y < 0)
		this->CurrentTextures = StaticTextureKey::PlayerNorth;

	this->Image = this->Assets.StaticTextures.at(this->CurrentTextures);
}

void Player::Move()
{
	this->Rect.x += this->Speed * this->Direction.x * TICK_TIME;
	this->Rect.y += this->Speed * this->Direction.y * TICK_TIME;

	this->Centre = { this->Rect.x + PLAYER_TEXTURE_TILE_WIDTH / 2.0f, this->Rect.y + PLAYER_TEXTURE_TILE_HEIGHT / 2.0f };

	this->Aura.x = this->Rect.x - this->Aura.width / 2.0f;
	this->Aura.y = this->Rect.y - this->Aura.height / 2.0f;
}
