#include "gameText.hpp"
#include "raylib.h"
#include "raymath.h"
#include "constants.hpp"

#include <string>
#include <cstddef>

GameText::GameText(float x, float y, Vector2 direction, float speed, std::string&& text, int size, Color colour, bool lerp, size_t creation, size_t expiry) noexcept : 
	Rect(x, y, static_cast<float>(size), static_cast<float>(size)),
	Direction(direction),
	Speed(speed),
	Text(std::move(text)),
	Size(size),
	Colour(colour),
	Lerp(lerp),
	Creation(creation),
	Expiry(expiry)
{}

void GameText::Draw(size_t ticks) const noexcept
{
	if (this->Lerp)
	{
		int size = this->Size * (this->Expiry - ticks) / (this->Expiry - this->Creation);
		DrawText(this->Text.c_str(), this->Rect.x, this->Rect.y, size, this->Colour);
	}
	else
		DrawText(this->Text.c_str(), this->Rect.x, this->Rect.y, this->Size, this->Colour);
}

void GameText::Update() noexcept
{
	this->Rect.x += this->Speed * this->Direction.x * TICK_TIME;
	this->Rect.y += this->Speed * this->Direction.y * TICK_TIME;
}


