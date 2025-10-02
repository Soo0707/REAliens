#include "projectiles.hpp"
#include "raylib.h"
#include "game.hpp"

Projectile::Projectile(float pos_x, float pos_y, Texture2D image, unsigned int speed, Vector2 direction, float rotation, float scale, ProjectileType type, float damage):
	Rect({pos_x, pos_y, image.width * scale, image.height * scale}), Speed(speed), Direction(direction), Rotation(rotation), Scale(scale), Type(type), Damage(damage), Image(image)
{}

void Projectile::Draw() const
{
	Vector2 position = (Vector2) { this->Rect.x, this->Rect.y };
	DrawTextureEx(this->Image, position, this->Rotation, this->Scale, WHITE);
}

void Projectile::Update()
{
	this->Rect.x += this->Direction.x * this->Speed * TICK_TIME;
	this->Rect.y += this->Direction.y * this->Speed * TICK_TIME;
}
