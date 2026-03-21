/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "game.hpp"

#include <memory>
#include <string>

#include "raylib.h"

#include "constants.hpp"

#include "globalDataWrapper.hpp"
#include "gameInputSystem.hpp"
#include "gameDrawSystem.hpp"

#include "settingsManager.hpp"
#include "assetManager.hpp"

#include "player.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"
#include "modifierSystem.hpp"
#include "particleSystem.hpp"
#include "projectileSystem.hpp"
#include "enemySystem.hpp"
#include "statSystem.hpp"
#include "xpSystem.hpp"

//TODO: inject player as well
Game::Game(
		std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
		std::shared_ptr<SettingsManager> settings, std::shared_ptr<struct MessageSystem> message_system,
		std::shared_ptr<class TimerSystem> timer_system, std::shared_ptr<class ModifierSystem> modifier_system,
		std::shared_ptr<class ParticleSystem> particle_system, std::shared_ptr<class ProjectileSystem> projectile_system,
		std::shared_ptr<class EnemySystem> enemy_system, std::shared_ptr<class StatSystem> stat_system,
		std::shared_ptr<class XpSystem> xp_system, std::shared_ptr<class CollisionSystem> collision_system
		) :
	Player(std::make_unique<class Player>()),
	GlobalData(global_data),
	Assets(assets),
	Settings(settings),
	MessageSystem(message_system),
	TimerSystem(timer_system),
	ModifierSystem(modifier_system),
	ParticleSystem(particle_system),
	ProjectileSystem(projectile_system),
	EnemySystem(enemy_system),
	StatSystem(stat_system),
	XpSystem(xp_system),
	CollisionSystem(collision_system)
{
	this->Camera = { 0 };
	this->Camera.offset = { REFERENCE_WIDTH / 2.0f, REFERENCE_HEIGHT / 2.0f };
	this->Camera.rotation = 0.0f;
    this->Camera.zoom = 1.0f;

	this->UpdateArea = {
		this->Player->Centre.x - (REFERENCE_WIDTH / 2.0f),
		this->Player->Centre.y - (REFERENCE_HEIGHT / 2.0f),
		static_cast<float>(REFERENCE_WIDTH),
		static_cast<float>(REFERENCE_HEIGHT)
	};

	this->LightingLayer = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);
	this->GameLayer = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);
}

Game::~Game()
{
	UnloadRenderTexture(this->GameLayer);
	UnloadRenderTexture(this->LightingLayer);
}

void Game::Reset() noexcept
{
	this->Player->Reset();

	this->CanPerform = { 0 };
}

void Game::Draw(const size_t ticks, const RenderTexture2D& canvas) const noexcept
{
	const Camera2D camera = this->Camera;

	BeginTextureMode(this->GameLayer);
		ClearBackground(BLACK);

		BeginMode2D(camera);
			GameDrawSystem::DrawGame(*this, *this->ModifierSystem, *this->Assets, ticks);
		EndMode2D();
	EndTextureMode();

	BeginTextureMode(this->LightingLayer);
		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);
			GameDrawSystem::DrawLighting(*this, *this->ModifierSystem);
		EndMode2D();
	EndTextureMode();

	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawTextureRec(
			this->GameLayer.texture, 
			(Rectangle) { 0, 0, REFERENCE_WIDTH, -static_cast<float>(REFERENCE_HEIGHT) },
			(Vector2) { 0, 0 }, 
			WHITE
		);

		BeginBlendMode(BLEND_MULTIPLIED);
			DrawTextureRec(
				this->LightingLayer.texture, 
				(Rectangle) { 0, 0, REFERENCE_WIDTH, -static_cast<float>(REFERENCE_HEIGHT) },
				(Vector2) { 0, 0 }, 
				WHITE
			);
		EndBlendMode();

		GameDrawSystem::DrawOverlay(*this, *this->TimerSystem, *this->ModifierSystem, *this->GlobalData, *this->Assets, ticks);
	EndTextureMode();
}

