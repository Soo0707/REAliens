#include "mainMenu.hpp"

#include <memory>
#include <variant>

#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"
#include "messageSystem.hpp"

MainMenu::MainMenu(
		std::shared_ptr<struct GlobalDataWrapper> global_data, std::shared_ptr<class AssetManager> assets,
		std::shared_ptr<struct MessageSystem> message_system
		) :
	GlobalData(global_data),
	Assets(assets),
	MessageSystem(message_system)
{}

void MainMenu::Draw(const RenderTexture2D& canvas) const noexcept
{
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("RE::ALIENS", 292, 100, 128, VIOLET);
		DrawText("No Sounds Included", 407, 250, 48, GOLD);
		DrawText("[SPACE] Start, [Esc] Quit, [F11] Toggle Fullscreen", 240.5, 620, 32, LIGHTGRAY);
	EndTextureMode();
}

void MainMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_SPACE))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<SetState>, State::SystemsReset);

	if (IsKeyPressed(KEY_ESCAPE))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<SetTerminate>, true);
}
