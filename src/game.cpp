#include <memory>
#include <algorithm>
#include <array>
#include <iostream>
#include "raylib.h"
#include "raymath.h"

#include "game.hpp"
#include "globalDataWrapper.hpp"
#include "constants.hpp"

#include "player.hpp"
#include "assetManager.hpp"
#include "projectiles.hpp"
#include "collisions.hpp"
#include "enemy.hpp"
#include "enemyData.hpp"
#include "xp.hpp"

Game::Game(std::shared_ptr<GlobalDataWrapper> global_data) :
	GlobalData(global_data),
	StartWorkers(2, [](){}),
	StopWorkers(2, [](){})
{
	this->Assets = std::make_shared<AssetManager>();

	this->PlayerInstance = std::make_unique<Player>(500, 500, *(this->Assets));

	this->Camera = { 0 };
	this->Camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
	this->Camera.rotation = 0.0f;
    this->Camera.zoom = 1.0f;
	
	this->UpdateArea = {
		this->PlayerInstance->Centre.x - (GetScreenWidth() / 2.0f),
		this->PlayerInstance->Centre.y - (GetScreenHeight() / 2.0f),
		(float) GetScreenWidth(),
		(float) GetScreenHeight()
	};

	this->Threads.emplace_back([this](){Game::UpdateThread1();});
}

Game::~Game()
{
	this->RunThreads = false;
	(void) this->StartWorkers.arrive();

	for (auto& thread : this->Threads)
	{
		if (thread.joinable())
			thread.join();
	}
}

void Game::Draw()
{
	BeginMode2D(this->Camera);
	
	static unsigned int map_width = this->Assets->Ground.width;
	static unsigned int map_height = this->Assets->Ground.height;

	Rectangle viewport = this->UpdateArea;

	if (viewport.x < 0) {
		viewport.width += viewport.x;
		viewport.x = 0;
	}
	
	if (viewport.x + viewport.width > map_width) {
		viewport.width = map_width - viewport.x;
	}

	if (viewport.y < 0) {
		viewport.height += viewport.y;
		viewport.y = 0;
	}
	
	if (viewport.y + viewport.height > map_height) {
		viewport.height = map_height - viewport.y;
	}
	
	DrawTextureRec(this->Assets->Ground, viewport, (Vector2) { viewport.x, viewport.y }, WHITE);

	for (auto const &xp : this->Xps)
	{
		if (CheckCollisionRecs(this->UpdateArea, xp.Rect))
			xp.Draw();
	}
	
	for (auto &enemy : this->Enemies)
	{
		if (CheckCollisionRecs(this->UpdateArea, enemy.Rect))
		{
			enemy.Animate();
			enemy.Draw();
		}
	}

	for (auto const &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
			projectile.Draw();
	}

	this->PlayerInstance->Animate();
	this->PlayerInstance->Draw();

	EndMode2D();
}

void Game::Update()
{
	this->Accumulator += GetFrameTime();

	if (this->Accumulator >= MAX_TICK_TIME)
		this->Accumulator = MAX_TICK_TIME;
	
	while (this->Accumulator >= TICK_TIME)
	{
		size_t ticks = this->GlobalData->Ticks;
		
		if (this->PlayerInstance->Health <= 0)
			this->GlobalData->ActiveState = State::GameOverMenu;

		if (ticks - this->LastLMB >= this->GlobalData->Attributes[Attribute::BulletCooldown])
			this->CanLMB = true;

		if (ticks - this->LastRMB >= this->GlobalData->Attributes[Attribute::LazerCooldown])
			this->CanRMB = true;
		
		if ((ticks - this->LastSpawn >= this->SpawnTimeout) || this->Enemies.size() == 0)
		{
			Game::SpawnEnemies();
			this->LastSpawn = ticks;
		}
		
		if (this->CollectedXp >= this->GlobalData->LevelUpTreshold)
		{
			this->GlobalData->Level++;
			this->CollectedXp = 0;
			this->GlobalData->LevelUpTreshold *= 2;
			this->GlobalData->ActiveState = State::PowerupMenu;
		}

		Game::HandleEvents();
		Game::HandleInput();

		this->PlayerInstance->Update();
		
		(void) this->StartWorkers.arrive();

		(void) this->StopWorkers.arrive_and_wait();

		Game::LoopOverMap(this->PlayerInstance->Rect);
		
		this->UpdateArea.x = this->PlayerInstance->Centre.x - GetScreenWidth() / 2.0f;
		this->UpdateArea.y =  this->PlayerInstance->Centre.y - GetScreenHeight() / 2.0f;
		
		this->Camera.target = this->PlayerInstance->Centre;

		if (this->GlobalData->Attributes.count(Attribute::Aussie))
			this->Camera.rotation = 180.0f;
		else
			this->Camera.rotation = 0.0f;

		for (auto &xp : this->Xps)
		{
			if (CheckCollisionRecs(this->PlayerInstance->Rect, xp.Rect) || this->GlobalData->Attributes.count(Attribute::Magnetism))
			{
				this->CollectedXp += xp.Value;
				xp.Kill = true;
			}
		}
		std::erase_if(this->Xps, [](Xp& xp) { return xp.Kill; });

		this->Accumulator -= TICK_TIME;
		this->GlobalData->Ticks++;
	} 
}

