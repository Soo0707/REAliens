#pragma once

#include <memory>

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"

class GameOverMenu
{
	public:
		GameOverMenu(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets);
		~GameOverMenu() = default;

		void Draw(RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;

		void GenerateStats() noexcept;

	private:
		std::shared_ptr<GlobalDataWrapper> GlobalData;
		std::shared_ptr<AssetManager> Assets;
};
