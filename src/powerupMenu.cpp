#include "powerupMenu.hpp"

#include <memory>
#include <set>
#include "raylib.h"

#include "globalDataWrapper.hpp"
#include "constants.hpp"

PowerupMenu::PowerupMenu(std::shared_ptr<GlobalDataWrapper> global_data) :
	GlobalData(global_data)
{
	PowerupMenu::GenerateList();
}

PowerupMenu::~PowerupMenu()
{}

void PowerupMenu::Draw(RenderTexture2D& canvas)
{
	static constexpr float mid = REFERENCE_WIDTH / 2.0f;

	int x1 = mid - MeasureText(this->SelectionList[0].DisplayName.c_str(), 24) / 2.0f;
	int x2 = mid - MeasureText(this->SelectionList[1].DisplayName.c_str(), 24) / 2.0f;
	int x3 = mid - MeasureText(this->SelectionList[2].DisplayName.c_str(), 24) / 2.0f;

	BeginTextureMode(canvas);
		ClearBackground(BLACK);
		DrawText("Select A Powerup", 452.5, 80, 42, VIOLET);

		DrawText(this->SelectionList[0].DisplayName.c_str(), x1, 260, 24, GOLD);
		DrawText(this->SelectionList[1].DisplayName.c_str(), x2, 360, 24, GOLD);
		DrawText(this->SelectionList[2].DisplayName.c_str(), x3, 460, 24, GOLD);

		DrawText("[TAB] Toggle Powerup Menu", 491.5, 580, 21, LIGHTGRAY);

		DrawText("Unacquired Powerups: ", 50, 670, 21, GRAY);
		DrawText(std::to_string(this->GlobalData->UnclaimedPowerups).c_str(), 310, 670, 21, WHITE);

	EndTextureMode();
	//std::cout << MeasureText("[TAB] Toggle Powerup Menu", 21) << std::endl;
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

	if (IsKeyPressed(KEY_TAB))
	{
		this->GlobalData->Settings[Setting::ShowPowerupMenuOnLevelUp] = 0;
		this->GlobalData->ActiveState = State::Game;
	}
}

void PowerupMenu::ApplyPowerup(Powerup powerup)
{
	switch (powerup)
	{
		case Powerup::Aura:
			PowerupMenu::ApplyAura();
			break;

		case Powerup::Buckshot:
			PowerupMenu::ApplyBuckshot();
			break;

		case Powerup::Projectile:
			PowerupMenu::ApplyProjectile();
			break;

		case Powerup::Lazer:
			PowerupMenu::ApplyLazer();
			break;

		case Powerup::Ball:
			PowerupMenu::ApplyBall();
			break;

		case Powerup::Greenbull:
			PowerupMenu::ApplyEffect(Effect::Greenbull, Event::GreenbullExpire, SECONDS_TO_TICKS(1000));
			break;

		case Powerup::Milk:
			PowerupMenu::ApplyMilk();
			break;

		case Powerup::Magnetism:
			PowerupMenu::ApplyEffect(Effect::Magnetism, Event::MagnetismExpire, SECONDS_TO_TICKS(1000));
			break;

		case Powerup::LifeSteal:
			PowerupMenu::ApplyLifeSteal();
			break;
		case Powerup::PlotArmour:
			this->GlobalData->Events[Event::IncreasePlotArmour] = 0;
			break;
		case Powerup::SpeedBoots:
			this->GlobalData->Events[Event::IncreasePlayerSpeed] = 0;
			break;
	}

	PowerupMenu::GenerateList();
	this->GlobalData->UnclaimedPowerups--;

	if (this->GlobalData->UnclaimedPowerups == 0)
		this->GlobalData->ActiveState = State::Game;
}




void PowerupMenu::ApplyEffect(const Effect effect, const Event event, const unsigned int duration)
{
	this->GlobalData->Effects.insert(effect);
	this->GlobalData->Events[event] = this->GlobalData->Ticks + duration;
}

void PowerupMenu::ApplyMilk()
{
	PowerupMenu::ApplyEffect(Effect::Milk, Event::MilkExpire, SECONDS_TO_TICKS(1000));

	this->GlobalData->Effects.erase(Effect::Aussie);
	this->GlobalData->Effects.erase(Effect::Drunk);
	this->GlobalData->Effects.erase(Effect::Trapped);

	this->GlobalData->Effects.erase(Effect::Poison);

	this->GlobalData->Attributes.erase(Attribute::PoisonDamage);
	this->GlobalData->Events.erase(Event::PoisonTick);
	this->GlobalData->Events.erase(Event::PoisonExpire);
}

