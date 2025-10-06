#include <memory>
#include <cmath>
#include <algorithm>
#include <array>
#include <iostream>
#include "raylib.h"
#include "raymath.h"

#include "game.hpp"
#include "globalDataWrapper.hpp"

#include "player.hpp"
#include "assetManager.hpp"
#include "projectiles.hpp"
#include "collisions.hpp"
#include "enemy.hpp"

Game::Game(std::shared_ptr<GlobalDataWrapper> global_data) : GlobalData(global_data)
{
	this->Assets = std::make_shared<AssetManager>();

	this->PlayerInstance = std::make_unique<Player>(500, 500, *(this->Assets));

	this->Camera = { 0 };
	this->Camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
	this->Camera.rotation = 0.0f;
    this->Camera.zoom = 1.0f;
	
	Vector2 player_pos = { this->PlayerInstance->Rect.x, this->PlayerInstance->Rect.y };
	this->UpdateArea = {player_pos.x - (GetScreenWidth() / 2.0f), player_pos.y - (GetScreenHeight() / 2.0f), (float) GetScreenWidth(), (float) GetScreenHeight()};

	this->Projectiles.emplace_back(500, 500, (Vector2) {0, 0}, ProjectileType::Circle, *this->GlobalData, *this->Assets);
}

Game::~Game()
{}

void Game::Draw()
{
	if (this->GlobalData->Attributes.count(Attribute::Aussie))
		this->Camera.rotation = 180.0f;

	BeginMode2D(this->Camera);
	
	DrawTexture(this->Assets->Ground, 0, 0, WHITE);

	for (auto const &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
			projectile.Draw();
	}

	for (auto &enemy : this->Enemies)
	{
		if (CheckCollisionRecs(this->UpdateArea, enemy.Rect))
			enemy.Animate();
			enemy.Draw();
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
		if (this->Ticks - this->LastLMB >= this->GlobalData->Attributes[Attribute::BulletCooldown])
			this->CanLMB = true;

		if (this->Ticks - this->LastRMB >= this->GlobalData->Attributes[Attribute::LazerCooldown])
			this->CanRMB = true;
		/*
		if ((this->Ticks - this->LastSpawn >= this->SpawnTimeout) || this->Enemies.size() == 0)
		{
			Game::SpawnEnemies();
			this->LastSpawn = this->Ticks;
		}
		*/
		this->PlayerInstance->Update();

		Game::LoopOverMap(this->PlayerInstance->Rect);
		
		this->UpdateArea.x = this->PlayerInstance->Rect.x - GetScreenWidth() / 2.0f;
		this->UpdateArea.y = this->PlayerInstance->Rect.y - GetScreenHeight() / 2.0f;
		
		this->Camera.target = { this->PlayerInstance->Rect.x, this->PlayerInstance->Rect.y };

		for (auto &projectile : this->Projectiles)
		{
			if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
			{
				projectile.Update(this->PlayerInstance->Rect);

				if (projectile.Type != ProjectileType::Circle)
					Game::LoopOverMap(projectile.Rect);
				
				Collisions::ProjectileCollision(projectile, this->Enemies, *this->GlobalData, this->Ticks);
			}
			else
				projectile.Kill = true;
		}
		std::erase_if(this->Projectiles, [](Projectile& proj){ return proj.Kill; });

		for (auto &enemy : this->Enemies)
		{
			if (CheckCollisionRecs(this->UpdateArea, enemy.Rect))
			{
				enemy.Update(this->PlayerInstance->Rect, this->Ticks);
				Game::LoopOverMap(enemy.Rect);
				Collisions::LeAttack(*(this->PlayerInstance), enemy, *this->GlobalData);
			}
		}
		std::erase_if(this->Enemies, [](Enemy& enemy) { return (enemy.Health <= 0); });

		this->Accumulator -= TICK_TIME;
		this->Ticks++;
	} 

}

