/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gameDrawSystem.hpp"

#include <cstddef>

#include "raylib.h"

#include "constants.hpp"

#include "game.hpp"
#include "stringCache.hpp"
#include "assetManager.hpp"

#include "modifiers.hpp"
#include "modifierSystem.hpp"
#include "timers.hpp"
#include "timerSystem.hpp"
#include "cameraSystem.hpp"
#include "inventorySystem.hpp"

void GameDrawSystem::DrawGame(
		const Game& game, const CameraSystem& camera_system, const ModifierSystem& modifier_system,
		const AssetManager& assets
		) noexcept
{
	const float ground_width = assets.Ground.width;
	const float ground_height = assets.Ground.height;
	const Rectangle update_area = camera_system.GetUpdateArea();

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

	game.ItemSystem->Draw(assets);
	game.EnemySystem->Draw(assets);
	game.ProjectileSystem->Draw(assets);
	game.ParticleSystem->Draw(assets);

	if (!modifier_system.EffectStatus(Effect::Invisible))
		game.Player->Draw(assets);
}

void GameDrawSystem::DrawLighting(const Game& game, const ModifierSystem& modifier_system) noexcept
{
	if (!modifier_system.EffectStatus(Effect::Invisible))
		game.Player->DrawLightmap();

	game.ItemSystem->DrawLightmap();
	game.ProjectileSystem->DrawLightmap();
}

void GameDrawSystem::DrawOverlay(
		const Game& game, const TimerSystem& timer_system, const ModifierSystem& modifier_system,
		const InventorySystem& inventory_system, const StringCache& string_cache, const AssetManager& assets
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

	const float xp_percentage = static_cast<float>(modifier_system.GetCollectedXp()) / static_cast<float>(modifier_system.GetLevelUpThreshold());

	DrawTexturePro(
			assets.GetTexture(TextureKey::WhitePixel),
			(Rectangle) { 0, 0, 1, 1 },
			(Rectangle) { 100, 680, xp_percentage * 1080, 15 },
			(Vector2) { 0.0f, 0.0f },
			0.0f,
			CYAN
			);


	if (modifier_system.EffectStatus(Effect::Drunk))
		DrawTexture(assets.GetTexture(TextureKey::DrunkIcon), 1245, 40, WHITE);

	if (modifier_system.EffectStatus(Effect::Milk))
		DrawTexture(assets.GetTexture(TextureKey::MilkIcon), 1225, 40, WHITE);
	
	if (modifier_system.EffectStatus(Effect::Greenbull))
		DrawTexture(assets.GetTexture(TextureKey::GreenbullIcon), 1205, 40, WHITE);

	if (modifier_system.EffectStatus(Effect::Magnetism))
		DrawTexture(assets.GetTexture(TextureKey::MagnetismIcon), 1185, 40, WHITE);

	if (modifier_system.EffectStatus(Effect::Tariffs))
		DrawTexture(assets.GetTexture(TextureKey::TariffIcon), 1165, 40, WHITE);

	if (modifier_system.EffectStatus(Effect::Weakness))
		DrawTexture(assets.GetTexture(TextureKey::WeaknessIcon), 1145, 40, WHITE);

	DrawText(string_cache.Data[static_cast<size_t>(GameString::Duration)].c_str(), 20, 20, 24, LIGHTGRAY);
	DrawText(string_cache.Data[static_cast<size_t>(GameString::LevelText)].c_str(), 20, 50, 24, (modifier_system.GetLevel() % 5) ? LIGHTGRAY : GOLD);

	Color use_item_colour = GRAY;

	if (game.CanPerform[static_cast<size_t>(Action::UseItem)] && !inventory_system.IsEmpty())
		use_item_colour = LIME;
	
	// TODO: position [E] horizontally properly
	DrawText("[E]", 1186, 620, 20, use_item_colour);
	DrawText("[TAB]", 1206, 620, 20, (modifier_system.GetUnclaimedPowerups()) ? GOLD : GRAY);

	DrawText("[LMB]", 1058, 640, 20, (game.CanPerform[static_cast<size_t>(Action::LMB)] ? YELLOW : GRAY));
	DrawText("[RMB]", 1120, 640, 20, (game.CanPerform[static_cast<size_t>(Action::RMB)] ? CYAN : GRAY));
	DrawText("[SHIFT]", 1184, 640, 20, (game.CanPerform[static_cast<size_t>(Action::Slide)] ? ORANGE : GRAY));
}
