#include "projectiles.hpp"

#include "raylib.h"
#include "game.hpp"
#include "globalDataWrapper.hpp"
#include "raymath.h"
#include "constants.hpp"

#include <cmath>

Projectile::Projectile(float x, float y, Vector2 direction, ProjectileType type, GlobalDataWrapper& global_data, AssetManager& assets) noexcept :
	Type(type),
	Layer(global_data.CurrentLayer) 
{
	if (direction.x != 0.0f || direction.y != 0.0f)
		this->Direction = Vector2Normalize(direction);
	else
		this->Direction = direction;
	
	switch (this->Type)
	{
		case ProjectileType::Bullet:
			this->Image = assets.Textures.at(TextureKey::Bullet);
			this->Speed = global_data.Attributes.at(Attribute::BulletSpeed);
			this->Rotation = atan2(this->Direction.y, this->Direction.x) * TO_DEG;
			this->Scale = 1.0f;

			break;
		case ProjectileType::Lazer:
			this->Image = assets.Textures.at(TextureKey::Lazer);
			this->Speed = global_data.Attributes.at(Attribute::LazerSpeed);
			this->Rotation = atan2(this->Direction.y, this->Direction.x) * TO_DEG;
			this->Scale = global_data.Attributes.at(Attribute::LazerScale);

			break;
		case ProjectileType::Ball:
			this->Image = assets.Textures.at(TextureKey::Ball);
			this->Scale = global_data.Attributes.at(Attribute::BallScale);
			this->Speed = global_data.Attributes.at(Attribute::BallAngularSpeed);
			this->Rotation = global_data.Attributes.at(Attribute::BallAngularSpeed) * TO_DEG;
			this->Radius = global_data.Attributes.at(Attribute::BallRadius);

			break;
	}

	this->Rect = { x, y, this->Image.width * this->Scale, this->Image.height * this->Scale };
}

void Projectile::Draw() const noexcept
{
	Vector2 position = (Vector2) { this->Rect.x, this->Rect.y };
	DrawTextureEx(this->Image, position, this->Rotation, this->Scale, WHITE);
}

void Projectile::Update(const Vector2 player_centre) noexcept
{
	switch (this->Type)
	{
		case ProjectileType::Ball:
		{
			this->CurrentAngle += this->Speed * TICK_TIME;

			this->CurrentAngle = std::fmod(this->CurrentAngle, 2 * PI);

			if (this->CurrentAngle < 0.0f)
				this->CurrentAngle += 2 * PI;

			this->Rect.width = this->Image.width * this->Scale;
			this->Rect.height = this->Image.height * this->Scale;

			this->Rect.x = player_centre.x + this->Radius * cos(this->CurrentAngle);
			this->Rect.y = player_centre.y + this->Radius * sin(this->CurrentAngle);

			break;
		}
		default:
			this->Rect.x += this->Direction.x * this->Speed * TICK_TIME;
			this->Rect.y += this->Direction.y * this->Speed * TICK_TIME;
	}
}
