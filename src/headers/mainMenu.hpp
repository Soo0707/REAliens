#pragma once

#include <memory>

#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"
#include "messageSystem.hpp"

class MainMenu
{
	public:
		MainMenu(
				std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
				std::shared_ptr<struct MessageSystem> message_system
		);
		~MainMenu() = default;

		void HandleInput() noexcept;
		void Draw(const RenderTexture2D& canvas) const noexcept;

	private:
		std::shared_ptr<struct GlobalDataWrapper> GlobalData;
		std::shared_ptr<class AssetManager> Assets;
		std::shared_ptr<struct MessageSystem> MessageSystem;
};
