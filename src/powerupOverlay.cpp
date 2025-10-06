#include "powerupOverlay.hpp"

#include <memory>
#include <set>
#include "raylib.h"

#include "globalDataWrapper.hpp"

PowerupOverlay::PowerupOverlay(std::shared_ptr<GlobalDataWrapper> global_data) :
	GlobalData(global_data)
{
	int screen_width = GetScreenWidth();
	int screen_height = GetScreenHeight();

	this->Rect = { 390, 160, 500, 400 };

	PowerupOverlay::GenerateList();
}

PowerupOverlay::~PowerupOverlay()
{}

void PowerupOverlay::Draw()
{
	DrawRectangleRounded(this->Rect, 0.2, 0, {0, 0, 0, 255});

	float mid = this->Rect.x + this->Rect.width / 2.0f;

	int x1 = mid - MeasureText(this->SelectionList[0].DisplayName.c_str(), 24) / 2.0f;
	int x2 = mid - MeasureText(this->SelectionList[1].DisplayName.c_str() , 24) / 2.0f;
	int x3 = mid - MeasureText(this->SelectionList[2].DisplayName.c_str() , 24) / 2.0f;

	DrawText(this->SelectionList[0].DisplayName.c_str(), x1, 260, 24, VIOLET);
	DrawText(this->SelectionList[1].DisplayName.c_str(), x2, 360, 24, VIOLET);
	DrawText(this->SelectionList[2].DisplayName.c_str(), x3, 460, 24, VIOLET);
}

void PowerupOverlay::GenerateList()
{
	std::set<Powerup> powerup_set = {};
	while (true)
	{
		Powerup powerup = static_cast<Powerup>( GetRandomValue(0, ( static_cast<int>(Powerup::COUNT) ) - 1) );
		
		if (!powerup_set.count(powerup))
			powerup_set.insert(powerup);

		if (powerup_set.size() == 3)
			break;
	}

	this->SelectionList.clear();
	
	for (auto const &powerup : powerup_set)
	{
		int i = this->SelectionList.size();
		this->SelectionList.emplace_back( powerup, std::to_string(i + 1) + ". " + this->PowerupNames.at(powerup) );
	}
}

void PowerupOverlay::HandleInput()
{
	if (IsKeyPressed(KEY_ONE))
		PowerupOverlay::ApplyPowerup(this->SelectionList[0].Powerup);
	else if (IsKeyPressed(KEY_TWO))
		PowerupOverlay::ApplyPowerup(this->SelectionList[1].Powerup);
	else if (IsKeyPressed(KEY_THREE))
		PowerupOverlay::ApplyPowerup(this->SelectionList[2].Powerup);
}

void PowerupOverlay::ApplyPowerup(Powerup powerup)
{
	switch (powerup)
	{
		case Powerup::Aura:
			break;
		case Powerup::Buckshot:
			this->GlobalData->Attributes[Attribute::Buckshot] += 2;

			if (this->GlobalData->Attributes[Attribute::BuckshotSpread] >= 0.002f)
				this->GlobalData->Attributes[Attribute::BuckshotSpread] -= 0.002f;
			break;
		case Powerup::Projectile:
			if (this->GlobalData->Attributes[Attribute::BulletCooldown] > 0)
				this->GlobalData->Attributes[Attribute::BulletCooldown] -= 5;

			this->GlobalData->Attributes[Attribute::BulletDamage] += 5;
			this->GlobalData->Attributes[Attribute::BulletSpeed] -= 5;

			break;
		case Powerup::Lazer:
			if (this->GlobalData->Attributes[Attribute::LazerCooldown] > 0)
				this->GlobalData->Attributes[Attribute::LazerCooldown] -= 10;

			this->GlobalData->Attributes[Attribute::LazerDamage] += 10;
			this->GlobalData->Attributes[Attribute::LazerScale] += 0.5;
			this->GlobalData->Attributes[Attribute::LazerSpeed] += 10;

			break;
		case Powerup::Circle:
			this->GlobalData->Attributes[Attribute::CircleDamage] += 10;
			this->GlobalData->Attributes[Attribute::CircleScale] += 0.2;
			this->GlobalData->Attributes[Attribute::CircleAngularSpeed] *= 2;
			this->GlobalData->Attributes[Attribute::CircleRadius] += 4;

			break;
		case Powerup::Greenbull:
			break;
		case Powerup::Milk:
			break;
		case Powerup::LifeSteal:
			break;
		case Powerup::Regeneration:
			break;
	}

	this->GlobalData->ActiveState = State::Game;
}