void Game::HandleInput()
{
	if (IsKeyDown(KEY_ESCAPE))
		this->GlobalData->ActiveState = State::PauseMenu;

	if (this->GlobalData->Attributes.count(Attribute::Trapped))
	{
		if (IsKeyDown(KEY_SPACE))
			this->GlobalData->Attributes[Attribute::Trapped]--;

		if (this->GlobalData->Attributes[Attribute::Trapped] <= 0)
			this->GlobalData->Attributes.erase(Attribute::Trapped);

		this->PlayerInstance->Direction = {0, 0};
	}
	else
	{
		this->PlayerInstance->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
		this->PlayerInstance->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
	}

	if (this->PlayerInstance->Direction.x != 0.0f && this->PlayerInstance->Direction.y != 0.0f)
		this->PlayerInstance->Direction = Vector2Normalize(this->PlayerInstance->Direction);

	if (IsMouseButtonDown(0) && this->CanLMB)
	{
		Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), this->Camera);

		Vector2 centre_direction = Vector2Subtract(mouse_pos, this->PlayerInstance->Centre);
		Vector2 player_centre = this->PlayerInstance->Centre;

		float spread_angle = this->GlobalData->Attributes[Attribute::BuckshotSpread];
		int buckshot = (int) (this->GlobalData->Attributes[Attribute::Buckshot] - 1) / 2;

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

		this->CanLMB = false;
		this->LastLMB = this->GlobalData->Ticks;
	}
	
	if (IsMouseButtonDown(1) && this->CanRMB)
	{
		static constexpr std::array<Vector2, 4> directions = { Vector2{1.0f, 0.0f}, Vector2{0.0f, 1.0f}, Vector2{-1.0f, 0.0f}, Vector2{0.0f, -1.0f} };

		Vector2 centre = this->PlayerInstance->Centre;
		
		for (int i = 0; i < 4; i++)
			this->Projectiles.emplace_back(centre.x, centre.y, directions[i], ProjectileType::Lazer, *this->GlobalData, *this->Assets);

		this->CanRMB = false;
		this->LastRMB = this->GlobalData->Ticks;
	}
}

void Game::SpawnEnemies()
{
	std::vector<Vector2> rand_nums;

	for (int i = 0; i < this->GlobalData->Level * 5; i++)
	{
		rand_nums.emplace_back(
				(Vector2) {
				(float) GetRandomValue(32, (int) this->UpdateArea.width / 2), 
				(float) GetRandomValue(32, (int) this->UpdateArea.height / 2) 
				});
	}

	for (auto &location : rand_nums)
	{
		if (GetRandomValue(1, 50) % 2)
			location.x *= -1;

		if (GetRandomValue(1, 50) % 2)
			location.y *= -1;

		float x = this->PlayerInstance->Rect.x + location.x;
		float y = this->PlayerInstance->Rect.y + location.y;

		EnemyType type = (EnemyType) GetRandomValue(0, 5);

		this->Enemies.emplace_back(x, y, this->Assets, type, BehaviourModifier::None);
	}
}

void Game::LoopOverMap(Rectangle& m_obj)
{
	static unsigned int map_width = this->Assets->Ground.width;
	static unsigned int map_height = this->Assets->Ground.height;

	if (m_obj.x < 0)
		m_obj.x = map_width - m_obj.width;
	else if (m_obj.x > map_width)
		m_obj.x = 0;

	if (m_obj.y < 0)
		m_obj.y = map_height - m_obj.height;
	else if (m_obj.y > map_height)
		m_obj.y = 0;
}

