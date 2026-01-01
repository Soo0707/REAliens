#include "gameState.hpp"

#include <algorithm>
#include <memory>

#include "raylib.h"

#include "assetManager.hpp"
#include "globalDataWrapper.hpp"
#include "settingsManager.hpp"
#include "gameHelpers.hpp"

#include "constants.hpp"
#include "collisions.hpp"
#include "player.hpp"
#include "projectiles.hpp"
#include "xp.hpp"
#include "enemy.hpp"
#include "enemyData.hpp"
#include "gameText.hpp"
#include "particle.hpp"

GameState::GameState(AssetManager& assets) noexcept :
	Player(std::make_unique<class Player>(500, 500, assets))
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

	this->Projectiles.reserve(100);
	this->Xps.reserve(100);
	this->GameTexts.reserve(100);
	this->Particles.reserve(1024);
}

void GameState::Reset() noexcept
{
	this->Player->Reset();

	this->Enemies.clear();
	this->Projectiles.clear();
	this->Xps.clear();
	this->GameTexts.clear();
	this->Particles.clear();

	this->CollectedXp = 0;
	this->LevelUpTreshold = 5;

	this->Attributes = 
	{
		{ Attribute::BulletCooldown, 150 },
		{ Attribute::BulletDamage, 25.0f },
		{ Attribute::BulletSpeed, 1000.0f },

		{ Attribute::Buckshot, 3 },
		{ Attribute::BuckshotSpread, PI / 8 },

		{ Attribute::LazerCooldown, 450 },
		{ Attribute::LazerDamage, 25.0f },
		{ Attribute::LazerScale, 1.0f },
		{ Attribute::LazerSpeed, 3000.0f },

		{ Attribute::SlideSpeed, 4.0f }	
	};
	
	this->Events.clear();
	this->Effects.clear();

	this->Stats = { 0 };

	this->CanPerform = { true, true, true };
	this->LastPerformed = { 0 };

	this->Ticks = 0;
	this->Level = 1;

	this->UnclaimedPowerups = 0;

	this->CollectedXp = 0;
	this->LevelUpTreshold = 5;
}


void GameState::UpdateEnemies(const AssetManager& assets) noexcept
{
	const size_t ticks = this->Ticks;

	if (this->Enemies.size() <= 5 && !this->Events.count(Event::SpawnEnemies))
	{
		this->GenerateRandomLocations(assets);
		this->Events[Event::SpawnEnemies] = ticks + SECONDS_TO_TICKS(3);
	}

	const float ground_width = assets.Ground.width;
	const float ground_height = assets.Ground.height;

	const bool has_greenbull = this->Effects.count(Effect::Greenbull);
	const bool is_sliding = this->Player->Sliding;
	const bool is_stinky = this->Effects.count(Effect::Stinky);

	const Rectangle update_area = this->UpdateArea;
	const Vector2 player_centre = this->Player->Centre;

	for (auto &enemy : this->Enemies)
	{
		long long slide_damage = 0;

		if (CheckCollisionRecs(update_area, enemy.Rect))
		{
			enemy.Update(player_centre, ticks, is_stinky);

			GameHelper::LoopOverMap(ground_width, ground_height, enemy.Rect);
			
			if (!has_greenbull && !is_sliding)
				Collisions::LeAttack(*this->Player, enemy, ticks, this->Effects, this->Events);
			
			if (is_sliding)
				slide_damage = Collisions::SlideAttack(*this->Player, enemy);
		}

		if (slide_damage > 0)
		{
			this->Stats[static_cast<size_t>(Stat::TotalDamage)] += slide_damage;

			this->GameTexts.emplace_back( 
					enemy.Rect.x, enemy.Rect.y, 64.0f, std::to_string(slide_damage),
					52,	ORANGE, ticks, ticks + TICK_RATE / 4
					);

			GameState::EmitParticles(
					50, enemy.Rect.x, enemy.Rect.y, 10, 25,
					60, TICK_RATE, this->Player->Direction, 512, ORANGE, RED, assets);
		}

		if (enemy.Health <= 0)
		{
			const unsigned int value = EnemyXpValues[static_cast<size_t>(enemy.Type)];
			this->Xps.emplace_back(enemy.Rect.x, enemy.Rect.y, value * static_cast<int>(enemy.Scale), assets);

			this->Stats[static_cast<size_t>(Stat::Kills)]++;
		}
	}

	std::erase_if(this->Enemies, [](const Enemy& enemy) { return (enemy.Health <= 0); });
}

