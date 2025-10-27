#include "pauseMenu.hpp"

#include "raylib.h"
#include "globalDataWrapper.hpp"

PauseMenu::PauseMenu(std::shared_ptr<GlobalDataWrapper> global_data) :
	GlobalData(global_data)
{}

PauseMenu::~PauseMenu()
{}

void PauseMenu::Draw(RenderTexture2D& canvas) const noexcept
{
	static constexpr Rectangle left_rect = { 597.5, 310, 35, 100 };
	static constexpr Rectangle right_rect = { 647.5, 310, 35, 100 };
	
	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawRectangleRec(left_rect, LIGHTGRAY);
		DrawRectangleRec(right_rect, LIGHTGRAY);

		DrawText("PAUSED", 597, 420, 21, LIGHTGRAY);

		DrawText("[Space] Resume, [Esc] Main Menu", 469, 620, 21, LIGHTGRAY);
	EndTextureMode();
}

void PauseMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_SPACE))
		this->GlobalData->ActiveState = State::Game;
}
