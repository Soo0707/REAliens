#include "gameText.hpp"

#include "raylib.h"
#include "constants.hpp"

#include <string>
#include <cstddef>

GameText::GameText(float x, float y, float speed, std::string&& text, unsigned int size, Color colour, size_t creation, size_t expiry) noexcept : 
	Rect(x, y, static_cast<float>(size), static_cast<float>(size)),
	Direction({ 0.0f, -1.0f }),
	Speed(speed),
	Text(std::move(text)),
	Size(size),
	Colour(colour),
	Creation(creation),
	Expiry(expiry)
{}

void GameText::Draw(size_t ticks) const noexcept
{
	const unsigned int size = this->Size * (this->Expiry - ticks) / (this->Expiry - this->Creation);
	DrawText(this->Text.c_str(), this->Rect.x, this->Rect.y, size, this->Colour);
}

void GameText::Update() noexcept
{
	this->Rect.x += this->Speed * this->Direction.x * TICK_TIME;
	this->Rect.y += this->Speed * this->Direction.y * TICK_TIME;
}


