#pragma once

#include "raylib.h"

class BasicGameObject
{
	public:
		BasicGameObject(float pos_x, float pos_y, Texture2D initial_image);
		virtual ~BasicGameObject() = default;
		virtual void Draw() const;
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

class MovableGameObject : public UpdatableGameObject
{
	public:
		MovableGameObject(float pos_x, float pos_y, Texture2D initial_image);
		virtual ~MovableGameObject() = default;

		virtual void MoveX();
		virtual void MoveY();

		Vector2 Direction = { 0.0f, 0.0f };
		Rectangle NextRect = { 0.0f, 0.0f, 0, 0 };
		unsigned int Speed = 100;
};

class Wall : public BasicGameObject
{
	public:
		Wall(float pos_x, float pos_y, Texture2D initial_image);
		~Wall() = default;
		
	private:
		bool Destroyed = false;
};

class Spawner : public BasicGameObject
{
	public:
		Spawner(float pos_x, float pos_y, Texture2D initial_image);
		~Spawner() = default;
};
