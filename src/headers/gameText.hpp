#pragma once

#include "raylib.h"

#include <string>
#include <cstddef>

class GameText
{
	public:
		GameText(float x, float y, std::string&& text, size_t ticks) noexcept;
		~GameText() = default;

		void Draw() const noexcept;
		void Update() noexcept;

		Rectangle Rect;
		size_t Expiry;

	private:
		std::string Text;
		float Speed = 64;
};
