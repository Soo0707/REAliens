#include "game.hpp"


#include <memory>
#include <algorithm>
#include <array>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "globalDataWrapper.hpp"
#include "gameEventHandlers.hpp"
#include "constants.hpp"

#include "player.hpp"
#include "assetManager.hpp"
#include "projectiles.hpp"
#include "collisions.hpp"
#include "enemy.hpp"
#include "enemyData.hpp"
#include "xp.hpp"


Game::Game(std::shared_ptr<GlobalDataWrapper> global_data) :
	GlobalData(global_data)
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
}

Game::~Game()
{}

void Game::Draw()
{
	static unsigned int map_width = this->Assets->Ground.width;
	static unsigned int map_height = this->Assets->Ground.height;

	Rectangle viewport = this->UpdateArea;

	if (viewport.x < 0)
	{
		viewport.width += viewport.x;
		viewport.x = 0;
	}
	
	if (viewport.x + viewport.width > map_width)
		viewport.width = map_width - viewport.x;

	if (viewport.y < 0)
	{
		viewport.height += viewport.y;
		viewport.y = 0;
	}
	
	if (viewport.y + viewport.height > map_height)
		viewport.height = map_height - viewport.y;
	
	BeginMode2D(this->Camera);

	DrawTextureRec(this->Assets->Ground, viewport, (Vector2) { viewport.x, viewport.y }, WHITE);

	for (auto const &xp : this->Xps)
	{
		if (CheckCollisionRecs(this->UpdateArea, xp.Rect))
			xp.Draw();
	}
	
	for (auto &enemy : this->Enemies)
	{
		if (CheckCollisionRecs(this->UpdateArea, enemy.Rect))
			enemy.Draw();
	}
	
	for (auto const &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
			projectile.Draw();
	}

	this->PlayerInstance->Draw();

	EndMode2D();

	Game::DrawOverlay();
}

void Game::DrawOverlay()
{
	static constexpr Rectangle xp_background = { 100, 680, 1080, 15 };
	static constexpr Rectangle health_background = { 1060, 20, 200, 10 };

	static constexpr Rectangle magnetism_half_1 = { 1185, 40, 7.5, 15 };
	static constexpr Rectangle magnetism_half_2 = { 1192.5, 40, 7.5, 15 };

	static constexpr Rectangle greenbull_square = { 1205, 40, 15, 15 };
	static constexpr Rectangle milk_square = { 1225, 40, 15, 15 };
	static constexpr Rectangle drunk_square = { 1245, 40, 15, 15 };

	float health_percentage = this->PlayerInstance->Health / this->PlayerInstance->HealthMax;

	Rectangle health_bar = {
		1060,
		20,
		health_percentage * 200,
		10
	};

	bool is_poisoned = this->GlobalData->Effects.count(Effect::Poison);

	float xp_percentage = (float) this->CollectedXp / (float) this->GlobalData->LevelUpTreshold;

	Rectangle xp_bar = {
		100,
		680,
		xp_percentage * 1080,
		15
	};

	DrawRectangleRec(health_background, BLACK);
	DrawRectangleRec(health_bar, (is_poisoned) ? VIOLET : GREEN);

	DrawRectangleRec(xp_background, BLACK);
	DrawRectangleRec(xp_bar, (Color) { 0, 243, 255, 255 });

	if (this->GlobalData->Effects.count(Effect::Greenbull))
		DrawRectangleRec(greenbull_square, GREEN);

	if (this->GlobalData->Effects.count(Effect::Milk))
		DrawRectangleRec(milk_square, WHITE);
	
	if (this->GlobalData->Effects.count(Effect::Drunk))
		DrawRectangleRec(drunk_square, YELLOW);

	if (this->GlobalData->Effects.count(Effect::Magnetism))
	{
		DrawRectangleRec(magnetism_half_1, DARKBLUE);
		DrawRectangleRec(magnetism_half_2, RED);
	}
}


