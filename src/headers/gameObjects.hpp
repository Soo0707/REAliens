#pragma once

#include "raylib.h"

class BasicGameObject
{
	public:
		BasicGameObject(float pos_x, float pos_y, Texture2D initial_image);
		virtual ~BasicGameObject() = default;
		virtual void Draw() = 0;
		Rectangle Rect;

	protected:
		Texture2D Image;
};

class UpdatableGameObject : public BasicGameObject
{
	public:
		UpdatableGameObject(float pos_x, float pos_y, Texture2D initial_image);
		virtual ~UpdatableGameObject() = default;
		virtual void Update() = 0;
};

class Wall : public BasicGameObject
{
	public:
		Wall(float pos_x, float pos_y, Texture2D initial_image);
		~Wall() = default;
		
		void Draw() override;

	private:
		bool Destroyed = false;
};
