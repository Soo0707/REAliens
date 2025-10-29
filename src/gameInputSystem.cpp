#include "gameInputSystem.hpp"

#include "raylib.h"
#include "raymath.h"
#include "game.hpp"
#include "constants.hpp"

#include <array>
#include <string>

void GameInputSystem::HandleTickedInput(Game& game) noexcept
{
	if (IsKeyDown(KEY_Q) && game.CanLayerDown && game.GlobalData->CurrentLayer - 1 >= 0)
	{
		game.GlobalData->CurrentLayer--;
		game.GlobalData->CachedStrings[CachedString::LayerText] = "Current Layer: " + std::to_string(game.GlobalData->CurrentLayer);

		game.LastLayerDown = game.GlobalData->Ticks;
		game.CanLayerDown = false;
	}

	if (IsKeyDown(KEY_E) && game.CanLayerUp)
	{
		game.GlobalData->CurrentLayer++;
		game.GlobalData->CachedStrings[CachedString::LayerText] = "Current Layer: " + std::to_string(game.GlobalData->CurrentLayer);

		game.LastLayerUp = game.GlobalData->Ticks;
		game.CanLayerUp = false;
	}

	if (game.GlobalData->Effects.count(Effect::Trapped))
	{
		if (IsKeyDown(KEY_SPACE))
			game.GlobalData->Effects.erase(Effect::Trapped);

		game.PlayerInstance->Direction = { 0, 0 };
	}
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

	bool auto_click = game.GlobalData->Settings.at(Setting::AutoClick);

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
	Vector2 scale_factors = { static_cast<float>(GetScreenWidth()) / REFERENCE_WIDTH, static_cast<float>(GetScreenHeight()) / REFERENCE_HEIGHT };
	Vector2 scaled_mouse_pos = { GetMouseX() / scale_factors.x, GetMouseY() / scale_factors.y };
	Vector2 mouse_pos = GetScreenToWorld2D(scaled_mouse_pos, game.Camera);

	Vector2 centre_direction = Vector2Subtract(mouse_pos, game.PlayerInstance->Centre);
	Vector2 player_centre = game.PlayerInstance->Centre;

	float spread_angle = game.GlobalData->Attributes[Attribute::BuckshotSpread];
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
	static constexpr std::array<Vector2, 4> directions = { Vector2{1.0f, 0.0f}, Vector2{0.0f, 1.0f}, Vector2{-1.0f, 0.0f}, Vector2{0.0f, -1.0f} };

	Vector2 centre = game.PlayerInstance->Centre;
	
	for (int i = 0; i < 4; i++)
		game.Projectiles.emplace_back(centre.x, centre.y, directions[i], ProjectileType::Lazer, *game.GlobalData, *game.Assets);
}
