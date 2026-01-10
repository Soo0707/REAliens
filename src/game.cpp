#include "game.hpp"

#include <memory>
#include <string>

#include "raylib.h"

#include "constants.hpp"

#include "globalDataWrapper.hpp"
#include "gameInputSystem.hpp"
#include "gameDrawSystem.hpp"
#include "gameHelpers.hpp"

#include "settingsManager.hpp"
#include "assetManager.hpp"

#include "player.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"
#include "modifierSystem.hpp"
#include "particleSystem.hpp"
#include "gameTextSystem.hpp"
#include "projectileSystem.hpp"
#include "enemySystem.hpp"
#include "statSystem.hpp"
#include "xpSystem.hpp"

Game::Game(
		std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
		std::shared_ptr<SettingsManager> settings, std::shared_ptr<struct MessageSystem> message_system,
		std::shared_ptr<class TimerSystem> timer_system, std::shared_ptr<class ModifierSystem> modifier_system,
		std::shared_ptr<class ParticleSystem> particle_system, std::shared_ptr<class GameTextSystem> game_text_system,
		std::shared_ptr<class ProjectileSystem> projectile_system, std::shared_ptr<class EnemySystem> enemy_system,
		std::shared_ptr<class StatSystem> stat_system, std::shared_ptr<class XpSystem> xp_system
		) :
	Player(std::make_unique<class Player>(500.0f, 500.0f, *assets)),
	GlobalData(global_data),
	Assets(assets),
	Settings(settings),
	MessageSystem(message_system),
	TimerSystem(timer_system),
	ModifierSystem(modifier_system),
	ParticleSystem(particle_system),
	GameTextSystem(game_text_system),
	ProjectileSystem(projectile_system),
	EnemySystem(enemy_system),
	StatSystem(stat_system),
	XpSystem(xp_system)
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

	this->CollectedXp = 0;
	this->LevelUpThreshold = 5;

	this->CanPerform = { true, true, true };
	this->LastPerformed = { 0 };

	this->Ticks = 0;
	this->Level = 1;
}

void Game::Draw(RenderTexture2D& canvas) const noexcept
{
	const Camera2D camera = this->Camera;

	BeginTextureMode(this->GameLayer);
		ClearBackground(BLACK);

		BeginMode2D(camera);
			GameDrawSystem::DrawGame(*this, *this->ModifierSystem, *this->Assets);
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

		BeginMode2D(camera);
			GameDrawSystem::DrawScreenLayer(*this);
		EndMode2D();

		GameDrawSystem::DrawOverlay(*this, *this->TimerSystem, *this->ModifierSystem, *this->GlobalData, *this->Assets);
	EndTextureMode();
}

void Game::Update() noexcept
{
	const size_t ticks = this->Ticks;

	if (!(ticks % TICK_RATE))
		this->GlobalData->CachedStrings[CachedString::Duration] = "Duration: " + std::to_string(TICKS_TO_SECONDS(ticks)) + "s";

	if (IsKeyPressed(KEY_ESCAPE))
		this->GlobalData->ActiveState = State::PauseMenu;
	
	if (IsKeyPressed(KEY_TAB) && (this->GlobalData->UnclaimedPowerups > 0 || this->Settings->Data.at(SettingKey::UnlimitedPowerups)))
		this->GlobalData->ActiveState = State::PowerupMenu;
}

void Game::TickedUpdate() noexcept
{
	this->Accumulator += GetFrameTime();

	if (this->Accumulator >= MAX_TICK_TIME)
		this->Accumulator = MAX_TICK_TIME;

	while (this->Accumulator >= TICK_TIME)
	{
		const size_t ticks = this->Ticks;
		const Rectangle update_area = this->UpdateArea;

		while (this->CollectedXp >= this->LevelUpThreshold)
		{
			const size_t distance = this->CollectedXp - this->LevelUpThreshold;

			this->LevelUp();
			this->CollectedXp = distance;
		}

		GameInputSystem::HandleTickedInput(*this, *this->MessageSystem, *this->ModifierSystem, *this->Settings);

		this->UpdateTimeouts();
		this->UpdateTimerSystem(ticks);

		this->UpdateCamera();
		this->UpdatePlayer(ticks);

		this->UpdateModifierSystem();

		this->UpdateParticleSystem(ticks, update_area);
		this->UpdateGameTextSystem(ticks, update_area);
		this->UpdateProjectileSystem(ticks, update_area);
		this->UpdateEnemySystem(ticks, this->Level, update_area);
		this->UpdateStatSystem();
		this->UpdateXpSystem(ticks, update_area);

		this->Accumulator -= TICK_TIME;
		this->Ticks++;
	} 
}

