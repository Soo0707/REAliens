#include "globalDataWrapper.hpp"

#include "raylib.h"

void GlobalDataWrapper::Reset() noexcept
{
	this->CachedStrings =
	{
		{ CachedString::LevelText, "Level: 1" },
		{ CachedString::Duration, "Duration: 0s" },

		{ CachedString::UnclaimedPowerups, "" },
		{ CachedString::LevelDebuff, "" }
	};
}
