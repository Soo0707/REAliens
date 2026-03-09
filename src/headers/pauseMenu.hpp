#pragma once

#include <memory>

#include "globalDataWrapper.hpp"
#include "assetManager.hpp"
#include "messageSystem.hpp"

class PauseMenu
{
	public:
		PauseMenu(
				std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
				std::shared_ptr<MessageSystem> message_system
				);
		~PauseMenu() = default;

		void Draw(const RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;

	private:
		std::shared_ptr<class GlobalDataWrapper> GlobalData;
		std::shared_ptr<class AssetManager> Assets;
		std::shared_ptr<struct MessageSystem> MessageSystem;
};
