#pragma once

#include "gameObjects.hpp"
#include "raylib.h"
#include "assetManager.hpp"

class Player : public UpdatableGameObject
{
	public:
		Player(float pos_x, float pos_y, AssetManager &assets);
		~Player(); 
		//void Draw() override;
		void Update() override;
		
		int Health = 100;
		unsigned int Speed = 300;
		Vector2 Direction = {0.0f, 0.0f};

	private:
		AssetManager& Assets;
		float ImageIndex = 0.0f;
		float AnimationSpeed = 10.0f;

		EntityTextureKey CurrentTextures = EntityTextureKey::PlayerSouth;

		void Animate();
		void MoveX();
		void MoveY();
		void SetCurrentTextures();
};
