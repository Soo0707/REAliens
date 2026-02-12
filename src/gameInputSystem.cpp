#include "gameInputSystem.hpp"

#include <variant>

#include "raylib.h"
#include "raymath.h"
#include "settingsManager.hpp"

#include "signals.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"

void GameInputSystem::HandleTickedInput(
		Game& game, MessageSystem& message_system, const ModifierSystem& modifier_system,
		const SettingsManager& settings
		) noexcept
{
	Vector2 player_direction;

	if (IsKeyDown(KEY_SPACE))
		message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveTrapped)]++;

	if (modifier_system.EffectStatus(Effect::Trapped))
		player_direction = { 0.0f, 0.0f };
	else
	{
		player_direction = {
			static_cast<float>(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)),
			static_cast<float>(IsKeyDown(KEY_S) - IsKeyDown(KEY_W))
		};
	}

	if (modifier_system.EffectStatus(Effect::Drunk))
		player_direction = Vector2Scale(player_direction, -1.0f);

	message_system.PlayerCommands.emplace_back(std::in_place_type<struct SetPlayerDirection>, player_direction);

	const size_t ticks = game.Ticks;

	if (IsKeyDown(KEY_LEFT_SHIFT) && game.CanPerform[static_cast<size_t>(Action::Slide)])
	{
		message_system.PlayerSignals[static_cast<size_t>(PlayerSignal::ApplySlide)]++;

		message_system.TimerSystemCommands.emplace_back(
				std::in_place_type<struct RegisterTimer>, TICK_RATE / 6,
				false, Timer::PlayerSlideExpire
				);

		message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct EnableTimer>, false, Timer::Slide);
		game.CanPerform[static_cast<size_t>(Action::Slide)] = false;
	}

	const bool auto_click = settings.Data.at(SettingKey::AutoClick);

	const Vector2 player_centre = game.Player->Centre;
	const Camera2D camera = game.Camera;

	if ((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || auto_click) && game.CanPerform[static_cast<size_t>(Action::LMB)])
	{
		GameInputSystem::HandleLeftClick(message_system, modifier_system, player_centre, camera);

		message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct EnableTimer>, false, Timer::LMB);
		game.CanPerform[static_cast<size_t>(Action::LMB)] = false;
	}
	
	if ((IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || auto_click) && game.CanPerform[static_cast<size_t>(Action::RMB)])
	{
		GameInputSystem::HandleRightClick(message_system, modifier_system, player_centre, camera);

		message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct EnableTimer>, false, Timer::RMB);
		game.CanPerform[static_cast<size_t>(Action::RMB)] = false;
	}
}

void GameInputSystem::HandleLeftClick(
		MessageSystem& message_system, const ModifierSystem& modifier_system,
		const Vector2 player_centre, const Camera2D camera
		) noexcept
{
	const Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);

	const Vector2 centre_direction = Vector2Subtract(mouse_pos, player_centre);

	const float spread_angle = modifier_system.GetAttribute(Attribute::BuckshotSpread);
	const int buckshot = static_cast<int>((modifier_system.GetAttribute(Attribute::Buckshot) - 1) / 2);
	
	const float speed = modifier_system.GetAttribute(Attribute::BulletSpeed);

	for (int i = -buckshot; i <= buckshot; i++)
	{
		if (i == 0)
		{
			message_system.ProjectileSystemCommands.emplace_back(
					std::in_place_type<struct CreateProjectile>, ProjectileType::Bullet, centre_direction,
					player_centre.x, player_centre.y, speed, 1.0f
					);

			continue;
		}

		const Vector2 direction = Vector2Rotate(centre_direction, spread_angle * i);

		message_system.ProjectileSystemCommands.emplace_back(
				std::in_place_type<struct CreateProjectile>, ProjectileType::Bullet, direction,
				player_centre.x, player_centre.y, speed, 1.0f
				);
	}
}

void GameInputSystem::HandleRightClick(
		MessageSystem& message_system, const ModifierSystem& modifier_system,
		const Vector2 player_centre, const Camera2D camera
		) noexcept
{
	const Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);

	const Vector2 direction = Vector2Subtract(mouse_pos, player_centre);

	const float speed = modifier_system.GetAttribute(Attribute::LazerSpeed);
	const float scale = modifier_system.GetAttribute(Attribute::LazerScale);

	message_system.ProjectileSystemCommands.emplace_back(
			std::in_place_type<struct CreateProjectile>, ProjectileType::Lazer, direction,
			player_centre.x, player_centre.y, speed, scale
			);
}
