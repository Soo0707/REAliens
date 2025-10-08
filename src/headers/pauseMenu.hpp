#pragma once

#include <memory>

#include "globalDataWrapper.hpp"

class PauseMenu
{
	public:
		PauseMenu(std::shared_ptr<GlobalDataWrapper> global_data);
		~PauseMenu();
		void Draw();
		void HandleInput();

	private:
		std::shared_ptr<GlobalDataWrapper> GlobalData;
};
