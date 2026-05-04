/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "stringCache.hpp"

#include <string>

void StringCache::Reset() noexcept
{
	this->CacheString("Level: 1", GameString::LevelText);
	this->CacheString("Duration: 0s", GameString::Duration);
	this->CacheString("", GameString::UnclaimedPowerups);
	this->CacheString("Kills: 0", GameString::EnemiesKilled);
	this->CacheString("Enemies: 0", GameString::CurrentEnemyCount);
	this->CacheString("Total Entities: 0", GameString::TotalEntities);
}

void StringCache::CacheString(std::string&& string, const GameString game_string) noexcept
{
	const size_t index = static_cast<size_t>(game_string);

	this->Data[index] = std::move(string);
}
