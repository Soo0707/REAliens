#include "globalDataWrapper.hpp"

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
	
	this->Level = 1;

	this->CurrentLayer = 0;

	this->UnclaimedPowerups = 0;
}
/*
void GlobalDataWrapper::ApplyMicroscope() noexcept
{}
*/
