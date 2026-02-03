#include "player.hpp"
#include <cstdint>
#include <cmath>

#include "assetManager.hpp"
#include "raylib.h"
#include "raymath.h"
#include "constants.hpp"

#include "modifierSystem.hpp"
#include "projectileData.hpp"
#include "messageSystem.hpp"
#include "commands.hpp"
#include "signals.hpp"

Player::Player(float pos_x, float pos_y, AssetManager &assets) :
	Image(assets.GetTexture(TextureKey::Player))
{
	this->Rect = { pos_x, pos_y, PLAYER_TEXTURE_TILE_SIZE, PLAYER_TEXTURE_TILE_SIZE };
	this->Centre = { this->Rect.x + PLAYER_TEXTURE_TILE_SIZE / 2.0f, this->Rect.y + PLAYER_TEXTURE_TILE_SIZE / 2.0f };
}

void Player::PollSignals(MessageSystem& message_system, const ModifierSystem& modifier_system) noexcept
{
	for (size_t i = 0; i < static_cast<size_t>(PlayerSignal::COUNT); i++)
	{
		const uint16_t times = static_cast<uint16_t>(message_system.PlayerSignals[i]);
		
		if (times > 0)
		{
			auto signal_handler = this->SignalHandlers[i];
			(this->*signal_handler)(message_system, times, modifier_system);

			message_system.PlayerSignals[i] = 0;
		}
	}
}

void Player::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto const& command : message_system.PlayerCommands)
	{
		const size_t handler_index = command.index();

		auto command_handler = this->CommandHandlers[handler_index];

		(this->*command_handler)(command);
	}

	message_system.PlayerCommands.clear();
}

void Player::Update(MessageSystem& message_system, const size_t ticks, const float slide_speed) noexcept
{
	Player::SetBearing();
	Player::Animate(ticks);

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

void Player::TakeDamage(const PlayerCommand& command) noexcept
{
	const DamagePlayer& data = std::get<DamagePlayer>(command);
	this->Health -= data.DamageAmount;
}

void Player::IncreaseHealth(const PlayerCommand& command) noexcept
{
	const IncreasePlayerHealth& data = std::get<IncreasePlayerHealth>(command);

	if (this->Health + data.Amount <= this->HealthMax)
		this->Health += data.Amount;
	else
		this->Health = this->HealthMax;
}

void Player::SetDirection(const PlayerCommand& command) noexcept
{
	const SetPlayerDirection& data = std::get<SetPlayerDirection>(command);

	this->Direction = Vector2Normalize(data.Direction);
}

void Player::Reset() noexcept
{
	this->Rect.x = 500;
	this->Rect.y = 500;

	this->Health = 100;
	this->HealthMax = 100;
	this->Direction = { 0.0f, 0.0f };

	this->Speed = 300.0f;
	this->Sliding = false;

	this->Radius = 10.0f;
	this->Bearing = Bearing::South;

	this->LastAnimationUpdate = 0;
	this->ImageIndex = 0;
}

void Player::IncreasePlotArmour(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept
{
	this->HealthMax *= pow(2.0, static_cast<float>(times));
	this->Health = this->HealthMax;
}

void Player::ApplySpeedBoots(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept
{
	this->Speed *= pow(1.2, static_cast<float>(times));
}

void Player::ApplySlide(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept
{
	this->Sliding = true;
}

void Player::RemoveSlide(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept
{
	this->Sliding = false;
}

void Player::SpawnBall(MessageSystem& message_system, const uint16_t times, const ModifierSystem& modifier_system) noexcept
{
	Vector2 ball_direction = { 1.0f, 0.0f };

	ball_direction = Vector2Rotate(ball_direction, static_cast<float>(GetRandomValue(0, 359)) * TO_RAD);

	const Vector2 ball_location = this->Centre;
	const float ball_speed = modifier_system.GetAttribute(Attribute::BallSpeed);
	const float ball_scale = modifier_system.GetAttribute(Attribute::BallScale);

	message_system.ProjectileSystemCommands.emplace_back(
			std::in_place_type<struct CreateProjectile>, ProjectileType::Ball,
			ball_direction, ball_location.x, ball_location.y, ball_speed, ball_scale
			);
}
