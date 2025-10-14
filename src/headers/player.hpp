#pragma once

#include "raylib.h"
#include "assetManager.hpp"
#include <cstddef>

class Player
{
	public:
		Player(float pos_x, float pos_y, AssetManager &assets);
		~Player(); 
		
		void Move();
		void Update(size_t ticks);
		void Animate(size_t ticks);
		void Draw() const;
		
		float Health = 100;

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle Rect;
		Rectangle Aura;
		Vector2 Centre;

		float Speed = 300;

	private:
		AssetManager& Assets;

		Texture2D Image;

		size_t LastAnimationUpdate = 0;
		unsigned int AnimationFrames = 2;
		unsigned int ImageIndex = 0;
		unsigned int AnimationSpeed = 120;


		StaticTextureKey CurrentTextures = StaticTextureKey::PlayerSouth;

		void SetCurrentTextures();
};