void PowerupMenu::ApplyAura()
{
	if (this->GlobalData->Attributes.count(Attribute::AuraSize))
	{
		this->GlobalData->Attributes[Attribute::AuraSize] += 50;
		this->GlobalData->Attributes[Attribute::AuraDamage] += 5;

		if (this->GlobalData->Attributes[Attribute::AuraCooldown] - 25 > TICK_RATE)
			this->GlobalData->Attributes[Attribute::AuraCooldown] -= 25;
		else
			this->GlobalData->Attributes[Attribute::AuraCooldown] = TICK_RATE;
	}
	else
	{
		this->GlobalData->Attributes[Attribute::AuraSize] = 100;
		this->GlobalData->Attributes[Attribute::AuraDamage] = 5;
		this->GlobalData->Attributes[Attribute::AuraCooldown] = SECONDS_TO_TICKS(2);
	}

	this->GlobalData->Events[Event::IncreaseAura] = 0;
	this->GlobalData->Events[Event::AuraTick] = this->GlobalData->Ticks + this->GlobalData->Attributes.at(Attribute::AuraCooldown);
}

void PowerupMenu::ApplyBall()
{
	if (this->GlobalData->Attributes.count(Attribute::BallRadius)) 
	{
		this->GlobalData->Attributes[Attribute::BallDamage] += 10;
		this->GlobalData->Attributes[Attribute::BallScale] += 0.2;

		if (this->GlobalData->Attributes[Attribute::BallAngularSpeed] <= 4 * PI)
			this->GlobalData->Attributes[Attribute::BallAngularSpeed] *= 2;

		if (this->GlobalData->Attributes[Attribute::BallRadius] + 12 < 320)
			this->GlobalData->Attributes[Attribute::BallRadius] += 12;
	}
	else
	{
		this->GlobalData->Attributes[Attribute::BallDamage] = 5.0f;
		this->GlobalData->Attributes[Attribute::BallScale] = 1.0f;
		this->GlobalData->Attributes[Attribute::BallAngularSpeed] = PI / 2;
		this->GlobalData->Attributes[Attribute::BallRadius] = 64;
	}

	this->GlobalData->Events[Event::SpawnAndUpgradeBall] = 0;
}

void PowerupMenu::ApplyBuckshot()
{
	this->GlobalData->Attributes[Attribute::Buckshot] += 2;

	if (this->GlobalData->Attributes[Attribute::BuckshotSpread] >= 0.002f)
		this->GlobalData->Attributes[Attribute::BuckshotSpread] -= 0.002f;
}

void PowerupMenu::ApplyProjectile()
{
	if (this->GlobalData->Attributes[Attribute::BulletCooldown] - 25 > 80)
		this->GlobalData->Attributes[Attribute::BulletCooldown] -= 80;
	else
		this->GlobalData->Attributes[Attribute::BulletCooldown] = 80;

	this->GlobalData->Attributes[Attribute::BulletDamage] += 5;
	this->GlobalData->Attributes[Attribute::BulletSpeed] += 100;
}

void PowerupMenu::ApplyLazer()
{
	if (this->GlobalData->Attributes[Attribute::LazerCooldown] - 50 > 60)
		this->GlobalData->Attributes[Attribute::LazerCooldown] -= 50;
	else
		this->GlobalData->Attributes[Attribute::LazerCooldown] = 60;

	this->GlobalData->Attributes[Attribute::LazerDamage] += 10;
	this->GlobalData->Attributes[Attribute::LazerScale] += 0.5;
	this->GlobalData->Attributes[Attribute::LazerSpeed] += 10;
}

void PowerupMenu::ApplyLifeSteal()
{
	if (this->GlobalData->Effects.count(Effect::LifeSteal))
		this->GlobalData->Attributes[Attribute::LifeStealMultiplier] += 0.2;
	else
	{
		this->GlobalData->Attributes[Attribute::LifeStealMultiplier] = 0.1;
		this->GlobalData->Effects.insert(Effect::LifeSteal);
	}
}
