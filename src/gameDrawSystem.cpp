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

	if (!game.GlobalData->Effects.count(Effect::Invisible))
		game.PlayerInstance->Draw();
}

void GameDrawSystem::DrawLighting(const Game& game) noexcept
{
	if (!game.GlobalData->Effects.count(Effect::Invisible))
		game.PlayerInstance->DrawLightmap();

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
}

void GameDrawSystem::DrawScreenLayer(const Game& game) noexcept
{
	size_t ticks = game.GlobalData->Ticks;

	for (auto const &text : game.GameTexts)
	{
		if (CheckCollisionRecs(game.UpdateArea, text.Rect))
			text.Draw(ticks);
	}
}

void GameDrawSystem::DrawOverlay(const Game& game) noexcept
{
	DrawTexture(game.Assets->Textures.at(TextureKey::HealthBarBackground), 1060, 20, WHITE);

	bool is_poisoned = game.GlobalData->Effects.count(Effect::Poison);
	float health_percentage = game.PlayerInstance->Health / game.PlayerInstance->HealthMax;

	DrawTexturePro(
			game.Assets->Textures.at(TextureKey::WhitePixel),
			(Rectangle) { 0, 0, 1, 1 },
			(Rectangle) { 1060, 20, health_percentage * 200, 10 },
			(Vector2) { 0.0f, 0.0f },
			0.0f,
			(is_poisoned) ? VIOLET : GREEN
			);


	DrawTexture(game.Assets->Textures.at(TextureKey::XpBarBackground), 100, 680, WHITE);

	float xp_percentage = static_cast<float>(game.CollectedXp) / static_cast<float>(game.LevelUpTreshold);

	DrawTexturePro(
			game.Assets->Textures.at(TextureKey::WhitePixel),
			(Rectangle) { 0, 0, 1, 1 },
			(Rectangle) { 100, 680, xp_percentage * 1080, 15 },
			(Vector2) { 0.0f, 0.0f },
			0.0f,
			CYAN
			);

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

	DrawText(game.GlobalData->CachedStrings.at(CachedString::LevelDebuff).c_str(), 20, 110, 24, GOLD);
}


void GameDrawSystem::DrawGreenbull(const Game& game) noexcept
{
	size_t expiry = game.GlobalData->Events.at(Event::GreenbullExpire) - game.GlobalData->Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(game.Assets->Textures.at(TextureKey::GreenbullIcon), 1205, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(game.Assets->Textures.at(TextureKey::GreenbullIcon), 1205, 40, WHITE);
}

void GameDrawSystem::DrawMilk(const Game& game) noexcept
{
	size_t expiry = game.GlobalData->Events.at(Event::MilkExpire) - game.GlobalData->Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(game.Assets->Textures.at(TextureKey::MilkIcon), 1225, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(game.Assets->Textures.at(TextureKey::MilkIcon), 1225, 40, WHITE);
}


void GameDrawSystem::DrawDrunk(const Game& game) noexcept
{
	size_t expiry = game.GlobalData->Events.at(Event::DrunkExpire) - game.GlobalData->Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(game.Assets->Textures.at(TextureKey::DrunkIcon), 1245, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(game.Assets->Textures.at(TextureKey::DrunkIcon), 1245, 40, WHITE);
}

void GameDrawSystem::DrawMagnetism(const Game& game) noexcept
{
	size_t expiry = game.GlobalData->Events.at(Event::MagnetismExpire) - game.GlobalData->Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(game.Assets->Textures.at(TextureKey::MagnetismIcon), 1185, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(game.Assets->Textures.at(TextureKey::MagnetismIcon), 1185, 40, WHITE);
}
