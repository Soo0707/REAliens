#include "gameInputSystem.hpp"

#include <variant>

#include "raylib.h"
#include "raymath.h"
#include "settingsManager.hpp"

#include "commands.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"

void GameInputSystem::HandleTickedInput(
		Game& game, MessageSystem& message_system, const ModifierSystem& modifier_system,
		const SettingsManager& settings
		) noexcept
{
	if (IsKeyDown(KEY_SPACE))
		message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::RemoveTrapped);

	if (modifier_system.EffectStatus(Effect::Trapped))
		game.Player->Direction = { 0, 0 };
	else
	{
		game.Player->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
		game.Player->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
	}

	if (modifier_system.EffectStatus(Effect::Drunk))
	{
		game.Player->Direction.x *= -1;
		game.Player->Direction.y *= -1;
	}

	game.Player->Direction = Vector2Normalize(game.Player->Direction);

	const size_t ticks = game.Ticks;

	if (IsKeyDown(KEY_LEFT_SHIFT) && game.CanPerform[static_cast<size_t>(Action::Slide)])
	{
		game.Player->Sliding = true;
		game.Player->SlideExpire = ticks + TICK_RATE / 6;

		game.CanPerform[static_cast<size_t>(Action::Slide)] = false;
		game.LastPerformed[static_cast<size_t>(Action::Slide)] = ticks;
	}

	const bool auto_click = settings.Data.at(SettingKey::AutoClick);

	const Vector2 player_centre = game.Player->Centre;
	const Camera2D camera = game.Camera;

	if ((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || auto_click) && game.CanPerform[static_cast<size_t>(Action::LMB)])
	{
		GameInputSystem::HandleLeftClick(message_system, modifier_system, player_centre, camera);

		game.CanPerform[static_cast<size_t>(Action::LMB)] = false;
		game.LastPerformed[static_cast<size_t>(Action::LMB)] = ticks;
	}
	
	if ((IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || auto_click) && game.CanPerform[static_cast<size_t>(Action::RMB)])
	{
		GameInputSystem::HandleRightClick(message_system, modifier_system, player_centre, camera);

		game.CanPerform[static_cast<size_t>(Action::RMB)] = false;
		game.LastPerformed[static_cast<size_t>(Action::RMB)] = ticks;
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
