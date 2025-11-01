#include "player.hpp"
#include "assetManager.hpp"
#include "raylib.h"
#include "constants.hpp"


Player::Player(float pos_x, float pos_y, AssetManager &assets) :
	Assets(assets),
	Image(assets.Textures[TextureKey::PlayerSouth])
{
	this->Rect = { pos_x, pos_y, PLAYER_TEXTURE_TILE_WIDTH, PLAYER_TEXTURE_TILE_HEIGHT };
	this->Centre = { this->Rect.x + PLAYER_TEXTURE_TILE_WIDTH / 2.0f, this->Rect.y + PLAYER_TEXTURE_TILE_HEIGHT / 2.0f };
	this->Aura = { 0 };
}

void Player::Update(size_t ticks) noexcept
{
	Player::SetCurrentTextures();
	Player::Animate(ticks);

	if (ticks >= this->SlideExpire)
		this->Sliding = false;

	Player::Move();
}

void Player::Draw() const noexcept
{
	DrawTextureRec(
			this->Image,
			(Rectangle) { (float) this->ImageIndex * PLAYER_TEXTURE_TILE_WIDTH, 0, PLAYER_TEXTURE_TILE_WIDTH, PLAYER_TEXTURE_TILE_HEIGHT },
			(Vector2) { this->Rect.x, this->Rect.y },
			WHITE
			);
}

void Player::DrawLightmap() const noexcept
{
	DrawCircleGradient(this->Centre.x, this->Centre.y, 128, WHITE, LIGHTGRAY);
}

void Player::Animate(size_t ticks) noexcept
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

void Player::SetCurrentTextures() noexcept
{
	if (this->Direction.x > 0)
		this->CurrentTextures = TextureKey::PlayerEast;
	else if (this->Direction.x < 0)
		this->CurrentTextures = TextureKey::PlayerWest;
	else if (this->Direction.y > 0)
		this->CurrentTextures = TextureKey::PlayerSouth;
	else if (this->Direction.y < 0)
		this->CurrentTextures = TextureKey::PlayerNorth;

	this->Image = this->Assets.Textures.at(this->CurrentTextures);
}

void Player::Move() noexcept
{
	float speed = this->Speed;

	if (this->Sliding)
		speed *= 4.0f;

	this->Rect.x += speed * this->Direction.x * TICK_TIME;
	this->Rect.y += speed * this->Direction.y * TICK_TIME;

	this->Centre = { this->Rect.x + PLAYER_TEXTURE_TILE_WIDTH / 2.0f, this->Rect.y + PLAYER_TEXTURE_TILE_HEIGHT / 2.0f };

	this->Aura.x = this->Centre.x - this->Aura.width / 2.0f;
	this->Aura.y = this->Centre.y - this->Aura.height / 2.0f;
}

void Player::IncreaseHealth(float addition) noexcept
{
	if (this->Health + addition <= this->HealthMax)
		this->Health += addition;
	else
		this->Health = this->HealthMax;
}
