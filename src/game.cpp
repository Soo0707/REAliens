#include "game.hpp"

#include <memory>
#include <algorithm>
#include <vector>
#include <string>

#include "raylib.h"

#include "globalDataWrapper.hpp"

#include "gameEventSystem.hpp"
#include "gameInputSystem.hpp"
#include "gameDrawSystem.hpp"
#include "gameHelpers.hpp"
#include "gameText.hpp"

#include "constants.hpp"

#include "player.hpp"
#include "assetManager.hpp"
#include "projectiles.hpp"
#include "collisions.hpp"

#include "enemy.hpp"

#include "enemyData.hpp"
#include "xp.hpp"


Game::Game(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets) :
	GlobalData(global_data),
	Assets(assets)
{
	this->PlayerInstance = std::make_unique<Player>(500, 500, *this->Assets);

	this->LightingLayer = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);
	this->GameLayer = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);

	this->Camera = { 0 };
	this->Camera.offset = { REFERENCE_WIDTH / 2.0f, REFERENCE_HEIGHT / 2.0f };
	this->Camera.rotation = 0.0f;
    this->Camera.zoom = 1.0f;

	this->UpdateArea = {
		this->PlayerInstance->Centre.x - (REFERENCE_WIDTH / 2.0f),
		this->PlayerInstance->Centre.y - (REFERENCE_HEIGHT / 2.0f),
		static_cast<float>(REFERENCE_WIDTH),
		static_cast<float>(REFERENCE_HEIGHT)
	};
}

Game::~Game()
{
	UnloadRenderTexture(this->GameLayer);
	UnloadRenderTexture(this->LightingLayer);
}

void Game::Draw(RenderTexture2D& canvas) const noexcept
{
	BeginTextureMode(this->GameLayer);
		ClearBackground(BLACK);

		BeginMode2D(this->Camera);
			GameDrawSystem::DrawGame(*this);
		EndMode2D();
	EndTextureMode();

	BeginTextureMode(this->LightingLayer);
		ClearBackground(LIGHTGRAY);
		
		BeginMode2D(this->Camera);
			GameDrawSystem::DrawLighting(*this);
		EndMode2D();
	EndTextureMode();

	BeginTextureMode(canvas);
		ClearBackground(BLACK);

			DrawTextureRec(
			this->GameLayer.texture, 
			(Rectangle){ 0, 0, REFERENCE_WIDTH, -static_cast<float>(REFERENCE_HEIGHT) },
			(Vector2){ 0, 0 }, 
			WHITE
			);

			BeginBlendMode(BLEND_MULTIPLIED);
				DrawTextureRec(
				this->LightingLayer.texture, 
				(Rectangle){ 0, 0, REFERENCE_WIDTH, -static_cast<float>(REFERENCE_HEIGHT) },
				(Vector2){ 0, 0 }, 
				WHITE
				);
			EndBlendMode();

			BeginMode2D(this->Camera);
				GameDrawSystem::DrawScreenLayer(*this);
			EndMode2D();

		GameDrawSystem::DrawOverlay(*this);
	EndTextureMode();
}

void Game::Update() noexcept
{
	this->Accumulator += GetFrameTime();

	if (this->Accumulator >= MAX_TICK_TIME)
		this->Accumulator = MAX_TICK_TIME;
	
	while (this->Accumulator >= TICK_TIME)
	{
		size_t ticks = this->GlobalData->Ticks;
		
		if (this->PlayerInstance->Health <= 0 && !this->GlobalData->Settings.at(Setting::DisableHealthCheck))
			this->GlobalData->ActiveState = State::GameOverMenu;
		
		if (ticks - this->LastLMB >= this->GlobalData->Attributes.at(Attribute::BulletCooldown))
			this->CanLMB = true;

		if (ticks - this->LastRMB >= this->GlobalData->Attributes.at(Attribute::LazerCooldown))
			this->CanRMB = true;

		if (ticks - this->LastLayerUp >= TICK_RATE)
			this->CanLayerUp = true;

		if (ticks - this->LastLayerDown >= TICK_RATE)
			this->CanLayerDown = true;
	
		if (this->CollectedXp >= this->GlobalData->LevelUpTreshold)
			GameHelper::LevelUp(*this);

		GameInputSystem::HandleTickedInput(*this);

		GameEventSystem::HandleEvents(*this);

		this->PlayerInstance->Update(ticks);
		GameHelper::LoopOverMap(*this->Assets, this->PlayerInstance->Rect);


		this->UpdateArea.x = this->PlayerInstance->Centre.x - REFERENCE_WIDTH / 2.0f;
		this->UpdateArea.y =  this->PlayerInstance->Centre.y - REFERENCE_HEIGHT / 2.0f;
		
		this->Camera.target = this->PlayerInstance->Centre;

		if (this->GlobalData->Effects.count(Effect::Aussie))
			this->Camera.rotation = 180.0f;
		else
			this->Camera.rotation = 0.0f;


		Game::UpdateEnemies();
		Game::UpdateProjectiles();
		Game::UpdateXps();
		Game::UpdateGameTexts();


		this->Accumulator -= TICK_TIME;
		this->GlobalData->Ticks++;
	} 
}


