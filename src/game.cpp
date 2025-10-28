#include "game.hpp"

#include <memory>
#include <algorithm>
#include <array>
#include <vector>
#include <string>

#include "raylib.h"
#include "raymath.h"

#include "globalDataWrapper.hpp"
#include "gameEventSystem.hpp"
#include "gameDrawSystem.hpp"

#include "constants.hpp"

#include "player.hpp"
#include "assetManager.hpp"
#include "projectiles.hpp"
#include "collisions.hpp"
#include "gameText.hpp"

#include "enemy.hpp"

#include "enemyData.hpp"
#include "xp.hpp"


Game::Game(std::shared_ptr<GlobalDataWrapper> global_data) :
	GlobalData(global_data),
	SpawnTimeout(SECONDS_TO_TICKS(30))
{
	this->Assets = std::make_shared<AssetManager>();

	this->PlayerInstance = std::make_unique<Player>(500, 500, *this->Assets);

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
{}

void Game::Draw(RenderTexture2D& virtual_canvas) const noexcept
{
	BeginTextureMode(virtual_canvas);
		ClearBackground(BLACK);

		GameDrawSystem::DrawGame(*this);

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
			Game::LevelUp();

		Game::HandleTickedInput();

		GameEventSystem::HandleEvents(*this);

		this->PlayerInstance->Update(ticks);
		Game::LoopOverMap(this->PlayerInstance->Rect);


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
	if ((this->GlobalData->Ticks - this->LastSpawn >= this->SpawnTimeout) || this->Enemies.size() == 0)
	{
		Game::SpawnEnemies();
		this->LastSpawn = this->GlobalData->Ticks;
	}

	bool has_greenbull = this->GlobalData->Effects.count(Effect::Greenbull);

	for (auto &enemy : this->Enemies)
	{
		if (enemy.Layer == this->GlobalData->CurrentLayer && CheckCollisionRecs(this->UpdateArea, enemy.Rect))
		{
			enemy.Update(this->PlayerInstance->Rect, this->GlobalData->Ticks);
			Game::LoopOverMap(enemy.Rect);
			
			if (!has_greenbull)
				Collisions::LeAttack(*this->PlayerInstance, enemy, *this->GlobalData);
		}

		if (enemy.Health <= 0)
		{
			unsigned int value = EnemyXpValues.at(enemy.Type);
			this->Xps.emplace_back(enemy.Rect.x, enemy.Rect.y, value * static_cast<int>(enemy.Scale), *this->Assets);
		}
	}

	std::erase_if(this->Enemies, [](const Enemy& enemy) { return (enemy.Health <= 0); });
}

void Game::UpdateProjectiles() noexcept
{
	unsigned int damage_done = 0;

	for (auto &projectile : this->Projectiles)
	{
		if (projectile.Layer == this->GlobalData->CurrentLayer && CheckCollisionRecs(this->UpdateArea, projectile.Rect))
		{
			projectile.Update(this->PlayerInstance->Centre);

			Game::LoopOverMap(projectile.Rect);

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

	for (auto &xp : this->Xps)
	{
		if (CheckCollisionRecs(this->PlayerInstance->Rect, xp.Rect) || has_magnetism)
		{
			this->CollectedXp += xp.Value;
			xp.Kill = true;
		}
	}

	std::erase_if(this->Xps, [](const Xp& xp) { return xp.Kill; });
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
		this->GlobalData->ActiveState = State::PauseMenu;
	
	if (IsKeyPressed(KEY_TAB) && this->GlobalData->UnclaimedPowerups > 0)
		this->GlobalData->ActiveState = State::PowerupMenu;
}

void Game::HandleTickedInput() noexcept
{
	if (IsKeyDown(KEY_Q) && CanLayerDown && this->GlobalData->CurrentLayer - 1 >= 0)
	{
		this->GlobalData->CurrentLayer--;
		this->GlobalData->CachedStrings[CachedString::LayerText] = "Current Layer: " + std::to_string(this->GlobalData->CurrentLayer);

		this->LastLayerDown = this->GlobalData->Ticks;
		this->CanLayerDown = false;
	}

	if (IsKeyDown(KEY_E) && CanLayerUp)
	{
		this->GlobalData->CurrentLayer++;
		this->GlobalData->CachedStrings[CachedString::LayerText] = "Current Layer: " + std::to_string(this->GlobalData->CurrentLayer);

		this->LastLayerUp = this->GlobalData->Ticks;
		this->CanLayerUp = false;
	}

	if (this->GlobalData->Effects.count(Effect::Trapped))
	{
		if (IsKeyDown(KEY_SPACE))
			this->GlobalData->Effects.erase(Effect::Trapped);

		this->PlayerInstance->Direction = { 0, 0 };
	}
	else
	{
		this->PlayerInstance->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
		this->PlayerInstance->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
	}

	if (this->GlobalData->Effects.count(Effect::Drunk))
	{
		this->PlayerInstance->Direction.x *= -1;
		this->PlayerInstance->Direction.y *= -1;
	}

	if (this->PlayerInstance->Direction.x != 0.0f && this->PlayerInstance->Direction.y != 0.0f)
		this->PlayerInstance->Direction = Vector2Normalize(this->PlayerInstance->Direction);

	bool auto_click = this->GlobalData->Settings.at(Setting::AutoClick);

	if ((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || auto_click) && this->CanLMB)
	{
		Game::HandleLeftClick();
		this->CanLMB = false;
		this->LastLMB = this->GlobalData->Ticks;
	}
	
	if ((IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || auto_click) && this->CanRMB)
	{
		Game::HandleRightClick();
		this->CanRMB = false;
		this->LastRMB = this->GlobalData->Ticks;
	}
}

void Game::HandleLeftClick() noexcept
{
	Vector2 scale_factors = { static_cast<float>(GetScreenWidth()) / REFERENCE_WIDTH, static_cast<float>(GetScreenHeight()) / REFERENCE_HEIGHT };
	Vector2 scaled_mouse_pos = { GetMouseX() / scale_factors.x, GetMouseY() / scale_factors.y };
	Vector2 mouse_pos = GetScreenToWorld2D(scaled_mouse_pos, this->Camera);

	Vector2 centre_direction = Vector2Subtract(mouse_pos, this->PlayerInstance->Centre);
	Vector2 player_centre = this->PlayerInstance->Centre;

	float spread_angle = this->GlobalData->Attributes[Attribute::BuckshotSpread];
	int buckshot = static_cast<int>((this->GlobalData->Attributes.at(Attribute::Buckshot) - 1) / 2);

	for (int i = -buckshot; i <= buckshot; i++)
	{
		if (i == 0)
		{
			this->Projectiles.emplace_back(player_centre.x, player_centre.y, centre_direction, ProjectileType::Bullet, *this->GlobalData, *this->Assets);
			continue;
		}
		Vector2 direction = Vector2Rotate(centre_direction, spread_angle * i);

		this->Projectiles.emplace_back(player_centre.x, player_centre.y, direction, ProjectileType::Bullet, *this->GlobalData, *this->Assets);
	}
}

void Game::HandleRightClick() noexcept
{
	static constexpr std::array<Vector2, 4> directions = { Vector2{1.0f, 0.0f}, Vector2{0.0f, 1.0f}, Vector2{-1.0f, 0.0f}, Vector2{0.0f, -1.0f} };

	Vector2 centre = this->PlayerInstance->Centre;
	
	for (int i = 0; i < 4; i++)
		this->Projectiles.emplace_back(centre.x, centre.y, directions[i], ProjectileType::Lazer, *this->GlobalData, *this->Assets);
}



void Game::SpawnEnemies() noexcept
{
	std::vector<Vector2> rand_nums;

	for (size_t i = 0; i < this->GlobalData->Level * 15; i++)
	{
		rand_nums.emplace_back(
				(Vector2) {
				static_cast<float>( GetRandomValue(0, this->Assets->Ground.width) ),
				static_cast<float>( GetRandomValue(0, this->Assets->Ground.height) )
				});
	}

	for (auto &location : rand_nums)
	{
		EnemyType type = (EnemyType) GetRandomValue(0, 4);
		BehaviourModifier modifier = BehaviourModifier::None;
		
		unsigned int random1 = GetRandomValue(0, 100);
		unsigned int random2 = GetRandomValue(0, 100);
		unsigned int random3 = GetRandomValue(0, 100);

		if (random1 > 30)
			modifier = modifier | BehaviourModifier::IncreasedSpeed;

		if (random2 > 50)
			modifier = modifier | BehaviourModifier::OverrideDirection;

		if (random3 > 95 && this->GlobalData->Level > 10)
			modifier = modifier | BehaviourModifier::Big;

		this->Enemies.emplace_back(location.x, location.y, this->GlobalData->CurrentLayer, *this->Assets, type, modifier);
	}
}

void Game::LoopOverMap(Rectangle& m_obj) noexcept
{
	if (m_obj.x < 0)
		m_obj.x = this->Assets->Ground.width - m_obj.width;
	else if (m_obj.x > this->Assets->Ground.width)
		m_obj.x = 0;

	if (m_obj.y < 0)
		m_obj.y = this->Assets->Ground.height - m_obj.height;
	else if (m_obj.y > this->Assets->Ground.height)
		m_obj.y = 0;
}

void Game::LevelUp() noexcept
{
	this->GlobalData->Level++;
	this->GlobalData->UnclaimedPowerups++;

	this->GlobalData->CachedStrings[CachedString::LevelText] = "Level: " + std::to_string(this->GlobalData->Level);
	this->GlobalData->CachedStrings[CachedString::UnclaimedPowerups] = "Unclaimed Powerups: " + std::to_string(this->GlobalData->UnclaimedPowerups);

	this->CollectedXp = 0;
	this->GlobalData->LevelUpTreshold += 5;
	
	if (this->GlobalData->Settings.at(Setting::ShowPowerupMenuOnLevelUp))
		this->GlobalData->ActiveState = State::PowerupMenu;
}
