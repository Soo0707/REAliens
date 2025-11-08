#include "gameDrawSystem.hpp"
#include "constants.hpp"
#include "raylib.h"
#include "game.hpp"

void GameDrawSystem::DrawGame(const Game& game) noexcept
{
	Rectangle viewport = game.UpdateArea;

	if (viewport.x < 0)
	{
		viewport.width += viewport.x;
		viewport.x = 0;
	}
	
	if (viewport.x + viewport.width > game.Assets->Ground.width)
		viewport.width = game.Assets->Ground.width - viewport.x;

	if (viewport.y < 0)
	{
		viewport.height += viewport.y;
		viewport.y = 0;
	}
	
	if (viewport.y + viewport.height > game.Assets->Ground.height)
		viewport.height = game.Assets->Ground.height - viewport.y;

	DrawTextureRec(game.Assets->Ground, viewport, (Vector2) { viewport.x, viewport.y }, WHITE);
	
	for (auto const &xp : game.Xps)
	{
		if (CheckCollisionRecs(game.UpdateArea, xp.Rect))
			xp.Draw();
	}
	
	for (auto const &enemy : game.Enemies)
	{
		if (enemy.Layer <= game.GlobalData->CurrentLayer && CheckCollisionRecs(game.UpdateArea, enemy.Rect))
			enemy.Draw();
	}
	
	for (auto const &projectile : game.Projectiles)
	{
		if (CheckCollisionRecs(game.UpdateArea, projectile.Rect))
			projectile.Draw();
	}


	game.PlayerInstance->Draw();
}

void GameDrawSystem::DrawLighting(const Game& game) noexcept
{
	for (auto const &xp : game.Xps)
	{
		if (CheckCollisionRecs(game.UpdateArea, xp.Rect))
			xp.DrawLightmap();
	}

	for (auto const &projectile : game.Projectiles)
	{
		if (CheckCollisionRecs(game.UpdateArea, projectile.Rect))
			projectile.DrawLightmap();
	}
	
	game.PlayerInstance->DrawLightmap();
}

void GameDrawSystem::DrawScreenLayer(const Game& game) noexcept
{
	for (auto const &text : game.GameTexts)
	{
		if (CheckCollisionRecs(game.UpdateArea, text.Rect))
			text.Draw();
	}
}

void GameDrawSystem::DrawOverlay(const Game& game) noexcept
{
	float health_percentage = game.PlayerInstance->Health / game.PlayerInstance->HealthMax;

	Rectangle health_bar = {
		1060,
		20,
		health_percentage * 200,
		10
	};

	bool is_poisoned = game.GlobalData->Effects.count(Effect::Poison);

	float xp_percentage = (float) game.CollectedXp / (float) game.LevelUpTreshold;

	Rectangle xp_bar = {
		100,
		680,
		xp_percentage * 1080,
		15
	};


	DrawRectangleRec(HEALTH_BACKGROUND, BLACK);
	DrawRectangleRec(health_bar, (is_poisoned) ? VIOLET : GREEN);

	DrawRectangleRec(XP_BACKGROUND, BLACK);
	DrawRectangleRec(xp_bar, CYAN);

	if (game.GlobalData->Effects.count(Effect::Greenbull))
		GameDrawSystem::DrawGreenbull(game);

	if (game.GlobalData->Effects.count(Effect::Milk))
		GameDrawSystem::DrawMilk(game);
	
	if (game.GlobalData->Effects.count(Effect::Drunk))
		GameDrawSystem::DrawDrunk(game);

	if (game.GlobalData->Effects.count(Effect::Magnetism))
		GameDrawSystem::DrawMagnetism(game);

	if (game.GlobalData->Effects.count(Effect::Trapped))
		DrawText("[Space] to untrap.", 533, 620, 24, WHITE);

	DrawText(game.GlobalData->CachedStrings.at(CachedString::Duration).c_str(), 20, 20, 24, LIGHTGRAY);
	DrawText(game.GlobalData->CachedStrings.at(CachedString::LayerText).c_str(), 20, 50, 24, LIGHTGRAY);
	DrawText(game.GlobalData->CachedStrings.at(CachedString::LevelText).c_str(), 20, 80, 24, LIGHTGRAY);
}


void GameDrawSystem::DrawGreenbull(const Game& game) noexcept
{
	size_t expiry = game.GlobalData->Events.at(Event::GreenbullExpire) - game.GlobalData->Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawRectangleRec(GREENBULL_SQUARE, GREEN);
	else if ((expiry / TICK_RATE) % 2)
		DrawRectangleRec(GREENBULL_SQUARE, GREEN);
}

void GameDrawSystem::DrawMilk(const Game& game) noexcept
{
	size_t expiry = game.GlobalData->Events.at(Event::MilkExpire) - game.GlobalData->Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawRectangleRec(MILK_SQUARE, WHITE);
	else if ((expiry / TICK_RATE) % 2)
		DrawRectangleRec(MILK_SQUARE, WHITE);
}


void GameDrawSystem::DrawDrunk(const Game& game) noexcept
{
	size_t expiry = game.GlobalData->Events.at(Event::DrunkExpire) - game.GlobalData->Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawRectangleRec(DRUNK_SQUARE, YELLOW);
	else if ((expiry / TICK_RATE) % 2)
		DrawRectangleRec(DRUNK_SQUARE, YELLOW);
}

void GameDrawSystem::DrawMagnetism(const Game& game) noexcept
{
	size_t expiry = game.GlobalData->Events.at(Event::MagnetismExpire) - game.GlobalData->Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
	{
		DrawRectangleRec(MAGNETISM_HALF_1, DARKBLUE);
		DrawRectangleRec(MAGNETISM_HALF_2, RED);
	}
	else if ((expiry / TICK_RATE) % 2)
	{
		DrawRectangleRec(MAGNETISM_HALF_1, DARKBLUE);
		DrawRectangleRec(MAGNETISM_HALF_2, RED);
	}
}