void GameState::UpdateProjectiles(const AssetManager& assets) noexcept
{
	const size_t ticks = this->Ticks;
	unsigned int total_damage_done = 0;

	const bool spawn_particle = GetRandomValue(1, 100) < 5;

	const float ground_width = assets.Ground.width;
	const float ground_height = assets.Ground.height;

	const Rectangle update_area = this->UpdateArea;

	for (auto &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(update_area, projectile.Rect))
		{
			projectile.Update();

			//GameHelper::LoopOverMap(ground_width, ground_height, projectile.Rect);

			const unsigned int damage = Collisions::ProjectileCollision(projectile, this->Enemies, ticks, this->Attributes); 

			if (damage > 0)
			{
				total_damage_done += damage;

				this->GameTexts.emplace_back( 
						projectile.Rect.x, projectile.Rect.y, 64.0f, std::to_string(damage),
						48,	YELLOW, ticks, ticks + TICK_RATE / 4
						);

				GameState::EmitParticles(
						5, projectile.Rect.x, projectile.Rect.y, 5, 20,
						120, TICK_RATE, projectile.Direction, 256, RED, RED, assets
						);
			}

			if (spawn_particle)
			{
				GameState::EmitParticles(
						1, projectile.Rect.x, projectile.Rect.y, 5, 20, 120, TICK_RATE, 
						projectile.Direction, 128, projectile.Colour, projectile.Colour, assets);
			}
		}
		else
			projectile.Kill = true;
	}

	std::erase_if(this->Projectiles, [](const Projectile& proj){ return proj.Kill; });

	if (this->Effects.count(Effect::LifeSteal))
		this->Player->IncreaseHealth( total_damage_done * this->Attributes.at(Attribute::LifeStealMultiplier) );

	this->Stats[static_cast<size_t>(Stat::TotalDamage)] += total_damage_done;
}

void GameState::UpdateXps(const AssetManager& assets) noexcept
{
	const size_t ticks = this->Ticks;
	
	const bool has_magnetism = this->Effects.count(Effect::Magnetism);
	const bool spawn_particles = GetRandomValue(1, 100) < 3;

	for (auto &xp : this->Xps)
	{
		if (CheckCollisionRecs(this->Player->Rect, xp.Rect) || has_magnetism)
		{
			this->CollectedXp += xp.Value;
			xp.Kill = true;
		}

		if (CheckCollisionRecs(this->UpdateArea, xp.Rect) && spawn_particles) 
		{
			const Vector2 velocity = (Vector2) { static_cast<float>(GetRandomValue(-64, 64)), static_cast<float>(GetRandomValue(-96, -32)) };

			GameState::EmitParticles(1, xp.Rect.x, xp.Rect.y, 10, 25, 120, TICK_RATE, velocity, 0, GREEN, DARKGREEN, assets);
		}
	}

	std::erase_if(this->Xps, [](const Xp& xp) { return xp.Kill; });
}

void GameState::UpdateGameTexts() noexcept
{
	const size_t ticks = this->Ticks;

	for (auto &text : this->GameTexts)
	{
		if (CheckCollisionRecs(this->UpdateArea, text.Rect))
			text.Update();
	}

	std::erase_if(this->GameTexts, [ticks](const GameText& text) { return (ticks >= text.Expiry); });
}

void GameState::UpdateParticles() noexcept
{
	const size_t ticks = this->Ticks;

	for (auto &particle : this->Particles)
	{
		if (CheckCollisionRecs(this->UpdateArea, particle.Rect))
			particle.Update(ticks);
	}

	std::erase_if(this->Particles, [ticks](const Particle& particle) { return (ticks >= particle.Expiry); });
}

void GameState::UpdatePlayer(GlobalDataWrapper& global_data, const SettingsManager& settings, const AssetManager& assets) noexcept
{
	if (this->Player->Health <= 0 && !settings.Data.at(SettingKey::DisableHealthCheck))
	{
		global_data.CachedStrings[CachedString::GameOverReason] = "Reason: Player Died";
		global_data.ActiveState = State::GenerateGameOverStats;
	}

	const float slide_speed = this->Attributes.at(Attribute::SlideSpeed);
	size_t* total_distance = &this->Stats[static_cast<size_t>(Stat::TotalDistance)];

	this->Player->Update(this->Ticks, total_distance, slide_speed);

	const float ground_width = assets.Ground.width;
	const float ground_height = assets.Ground.height;

	GameHelper::LoopOverMap(ground_width, ground_height, this->Player->Rect);
}

void GameState::UpdateLocations(const AssetManager& assets) noexcept
{
	const bool spawn_particles = !(this->Ticks % TICK_RATE);

	if (spawn_particles)
	{
		for (auto const& location : this->SpawnLocations)
		{
			const Vector2 velocity = (Vector2) { 
				static_cast<float>(GetRandomValue(-8, 8)),
				static_cast<float>(GetRandomValue(-96, -64))
			};

			GameState::EmitParticles(
					20, location.x, location.y, 5, 20,
					120, TICK_RATE, velocity, 32, RED, MAGENTA, assets
					);
		}
	}
}