void Game::UpdateEnemies() noexcept
{
	if ((this->GlobalData->Ticks - this->LastSpawn >= SECONDS_TO_TICKS(30)) || this->Enemies.size() == 0)
	{
		GameHelper::SpawnEnemies(*this);
		this->LastSpawn = this->GlobalData->Ticks;
	}

	bool has_greenbull = this->GlobalData->Effects.count(Effect::Greenbull);
	bool player_hit = false;

	for (auto &enemy : this->Enemies)
	{
		if (enemy.Layer == this->GlobalData->CurrentLayer && CheckCollisionRecs(this->UpdateArea, enemy.Rect))
		{
			enemy.Update(this->PlayerInstance->Rect, this->GlobalData->Ticks);
			GameHelper::LoopOverMap(*this->Assets, enemy.Rect);
			
			if (!has_greenbull)
				player_hit = Collisions::LeAttack(*this->PlayerInstance, enemy, *this->GlobalData);
		}

		if (enemy.Health <= 0)
		{
			unsigned int value = EnemyXpValues.at(enemy.Type);
			this->Xps.emplace_back(enemy.Rect.x, enemy.Rect.y, value * static_cast<int>(enemy.Scale), *this->Assets);
		}
	}

	std::erase_if(this->Enemies, [](const Enemy& enemy) { return (enemy.Health <= 0); });

	if (player_hit)
		PlaySound(this->Assets->Sounds.at(SoundKey::PlayerDamage));
}

void Game::UpdateProjectiles() noexcept
{
	unsigned int damage_done = 0;

	for (auto &projectile : this->Projectiles)
	{
		if (projectile.Layer == this->GlobalData->CurrentLayer && CheckCollisionRecs(this->UpdateArea, projectile.Rect))
		{
			projectile.Update(this->PlayerInstance->Centre);

			GameHelper::LoopOverMap(*this->Assets, projectile.Rect);

			unsigned int damage = Collisions::ProjectileCollision(projectile, this->Enemies, *this->GlobalData); 

			if (damage > 0)
				this->GameTexts.emplace_back(projectile.Rect.x, projectile.Rect.y, std::to_string(damage), this->GlobalData->Ticks);

			damage_done += damage;
		}
		else // TODO: switching layers just kill
			projectile.Kill = true;
	}

	std::erase_if(this->Projectiles, [](const Projectile& proj){ return proj.Kill; });

	if (this->GlobalData->Effects.count(Effect::LifeSteal))
		this->PlayerInstance->IncreaseHealth( damage_done * this->GlobalData->Attributes.at(Attribute::LifeStealMultiplier) );
}

void Game::UpdateXps() noexcept
{
	bool has_magnetism = this->GlobalData->Effects.count(Effect::Magnetism);
	bool play_sound = false;

	for (auto &xp : this->Xps)
	{
		if (CheckCollisionRecs(this->PlayerInstance->Rect, xp.Rect) || has_magnetism)
		{
			this->CollectedXp += xp.Value;
			xp.Kill = true;
			play_sound = true;
		}
	}

	std::erase_if(this->Xps, [](const Xp& xp) { return xp.Kill; });

	if (play_sound)
		PlaySound(this->Assets->Sounds.at(SoundKey::Xp));
}

void Game::UpdateGameTexts() noexcept
{
	for (auto &text : this->GameTexts)
	{
		if (CheckCollisionRecs(this->UpdateArea, text.Rect))
			text.Update();
	}

	std::erase_if(this->GameTexts, [ticks = this->GlobalData->Ticks](const GameText& text) { return (ticks >= text.Expiry); });
}


void Game::HandleEssentialInput() noexcept
{
	if (IsKeyPressed(KEY_ESCAPE))
	{
		this->GlobalData->ActiveState = State::PauseMenu;
		PlaySound(this->Assets->Sounds.at(SoundKey::Pause));
	}
	
	if (IsKeyPressed(KEY_TAB) && this->GlobalData->UnclaimedPowerups > 0)
		this->GlobalData->ActiveState = State::PowerupMenu;
}

void Game::Reset() noexcept
{
	this->Enemies.clear();
	this->Projectiles.clear();
	this->Xps.clear();
	this->GameTexts.clear();

	this->PlayerInstance = std::make_unique<Player>(500, 500, *this->Assets);
	this->CollectedXp = 0;
	
	this->LastLMB = 0;
	this->CanLMB = true;

	this->LastRMB = 0;
	this->CanRMB = true;

	this->LastLayerDown = 0;
	this->CanLayerDown = true;

	this->LastLayerUp = 0;
	this->CanLayerUp = true;

	this->LastSpawn = 0;

	this->GlobalData->Reset();
}
