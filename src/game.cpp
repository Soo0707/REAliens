#include <memory>

#include <iostream>

#include "raylib.h"
#include "raymath.h"


#include "game.hpp"
#include "player.hpp"
#include "assetManager.hpp"
#include "gameObjects.hpp"



Game::Game()
{
/*
	this->Ground = LoadTexture("/mnt/tmpfs/assets/new_map_format/ground.png");
	for (int x = 0; x * 32 <= ground.width; x++)
	{
		for (int y = 0; y * 32 <= ground.height; y++)
		{
			this->AllObjects.push_back(std::make_shared<BasicGameObject>((float) x, (float) y, LoadTextureFromImage(ImageFromImage(ground, {(float) x, (float) y, 32, 32}))));
		}
	}
	*/
	this->AssetManagerInstance = std::make_unique<AssetManager>();

	std::shared_ptr PlayerInstance = std::make_shared<Player>(500, 500, *(this->AssetManagerInstance));

	this->AllObjects.push_back(PlayerInstance);
	this->UpdatableObjects.push_back(PlayerInstance);

	this->PlayerInstance = PlayerInstance;

}

Game::~Game()
{}

void Game::Draw()
{
	//DrawTexture(this->Ground, 0, 0, WHITE);
	for (auto const &obj : this->AllObjects)
		obj->Draw();
}

void Game::Update()
{
	for (auto const &obj : this->UpdatableObjects)
		obj->Update();
}

void Game::HandleInput()
{
	this->PlayerInstance->Direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	this->PlayerInstance->Direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);

	if (this->PlayerInstance->Direction.x != 0.0f && this->PlayerInstance->Direction.y != 0.0f)
		this->PlayerInstance->Direction = Vector2Normalize(this->PlayerInstance->Direction);
}
