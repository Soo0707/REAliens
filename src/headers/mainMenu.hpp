#pragma once

#include <memory>

#include "raylib.h"
#include "globalDataWrapper.hpp"
#include "assetManager.hpp"

class MainMenu
{
	public:
		MainMenu(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets);
		~MainMenu() = default;

		void HandleInput() noexcept;
		void Draw(RenderTexture2D& canvas) const noexcept;

	private:
		std::shared_ptr<GlobalDataWrapper> GlobalData;
		std::shared_ptr<AssetManager> Assets;
};
