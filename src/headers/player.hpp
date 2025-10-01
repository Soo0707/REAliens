#pragma once

#include "raylib.h"
#include "assetManager.hpp"

class Player
{
	public:
		Player(float pos_x, float pos_y, AssetManager &assets);
		~Player(); 
		
		void Move();
		void Update();
		void Draw() const;
		
		int Health = 100;

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle Rect;
		unsigned int Speed = 100;

	private:
		AssetManager& Assets;

		Texture2D Image;

		float ImageIndex = 0.0f;
		float AnimationSpeed = 10.0f;

		EntityTextureKey CurrentTextures = EntityTextureKey::PlayerSouth;

		void Animate();
		void SetCurrentTextures();
};
