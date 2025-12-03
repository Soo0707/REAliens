#include "player.hpp"
#include "assetManager.hpp"
#include "raylib.h"
#include "constants.hpp"


Player::Player(float pos_x, float pos_y, AssetManager &assets) :
	Image(assets.Textures.at(TextureKey::Player))
{
	this->Rect = { pos_x, pos_y, PLAYER_TEXTURE_TILE_SIZE, PLAYER_TEXTURE_TILE_SIZE };
	this->Centre = { this->Rect.x + PLAYER_TEXTURE_TILE_SIZE / 2.0f, this->Rect.y + PLAYER_TEXTURE_TILE_SIZE / 2.0f };
}

void Player::Update(size_t ticks, size_t* total_distance_moved, float slide_speed) noexcept
{
	Player::SetBearing();
	Player::Animate(ticks);

	if (ticks >= this->SlideExpire)
		this->Sliding = false;

	Player::Move(total_distance_moved, slide_speed);
}

void Player::Draw() const noexcept
{
	DrawTextureRec(
			this->Image,
			(Rectangle) { 
				static_cast<float>(this->ImageIndex * PLAYER_TEXTURE_TILE_SIZE),
				static_cast<float>(this->Bearing),
				PLAYER_TEXTURE_TILE_SIZE,
				PLAYER_TEXTURE_TILE_SIZE
			},
			(Vector2) { this->Rect.x, this->Rect.y },
			WHITE
			);
}

void Player::DrawLightmap() const noexcept
{
	DrawCircleGradient(this->Centre.x, this->Centre.y, 64, WHITE, LIGHTGRAY);
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

void Player::SetBearing() noexcept
{
	if (this->Direction.x > 0)
		this->Bearing = Bearing::East;
	else if (this->Direction.x < 0)
		this->Bearing = Bearing::West;
	else if (this->Direction.y > 0)
		this->Bearing = Bearing::South;
	else if (this->Direction.y < 0)
		this->Bearing = Bearing::North;
}

void Player::Move(size_t* total_distance_moved, float slide_speed) noexcept
{
	float speed = this->Speed;

	if (this->Sliding)
		speed *= slide_speed;

	this->Rect.x += speed * this->Direction.x * TICK_TIME;
	this->Rect.y += speed * this->Direction.y * TICK_TIME;

	*total_distance_moved += speed * TICK_TIME;

	this->Centre = { this->Rect.x + PLAYER_TEXTURE_TILE_SIZE / 2.0f, this->Rect.y + PLAYER_TEXTURE_TILE_SIZE / 2.0f };
}

void Player::IncreaseHealth(float addition) noexcept
{
	if (this->Health + addition <= this->HealthMax)
		this->Health += addition;
	else
		this->Health = this->HealthMax;
}

void Player::Reset() noexcept
{
	this->Rect.x = 500;
	this->Rect.y = 500;

	this->Health = 100;
	this->HealthMax = 100;

	this->Speed = 300;
	this->Sliding = false;
	this->SlideExpire = 0;
}
