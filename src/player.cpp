#include "player.hpp"

#include <cstdint>
#include <cmath>

#include "assetManager.hpp"
#include "raylib.h"
#include "constants.hpp"

#include "messageSystem.hpp"
#include "commands.hpp"
#include "signals.hpp"

Player::Player(float pos_x, float pos_y, AssetManager &assets) :
	Image(assets.GetTexture(TextureKey::Player))
{
	this->Rect = { pos_x, pos_y, PLAYER_TEXTURE_TILE_SIZE, PLAYER_TEXTURE_TILE_SIZE };
	this->Centre = { this->Rect.x + PLAYER_TEXTURE_TILE_SIZE / 2.0f, this->Rect.y + PLAYER_TEXTURE_TILE_SIZE / 2.0f };
}

void Player::PollSignals(MessageSystem& message_system) noexcept
{
	for (size_t i = 0; i < static_cast<size_t>(PlayerSignal::COUNT); i++)
	{
		const uint16_t times = static_cast<uint16_t>(message_system.PlayerSignals[i]);
		
		if (times > 0)
		{
			auto signal_handler = this->SignalHandlers[i];
			(this->*signal_handler)(times);

			message_system.PlayerSignals[i] = 0;
		}
	}
}

void Player::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto const& command : message_system.PlayerCommands)
	{
		this->TakeDamage(command);
	}
	message_system.PlayerCommands.clear();
}

void Player::Update(MessageSystem& message_system, const size_t ticks, const float slide_speed) noexcept
{
	Player::SetBearing();
	Player::Animate(ticks);

	if (ticks >= this->SlideExpire)
		this->Sliding = false;

	Player::Move(message_system, slide_speed);
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

void Player::Animate(const size_t ticks) noexcept
{
	if (this->Direction.x == 0.0f && this->Direction.y == 0.0f)
	{
		this->ImageIndex = 0;
		this->LastAnimationUpdate = ticks;
		return;
	}

	if (ticks - this->LastAnimationUpdate >= this->AnimationInterval)
	{
		this->ImageIndex++;
		this->LastAnimationUpdate = ticks;
		this->ImageIndex %= this->AnimationFrames;
	}
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

void Player::Move(MessageSystem& message_system, const float slide_speed) noexcept
{
	float speed = this->Speed;

	if (this->Sliding)
		speed *= slide_speed;

	this->Rect.x += speed * this->Direction.x * TICK_TIME;
	this->Rect.y += speed * this->Direction.y * TICK_TIME;

	message_system.StatSystemCommands.emplace_back(Stat::TotalDistance, static_cast<uint32_t>(speed * TICK_TIME));

	this->Centre = { this->Rect.x + PLAYER_TEXTURE_TILE_SIZE / 2.0f, this->Rect.y + PLAYER_TEXTURE_TILE_SIZE / 2.0f };
}

void Player::IncreaseHealth(const float addition) noexcept
{
	if (this->Health + addition <= this->HealthMax)
		this->Health += addition;
	else
		this->Health = this->HealthMax;
}

void Player::TakeDamage(const PlayerCommand& command) noexcept
{
	const DamagePlayer& data = std::get<DamagePlayer>(command);
	this->Health -= data.DamageAmount;
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

void Player::IncreasePlotArmour(const uint16_t times) noexcept
{
	this->HealthMax *= pow(2.0, static_cast<float>(times));
	this->Health = this->HealthMax;
}

void Player::ApplySpeedBoots(const uint16_t times) noexcept
{
	this->Speed *= pow(1.2, static_cast<float>(times));
}
