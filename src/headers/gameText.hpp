#pragma once

#include "raylib.h"

#include <string>
#include <cstddef>

class GameText
{
	public:
		GameText(float x, float y, float speed, std::string&& text, unsigned int size, Color colour, size_t creation, size_t expiry) noexcept;
		~GameText() = default;

		void Draw(size_t ticks) const noexcept;
		void Update() noexcept;

		Rectangle Rect;
		size_t Expiry;

	private:
		int Size;
		Vector2 Direction;
		float Speed;

		Color Colour;
		size_t Creation;

		std::string Text;
};
