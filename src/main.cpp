#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"
#include "settingsManager.hpp"

#include "messageSystem.hpp"
#include "timerSystem.hpp"
#include "modifierSystem.hpp"
#include "particleSystem.hpp"
#include "projectileSystem.hpp"
#include "enemySystem.hpp"
#include "statSystem.hpp"
#include "xpSystem.hpp"
#include "collisionSystem.hpp"

#include "game.hpp"
#include "powerupMenu.hpp"
#include "gameOverMenu.hpp"
#include "pauseMenu.hpp"
#include "mainMenu.hpp"

#include "constants.hpp"

int main(void)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(REFERENCE_WIDTH, REFERENCE_HEIGHT, "RE::Aliens");

	Image window_icon = LoadImage("assets/icon.png");
	SetWindowIcon(window_icon);

	RenderTexture2D virtual_canvas = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);
	SetTextureFilter(virtual_canvas.texture, TEXTURE_FILTER_POINT);

	SetWindowMinSize(REFERENCE_WIDTH, REFERENCE_HEIGHT);

	SetExitKey(KEY_NULL);

	const std::shared_ptr<AssetManager> assets = std::make_shared<AssetManager>();
	const std::shared_ptr<SettingsManager> settings = std::make_shared<SettingsManager>();
	const std::shared_ptr<GlobalDataWrapper> global_data = std::make_shared<GlobalDataWrapper>();

	unsigned int target_refresh_rate;

	if (settings->Get(SettingKey::TargetFramerate) != 0)
		target_refresh_rate = settings->Get(SettingKey::TargetFramerate);
	else
		target_refresh_rate = 4 * GetMonitorRefreshRate(GetCurrentMonitor());

	SetTargetFPS(target_refresh_rate);

	const std::shared_ptr<MessageSystem> message_system = std::make_shared<MessageSystem>();
	const std::shared_ptr<TimerSystem> timer_system = std::make_shared<TimerSystem>();
	const std::shared_ptr<ModifierSystem> modifier_system = std::make_shared<ModifierSystem>();
	const std::shared_ptr<ParticleSystem> particle_system = std::make_shared<ParticleSystem>();
	const std::shared_ptr<ProjectileSystem> projectile_system = std::make_shared<ProjectileSystem>();
	const std::shared_ptr<EnemySystem> enemy_system = std::make_shared<EnemySystem>();
	const std::shared_ptr<StatSystem> stat_system = std::make_shared<StatSystem>();
	const std::shared_ptr<XpSystem> xp_system = std::make_shared<XpSystem>();

	// TODO: this is ugly
	const std::shared_ptr<CollisionSystem> collision_system = std::make_shared<CollisionSystem>(assets->Ground.width, assets->Ground.height);

	Game game = Game(
			global_data, assets, settings, message_system, timer_system, modifier_system,
			particle_system, projectile_system, enemy_system, stat_system, xp_system, collision_system
			);

	PowerupMenu powerup_menu = PowerupMenu(global_data, assets, settings, message_system, timer_system);
	GameOverMenu game_over = GameOverMenu(global_data, assets);
	PauseMenu pause = PauseMenu(global_data, assets);
	MainMenu main_menu = MainMenu(global_data, assets);

	State prev_state = global_data->ActiveState;

	while (!WindowShouldClose() && global_data->Running)
	{
		if (global_data->ActiveState != prev_state)
		{
			(global_data->ActiveState == State::Game) ? SetTargetFPS(target_refresh_rate) : SetTargetFPS(15);
			prev_state = global_data->ActiveState;
		}

		if (IsKeyPressed(KEY_F11))
		{
			ToggleBorderlessWindowed();
			EnableCursor();
		}

		SetMouseScale(REFERENCE_WIDTH / static_cast<float>(GetScreenWidth()), REFERENCE_HEIGHT / static_cast<float>(GetScreenHeight()));

		switch (global_data->ActiveState)
		{
			case State::GameReset:
				message_system->Reset();
				timer_system->Reset();
				modifier_system->Reset();
				particle_system->Reset();
				projectile_system->Reset();
				enemy_system->Reset();
				stat_system->Reset();
				xp_system->Reset();
				game.Reset();
				global_data->Reset();
				global_data->ActiveState = State::Game;
				break;
			case State::MainMenu:
				main_menu.HandleInput();
				main_menu.Draw(virtual_canvas);
				break;
			case State::Game:
				game.Update();
				game.TickedUpdate();
				game.Draw(virtual_canvas);
				break;
			case State::PowerupMenu:
				powerup_menu.HandleInput();
				powerup_menu.Draw(virtual_canvas);
				break;
			case State::GameOverMenu:
				game_over.HandleInput();
				game_over.Draw(virtual_canvas);
				break;
			case State::PauseMenu:
				pause.HandleInput();
				pause.Draw(virtual_canvas);
				break;
			case State::GenerateGameOverStats:
				game_over.GenerateStats(game, *stat_system);
				global_data->ActiveState = State::GameOverMenu;
				break;
		}

		BeginDrawing();
			ClearBackground(BLACK);
			DrawTexturePro(
					virtual_canvas.texture,
					(Rectangle) { 0, 0, REFERENCE_WIDTH, -static_cast<float>(REFERENCE_HEIGHT) },
					(Rectangle) { 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
					(Vector2) { 0, 0 },
					0.0f,
					WHITE
					);
		EndDrawing();
	}

	UnloadImage(window_icon);

	UnloadRenderTexture(virtual_canvas);
	CloseWindow();

	return 0;
}
