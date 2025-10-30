#pragma once

#include <memory>

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"

class PauseMenu
{
	public:
		PauseMenu(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets);
		~PauseMenu() = default;

		void Draw(RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;

	private:
		std::shared_ptr<GlobalDataWrapper> GlobalData;
		std::shared_ptr<AssetManager> Assets;
};
