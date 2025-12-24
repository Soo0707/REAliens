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
		game_state.m_Player->Direction = { 0, 0 };
	else
	{
		game_state.m_Player->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
		game_state.m_Player->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
	}

	if (game_state.Effects.count(Effect::Drunk))
	{
		game_state.m_Player->Direction.x *= -1;
		game_state.m_Player->Direction.y *= -1;
	}

	if (game_state.m_Player->Direction.x != 0.0f && game_state.m_Player->Direction.y != 0.0f)
		game_state.m_Player->Direction = Vector2Normalize(game_state.m_Player->Direction);

	const size_t ticks = game_state.Ticks;

	if (IsKeyDown(KEY_LEFT_SHIFT) && game_state.CanSlide)
	{
		game_state.m_Player->Sliding = true;
		game_state.m_Player->SlideExpire = ticks + TICK_RATE / 6;

		game_state.CanSlide = false;
		game_state.LastSlide = ticks;
	}

	const bool auto_click = settings.Data.at(SettingKey::AutoClick);

	if ((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || auto_click) && game_state.CanLMB)
	{
		GameInputSystem::HandleLeftClick(game_state, assets);
		game_state.CanLMB = false;
		game_state.LastLMB = ticks;
	}
	
	if ((IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || auto_click) && game_state.CanRMB)
	{
		GameInputSystem::HandleRightClick(game_state, assets);
		game_state.CanRMB = false;
		game_state.LastRMB = ticks;
	}
}

void GameInputSystem::HandleLeftClick(GameState& game_state, const AssetManager& assets) noexcept
{
	const Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), game_state.Camera);

	const Vector2 player_centre = game_state.m_Player->Centre;
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

	const Vector2 centre = game_state.m_Player->Centre;
	const Vector2 direction = Vector2Subtract(mouse_pos, centre);

	const float speed = game_state.Attributes.at(Attribute::LazerSpeed);
	const float scale = game_state.Attributes.at(Attribute::LazerScale);

	game_state.Projectiles.emplace_back(centre.x, centre.y, speed, scale, direction, ProjectileType::Lazer, assets);
}
