#include "pauseMenu.hpp"

#include <memory>
#include <variant>

#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"
#include "messageSystem.hpp"
#include "states.hpp"

PauseMenu::PauseMenu(
		std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
		std::shared_ptr<struct MessageSystem> message_system
		) :
	GlobalData(global_data),
	Assets(assets),
	MessageSystem(message_system)
{}

void PauseMenu::Draw(const RenderTexture2D& canvas) const noexcept
{
	static constexpr Rectangle left_rect = { 597.5, 310, 35, 100 };
	static constexpr Rectangle right_rect = { 647.5, 310, 35, 100 };
	
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawRectangleRec(left_rect, LIGHTGRAY);
		DrawRectangleRec(right_rect, LIGHTGRAY);

		DrawText("PAUSED", 597, 420, 21, LIGHTGRAY);

		DrawText("[Esc] Resume, [Q] Main Menu, [P] End Session", 404, 620, 21, LIGHTGRAY);
	EndTextureMode();
}

void PauseMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_ESCAPE))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<SetState>, State::Game);

	if (IsKeyPressed(KEY_Q))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<SetState>, State::MainMenu);

	if (IsKeyPressed(KEY_P))
		this->MessageSystem->StateManagerCommands.emplace_back(std::in_place_type<SetState>, State::GenerateGameOverStats);
}