void Game::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_ESCAPE))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<struct SetState>, State::PauseMenu);
	
	if (IsKeyPressed(KEY_TAB) && (this->ModifierSystem->GetUnclaimedPowerups() > 0 || this->Settings->Get(SettingKey::UnlimitedPowerups)))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<struct SetState>, State::PowerupMenu);

	GameInputSystem::HandleTickedInput(*this, *this->MessageSystem, *this->ModifierSystem, *this->Settings);
}

void Game::Update(const size_t ticks) noexcept
{
	const size_t level = this->ModifierSystem->GetLevel();
	const Rectangle update_area = this->UpdateArea;

	const float map_width = this->Assets->Ground.width;
	const float map_height = this->Assets->Ground.height;

	//TODO: make this not suck
	if (!(ticks % TICK_RATE))
		this->GlobalData->CacheString("Duration: " + std::to_string(TICKS_TO_SECONDS(ticks)) + "s", CachedString::Duration);

	this->TimerSystem->Update(*this->MessageSystem, ticks);
	this->PollSignals();

	this->Player->Update(
			*this->MessageSystem, *this->ModifierSystem, *this->Settings,
			map_width, map_height, ticks
			);

	this->UpdateCamera();
	this->ModifierSystem->Update(*this->MessageSystem, *this->Settings);
	
	this->StatSystem->Update(*this->MessageSystem);

	this->XpSystem->Update(*this->MessageSystem, *this->Assets, update_area, ticks);

	this->EnemySystem->Update(
			*this->MessageSystem, *this->Assets, *this->ModifierSystem,
			*this->TimerSystem, update_area, this->Player->Centre, ticks, level
			);

	this->ProjectileSystem->Update(*this->MessageSystem, *this->Assets, *this->ModifierSystem, update_area, ticks);
	
	this->CollisionSystem->Update(
			*this->MessageSystem, *this->ModifierSystem, this->EnemySystem->GetEnemyRect(),
			this->EnemySystem->GetEnemyHealth(), this->EnemySystem->GetEnemyAttackComponents(),
			this->EnemySystem->GetEnemyType(), this->ProjectileSystem->GetProjectileRect(),
			this->ProjectileSystem->GetProjectileType(), this->ProjectileSystem->GetProjectileDirection(),
			this->XpSystem->GetXpRect(), *this->Player, ticks
	);

	this->ParticleSystem->Update(*this->MessageSystem, *this->Assets, update_area, ticks);
}



void Game::UpdateCamera() noexcept
{
	this->UpdateArea.x = this->Player->Centre.x - REFERENCE_WIDTH / 2.0f;
	this->UpdateArea.y =  this->Player->Centre.y - REFERENCE_HEIGHT / 2.0f;
	
	this->Camera.target = this->Player->Centre;
	this->Camera.offset = { REFERENCE_WIDTH / 2.0f, REFERENCE_HEIGHT / 2.0f };

	if (this->ModifierSystem->EffectStatus(Effect::Earthquake))
	{
		const float shake_offset = static_cast<float>(GetRandomValue(-6, 6));

		this->Camera.offset.x += shake_offset;
		this->Camera.offset.y -= shake_offset;
	}

	this->Camera.rotation = this->ModifierSystem->EffectStatus(Effect::Aussie) * 180.0f;

	if (this->ModifierSystem->EffectStatus(Effect::Microscope))
		this->Camera.zoom = 2.0f;
	else
		this->Camera.zoom = 1.0f;
}


void Game::PollSignals() noexcept
{
	for (size_t i = 0, n = static_cast<size_t>(GameSignal::COUNT); i < n; i++)
	{
		const uint16_t count = this->MessageSystem->GameSignals[i];

		if (count)
			this->CanPerform[i] = true;

		this->MessageSystem->GameSignals[i] = 0;
	}
}
