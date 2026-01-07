#pragma once

#include <memory>

#include "globalDataWrapper.hpp"

#include "game.hpp"
#include "statSystem.hpp"
#include "assetManager.hpp"

class GameOverMenu
{
	public:
		GameOverMenu(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets);
		~GameOverMenu() = default;

		void Draw(RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;

		void GenerateStats(const Game& game, const StatSystem& stat_system) noexcept;

	private:
		const std::shared_ptr<GlobalDataWrapper> GlobalData;
		const std::shared_ptr<AssetManager> Assets;
};
