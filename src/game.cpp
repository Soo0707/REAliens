#include "game.hpp"

#include <memory>
#include <string>

#include "raylib.h"

#include "gameState.hpp"
#include "globalDataWrapper.hpp"

#include "gameEventSystem.hpp"
#include "gameInputSystem.hpp"
#include "gameDrawSystem.hpp"

#include "constants.hpp"

#include "settingsManager.hpp"
#include "assetManager.hpp"


Game::Game(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
		std::shared_ptr<SettingsManager> settings, std::shared_ptr<struct GameState> game_state) :
	GlobalData(global_data),
	Settings(settings),
	Assets(assets),
	GameState(game_state)
{
	this->LightingLayer = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);
	this->GameLayer = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);
}

Game::~Game()
{
	UnloadRenderTexture(this->GameLayer);
	UnloadRenderTexture(this->LightingLayer);
}

void Game::Draw(RenderTexture2D& canvas) const noexcept
{
	const Camera2D camera = this->GameState->Camera;

	BeginTextureMode(this->GameLayer);
		ClearBackground(BLACK);

		BeginMode2D(camera);
			GameDrawSystem::DrawGame(*this->GameState, *this->Assets);
		EndMode2D();
	EndTextureMode();

	BeginTextureMode(this->LightingLayer);
		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);
			GameDrawSystem::DrawLighting(*this->GameState);
		EndMode2D();
	EndTextureMode();

	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawTextureRec(
		this->GameLayer.texture, 
		(Rectangle) { 0, 0, REFERENCE_WIDTH, -static_cast<float>(REFERENCE_HEIGHT) },
		(Vector2) { 0, 0 }, 
		WHITE
		);

		BeginBlendMode(BLEND_MULTIPLIED);
			DrawTextureRec(
			this->LightingLayer.texture, 
			(Rectangle) { 0, 0, REFERENCE_WIDTH, -static_cast<float>(REFERENCE_HEIGHT) },
			(Vector2) { 0, 0 }, 
			WHITE
			);
		EndBlendMode();

		BeginMode2D(camera);
			GameDrawSystem::DrawScreenLayer(*this->GameState);
		EndMode2D();

		GameDrawSystem::DrawOverlay(*this->GameState, *this->GlobalData, *this->Assets);
	EndTextureMode();
}

void Game::Update() noexcept
{
	const size_t ticks = this->GameState->Ticks;

	if (!(ticks % TICK_RATE))
		this->GlobalData->CachedStrings[CachedString::Duration] = "Duration: " + std::to_string(TICKS_TO_SECONDS(ticks)) + "s";

	if (IsKeyPressed(KEY_ESCAPE))
		this->GlobalData->ActiveState = State::PauseMenu;
	
	if (IsKeyPressed(KEY_TAB) && (this->GameState->UnclaimedPowerups > 0 || this->Settings->Data.at(SettingKey::UnlimitedPowerups)))
		this->GlobalData->ActiveState = State::PowerupMenu;
}

void Game::TickedUpdate() noexcept
{
	this->Accumulator += GetFrameTime();

	if (this->Accumulator >= MAX_TICK_TIME)
		this->Accumulator = MAX_TICK_TIME;

	while (this->Accumulator >= TICK_TIME)
	{
		this->GameState->UpdateTimeouts();

		while (this->GameState->CollectedXp >= this->GameState->LevelUpTreshold)
		{
			const size_t distance = this->GameState->CollectedXp - this->GameState->LevelUpTreshold;

			this->GameState->LevelUp(*this->Settings, *this->GlobalData);
			this->GameState->CollectedXp = distance;
		}

		GameInputSystem::HandleTickedInput(*this->GameState, *this->Settings, *this->Assets);

		GameEventSystem::HandleEvents(*this->GameState, *this->Assets);

		this->GameState->UpdatePlayer(*this->GlobalData, *this->Settings, *this->Assets);
		this->GameState->UpdateCamera();

		this->GameState->UpdateEnemies(*this->Assets);
		this->GameState->UpdateProjectiles(*this->Assets);
		this->GameState->UpdateXps(*this->Assets);

		this->GameState->UpdateGameTexts();
		this->GameState->UpdateParticles();

		this->Accumulator -= TICK_TIME;
		this->GameState->Ticks++;
	} 
}

