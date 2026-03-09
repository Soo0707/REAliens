#include "raylib.h"

#include <cstddef>

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
#include "systemsResetState.hpp"
#include "powerupMenu.hpp"
#include "gameOverMenu.hpp"
#include "pauseMenu.hpp"
#include "mainMenu.hpp"
#include "stateManager.hpp"

#include "constants.hpp"

int main(void)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(REFERENCE_WIDTH, REFERENCE_HEIGHT, "RE::Aliens");

	Image window_icon = LoadImage("assets/icon.png");
	SetWindowIcon(window_icon);

	RenderTexture2D canvas = LoadRenderTexture(REFERENCE_WIDTH, REFERENCE_HEIGHT);
	SetTextureFilter(canvas.texture, TEXTURE_FILTER_POINT);

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

	// systems
	const std::shared_ptr<MessageSystem> message_system = std::make_shared<MessageSystem>();
	const std::shared_ptr<TimerSystem> timer_system = std::make_shared<TimerSystem>();
	const std::shared_ptr<ModifierSystem> modifier_system = std::make_shared<ModifierSystem>();
	const std::shared_ptr<ParticleSystem> particle_system = std::make_shared<ParticleSystem>();
	const std::shared_ptr<ProjectileSystem> projectile_system = std::make_shared<ProjectileSystem>();
	const std::shared_ptr<EnemySystem> enemy_system = std::make_shared<EnemySystem>();
	const std::shared_ptr<StatSystem> stat_system = std::make_shared<StatSystem>();
	const std::shared_ptr<XpSystem> xp_system = std::make_shared<XpSystem>();
	const std::shared_ptr<CollisionSystem> collision_system = std::make_shared<CollisionSystem>(assets->Ground.width, assets->Ground.height);

	// states
	const std::shared_ptr<Game> game_state = std::make_shared<Game>(
			global_data, assets, settings, message_system, timer_system, modifier_system,
			particle_system, projectile_system, enemy_system, stat_system, xp_system, collision_system
			);

	const std::shared_ptr<SystemsResetState> systems_reset_state = std::make_shared<SystemsResetState>(
			message_system, timer_system, modifier_system, particle_system, projectile_system, enemy_system,
			stat_system, xp_system, collision_system
			);

	const std::shared_ptr<PowerupMenu> powerup_menu_state = std::make_shared<PowerupMenu>(
			global_data, assets, settings, message_system, modifier_system, timer_system
			);

	const std::shared_ptr<GameOverMenu> game_over_menu_state = std::make_shared<GameOverMenu>(global_data, assets, stat_system, message_system);
	const std::shared_ptr<PauseMenu> pause_menu_state = std::make_shared<PauseMenu>(global_data, assets, message_system);
	const std::shared_ptr<MainMenu> main_menu_state = std::make_shared<MainMenu>(global_data, assets, message_system);

	StateManager state_manager = StateManager(
		game_state, systems_reset_state, powerup_menu_state, game_over_menu_state,
		pause_menu_state, main_menu_state
		);

	float accumulator = 0.0f;

	while (!WindowShouldClose() && !state_manager.Terminate)
	{
		accumulator += GetFrameTime();

		if (accumulator >= MAX_TICK_TIME)
			accumulator = MAX_TICK_TIME;

		while (accumulator >= TICK_TIME)
		{
			state_manager.Update(*message_system, canvas);

			accumulator -= TICK_TIME;
		}

		BeginDrawing();
			ClearBackground(BLACK);
			DrawTexturePro(
					canvas.texture,
					{ 0.0f, 0.0f, REFERENCE_WIDTH, -static_cast<float>(REFERENCE_HEIGHT) },
					{ 0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
					{ 0.0f, 0.0f },
					0.0f,
					WHITE
					);
		EndDrawing();
	}

	UnloadImage(window_icon);

	UnloadRenderTexture(canvas);
	CloseWindow();

	return 0;
}
