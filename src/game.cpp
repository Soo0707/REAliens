#include <memory>
#include <cmath>
#include <iostream>

#include "raylib.h"
#include "raymath.h"
#include "tmx.h"

#include "game.hpp"
#include "player.hpp"
#include "assetManager.hpp"
#include "gameObjects.hpp"
#include "projectiles.hpp"
#include "collisions.hpp"

Game::Game()
{
	this->AssetManagerInstance = std::make_unique<AssetManager>();

	this->PlayerInstance = std::make_unique<Player>(500, 500, *(this->AssetManagerInstance));


	tmx_map *map = tmx_load("assets/map/map.tmx");
	
	if (map == nullptr)
		exit(1);

	tmx_layer *walls_layer = tmx_find_layer_by_name(map, "Walls");
	tmx_layer *props_layer = tmx_find_layer_by_name(map, "Props");
	tmx_layer *spawners_layer = tmx_find_layer_by_name(map, "Spawners");
	
	if (walls_layer == nullptr || props_layer == nullptr || spawners_layer == nullptr) 
		exit(1);

	Game::InitialiseMapObjects(map, walls_layer, WALLS_LAYER);
	Game::InitialiseMapObjects(map, props_layer, PROPS_LAYER);
	Game::InitialiseMapObjects(map, spawners_layer, SPAWNERS_LAYER);
	tmx_map_free(map);

	this->Camera = { 0 };
	this->Camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
	this->Camera.rotation = 0.0f;
    this->Camera.zoom = 1.0f;
}

Game::~Game()
{}

void Game::Draw()
{
	BeginDrawing();
	ClearBackground(RAYWHITE);
	BeginMode2D(this->Camera);
	
	DrawTexture(this->AssetManagerInstance->Ground, 0, 0, WHITE);

	for (auto const &wall : this->Walls)
		wall.Draw();

	for (auto const &prop : this->Props)
		prop.Draw();

	for (auto const &projectile : this->Projectiles)
		projectile.Draw();

	this->PlayerInstance->Draw();

	EndMode2D();
	EndDrawing();
}

void Game::Update()
{
	this->PlayerInstance->MoveX();
	Collisions::ResolveCollisionPlayerX(*(this->PlayerInstance), this->Walls, this->Props);
	
	this->PlayerInstance->MoveY();
	Collisions::ResolveCollisionPlayerY(*(this->PlayerInstance), this->Walls, this->Props);

	this->PlayerInstance->Update();
	
	for (auto &projectile : this->Projectiles)
		projectile.Update();

	this->Camera.target = {this->PlayerInstance->Rect.x, this->PlayerInstance->Rect.y};
}

void Game::HandleInput()
{
	this->PlayerInstance->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	this->PlayerInstance->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);

	if (this->PlayerInstance->Direction.x != 0.0f && this->PlayerInstance->Direction.y != 0.0f)
		this->PlayerInstance->Direction = Vector2Normalize(this->PlayerInstance->Direction);

	if (IsMouseButtonPressed(0))
	{
		Rectangle player_rect = this->PlayerInstance->Rect;

		Vector2 player_centre = {player_rect.x + player_rect.width / 2, player_rect.y + player_rect.height / 2 };
		Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), this->Camera);

		Vector2 direction = Vector2Subtract(mouse_pos, player_centre);
		
		if (direction.x != 0.0f || direction.y != 0.0f)
			direction = Vector2Normalize(direction);
		
		Texture2D texture = this->AssetManagerInstance->StaticTextures[StaticTextureKey::Projectile];

		float angle = atan2(direction.y, direction.x) * 180 / 3.142;

		this->Projectiles.emplace_back(player_centre.x, player_centre.y, texture, 600, direction, angle, 1.0f);
	
	}
}

void Game::InitialiseMapObjects(tmx_map* map, tmx_layer* layer, const unsigned int type)
{
	for (int cell_y = 0; cell_y < map->height; cell_y++)
	{
		for (int cell_x = 0; cell_x < map->width; cell_x++)
		{
			int32_t cell = layer->content.gids[cell_y * map->width + cell_x];
			int32_t GID = cell & TMX_FLIP_BITS_REMOVAL;

			if (GID == 0)
				continue;

			if (!this->AssetManagerInstance->MapTextures.count(cell))
				exit(1);

			Texture2D texture = this->AssetManagerInstance->MapTextures[cell];
			float x_pos = cell_x * TILESIZE;
			float y_pos = cell_y * TILESIZE;

			switch (type)
			{
				case WALLS_LAYER:
				{
					this->Walls.emplace_back(x_pos, y_pos, texture);
				}
				case PROPS_LAYER:
				{
					this->Props.emplace_back(x_pos, y_pos, texture);

				}
				case SPAWNERS_LAYER:
				{
					this->Spawners.emplace_back(x_pos, y_pos, texture);
				}
			}

		}
	}
}
