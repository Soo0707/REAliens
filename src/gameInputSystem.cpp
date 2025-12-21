#include "gameInputSystem.hpp"

#include "raylib.h"
#include "raymath.h"
#include "game.hpp"
#include "settingsManager.hpp"
#include "constants.hpp"

void GameInputSystem::HandleTickedInput(Game& game) noexcept
{
	if (IsKeyDown(KEY_SPACE))
		game.GlobalData->Effects.erase(Effect::Trapped);

	if (game.GlobalData->Effects.count(Effect::Trapped))
		game.PlayerInstance->Direction = { 0, 0 };
	else
	{
		game.PlayerInstance->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
		game.PlayerInstance->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
	}

	if (game.GlobalData->Effects.count(Effect::Drunk))
	{
		game.PlayerInstance->Direction.x *= -1;
		game.PlayerInstance->Direction.y *= -1;
	}

	if (game.PlayerInstance->Direction.x != 0.0f && game.PlayerInstance->Direction.y != 0.0f)
		game.PlayerInstance->Direction = Vector2Normalize(game.PlayerInstance->Direction);


	if (IsKeyDown(KEY_LEFT_SHIFT) && game.CanSlide)
	{
		game.PlayerInstance->Sliding = true;
		game.PlayerInstance->SlideExpire = game.GlobalData->Ticks + TICK_RATE / 4;

		game.CanSlide = false;
		game.LastSlide = game.GlobalData->Ticks;
	}

	bool auto_click = game.Settings->Data.at(SettingKey::AutoClick);

	if ((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || auto_click) && game.CanLMB)
	{
		GameInputSystem::HandleLeftClick(game);
		game.CanLMB = false;
		game.LastLMB = game.GlobalData->Ticks;
	}
	
	if ((IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || auto_click) && game.CanRMB)
	{
		GameInputSystem::HandleRightClick(game);
		game.CanRMB = false;
		game.LastRMB = game.GlobalData->Ticks;
	}
}

void GameInputSystem::HandleLeftClick(Game& game) noexcept
{
	Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), game.Camera);

	Vector2 centre_direction = Vector2Subtract(mouse_pos, game.PlayerInstance->Centre);
	Vector2 player_centre = game.PlayerInstance->Centre;

	float spread_angle = game.GlobalData->Attributes.at(Attribute::BuckshotSpread);
	int buckshot = static_cast<int>((game.GlobalData->Attributes.at(Attribute::Buckshot) - 1) / 2);

	for (int i = -buckshot; i <= buckshot; i++)
	{
		if (i == 0)
		{
			game.Projectiles.emplace_back(player_centre.x, player_centre.y, centre_direction, ProjectileType::Bullet, *game.GlobalData, *game.Assets);
			continue;
		}
		Vector2 direction = Vector2Rotate(centre_direction, spread_angle * i);

		game.Projectiles.emplace_back(player_centre.x, player_centre.y, direction, ProjectileType::Bullet, *game.GlobalData, *game.Assets);
	}
}

void GameInputSystem::HandleRightClick(Game& game) noexcept
{
	Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), game.Camera);

	Vector2 centre = game.PlayerInstance->Centre;
	Vector2 direction = Vector2Subtract(mouse_pos, centre);

	game.Projectiles.emplace_back(centre.x, centre.y, direction, ProjectileType::Lazer, *game.GlobalData, *game.Assets);
}
