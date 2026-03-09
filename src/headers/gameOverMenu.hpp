#pragma once

#include <memory>

#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"
#include "statSystem.hpp"
#include "messageSystem.hpp"

class GameOverMenu
{
	public:
		GameOverMenu(
				std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
				std::shared_ptr<class StatSystem> stat_system, std::shared_ptr<struct MessageSystem> message_system
				);

		~GameOverMenu() = default;

		void Draw(const RenderTexture2D& canvas) const noexcept;
		void HandleInput() const noexcept;

		void GenerateStats(const size_t ticks) noexcept;

	private:
		const std::shared_ptr<class GlobalDataWrapper> GlobalData;
		const std::shared_ptr<class AssetManager> Assets;
		const std::shared_ptr<class StatSystem> StatSystem;
		const std::shared_ptr<struct MessageSystem> MessageSystem;
};
