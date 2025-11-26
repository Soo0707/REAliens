#include "globalDataWrapper.hpp"

#include <array>

#include "raylib.h"

void GlobalDataWrapper::Reset() noexcept
{
	this->Attributes = 
	{
		{ Attribute::BulletCooldown, 150 },
		{ Attribute::BulletDamage, 25.0f },
		{ Attribute::BulletSpeed, 1000.0f },

		{ Attribute::Buckshot, 3 },
		{ Attribute::BuckshotSpread, PI / 8 },

		{ Attribute::LazerCooldown, 450 },
		{ Attribute::LazerDamage, 25.0f },
		{ Attribute::LazerScale, 1.0f },
		{ Attribute::LazerSpeed, 3000.0f }	
	};

	this->CachedStrings =
	{
		{ CachedString::LayerText, "Current Layer: 0" },
		{ CachedString::LevelText, "Level: 1" },
		{ CachedString::Duration, "Duration: 0s" },

		{ CachedString::UnclaimedPowerups, "" }
	};
	
	this->Events.clear();
	this->Effects.clear();

	this->Ticks = 0;
	this->TotalDamage = 0;
	this->TotalDistance = 0;
	
	this->EnemiesKilled = 0;

	this->Level = 1;

	this->CurrentLayer = 0;

	this->UnclaimedPowerups = 0;
}

void GlobalDataWrapper::InsertLevelDebuff() noexcept
{
	Effect random_effect = this->DebuffList[GetRandomValue(0, this->DebuffList.size() - 1)];
	this->Effects.insert(random_effect);
}

void GlobalDataWrapper::RemoveLevelDebuff() noexcept
{
	for (auto const effect : this->DebuffList)
		this->Effects.erase(effect);
}


