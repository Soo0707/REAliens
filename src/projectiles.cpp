#include "projectiles.hpp"
#include "raylib.h"
#include "game.hpp"
#include "attributes.hpp"
#include "raymath.h"

#include <cmath>

Projectile::Projectile(float x, float y, Vector2 direction, ProjectileType type, std::shared_ptr<AttributeManager> attributes, AssetManager& assets):
	Type(type), Attributes(attributes)
{
	if (direction.x != 0.0f || direction.y != 0.0f)
		this->Direction = Vector2Normalize(direction);
	else
		this->Direction = direction;

	switch (this->Type)
	{
		case ProjectileType::Bullet:
			this->Image = assets.StaticTextures.at(StaticTextureKey::Bullet);
			this->Speed = attributes->Data.at(Attribute::BulletSpeed);
			this->Rotation = atan2(this->Direction.y, this->Direction.x) * 180 / 3.142;
			this->Scale = 1.0f;

			break;
		case ProjectileType::Lazer:
			this->Image = assets.StaticTextures.at(StaticTextureKey::Lazer);
			this->Speed = attributes->Data.at(Attribute::LazerSpeed);
			this->Rotation = atan2(this->Direction.y, this->Direction.x) * 180 / 3.142;
			this->Scale = attributes->Data.at(Attribute::LazerScale);

			break;
		case ProjectileType::Circle:
			this->Image = assets.StaticTextures.at(StaticTextureKey::Circle);
			this->Scale = attributes->Data.at(Attribute::CircleSize);
			this->Speed = attributes->Data.at(Attribute::CircleAngularSpeed);
			this->Rotation = attributes->Data.at(Attribute::CircleAngularSpeed) * 180 / 3.142;

			break;
	}

	this->Rect = { x, y, (float) this->Image.width * this->Scale, (float) this->Image.height * this->Scale };
}

void Projectile::Draw() const
{
	Vector2 position = (Vector2) { this->Rect.x, this->Rect.y };
	DrawTextureEx(this->Image, position, this->Rotation, this->Scale, WHITE);
}

void Projectile::Update(Rectangle& player_rect)
{
	switch (this->Type)
	{
		case ProjectileType::Circle:
		{
			Vector2 player_centre = { player_rect.x + player_rect.width / 2.0f, player_rect.y + player_rect.height / 2.0f };

			float radius = this->Attributes->Data[Attribute::CircleRadius];
			this->CurrentAngle += this->Speed * TICK_TIME;

			if (this->CurrentAngle >= 2 * 3.142)
				this->CurrentAngle = 0;

			this->Rect.x = player_centre.x + radius * cos(this->CurrentAngle) + this->Rect.width / 2;
			this->Rect.y = player_centre.y + radius * sin(this->CurrentAngle) - this->Rect.height / 2;
			break;
		}
		default:
			this->Rect.x += this->Direction.x * this->Speed * TICK_TIME;
			this->Rect.y += this->Direction.y * this->Speed * TICK_TIME;
	}
}
