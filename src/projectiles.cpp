#include "gameObjects.hpp"
#include "projectiles.hpp"
#include "raylib.h"

Projectile::Projectile(float pos_x, float pos_y, Texture2D initial_image, unsigned int speed, Vector2 direction, float rotation, float scale):
	UpdatableGameObject(pos_x, pos_y, initial_image), Speed(speed), Direction(direction), Rotation(rotation), Scale(scale)
{}

void Projectile::Draw()
{
	Vector2 position = (Vector2) { this->Rect.x, this->Rect.y };
	DrawTextureEx(this->Image, position, this->Rotation, this->Scale, WHITE);
}

void Projectile::Update()
{
	this->Rect.x += this->Direction.x * this->Speed * GetFrameTime();
	this->Rect.y += this->Direction.y * this->Speed * GetFrameTime();
}
