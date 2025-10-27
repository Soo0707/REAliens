#pragma once

#include <memory>

#include "globalDataWrapper.hpp"

class GameOverMenu
{
	public:
		GameOverMenu(std::shared_ptr<GlobalDataWrapper> global_data);
		~GameOverMenu();
		void Draw(RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;

	private:
		std::shared_ptr<GlobalDataWrapper> GlobalData;
};