void Game::UpdatePlayer(const size_t ticks) noexcept
{
	this->Player->PollSignals(*this->MessageSystem);

	if (this->Player->Health <= 0 && !this->Settings->Data.at(SettingKey::DisableHealthCheck))
	{
		this->GlobalData->CachedStrings[CachedString::GameOverReason] = "Reason: Player Died";
		this->GlobalData->ActiveState = State::GenerateGameOverStats;
	}

	const float slide_speed = this->ModifierSystem->GetAttribute(Attribute::SlideSpeedMultiplier);
	this->Player->Update(*this->MessageSystem, ticks, slide_speed);

	const float ground_width = this->Assets->Ground.width;
	const float ground_height = this->Assets->Ground.height;

	GameHelper::LoopOverMap(ground_width, ground_height, this->Player->Rect);
}

void Game::UpdateTimerSystem(const size_t ticks) noexcept
{
	this->TimerSystem->ExecuteCommands(*this->MessageSystem, ticks);
	this->TimerSystem->UpdateTimers(*this->MessageSystem, ticks);
}

void Game::UpdateModifierSystem() noexcept
{
	this->ModifierSystem->ExecuteCommands(*this->MessageSystem);
}

void Game::UpdateParticleSystem(const size_t ticks, const Rectangle update_area) noexcept
{
	this->ParticleSystem->ExecuteCommands(*this->MessageSystem, *this->Assets);
	this->ParticleSystem->UpdateParticles(update_area, ticks);
}

void Game::UpdateGameTextSystem(const size_t ticks, const Rectangle update_area) noexcept
{
	this->GameTextSystem->ExecuteCommands(*this->MessageSystem);
	this->GameTextSystem->UpdateGameText(ticks, update_area);
}

void Game::UpdateProjectileSystem(const size_t ticks, const Rectangle update_area) noexcept
{
	this->ProjectileSystem->ExecuteCommands(*this->MessageSystem, *this->Assets);
	this->ProjectileSystem->UpdateProjectiles(ticks, update_area, *this->MessageSystem);
}

void Game::UpdateEnemySystem(const size_t ticks, const size_t level, const Rectangle update_area) noexcept
{
	this->EnemySystem->PollSignals(*this->MessageSystem, *this->Assets, level);

	const Vector2 player_centre = this->Player->Centre;
	const float map_width = this->Assets->Ground.width;
	const float map_height = this->Assets->Ground.height;
	const bool is_stinky = this->ModifierSystem->EffectStatus(Effect::Stinky);

	this->EnemySystem->UpdateEnemies(ticks, update_area, player_centre, map_width, map_height, is_stinky, *this->MessageSystem);
}

void Game::UpdateStatSystem() noexcept
{
	this->StatSystem->ExecuteCommands(*this->MessageSystem);
}

void Game::UpdateXpSystem(const size_t ticks, const Rectangle update_area) noexcept
{
	this->XpSystem->ExecuteCommands(*this->MessageSystem, *this->Assets);
	this->XpSystem->UpdateXps(*this->MessageSystem, update_area, ticks);
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

	if (this->ModifierSystem->EffectStatus(Effect::Aussie))
		this->Camera.rotation = 180.0f;
	else
		this->Camera.rotation = 0.0f;

	if (this->ModifierSystem->EffectStatus(Effect::Microscope))
		this->Camera.zoom = 2.0f;
	else
		this->Camera.zoom = 1.0f;
}

void Game::UpdateTimeouts() noexcept
{
	const size_t ticks = this->Ticks;

	if (ticks - this->LastPerformed[static_cast<size_t>(Action::LMB)] >= this->ModifierSystem->GetAttribute(Attribute::BulletCooldown))
		this->CanPerform[static_cast<size_t>(Action::LMB)] = true;

	if (ticks - this->LastPerformed[static_cast<size_t>(Action::RMB)] >= this->ModifierSystem->GetAttribute(Attribute::LazerCooldown))
		this->CanPerform[static_cast<size_t>(Action::RMB)] = true;

	if (ticks - this->LastPerformed[static_cast<size_t>(Action::Slide)] >= TICK_RATE)
		this->CanPerform[static_cast<size_t>(Action::Slide)]= true;
}


void Game::LevelUp() noexcept
{
	if (!this->Settings->Data.at(SettingKey::UnlimitedPowerups))
	{
		this->GlobalData->UnclaimedPowerups++;
		this->GlobalData->CachedStrings[CachedString::UnclaimedPowerups] = "Unclaimed Powerups: " + std::to_string(this->GlobalData->UnclaimedPowerups);
	}

	this->Level++;
	this->GlobalData->CachedStrings[CachedString::LevelText] = "Level: " + std::to_string(this->Level);

	this->CollectedXp = 0;
	this->LevelUpThreshold += 5;
	
	if (this->Settings->Data.at(SettingKey::PowerupMenuInterrupt))
		this->GlobalData->ActiveState = State::PowerupMenu;
/*
	TODO: Emit signals to this
	if (this->Level % 5 == 0 && !this->Settings->Data.at(SettingKey::DisableLevelDebuffs))
		this->ModifierSystem->InsertLevelDebuff(*this->MessageSystem);
	else
		this->ModifierSystem->RemoveLevelDebuff(*this->MessageSystem);
		*/
}

