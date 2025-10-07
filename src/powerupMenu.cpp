#include "powerupMenu.hpp"

#include <memory>
#include <set>
#include "raylib.h"

#include "globalDataWrapper.hpp"

PowerupMenu::PowerupMenu(std::shared_ptr<GlobalDataWrapper> global_data) :
	GlobalData(global_data)
{
	PowerupMenu::GenerateList();
}

PowerupMenu::~PowerupMenu()
{}

void PowerupMenu::Draw()
{
	float mid = 1280 / 2.0f;

	int x1 = mid - MeasureText(this->SelectionList[0].DisplayName.c_str(), 24) / 2.0f;
	int x2 = mid - MeasureText(this->SelectionList[1].DisplayName.c_str() , 24) / 2.0f;
	int x3 = mid - MeasureText(this->SelectionList[2].DisplayName.c_str() , 24) / 2.0f;

	DrawText(this->SelectionList[0].DisplayName.c_str(), x1, 260, 24, VIOLET);
	DrawText(this->SelectionList[1].DisplayName.c_str(), x2, 360, 24, VIOLET);
	DrawText(this->SelectionList[2].DisplayName.c_str(), x3, 460, 24, VIOLET);
}

void PowerupMenu::GenerateList()
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

void PowerupMenu::HandleInput()
{
	if (IsKeyPressed(KEY_ONE))
		PowerupMenu::ApplyPowerup(this->SelectionList[0].Powerup);
	else if (IsKeyPressed(KEY_TWO))
		PowerupMenu::ApplyPowerup(this->SelectionList[1].Powerup);
	else if (IsKeyPressed(KEY_THREE))
		PowerupMenu::ApplyPowerup(this->SelectionList[2].Powerup);
}

void PowerupMenu::ApplyPowerup(Powerup powerup)
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
			this->GlobalData->Attributes[Attribute::CircleRadius] += 12;

			this->GlobalData->Events[Event::UpgradeCircle] = 0;

			break;
		case Powerup::Greenbull:
			this->GlobalData->Attributes[Attribute::Greenbull] = 0;
			this->GlobalData->Events[Event::GreenbullExpire] = this->GlobalData->Ticks + 240'000;
			break;
		case Powerup::Milk:
			this->GlobalData->Attributes[Attribute::Milk] = 0;
			this->GlobalData->Events[Event::GreenbullExpire] = this->GlobalData->Ticks + 240'000;
			break;
		case Powerup::Magnetism:
			this->GlobalData->Attributes[Attribute::Magnetism] = 0;
			this->GlobalData->Events[Event::MagnetismExpire] = this->GlobalData->Ticks + 240'000;
			break;
		case Powerup::LifeSteal:
			break;
		case Powerup::Regeneration:
			break;
	}

	PowerupMenu::GenerateList();
	this->GlobalData->ActiveState = State::Game;
}
