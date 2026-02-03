#include "gameDrawSystem.hpp"

#include "raylib.h"

#include "constants.hpp"

#include "game.hpp"
#include "globalDataWrapper.hpp"
#include "assetManager.hpp"

#include "modifiers.hpp"
#include "modifierSystem.hpp"
#include "timers.hpp"
#include "timerSystem.hpp"

void GameDrawSystem::DrawGame(const Game& game, const ModifierSystem& modifier_system, const AssetManager& assets) noexcept
{
	const size_t ticks = game.Ticks;

	const float ground_width = assets.Ground.width;
	const float ground_height = assets.Ground.height;
	const Rectangle update_area = game.UpdateArea;

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

	game.XpSystem->Draw(assets);
	game.EnemySystem->Draw(assets);
	game.ProjectileSystem->Draw(assets);
	game.ParticleSystem->Draw(assets, ticks);

	if(!modifier_system.EffectStatus(Effect::Invisible))
		game.Player->Draw();
}

void GameDrawSystem::DrawLighting(const Game& game, const ModifierSystem& modifier_system) noexcept
{
	if (!modifier_system.EffectStatus(Effect::Invisible))
		game.Player->DrawLightmap();

	game.XpSystem->DrawLightmap();
	game.ProjectileSystem->DrawLightmap();
}

void GameDrawSystem::DrawOverlay(
		const Game& game, const TimerSystem& timer_system, const ModifierSystem& modifier_system,
		const GlobalDataWrapper& global_data, const AssetManager& assets
		) noexcept
{
	DrawTexture(assets.GetTexture(TextureKey::HealthBarBackground), 1060, 20, WHITE);

	const bool is_poisoned = modifier_system.EffectStatus(Effect::Poison);
	const float health_percentage = game.Player->Health / game.Player->HealthMax;

	DrawTexturePro(
			assets.GetTexture(TextureKey::WhitePixel),
			(Rectangle) { 0, 0, 1, 1 },
			(Rectangle) { 1060, 20, health_percentage * 200, 10 },
			(Vector2) { 0.0f, 0.0f },
			0.0f,
			(is_poisoned) ? VIOLET : GREEN
			);


	DrawTexture(assets.GetTexture(TextureKey::XpBarBackground), 100, 680, WHITE);

	const float xp_percentage = static_cast<float>(game.CollectedXp) / static_cast<float>(game.LevelUpThreshold);

	DrawTexturePro(
			assets.GetTexture(TextureKey::WhitePixel),
			(Rectangle) { 0, 0, 1, 1 },
			(Rectangle) { 100, 680, xp_percentage * 1080, 15 },
			(Vector2) { 0.0f, 0.0f },
			0.0f,
			CYAN
			);

	const size_t ticks = game.Ticks;

	if (modifier_system.EffectStatus(Effect::Greenbull))
		GameDrawSystem::DrawGreenbull(timer_system, assets, ticks);

	if (modifier_system.EffectStatus(Effect::Milk))
		GameDrawSystem::DrawMilk(timer_system, assets, ticks);
	
	if (modifier_system.EffectStatus(Effect::Drunk))
		GameDrawSystem::DrawDrunk(timer_system, assets, ticks);

	if (modifier_system.EffectStatus(Effect::Magnetism))
		GameDrawSystem::DrawMagnetism(timer_system, assets, ticks);

	if (modifier_system.EffectStatus(Effect::Trapped))
		DrawText("[Space] to untrap.", 533, 620, 24, WHITE);

	DrawText(global_data.StringCache[static_cast<size_t>(CachedString::Duration)].c_str(), 20, 20, 24, LIGHTGRAY);
	DrawText(global_data.StringCache[static_cast<size_t>(CachedString::LevelText)].c_str(), 20, 50, 24, LIGHTGRAY);

	if (global_data.UnclaimedPowerups)
		DrawText("[TAB]", 40, 680, 15, GOLD);

	DrawText("[LMB]", 1058, 640, 20, (game.CanPerform[static_cast<size_t>(Action::LMB)] ? YELLOW : GRAY));
	DrawText("[RMB]", 1120, 640, 20, (game.CanPerform[static_cast<size_t>(Action::RMB)] ? CYAN : GRAY));
	DrawText("[SHIFT]", 1184, 640, 20, (game.CanPerform[static_cast<size_t>(Action::Slide)] ? ORANGE : GRAY));
}


void GameDrawSystem::DrawGreenbull(const TimerSystem& timer_system, const AssetManager& assets, const size_t ticks) noexcept
{
	const size_t expiry = timer_system.GetTimer(Timer::GreenbullExpire) - ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(assets.GetTexture(TextureKey::GreenbullIcon), 1205, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(assets.GetTexture(TextureKey::GreenbullIcon), 1205, 40, WHITE);
}

void GameDrawSystem::DrawMilk(const TimerSystem& timer_system, const AssetManager& assets, const size_t ticks) noexcept
{
	const size_t expiry = timer_system.GetTimer(Timer::MilkExpire) - ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(assets.GetTexture(TextureKey::MilkIcon), 1225, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(assets.GetTexture(TextureKey::MilkIcon), 1225, 40, WHITE);
}

void GameDrawSystem::DrawDrunk(const TimerSystem& timer_system, const AssetManager& assets, const size_t ticks) noexcept
{
	const size_t expiry = timer_system.GetTimer(Timer::DrunkExpire) - ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(assets.GetTexture(TextureKey::DrunkIcon), 1245, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(assets.GetTexture(TextureKey::DrunkIcon), 1245, 40, WHITE);
}

void GameDrawSystem::DrawMagnetism(const TimerSystem& timer_system, const AssetManager& assets, const size_t ticks) noexcept
{
	const size_t expiry = timer_system.GetTimer(Timer::MagnetismExpire) - ticks;

	if (expiry >= SECONDS_TO_TICKS(5))
		DrawTexture(assets.GetTexture(TextureKey::MagnetismIcon), 1185, 40, WHITE);
	else if ((expiry / (TICK_RATE / 2)) % 2)
		DrawTexture(assets.GetTexture(TextureKey::MagnetismIcon), 1185, 40, WHITE);
}


