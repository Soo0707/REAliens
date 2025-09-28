#pragma once

#include "raylib.h"
#include "assetManager.hpp"

enum class EnemyType
{
	Australian,
	BigMan,
	Bomber,
	Drunkard,
	Pleb,
	Poison,
	Trapper
};

enum class UniqueStates
{
	None
};

class Enemy
{
	public:
		Enemy(float pos_x, float pos_y, AssetManager &assets, EntityTextureKey texture_key, EnemyType type);
		~Enemy(); 
		
		void MoveX();
		void MoveY();
		void SetDirection(Rectangle& player_rect);

		void Draw();
		void Update();
		
		int Health;
		int Damage;

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle Rect;
		Rectangle NextRect = { 0.0f, 0.0f, 0, 0 };

		unsigned int Speed;
		bool Flash = false;

		EnemyType Type;
		UniqueStates UniqueState;

	private:
		AssetManager& Assets;
		EntityTextureKey TextureKey;

		Texture2D Image;

		float ImageIndex = 0.0f;
		float AnimationSpeed;

		void Animate();
		void SecondaryAttack();
};
