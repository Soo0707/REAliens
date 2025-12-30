#include "gameDrawSystem.hpp"

#include "raylib.h"
#include <iostream>

#include "constants.hpp"
#include "gameState.hpp"
#include "globalDataWrapper.hpp"
#include "assetManager.hpp"

void GameDrawSystem::DrawGame(const GameState& game_state, const AssetManager& assets) noexcept
{
	const size_t ticks = game_state.Ticks;

	const float ground_width = assets.Ground.width;
	const float ground_height = assets.Ground.height;
	const Rectangle update_area = game_state.UpdateArea;

	Rectangle viewport = update_area;

	if (viewport.x < 0)
	{
		viewport.width += viewport.x;
		viewport.x = 0;
	}
	
	if (viewport.x + viewport.width > ground_width)
		viewport.width = ground_width - viewport.x;

	if (viewport.y < 0)
	{
		viewport.height += viewport.y;
		viewport.y = 0;
	}
	
	if (viewport.y + viewport.height > ground_height)
		viewport.height = ground_height - viewport.y;

	DrawTextureRec(assets.Ground, viewport, (Vector2) { viewport.x, viewport.y }, WHITE);
	
	for (auto const &xp : game_state.Xps)
	{
		if (CheckCollisionRecs(update_area, xp.Rect))
			xp.Draw();
	}
	
	for (auto const &enemy : game_state.Enemies)
	{
		if (CheckCollisionRecs(update_area, enemy.Rect))
			enemy.Draw();
	}
	
	for (auto const &projectile : game_state.Projectiles)
	{
		if (CheckCollisionRecs(update_area, projectile.Rect))
			projectile.Draw();
	}

	for (auto const &particle : game_state.Particles)
	{
		if (CheckCollisionRecs(update_area, particle.Rect))
			particle.Draw(ticks);
	}

	if (!game_state.Effects.count(Effect::Invisible))
		game_state.Player->Draw();
}

void GameDrawSystem::DrawLighting(const GameState& game_state) noexcept
{
	if (!game_state.Effects.count(Effect::Invisible))
		game_state.Player->DrawLightmap();

	const Rectangle update_area = game_state.UpdateArea;

	for (auto const &xp : game_state.Xps)
	{
		if (CheckCollisionRecs(update_area, xp.Rect))
			xp.DrawLightmap();
	}

	for (auto const &projectile : game_state.Projectiles)
	{
		if (CheckCollisionRecs(update_area, projectile.Rect))
			projectile.DrawLightmap();
	}
}

void GameDrawSystem::DrawScreenLayer(const GameState& game_state) noexcept
{
	const size_t ticks = game_state.Ticks;

	for (auto const &text : game_state.GameTexts)
	{
		if (CheckCollisionRecs(game_state.UpdateArea, text.Rect))
			text.Draw(ticks);
	}
}

void GameDrawSystem::DrawOverlay(const GameState& game_state, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept
{
	DrawTexture(assets.Textures.at(TextureKey::HealthBarBackground), 1060, 20, WHITE);

	const bool is_poisoned = game_state.Effects.count(Effect::Poison);
	const float health_percentage = game_state.Player->Health / game_state.Player->HealthMax;

	DrawTexturePro(
			assets.Textures.at(TextureKey::WhitePixel),
			(Rectangle) { 0, 0, 1, 1 },
			(Rectangle) { 1060, 20, health_percentage * 200, 10 },
			(Vector2) { 0.0f, 0.0f },
			0.0f,
			(is_poisoned) ? VIOLET : GREEN
			);


	DrawTexture(assets.Textures.at(TextureKey::XpBarBackground), 100, 680, WHITE);

	const float xp_percentage = static_cast<float>(game_state.CollectedXp) / static_cast<float>(game_state.LevelUpTreshold);

	DrawTexturePro(
			assets.Textures.at(TextureKey::WhitePixel),
			(Rectangle) { 0, 0, 1, 1 },
			(Rectangle) { 100, 680, xp_percentage * 1080, 15 },
			(Vector2) { 0.0f, 0.0f },
			0.0f,
			CYAN
			);

	if (game_state.Effects.count(Effect::Greenbull))
		GameDrawSystem::DrawGreenbull(game_state, assets);

	if (game_state.Effects.count(Effect::Milk))
		GameDrawSystem::DrawMilk(game_state, assets);
	
	if (game_state.Effects.count(Effect::Drunk))
		GameDrawSystem::DrawDrunk(game_state, assets);

	if (game_state.Effects.count(Effect::Magnetism))
		GameDrawSystem::DrawMagnetism(game_state, assets);

	if (game_state.Effects.count(Effect::Trapped))
		DrawText("[Space] to untrap.", 533, 620, 24, WHITE);

	DrawText(global_data.CachedStrings.at(CachedString::Duration).c_str(), 20, 20, 24, LIGHTGRAY);
	DrawText(global_data.CachedStrings.at(CachedString::LevelText).c_str(), 20, 50, 24, LIGHTGRAY);

	DrawText(global_data.CachedStrings.at(CachedString::LevelDebuff).c_str(), 20, 110, 24, GOLD);

	if (game_state.UnclaimedPowerups)
		DrawText("[TAB]", 40, 680, 15, GOLD);

	DrawText("[LMB]", 1058, 640, 20, (game_state.CanPerform[static_cast<size_t>(Action::LMB)] ? YELLOW : GRAY));
	DrawText("[RMB]", 1120, 640, 20, (game_state.CanPerform[static_cast<size_t>(Action::RMB)] ? CYAN : GRAY));
	DrawText("[SHIFT]", 1184, 640, 20, (game_state.CanPerform[static_cast<size_t>(Action::Slide)] ? ORANGE : GRAY));
}


void GameDrawSystem::DrawGreenbull(const GameState& game_state, const AssetManager& assets) noexcept
{
	const size_t expiry = game_state.Events.at(Event::GreenbullExpire) - game_state.Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(assets.Textures.at(TextureKey::GreenbullIcon), 1205, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(assets.Textures.at(TextureKey::GreenbullIcon), 1205, 40, WHITE);
}

void GameDrawSystem::DrawMilk(const GameState& game_state, const AssetManager& assets) noexcept
{
	const size_t expiry = game_state.Events.at(Event::MilkExpire) - game_state.Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(assets.Textures.at(TextureKey::MilkIcon), 1225, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(assets.Textures.at(TextureKey::MilkIcon), 1225, 40, WHITE);
}

void GameDrawSystem::DrawDrunk(const GameState& game_state, const AssetManager& assets) noexcept
{
	const size_t expiry = game_state.Events.at(Event::DrunkExpire) - game_state.Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(assets.Textures.at(TextureKey::DrunkIcon), 1245, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(assets.Textures.at(TextureKey::DrunkIcon), 1245, 40, WHITE);
}

void GameDrawSystem::DrawMagnetism(const GameState& game_state, const AssetManager& assets) noexcept
{
	const size_t expiry = game_state.Events.at(Event::MagnetismExpire) - game_state.Ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(assets.Textures.at(TextureKey::MagnetismIcon), 1185, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(assets.Textures.at(TextureKey::MagnetismIcon), 1185, 40, WHITE);
}


