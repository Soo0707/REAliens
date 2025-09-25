#include "gameObjects.hpp"
#include "raylib.h"

BasicGameObject::BasicGameObject(float pos_x, float pos_y, Texture2D initial_image):
	Rect({pos_x, pos_y, (float) initial_image.width, (float) initial_image.height}), Image(initial_image)
{}

void BasicGameObject::Draw()
{
	DrawTexture(this->Image, (int) this->Rect.x, (int) this->Rect.y, WHITE);
}

UpdatableGameObject::UpdatableGameObject(float pos_x, float pos_y, Texture2D initial_image):
	BasicGameObject(pos_x, pos_y, initial_image)
{}

MovableGameObject::MovableGameObject(float pos_x, float pos_y, Texture2D initial_image):
	UpdatableGameObject(pos_x, pos_y, initial_image)
{}

void MovableGameObject::MoveX()
{
	this->NextRect.x = this->Rect.x + this->Speed * this->Direction.x * GetFrameTime();
	this->NextRect.width = this->Rect.width;
}

void MovableGameObject::MoveY()
{
	this->NextRect.y = this->Rect.y + this->Speed * this->Direction.y * GetFrameTime();
	this->NextRect.height = this->Rect.height;
}

Wall::Wall(float pos_x, float pos_y, Texture2D initial_image):
	BasicGameObject(pos_x, pos_y, initial_image)
{}

Spawner::Spawner(float pos_x, float pos_y, Texture2D initial_image):
	BasicGameObject(pos_x, pos_y, initial_image)
{}
