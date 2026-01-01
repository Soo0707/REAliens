#include "projectiles.hpp"

#include <cmath>
#include <unordered_map>

#include "raylib.h"
#include "raymath.h"

#include "assetManager.hpp"
#include "constants.hpp"


Projectile::Projectile(float x, float y, float speed, float scale, Vector2 direction, ProjectileType type, const AssetManager& assets) noexcept :
	Type(type),
	Speed(speed),
	Scale(scale),
	Kill(false)
{
	// Vector2Normalize already checks 0-length vectors
	this->Direction = Vector2Normalize(direction);

	auto hook = this->ConstructorHooks[static_cast<size_t>(type)];
	(this->*hook)(assets);
	
	this->Rect = { x, y, this->Image.width * this->Scale, this->Image.height * this->Scale };
}


void Projectile::Draw() const noexcept
{
	Vector2 position = (Vector2) { this->Rect.x, this->Rect.y };
	DrawTextureEx(this->Image, position, this->Rotation, this->Scale, WHITE);
}

void Projectile::DrawLightmap() const noexcept
{
	auto hook = this->DrawLightmapHooks[static_cast<size_t>(this->Type)];
	(this->*hook)();
}

void Projectile::Update() noexcept
{
	auto hook = this->UpdateHooks[static_cast<size_t>(this->Type)];
	(this->*hook)();
}



void Projectile::BulletConstructor(const AssetManager& assets) noexcept
{
	this->Image = assets.Textures.at(TextureKey::Bullet);
	this->Rotation = atan2(this->Direction.y, this->Direction.x) * TO_DEG;
	this->Colour = YELLOW;
}

void Projectile::LazerConstructor(const AssetManager& assets) noexcept
{
	this->Image = assets.Textures.at(TextureKey::Lazer);
	this->Rotation = atan2(this->Direction.y, this->Direction.x) * TO_DEG;
	this->Colour = CYAN;
}

void Projectile::BallConstructor(const AssetManager& assets) noexcept
{


}

/*
 * since AABB is axis aligned by definition, there is no way to sync the hitbox and the texture if rotated.
 * as such, calculating the centre is useless and we're better off using the top left x and y coordinates
*/
void Projectile::BulletDrawLightmap() const noexcept
{
	DrawCircleGradient(this->Rect.x, this->Rect.y, std::max(this->Rect.width, this->Rect.height) * 2.0f, YELLOW, LIGHTGRAY);
}

void Projectile::LazerDrawLightmap() const noexcept
{
	DrawCircleGradient(this->Rect.x, this->Rect.y, std::max(this->Rect.width, this->Rect.height) * 2.0f, CYAN, LIGHTGRAY);
}

void Projectile::BallDrawLightmap() const noexcept
{


}


void Projectile::BulletUpdate() noexcept
{
	this->Rect.x += this->Direction.x * this->Speed * TICK_TIME;
	this->Rect.y += this->Direction.y * this->Speed * TICK_TIME;
}

void Projectile::LazerUpdate() noexcept
{
	this->Rect.x += this->Direction.x * this->Speed * TICK_TIME;
	this->Rect.y += this->Direction.y * this->Speed * TICK_TIME;
}

void Projectile::BallUpdate() noexcept
{

}
