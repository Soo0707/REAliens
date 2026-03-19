/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "globalDataWrapper.hpp"

#include <string>

void GlobalDataWrapper::Reset() noexcept
{
	this->CacheString("Level: 1", CachedString::LevelText);
	this->CacheString("Duration: 0s", CachedString::Duration);
	this->CacheString("", CachedString::UnclaimedPowerups);
}

void GlobalDataWrapper::CacheString(std::string&& string, const CachedString cached_string) noexcept
{
	const size_t index = static_cast<size_t>(cached_string);

	this->StringCache[index] = std::move(string);
}
