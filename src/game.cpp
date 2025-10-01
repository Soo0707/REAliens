#include <memory>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <array>
#include <unordered_map>

#include "raylib.h"
#include "raymath.h"

#include "game.hpp"
#include "player.hpp"
#include "assetManager.hpp"
#include "gameObjects.hpp"
#include "projectiles.hpp"
#include "collisions.hpp"
#include "enemy.hpp"

Game::Game()
{
	this->AssetManagerInstance = std::make_shared<AssetManager>();

	this->PlayerInstance = std::make_unique<Player>(500, 500, *(this->AssetManagerInstance));

	this->Camera = { 0 };
	this->Camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
	this->Camera.rotation = 0.0f;
    this->Camera.zoom = 1.0f;
	
	Vector2 player_pos = { this->PlayerInstance->Rect.x, this->PlayerInstance->Rect.y };
	this->UpdateArea = {player_pos.x - (GetScreenWidth() / 2.0f), player_pos.y - (GetScreenHeight() / 2.0f), (float) GetScreenWidth(), (float) GetScreenHeight()};
}

Game::~Game()
{}

void Game::Draw()
{
	if (this->Effects.count(EffectKey::Aussie))
		this->Camera.rotation = 180.0f;

	BeginDrawing();
	ClearBackground(RAYWHITE);
	BeginMode2D(this->Camera);
	
	DrawTexture(this->AssetManagerInstance->Ground, 0, 0, WHITE);

	for (auto const &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
			projectile.Draw();
	}

	for (auto &enemy : this->Enemies)
	{
		if (CheckCollisionRecs(this->UpdateArea, enemy.Rect))
			enemy.Draw();
	}

	this->PlayerInstance->Draw();

	EndMode2D();
	EndDrawing();
}

void Game::Update()
{
	this->Accumulator += GetFrameTime();

	if (this->Accumulator >= MAX_TICK_TIME)
		this->Accumulator = MAX_TICK_TIME;
	
	while (this->Accumulator >= TICK_TIME);
	{
		this->PlayerInstance->Update();
		
		this->UpdateArea.x = this->PlayerInstance->Rect.x - GetScreenWidth() / 2.0f;
		this->UpdateArea.y = this->PlayerInstance->Rect.y - GetScreenHeight() / 2.0f;
		
		this->Camera.target = { this->PlayerInstance->Rect.x, this->PlayerInstance->Rect.y };

		for (auto &projectile : this->Projectiles)
		{
			if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
			{
				projectile.Update();
				Collisions::ProjectileCollision(projectile, this->Enemies, this->Ticks);
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
				Collisions::LeAttack(*(this->PlayerInstance), enemy, this->Effects, this->EffectTimeouts);
			}
		}
		std::erase_if(this->Enemies, [](Enemy& enemy) { return (enemy.Health <= 0); });

		this->Accumulator -= TICK_TIME;
		this->Ticks++;
	} 

	if (this->Ticks - this->LastLMB >= this->Effects[EffectKey::BulletCooldown])
		this->CanLMB = true;

	if (this->Ticks - this->LastRMB >= this->Effects[EffectKey::LazerCooldown])
		this->CanRMB = true;
/*
	if ((this->Ticks - this->LastSpawn >= this->SpawnTimeout) || this->Enemies.size() == 0)
	{
		Game::SpawnEnemies();
		this->LastSpawn = this->Ticks;
	}
	*/
}

void Game::HandleInput()
{
	this->PlayerInstance->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	this->PlayerInstance->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);

	if (this->PlayerInstance->Direction.x != 0.0f && this->PlayerInstance->Direction.y != 0.0f)
		this->PlayerInstance->Direction = Vector2Normalize(this->PlayerInstance->Direction);

	if (IsMouseButtonPressed(0) && this->CanLMB)
	{
		Rectangle player_rect = this->PlayerInstance->Rect;

		Vector2 player_centre = {player_rect.x + player_rect.width / 2, player_rect.y + player_rect.height / 2 };
		Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), this->Camera);

		Vector2 direction = Vector2Subtract(mouse_pos, player_centre);
		
		if (direction.x != 0.0f || direction.y != 0.0f)
			direction = Vector2Normalize(direction);
		
		Texture2D texture = this->AssetManagerInstance->StaticTextures[StaticTextureKey::Bullet];

		float speed = this->Effects[EffectKey::BulletSpeed];
		float damage = this->Effects[EffectKey::BulletDamage];

		float angle = atan2(direction.y, direction.x) * 180 / 3.142;

		this->Projectiles.emplace_back(player_centre.x, player_centre.y, texture, speed, direction, angle, 1.0f, ProjectileType::Bullet, damage);

		this->CanLMB = false;
		this->LastLMB = this->Ticks;
	}
	
	if (IsMouseButtonPressed(1) && this->CanRMB)
	{
		static constexpr std::array<Vector2, 4> directions = { Vector2{1.0f, 0.0f}, Vector2{0.0f, 1.0f}, Vector2{-1.0f, 0.0f}, Vector2{0.0f, -1.0f} };
		static constexpr std::array<float, 4> angles = { 0.0f, 90.0f, 0, 90.0f };

		Rectangle player_rect = this->PlayerInstance->Rect;

		Vector2 player_centre = { player_rect.x + player_rect.width / 2, player_rect.y + player_rect.height / 2 };
	
		Texture2D texture = this->AssetManagerInstance->StaticTextures[StaticTextureKey::Lazer];
		
		float speed = this->Effects[EffectKey::LazerSpeed];
		float damage = this->Effects[EffectKey::LazerDamage];
		float scale = this->Effects[EffectKey::LazerScale];

		for (int i = 0; i < 4; i++)
			this->Projectiles.emplace_back(player_centre.x, player_centre.y, texture, speed, directions[i], angles[i], scale, ProjectileType::Lazer, damage);

		this->CanRMB = false;
		this->LastRMB = this->Ticks;
	}
}

void Game::SpawnEnemies()
{
	std::vector<Vector2> rand_nums;

	for (int i = 0; i < this->Level * 5; i++)
		rand_nums.emplace_back(Vector2{ (float) GetRandomValue(-20, 20), (float) GetRandomValue(-20, 20) });

	for (auto const& location : rand_nums)
	{
		float x = this->PlayerInstance->Rect.x + location.x * 32;
		float y = this->PlayerInstance->Rect.y + location.y * 32;

		EnemyType type = (EnemyType) GetRandomValue(0, 6);

		this->Enemies.emplace_back(x, y, this->AssetManagerInstance, type, UniqueStates::None);
	}
}
