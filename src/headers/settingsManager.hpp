/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <array>

enum class SettingKey : uint8_t
{
	None,
	AutoClick,
	DisableHealthCheck,
	PowerupMenuInterrupt,

	UnlimitedPowerups,
	TargetFramerate,

	COUNT
};

class SettingsManager
{
	public:
		SettingsManager();
		~SettingsManager() = default;

		int Get(const SettingKey setting) const noexcept;

	private:
		SettingKey GetSettingKeyFromString(const std::string& key) const noexcept;
		void Set(const SettingKey setting, const int value) noexcept;
		void Reset() noexcept;
		void ReadConfig();

		std::array<int, static_cast<size_t>(SettingKey::COUNT)> Data;
};

