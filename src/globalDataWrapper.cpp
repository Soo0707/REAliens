#include "globalDataWrapper.hpp"

void GlobalDataWrapper::Reset() noexcept
{
	this->CachedStrings =
	{
		{ CachedString::LevelText, "Level: 1" },
		{ CachedString::Duration, "Duration: 0s" },

		{ CachedString::UnclaimedPowerups, "" },
		{ CachedString::LevelDebuff, "" }
	};

	this->UnclaimedPowerups = 0;
}