void Game::HandleEvents()
{
	size_t ticks = this->GlobalData->Ticks;
	for (auto it = this->GlobalData->Events.begin(); it != this->GlobalData->Events.end();)
	{
		switch (it->first)
		{
			case Event::SpawnAndUpgradeCircles:
			{
				this->Projectiles.emplace_back(
						this->PlayerInstance->Rect.x,
						this->PlayerInstance->Rect.y,
						(Vector2) { 0, 0 },
						ProjectileType::Circle,
						*this->GlobalData,
						*this->Assets
						);

				for (auto &proj : this->Projectiles)
				{
					if (proj.Type == ProjectileType::Circle)
					{
						proj.Scale = this->GlobalData->Attributes.at(Attribute::CircleScale);
						proj.Speed = this->GlobalData->Attributes.at(Attribute::CircleAngularSpeed);
						proj.Rotation = this->GlobalData->Attributes.at(Attribute::CircleAngularSpeed) * TO_DEG;
						proj.Radius = this->GlobalData->Attributes.at(Attribute::CircleRadius);
					}
				}

				it = this->GlobalData->Events.erase(it);
				continue;
			}

			case Event::GreenbullExpire:
				if (it->second <= ticks)
				{
					this->GlobalData->Attributes.erase(Attribute::Greenbull);
					it = this->GlobalData->Events.erase(it);
					continue;
				}
				break;

			case Event::MilkExpire:
				if (it->second <= ticks)
				{
					this->GlobalData->Attributes.erase(Attribute::Milk);
					it = this->GlobalData->Events.erase(it);
					continue;
				}
				break;

			case Event::PoisonExpire:
				if (it->second <= ticks)
				{
					this->GlobalData->Attributes.erase(Attribute::PoisonDamage);
					this->GlobalData->Events.erase(Event::PoisonTick);
					it = this->GlobalData->Events.erase(it);
					continue;
				}
				break;

			case Event::DrunkExpire:
				if (it->second <= ticks)
				{
					this->GlobalData->Attributes.erase(Attribute::Drunk);
					it = this->GlobalData->Events.erase(it);
					continue;
				}
				break;

			case Event::AussieExpire:
				if (it->second <= ticks)
				{
					this->GlobalData->Attributes.erase(Attribute::Aussie);
					it = this->GlobalData->Events.erase(it);
					continue;
				}
				break;

			case Event::PoisonTick:
				if (it->second <= ticks)
				{
					this->PlayerInstance->Health -= this->GlobalData->Attributes.at(Attribute::PoisonDamage);
					it->second += 240;
				}
				break;

			case Event::IncreasePlayerSpeed:
				this->PlayerInstance->Speed *= 1.2;
				it = this->GlobalData->Events.erase(it);
				break;

			case Event::MagnetismExpire:
				if (it->second <= ticks)
				{
					this->GlobalData->Attributes.erase(Attribute::Magnetism);
					it = this->GlobalData->Events.erase(it);
					continue;
				}
				break;
			case Event::AuraTick:
				if (it->second <= ticks)
				{
					this->PlayerInstance->Aura.width = this->GlobalData->Attributes.at(Attribute::AuraSize);
					this->PlayerInstance->Aura.height = this->GlobalData->Attributes.at(Attribute::AuraSize);

					float damage = this->GlobalData->Attributes.at(Attribute::AuraDamage);
					for (auto& enemy : this->Enemies)
					{
						if (CheckCollisionRecs(this->PlayerInstance->Aura, enemy.Rect))
							enemy.Health -= damage;
							enemy.FlashSprite(ticks);
					}
					this->GlobalData->Events[Event::AuraTick] = ticks + this->GlobalData->Attributes.at(Attribute::AuraCooldown);
				}
				break;
		}
		it++;
	}
}

void Game::UpdateThread1()
{
	while (true)
	{
		(void) this->StartWorkers.arrive_and_wait();

		if (!this->RunThreads)
			break;

		for (auto &projectile : this->Projectiles)
		{
			switch (projectile.Type)
			{
				case ProjectileType::Circle:
					projectile.Update(this->PlayerInstance->Rect);
					Collisions::ProjectileCollision(projectile, this->Enemies, *this->GlobalData);
					break;
				default:
					if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
					{
						projectile.Update(this->PlayerInstance->Rect);

						Game::LoopOverMap(projectile.Rect);
						
						Collisions::ProjectileCollision(projectile, this->Enemies, *this->GlobalData);
					}
					else
						projectile.Kill = true;
					break;
			}
		}
		std::erase_if(this->Projectiles, [](const Projectile& proj){ return proj.Kill; });

		for (auto &enemy : this->Enemies)
		{
			if (CheckCollisionRecs(this->UpdateArea, enemy.Rect))
			{
				enemy.Update(this->PlayerInstance->Rect, this->GlobalData->Ticks);
				Game::LoopOverMap(enemy.Rect);

				if (!this->GlobalData->Attributes.count(Attribute::Greenbull))
					Collisions::LeAttack(*(this->PlayerInstance), enemy, *this->GlobalData);
			}

			if (enemy.Health <= 0)
			{
				unsigned int value = EnemyXpValues.at(enemy.Type);
				this->Xps.emplace_back(enemy.Rect.x, enemy.Rect.y, value, *this->Assets);
			}
		}
		std::erase_if(this->Enemies, [](Enemy& enemy) { return (enemy.Health <= 0); });

		(void) this->StopWorkers.arrive_and_wait();
	}
}
