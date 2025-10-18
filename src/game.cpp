#include <memory>
#include <algorithm>
#include <array>

#include "raylib.h"
#include "raymath.h"

#include "globalDataWrapper.hpp"
#include "game.hpp"
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
}

void Game::Update()
{
	Game::HandleEssentialInput();

	this->Accumulator += GetFrameTime();

	if (this->Accumulator >= MAX_TICK_TIME)
		this->Accumulator = MAX_TICK_TIME;
	
	while (this->Accumulator >= TICK_TIME)
	{
		size_t ticks = this->GlobalData->Ticks;
		/*
		if (this->PlayerInstance->Health <= 0)
			this->GlobalData->ActiveState = State::GameOverMenu;
		*/
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
		Game::HandleEvents();

		this->PlayerInstance->Update(ticks);
		
		Game::LoopOverMap(this->PlayerInstance->Rect);
	

		this->UpdateArea.x = this->PlayerInstance->Centre.x - GetScreenWidth() / 2.0f;
		this->UpdateArea.y =  this->PlayerInstance->Centre.y - GetScreenHeight() / 2.0f;
		

		this->Camera.target = this->PlayerInstance->Centre;

		if (this->GlobalData->Effects.count(Effect::Aussie))
			this->Camera.rotation = 180.0f;
		else
			this->Camera.rotation = 0.0f;


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


		for (auto &projectile : this->Projectiles)
		{
			if (CheckCollisionRecs(this->UpdateArea, projectile.Rect))
			{
				projectile.Update(this->PlayerInstance->Centre);

				if (projectile.Type != ProjectileType::Circle)
					Game::LoopOverMap(projectile.Rect);
				
				Collisions::ProjectileCollision(projectile, this->Enemies, *this->GlobalData);
			}
			else if (projectile.Killable)
				projectile.Kill = true;
		}
		std::erase_if(this->Projectiles, [](const Projectile& proj){ return proj.Kill; });


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

		this->Accumulator -= TICK_TIME;
		this->GlobalData->Ticks++;
	} 
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

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && this->CanLMB)
	{
		Game::HandleLeftClick();
		this->CanLMB = false;
		this->LastLMB = this->GlobalData->Ticks;
	}
	
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && this->CanRMB)
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



void Game::HandleEvents()
{
	size_t ticks = this->GlobalData->Ticks;

	std::unordered_map<Event, size_t> new_events_map;

	for (auto &pair : this->GlobalData->Events)
	{
		switch (pair.first)
		{
			case Event::UpgradeCircle:
				Game::EventUpgradeCircle();
				break;

			case Event::SpawnCircle:
				Game::EventSpawnCircle();
				break;

			case Event::GreenbullExpire:
				Game::HandleEventExpiry(Event::GreenbullExpire, Effect::Greenbull, pair.second, new_events_map);
				break;

			case Event::MilkExpire:
				Game::HandleEventExpiry(Event::MilkExpire, Effect::Milk, pair.second, new_events_map);
				break;

			case Event::DrunkExpire:
				Game::HandleEventExpiry(Event::DrunkExpire, Effect::Drunk, pair.second, new_events_map);
				break;

			case Event::AussieExpire:
				Game::HandleEventExpiry(Event::AussieExpire, Effect::Aussie, pair.second, new_events_map);
				break;

			case Event::MagnetismExpire:
				Game::HandleEventExpiry(Event::MagnetismExpire, Effect::Magnetism, pair.second, new_events_map);
				break;
			
			case Event::PoisonExpire:
				if (Game::HandleEventExpiry(Event::PoisonExpire, Effect::Poison, pair.second, new_events_map))
				{
					/*
					 we cannot delete they key immediately as we might not have iterated through poison tick
					 which needs this. so we just don't copy PoisonExpire into the new map,
					 then signal using -1 saying so that PoisonTick can handle PoisonDamage's deletion
					*/
					this->GlobalData->Attributes[Attribute::PoisonDamage] = -1.0f;
				}
				break;

			case Event::PoisonTick:
				Game::EventPoisonTick(pair.second, new_events_map);
				break;

			case Event::IncreasePlayerSpeed:
				this->PlayerInstance->Speed *= 1.2;
				break;
			case Event::IncreaseAura:
				this->PlayerInstance->Aura.width = this->GlobalData->Attributes.at(Attribute::AuraSize);
				this->PlayerInstance->Aura.height = this->GlobalData->Attributes.at(Attribute::AuraSize);
				break;
			case Event::AuraTick:
				Game::EventAuraTick(pair.second, new_events_map);
				break;
		}
	}
	this->GlobalData->Events.swap(new_events_map);
}

void Game::EventUpgradeCircle()
{
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
}

void Game::EventSpawnCircle()
{
	this->Projectiles.emplace_back(
			this->PlayerInstance->Centre.x,
			this->PlayerInstance->Centre.y,
			(Vector2) { 0, 0 },
			ProjectileType::Circle,
			*this->GlobalData,
			*this->Assets
			);
}

bool Game::HandleEventExpiry(Event event, Effect effect, size_t expiry, std::unordered_map<Event, size_t>& new_events_map)
{
	if (expiry >= this->GlobalData->Ticks)
	{
		new_events_map[event] = expiry;
		return false;
	}
	else
	{
		this->GlobalData->Effects.erase(effect);
		return true;
	}
}

void Game::EventAuraTick(size_t next_tick, std::unordered_map<Event, size_t>& new_events_map)
{
	if (next_tick >= this->GlobalData->Ticks)
		new_events_map[Event::AuraTick] = next_tick;
	else
	{
		float damage;

		damage = this->GlobalData->Attributes.at(Attribute::AuraDamage);

		for (auto &enemy : this->Enemies)
			Collisions::Aura(damage, this->GlobalData->Ticks, this->PlayerInstance->Aura, enemy);

		new_events_map[Event::AuraTick] = this->GlobalData->Ticks + this->GlobalData->Attributes.at(Attribute::AuraCooldown);
	}
}

void Game::EventPoisonTick(size_t next_tick, std::unordered_map<Event, size_t>& new_events_map)
{
	if (next_tick >= this->GlobalData->Ticks)
		new_events_map[Event::PoisonTick] = next_tick;
	else
	{
		// damage may be -1 if poison expired, destruction of PoisonDamage and PoisonTick both happen here
		float damage = this->GlobalData->Attributes.at(Attribute::PoisonDamage);

		if (damage > 0)
		{
			this->PlayerInstance->Health -= damage;
			new_events_map[Event::PoisonTick] = next_tick + SECONDS_TO_TICKS(1);
		}
		else
			this->GlobalData->Attributes.erase(Attribute::PoisonDamage);
	}
}
