#include <memory>

#include <iostream>

#include "raylib.h"
#include "raymath.h"

#include "tmx.h"

#include "game.hpp"
#include "player.hpp"
#include "assetManager.hpp"


Game::Game()
{
	this->AssetManagerInstance = std::make_unique<AssetManager>();

	std::shared_ptr PlayerInstance = std::make_shared<Player>(500, 500, *(this->AssetManagerInstance));

	this->AllObjects.push_back(PlayerInstance);
	this->UpdatableObjects.push_back(PlayerInstance);

	this->PlayerInstance = PlayerInstance;
}

Game::~Game()
{
}

void Game::Draw()
{
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