void GameState::UpdateCamera() noexcept
{
	this->UpdateArea.x = this->Player->Centre.x - REFERENCE_WIDTH / 2.0f;
	this->UpdateArea.y =  this->Player->Centre.y - REFERENCE_HEIGHT / 2.0f;
	
	this->Camera.target = this->Player->Centre;
	this->Camera.offset = { REFERENCE_WIDTH / 2.0f, REFERENCE_HEIGHT / 2.0f };

	if (this->Effects.count(Effect::Earthquake))
	{
		const float shake_offset = static_cast<float>(GetRandomValue(-6, 6));

		this->Camera.offset.x += shake_offset;
		this->Camera.offset.y -= shake_offset;
	}

	if (this->Effects.count(Effect::Aussie))
		this->Camera.rotation = 180.0f;
	else
		this->Camera.rotation = 0.0f;

	if (this->Effects.count(Effect::Microscope))
		this->Camera.zoom = 2.0f;
	else
		this->Camera.zoom = 1.0f;
}

void GameState::UpdateTimeouts() noexcept
{
	const size_t ticks = this->Ticks;
	
	if (ticks - this->LastPerformed[static_cast<size_t>(Action::LMB)] >= this->Attributes.at(Attribute::BulletCooldown))
		this->CanPerform[static_cast<size_t>(Action::LMB)] = true;

	if (ticks - this->LastPerformed[static_cast<size_t>(Action::RMB)] >= this->Attributes.at(Attribute::LazerCooldown))
		this->CanPerform[static_cast<size_t>(Action::RMB)] = true;

	if (ticks - this->LastPerformed[static_cast<size_t>(Action::Slide)] >= TICK_RATE)
		this->CanPerform[static_cast<size_t>(Action::Slide)]= true;
}

void GameState::InsertLevelDebuff(GlobalDataWrapper& global_data) noexcept
{
	const int index = GetRandomValue(0, this->DebuffList.size() - 1);

	const Effect random_effect = this->DebuffList[index];

	this->Effects.insert(random_effect);
	global_data.CachedStrings[CachedString::LevelDebuff] = std::string(this->DebuffNames[index]);
}

void GameState::RemoveLevelDebuff(GlobalDataWrapper& global_data) noexcept
{
	for (auto const effect : this->DebuffList)
		this->Effects.erase(effect);

	global_data.CachedStrings[CachedString::LevelDebuff] = "";
}

void GameState::GenerateRandomLocations(const AssetManager& assets) noexcept
{
	const float ground_width = assets.Ground.width;
	const float ground_height = assets.Ground.height;

	const size_t spawn_count = this->Level * 15;

	this->SpawnLocations.reserve(spawn_count);

	for (size_t i = 0; i < spawn_count; i++)
	{
		this->SpawnLocations.emplace_back(
				(Vector2) {
				static_cast<float>( GetRandomValue(0, ground_width) ),
				static_cast<float>( GetRandomValue(0, ground_height) )
				});
	}
}

void GameState::LevelUp(const SettingsManager& settings, GlobalDataWrapper& global_data) noexcept
{
	if (!settings.Data.at(SettingKey::UnlimitedPowerups))
	{
		this->UnclaimedPowerups++;
		global_data.CachedStrings[CachedString::UnclaimedPowerups] = "Unclaimed Powerups: " + std::to_string(this->UnclaimedPowerups);
	}

	this->Level++;
	global_data.CachedStrings[CachedString::LevelText] = "Level: " + std::to_string(this->Level);

	this->CollectedXp = 0;
	this->LevelUpTreshold += 5;
	
	if (settings.Data.at(SettingKey::PowerupMenuInterrupt))
		global_data.ActiveState = State::PowerupMenu;

	if (this->Level % 5 == 0 && !settings.Data.at(SettingKey::DisableLevelDebuffs))
		this->InsertLevelDebuff(global_data);
	else
		this->RemoveLevelDebuff(global_data);
}


void GameState::EmitParticles(
		size_t number, float x, float y, unsigned int min_size, unsigned int max_size, unsigned int min_lifetime, unsigned int max_lifetime,
		Vector2 velocity, unsigned int max_speed, Color start_colour, Color end_colour, const AssetManager& assets
		) noexcept
{
	const size_t ticks = this->Ticks;

	for (size_t i = 0; i < number; i++)
	{
		const float size = static_cast<float>(GetRandomValue(min_size, max_size));
		const float rotation = static_cast<float>(GetRandomValue(0, 90));
		const size_t expiry = ticks + static_cast<size_t>(GetRandomValue(min_lifetime, max_lifetime));

		velocity.x += static_cast<float>(GetRandomValue(-max_speed, max_speed));
		velocity.y += static_cast<float>(GetRandomValue(-max_speed, max_speed));

		this->Particles.emplace_back(
				x, y, size, rotation, ticks,
				expiry, velocity, start_colour, end_colour, assets
				);
	}
}


