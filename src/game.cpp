#include <memory>
#include <iostream>

#include "raylib.h"
#include "raymath.h"
#include "tmx.h"

#include "game.hpp"
#include "player.hpp"
#include "assetManager.hpp"
#include "gameObjects.hpp"

Game::Game()
{
	this->AssetManagerInstance = std::make_unique<AssetManager>();

	std::shared_ptr PlayerInstance = std::make_shared<Player>(500, 500, *(this->AssetManagerInstance));

	this->AllObjects.push_back(PlayerInstance);
	this->UpdatableObjects.push_back(PlayerInstance);

	this->PlayerInstance = PlayerInstance;

	tmx_map *map = tmx_load("assets/map/map.tmx");
	
	if (map == nullptr)
		exit(1);

	tmx_layer *walls_layer = tmx_find_layer_by_name(map, "Walls");
	//tmx_layer *props_layer = tmx_find_layer_by_name(map, "Props");
	//tmx_layer *spawners_layer = tmx_find_layer_by_name(map, "Spawners");

	Game::InitialiseMapObjects(map, walls_layer);
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
	for (auto const &obj : this->AllObjects)
		obj->Draw();
	
	EndMode2D();
	EndDrawing();
}

void Game::Update()
{
	for (auto const &obj : this->UpdatableObjects)
		obj->Update();
	
	this->Camera.target = {this->PlayerInstance->Rect.x, this->PlayerInstance->Rect.y};
}

void Game::HandleInput()
{
	this->PlayerInstance->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	this->PlayerInstance->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);

	if (this->PlayerInstance->Direction.x != 0.0f && this->PlayerInstance->Direction.y != 0.0f)
		this->PlayerInstance->Direction = Vector2Normalize(this->PlayerInstance->Direction);
}

void Game::InitialiseMapObjects(tmx_map* map, tmx_layer* layer)
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

			this->AllObjects.push_back(
					std::make_shared<Wall>(
						(float) cell_x * TILESIZE, (float) cell_y * TILESIZE, this->AssetManagerInstance->MapTextures[cell]
						)
					);
		}
	}
}
