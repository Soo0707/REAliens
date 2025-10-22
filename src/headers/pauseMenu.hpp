#pragma once

#include <memory>

#include "globalDataWrapper.hpp"

class PauseMenu
{
	public:
		PauseMenu(std::shared_ptr<GlobalDataWrapper> global_data);
		~PauseMenu();
		void Draw(RenderTexture2D& canvas);
		void HandleInput();

	private:
		std::shared_ptr<GlobalDataWrapper> GlobalData;
};
