#include "gameInputSystem.hpp"

#include "raylib.h"
#include "raymath.h"
#include "gameState.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"
#include "constants.hpp"

void GameInputSystem::HandleTickedInput(GameState& game_state, const SettingsManager& settings, const AssetManager& assets) noexcept
{
	if (IsKeyDown(KEY_SPACE))
		game_state.Effects.erase(Effect::Trapped);

	if (game_state.Effects.count(Effect::Trapped))
		game_state.Player->Direction = { 0, 0 };
	else
	{
		game_state.Player->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
		game_state.Player->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
	}

	if (game_state.Effects.count(Effect::Drunk))
	{
		game_state.Player->Direction.x *= -1;
		game_state.Player->Direction.y *= -1;
	}

	game_state.Player->Direction = Vector2Normalize(game_state.Player->Direction);

	const size_t ticks = game_state.Ticks;

	if (IsKeyDown(KEY_LEFT_SHIFT) && game_state.CanPerform[static_cast<size_t>(Action::Slide)])
	{
		game_state.Player->Sliding = true;
		game_state.Player->SlideExpire = ticks + TICK_RATE / 6;

		game_state.CanPerform[static_cast<size_t>(Action::Slide)] = false;
		game_state.LastPerformed[static_cast<size_t>(Action::Slide)] = ticks;
	}

	const bool auto_click = settings.Data.at(SettingKey::AutoClick);

	if ((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || auto_click) && game_state.CanPerform[static_cast<size_t>(Action::LMB)])
	{
		GameInputSystem::HandleLeftClick(game_state, assets);

		game_state.CanPerform[static_cast<size_t>(Action::LMB)] = false;
		game_state.LastPerformed[static_cast<size_t>(Action::LMB)] = ticks;
	}
	
	if ((IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || auto_click) && game_state.CanPerform[static_cast<size_t>(Action::RMB)])
	{
		GameInputSystem::HandleRightClick(game_state, assets);

		game_state.CanPerform[static_cast<size_t>(Action::RMB)] = false;
		game_state.LastPerformed[static_cast<size_t>(Action::RMB)] = ticks;
	}
}

void GameInputSystem::HandleLeftClick(GameState& game_state, const AssetManager& assets) noexcept
{
	const Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), game_state.Camera);

	const Vector2 player_centre = game_state.Player->Centre;
	const Vector2 centre_direction = Vector2Subtract(mouse_pos, player_centre);

	const float spread_angle = game_state.Attributes.at(Attribute::BuckshotSpread);
	const int buckshot = static_cast<int>((game_state.Attributes.at(Attribute::Buckshot) - 1) / 2);
	
	const float speed = game_state.Attributes.at(Attribute::BulletSpeed);

	for (int i = -buckshot; i <= buckshot; i++)
	{
		if (i == 0)
		{
			game_state.Projectiles.emplace_back(player_centre.x, player_centre.y, speed, 1.0f, centre_direction, ProjectileType::Bullet, assets);
			continue;
		}
		const Vector2 direction = Vector2Rotate(centre_direction, spread_angle * i);

		game_state.Projectiles.emplace_back(player_centre.x, player_centre.y, speed, 1.0f, direction, ProjectileType::Bullet, assets);
	}
}

void GameInputSystem::HandleRightClick(GameState& game_state, const AssetManager& assets) noexcept
{
	const Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), game_state.Camera);

	const Vector2 centre = game_state.Player->Centre;
	const Vector2 direction = Vector2Subtract(mouse_pos, centre);

	const float speed = game_state.Attributes.at(Attribute::LazerSpeed);
	const float scale = game_state.Attributes.at(Attribute::LazerScale);

	game_state.Projectiles.emplace_back(centre.x, centre.y, speed, scale, direction, ProjectileType::Lazer, assets);
}