void Game::Update()
{
	this->Accumulator += GetFrameTime();

	if (this->Accumulator >= MAX_TICK_TIME)
		this->Accumulator = MAX_TICK_TIME;
	
	while (this->Accumulator >= TICK_TIME)
	{
		size_t ticks = this->GlobalData->Ticks;
		
		if (this->PlayerInstance->Health <= 0 && !this->GlobalData->Settings.at(Setting::DisableHealthCheck))
			this->GlobalData->ActiveState = State::GameOverMenu;
		
		if (ticks - this->LastLMB >= this->GlobalData->Attributes[Attribute::BulletCooldown])
			this->CanLMB = true;

		if (ticks - this->LastRMB >= this->GlobalData->Attributes[Attribute::LazerCooldown])
			this->CanRMB = true;
	
		if (this->CollectedXp >= this->GlobalData->LevelUpTreshold)
		{
			this->GlobalData->Level++;

			this->GlobalData->UnclaimedPowerups++;

			this->CollectedXp = 0;
			this->GlobalData->LevelUpTreshold += 2;

			if (this->GlobalData->Settings.at(Setting::ShowPowerupMenuOnLevelUp))
				this->GlobalData->ActiveState = State::PowerupMenu;
		}

		Game::HandleTickedInput();
		GameEventHandler::HandleEvents(*this);

		this->PlayerInstance->Update(ticks);
		Game::LoopOverMap(this->PlayerInstance->Rect);


		this->UpdateArea.x = this->PlayerInstance->Centre.x - GetScreenWidth() / 2.0f;
		this->UpdateArea.y =  this->PlayerInstance->Centre.y - GetScreenHeight() / 2.0f;
		
		this->Camera.target = this->PlayerInstance->Centre;

		if (this->GlobalData->Effects.count(Effect::Aussie))
			this->Camera.rotation = 180.0f;
		else
			this->Camera.rotation = 0.0f;


		Game::UpdateEnemies();
		Game::UpdateProjectiles();
		Game::UpdateXps();


		this->Accumulator -= TICK_TIME;
		this->GlobalData->Ticks++;
	} 
}

void Game::UpdateEnemies()
{
	if ((this->GlobalData->Ticks - this->LastSpawn >= this->SpawnTimeout) || this->Enemies.size() == 0)
	{
		Game::SpawnEnemies();
		this->LastSpawn = this->GlobalData->Ticks;
	}

	bool has_greenbull = this->GlobalData->Effects.count(Effect::Greenbull);

	for (auto &enemy : this->Enemies)
	{
		if (CheckCollisionRecs(this->UpdateArea, enemy.Rect))
		{
			enemy.Update(this->PlayerInstance->Rect, this->GlobalData->Ticks);
			Game::LoopOverMap(enemy.Rect);
			
			if (!has_greenbull)
				Collisions::LeAttack(*(this->PlayerInstance), enemy, *this->GlobalData);
		}

		if (enemy.Health <= 0)
		{
			unsigned int value = EnemyXpValues.at(enemy.Type);
			this->Xps.emplace_back(enemy.Rect.x, enemy.Rect.y, value, *this->Assets);
		}
	}
	std::erase_if(this->Enemies, [](const Enemy& enemy) { return (enemy.Health <= 0); });
}

void Game::UpdateProjectiles()
{
	unsigned int damage_done = 0;
	for (auto &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
		{
			projectile.Update(this->PlayerInstance->Centre);

			if (projectile.Type != ProjectileType::Circle)
				Game::LoopOverMap(projectile.Rect);
			
			damage_done += Collisions::ProjectileCollision(projectile, this->Enemies, *this->GlobalData);
		}
		else if (projectile.Killable)
			projectile.Kill = true;
	}
	std::erase_if(this->Projectiles, [](const Projectile& proj){ return proj.Kill; });

	if (this->GlobalData->Effects.count(Effect::LifeSteal))
		this->PlayerInstance->IncreaseHealth( damage_done * this->GlobalData->Attributes.at(Attribute::LifeStealMultiplier) );
}

void Game::UpdateXps()
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



void Game::HandleEssentialInput()
{
	if (IsKeyPressed(KEY_ESCAPE))
		this->GlobalData->ActiveState = State::PauseMenu;

	if (IsKeyPressed(KEY_TAB) && this->GlobalData->UnclaimedPowerups > 0)
		this->GlobalData->ActiveState = State::PowerupMenu;
}

void Game::HandleTickedInput()
{
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

void Game::HandleLeftClick()
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
}

void Game::HandleRightClick()
{
	static constexpr std::array<Vector2, 4> directions = { Vector2{1.0f, 0.0f}, Vector2{0.0f, 1.0f}, Vector2{-1.0f, 0.0f}, Vector2{0.0f, -1.0f} };

	Vector2 centre = this->PlayerInstance->Centre;
	
	for (int i = 0; i < 4; i++)
		this->Projectiles.emplace_back(centre.x, centre.y, directions[i], ProjectileType::Lazer, *this->GlobalData, *this->Assets);
}



void Game::SpawnEnemies()
{
	std::vector<Vector2> rand_nums;

	for (size_t i = 0; i < this->GlobalData->Level * 5; i++)
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

		EnemyType type = (EnemyType) GetRandomValue(0, 4);
		
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
