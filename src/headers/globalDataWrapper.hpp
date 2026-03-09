#pragma once

#include "raylib.h"

#include <string>
#include <array>
#include <cstddef>
#include <cstdint>

#include "constants.hpp"

enum class CachedString : uint8_t
{
	LevelText,
	UnclaimedPowerups,

	Duration,
	DamagePerSecond,

	AverageSpeed,

	EnemiesKilled,
	COUNT
};

struct GlobalDataWrapper
{
	GlobalDataWrapper() = default;
	~GlobalDataWrapper() = default;

	GlobalDataWrapper(const GlobalDataWrapper&) = delete;
	GlobalDataWrapper& operator=(const GlobalDataWrapper&) = delete;

	GlobalDataWrapper(const GlobalDataWrapper&&) = delete;
	GlobalDataWrapper& operator=(const GlobalDataWrapper&&) = delete;
	
	void CacheString(std::string&& string, const CachedString cached_string) noexcept;
	void Reset() noexcept;

	std::array<std::string, static_cast<size_t>(CachedString::COUNT)> StringCache;
};
