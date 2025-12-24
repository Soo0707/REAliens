#include "game.hpp"

#include <memory>
#include <algorithm>
#include <vector>
#include <string>

#include "raylib.h"
#include "raymath.h"

#include "gameState.hpp"
#include "globalDataWrapper.hpp"

#include "gameEventSystem.hpp"
#include "gameInputSystem.hpp"
#include "gameDrawSystem.hpp"
#include "gameHelpers.hpp"

#include "constants.hpp"

#include "settingsManager.hpp"
#include "assetManager.hpp"


Game::Game(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
		std::shared_ptr<SettingsManager> settings, std::shared_ptr<GameState> game_state) :
	GlobalData(global_data),
	Settings(settings),
	Assets(assets),
	m_GameState(game_state)
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
	const Camera2D camera = this->m_GameState->Camera;

	BeginTextureMode(this->GameLayer);
		ClearBackground(BLACK);

		BeginMode2D(camera);
			GameDrawSystem::DrawGame(*this->m_GameState, *this->Assets);
		EndMode2D();
	EndTextureMode();

	BeginTextureMode(this->LightingLayer);
		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);
			GameDrawSystem::DrawLighting(*this->m_GameState);
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
			GameDrawSystem::DrawScreenLayer(*this->m_GameState);
		EndMode2D();

		GameDrawSystem::DrawOverlay(*this->m_GameState, *this->GlobalData, *this->Assets);
	EndTextureMode();
}

void Game::Update() noexcept
{
	this->Accumulator += GetFrameTime();

	if (this->Accumulator >= MAX_TICK_TIME)
		this->Accumulator = MAX_TICK_TIME;

	while (this->Accumulator >= TICK_TIME)
	{
		this->m_GameState->UpdateTimeouts(*this->GlobalData);

		while (this->m_GameState->CollectedXp >= this->m_GameState->LevelUpTreshold)
		{
			size_t distance = this->m_GameState->CollectedXp - this->m_GameState->LevelUpTreshold;

			GameHelper::LevelUp(*this->m_GameState, *this->Settings, *this->GlobalData);
			this->m_GameState->CollectedXp = distance;
		}

		GameInputSystem::HandleTickedInput(*this->m_GameState, *this->Settings, *this->Assets);

		GameEventSystem::HandleEvents(*this->m_GameState, *this->Assets);

		this->m_GameState->UpdatePlayer(*this->GlobalData, *this->Settings, *this->Assets);
		this->m_GameState->UpdateCamera();

		this->m_GameState->UpdateEnemies(*this->Assets);
		this->m_GameState->UpdateProjectiles(*this->Assets);
		this->m_GameState->UpdateXps(*this->Assets);

		this->m_GameState->UpdateGameTexts();
		this->m_GameState->UpdateParticles();

		this->Accumulator -= TICK_TIME;
		this->m_GameState->Ticks++;
	} 
}

/*
void Game::UpdateTimeouts() noexcept
{
	size_t ticks = this->m_GameState->Ticks;

	if (!(ticks % TICK_RATE))
		this->GlobalData->CachedStrings[CachedString::Duration] = "Duration: " + std::to_string(TICKS_TO_SECONDS(ticks)) + "s";
}
*/

void Game::HandleEssentialInput() noexcept
{
	if (IsKeyPressed(KEY_ESCAPE))
		this->GlobalData->ActiveState = State::PauseMenu;
	
	if (IsKeyPressed(KEY_TAB) && (this->m_GameState->UnclaimedPowerups > 0 || this->Settings->Data.at(SettingKey::UnlimitedPowerups)))
		this->GlobalData->ActiveState = State::PowerupMenu;
}

