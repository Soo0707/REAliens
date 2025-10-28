#include "gameText.hpp"
#include "raylib.h"
#include "constants.hpp"

#include <string>
#include <cstddef>

GameText::GameText(float x, float y, std::string&& text, size_t ticks) noexcept :
	Rect({ x, y, 24, 24 }),
	Text(std::move(text)),
	Expiry(ticks + TICK_RATE)
{}

void GameText::Draw() const noexcept
{
	DrawText(this->Text.c_str(), this->Rect.x, this->Rect.y, 24, YELLOW);
}

void GameText::Update() noexcept
{
	this->Rect.y -= this->Speed * TICK_TIME;
}


