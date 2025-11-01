#include "projectiles.hpp"

#include "raylib.h"
#include "game.hpp"
#include "globalDataWrapper.hpp"
#include "raymath.h"
#include "constants.hpp"

#include <cmath>

Projectile::Projectile(float x, float y, const Vector2& direction, ProjectileType type, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept :
	Type(type)
{
	if (direction.x != 0.0f || direction.y != 0.0f)
		this->Direction = Vector2Normalize(direction);
	else
		this->Direction = direction;

	auto hook = this->ConstructorHooks[static_cast<size_t>(type)];
	(this->*hook)(x, y, direction, global_data, assets);
	
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

void Projectile::Update(const Vector2& player_centre) noexcept
{
	auto hook = this->UpdateHooks[static_cast<size_t>(this->Type)];
	(this->*hook)(player_centre);
}



void Projectile::BulletConstructor(float x, float y, const Vector2& direction, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept
{
	this->Image = assets.Textures.at(TextureKey::Bullet);
	this->Speed = global_data.Attributes.at(Attribute::BulletSpeed);
	this->Rotation = atan2(this->Direction.y, this->Direction.x) * TO_DEG;
	this->Scale = 1.0f;
}

void Projectile::LazerConstructor(float x, float y, const Vector2& direction, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept
{
	this->Image = assets.Textures.at(TextureKey::Lazer);
	this->Speed = global_data.Attributes.at(Attribute::LazerSpeed);
	this->Rotation = atan2(this->Direction.y, this->Direction.x) * TO_DEG;
	this->Scale = global_data.Attributes.at(Attribute::LazerScale);
}

void Projectile::BallConstructor(float x, float y, const Vector2& direction, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept
{
	this->Image = assets.Textures.at(TextureKey::Ball);
	this->Scale = global_data.Attributes.at(Attribute::BallScale);
	this->Speed = global_data.Attributes.at(Attribute::BallAngularSpeed);
	this->Rotation = 0.0f; //global_data.Attributes.at(Attribute::BallAngularSpeed) * TO_DEG;
	this->Radius = global_data.Attributes.at(Attribute::BallRadius);
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
	DrawCircleGradient(this->Rect.x, this->Rect.y, std::max(this->Rect.width, this->Rect.height) * 2.0f, CYAN, LIGHTGRAY);
}



void Projectile::BulletUpdate(const Vector2& player_centre) noexcept
{
	this->Rect.x += this->Direction.x * this->Speed * TICK_TIME;
	this->Rect.y += this->Direction.y * this->Speed * TICK_TIME;
}

void Projectile::LazerUpdate(const Vector2& player_centre) noexcept
{

	this->Rect.x += this->Direction.x * this->Speed * TICK_TIME;
	this->Rect.y += this->Direction.y * this->Speed * TICK_TIME;
}

void Projectile::BallUpdate(const Vector2& player_centre) noexcept
{
	this->CurrentAngle += this->Speed * TICK_TIME;

	this->CurrentAngle = std::fmod(this->CurrentAngle, 2 * PI);

	if (this->CurrentAngle < 0.0f)
		this->CurrentAngle += 2 * PI;

	this->Rect.width = this->Image.width * this->Scale;
	this->Rect.height = this->Image.height * this->Scale;

	this->Rect.x = player_centre.x + this->Radius * cos(this->CurrentAngle);
	this->Rect.y = player_centre.y + this->Radius * sin(this->CurrentAngle);
}