void Game::HandleInput()
{
	this->PlayerInstance->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	this->PlayerInstance->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);

	if (this->PlayerInstance->Direction.x != 0.0f && this->PlayerInstance->Direction.y != 0.0f)
		this->PlayerInstance->Direction = Vector2Normalize(this->PlayerInstance->Direction);

	if (IsMouseButtonDown(0) && this->CanLMB)
	{
		Rectangle player_rect = this->PlayerInstance->Rect;

		Vector2 player_centre = { player_rect.x + player_rect.width / 2, player_rect.y + player_rect.height / 2 };
		Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), this->Camera);

		Vector2 centre_direction = Vector2Subtract(mouse_pos, player_centre);
		
		if (centre_direction.x != 0.0f || centre_direction.y != 0.0f)
			centre_direction = Vector2Normalize(centre_direction);

		float centre_rotation = atan2(centre_direction.y, centre_direction.x) * 180 / 3.142;

		Texture2D texture = this->Assets->StaticTextures[StaticTextureKey::Bullet];

		this->Projectiles.emplace_back(player_centre.x, player_centre.y, centre_direction, ProjectileType::Bullet, *this->GlobalData, *this->Assets);

		float spread_angle = this->GlobalData->Attributes[Attribute::BuckshotSpread];
		int buckshot = (int) (this->GlobalData->Attributes[Attribute::Buckshot] - 1) / 2;

		for (int i = 1; i <= buckshot; i++)
		{
			// result of vector * rotation matrix
			Vector2 direction_pos = {
				centre_direction.x * cos(spread_angle * i) - centre_direction.y * sin(spread_angle * i),
				centre_direction.x * sin(spread_angle * i) + centre_direction.y * cos(spread_angle * i) 
			};

			Vector2 direction_neg = {
				centre_direction.x * cos(-spread_angle * i) - centre_direction.y * sin(-spread_angle * i),
				centre_direction.x * sin(-spread_angle * i) + centre_direction.y * cos(-spread_angle * i) 
			};

			this->Projectiles.emplace_back(player_centre.x, player_centre.y, direction_pos, ProjectileType::Bullet, *this->GlobalData, *this->Assets);
			this->Projectiles.emplace_back(player_centre.x, player_centre.y, direction_neg, ProjectileType::Bullet, *this->GlobalData, *this->Assets);
		}

		this->CanLMB = false;
		this->LastLMB = this->Ticks;
	}
	
	if (IsMouseButtonDown(1) && this->CanRMB)
	{
		static constexpr std::array<Vector2, 4> directions = { Vector2{1.0f, 0.0f}, Vector2{0.0f, 1.0f}, Vector2{-1.0f, 0.0f}, Vector2{0.0f, -1.0f} };

		Rectangle player_rect = this->PlayerInstance->Rect;

		Vector2 player_centre = { player_rect.x + player_rect.width / 2, player_rect.y + player_rect.height / 2 };
		
		for (int i = 0; i < 4; i++)
			this->Projectiles.emplace_back(player_centre.x, player_centre.y, directions[i], ProjectileType::Lazer, *this->GlobalData, *this->Assets);

		this->CanRMB = false;
		this->LastRMB = this->Ticks;
	}
}

void Game::SpawnEnemies()
{
	std::vector<Vector2> rand_nums;

	for (int i = 0; i < this->GlobalData->Level * 5; i++)
		rand_nums.emplace_back(Vector2{ (float) GetRandomValue(32, (int) this->UpdateArea.width / 2), (float) GetRandomValue(32, (int) this->UpdateArea.height / 2) });

	for (auto &location : rand_nums)
	{
		if (GetRandomValue(1, 50) % 2)
			location.x *= -1;

		if (GetRandomValue(1, 50) % 2)
			location.y *= -1;

		float x = this->PlayerInstance->Rect.x + location.x;
		float y = this->PlayerInstance->Rect.y + location.y;

		EnemyType type = (EnemyType) GetRandomValue(0, 6);

		this->Enemies.emplace_back(x, y, this->Assets, type, BehaviourModifier::None);
	}
}

void Game::LoopOverMap(Rectangle& m_obj)
{
	static float map_width = this->Assets->Ground.width;
	static float map_height = this->Assets->Ground.height;

	if (m_obj.x < 0)
		m_obj.x = map_width - m_obj.width;
	else if (m_obj.x > map_width)
		m_obj.x = 0;

	if (m_obj.y < 0)
		m_obj.y = map_height - m_obj.height;
	else if (m_obj.y > map_height)
		m_obj.y = 0;
}
/*
void Game::HandleEvents()
{
	for (auto it = this->Events.begin(); it != this->Events.end();)
	{
		switch(it->first)
		{
			case EventKey::GreenbullExpire:
				break;
			case EventKey::MilkExpire:
				break;
			case EventKey::PoisonedExpire:
				break;
			case EventKey::DrunkExpire:
				break;
			case EventKey::AussieExpire:
				break;
		}
		it++;
	}
}
*/
