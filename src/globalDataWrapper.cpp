#include "globalDataWrapper.hpp"
#include <string>

void GlobalDataWrapper::Reset() noexcept
{
	this->CacheString("Level: 1", CachedString::LevelText);
	this->CacheString("Duration: 0s", CachedString::Duration);
	this->CacheString("", CachedString::UnclaimedPowerups);
	this->CacheString("", CachedString::LevelDebuff);

	this->UnclaimedPowerups = 0;
}

void GlobalDataWrapper::CacheString(std::string&& string, const CachedString cached_string) noexcept
{
	const size_t index = static_cast<size_t>(cached_string);

	this->StringCache[index] = std::move(string);
}
