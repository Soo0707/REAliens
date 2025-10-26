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

void Game::Draw(RenderTexture2D& virtual_canvas)
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


	BeginTextureMode(virtual_canvas);
		ClearBackground(BLACK);

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
			{
				projectile.Draw();
			}
		}

		this->PlayerInstance->Draw();

		EndMode2D();
		Game::DrawOverlay();
	EndTextureMode();
}

void Game::DrawOverlay()
{
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

	DrawRectangleRec(HEALTH_BACKGROUND, BLACK);
	DrawRectangleRec(health_bar, (is_poisoned) ? VIOLET : GREEN);

	DrawRectangleRec(XP_BACKGROUND, BLACK);
	DrawRectangleRec(xp_bar, CYAN);

	if (this->GlobalData->Effects.count(Effect::Greenbull))
		DrawRectangleRec(GREENBULL_SQUARE, GREEN);

	if (this->GlobalData->Effects.count(Effect::Milk))
		DrawRectangleRec(MILK_SQUARE, WHITE);
	
	if (this->GlobalData->Effects.count(Effect::Drunk))
		DrawRectangleRec(DRUNK_SQUARE, YELLOW);

	if (this->GlobalData->Effects.count(Effect::Magnetism))
	{
		DrawRectangleRec(MAGNETISM_HALF_1, DARKBLUE);
		DrawRectangleRec(MAGNETISM_HALF_2, RED);
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
			Game::LevelUp();

		Game::HandleTickedInput();
		GameEventHandler::HandleEvents(*this);

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

void Game::UpdateProjectiles()
{
	unsigned int damage_done = 0;
	for (auto &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
		{
			projectile.Update(this->PlayerInstance->Centre);

			Game::LoopOverMap(projectile.Rect);
			
			damage_done += Collisions::ProjectileCollision(projectile, this->Enemies, *this->GlobalData);
		}
		else
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
	Vector2 scale_factors = { static_cast<float>(GetScreenWidth()) / REFERENCE_WIDTH, static_cast<float>(GetScreenHeight()) / REFERENCE_HEIGHT };
	Vector2 scaled_mouse_pos = { GetMouseX() / scale_factors.x, GetMouseY() / scale_factors.y };
	Vector2 mouse_pos = GetScreenToWorld2D(scaled_mouse_pos, this->Camera);

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
	static unsigned int map_width = this->Assets->Ground.width;
	static unsigned int map_height = this->Assets->Ground.height;

	std::vector<Vector2> rand_nums;

	for (size_t i = 0; i < this->GlobalData->Level * 15; i++)
	{
		rand_nums.emplace_back(
				(Vector2) {
				static_cast<float>( GetRandomValue(0, map_height) ),
				static_cast<float>( GetRandomValue(0, map_width) )
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

		this->Enemies.emplace_back(location.x, location.y, this->Assets, type, modifier);
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

void Game::LevelUp()
{
	this->GlobalData->Level++;

	this->GlobalData->UnclaimedPowerups++;

	this->CollectedXp = 0;
	this->GlobalData->LevelUpTreshold += 5;
	
	if (this->GlobalData->Settings.at(Setting::ShowPowerupMenuOnLevelUp))
		this->GlobalData->ActiveState = State::PowerupMenu;
}
