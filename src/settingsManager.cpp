/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "settingsManager.hpp"

#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <string>

SettingsManager::SettingsManager()
{
	this->Reset();
	this->ReadConfig();
}

SettingKey SettingsManager::GetSettingKeyFromString(const std::string& key) const noexcept
{
	static const std::unordered_map<std::string, SettingKey> lookup =
	{
		{ "POWERUP_MENU_STEALS_FOCUS", SettingKey::PowerupMenuInterrupt },
		{ "AUTO_CLICK", SettingKey::AutoClick },
		{ "DISABLE_HEALTH_CHECK", SettingKey::DisableHealthCheck },

		{ "UNLIMITED_POWERUPS", SettingKey::UnlimitedPowerups },
		{ "TARGET_FRAMERATE", SettingKey::TargetFramerate }
	};

	return lookup.count(key) ? lookup.at(key) : SettingKey::None;
}

int SettingsManager::Get(const SettingKey setting) const noexcept
{
	return this->Data[static_cast<size_t>(setting)];
}

void SettingsManager::Set(const SettingKey setting, const int value) noexcept
{
	this->Data[static_cast<size_t>(setting)] = value;
}

void SettingsManager::Toggle(const SettingKey setting) noexcept
{
	this->Data[static_cast<size_t>(setting)] = !this->Data[static_cast<size_t>(setting)];
}

void SettingsManager::ReadConfig()
{
	if (!std::filesystem::exists("config.txt"))
		return;

	std::ifstream config_file("config.txt");

	if (!config_file.is_open())
		return;

	std::string line;

	std::string str_key;
	std::string str_value;

	while (std::getline(config_file, line))
	{
		const size_t comment = line.find("#");

		if (comment != std::string::npos)
			continue;

		const size_t delimiter_location = line.find("=");

		if (delimiter_location == std::string::npos)
			continue;

		str_key = line.substr(0, delimiter_location);
		str_value = line.substr(delimiter_location + 1, line.length());

		int value = 0;

		try
		{
			value = std::stoi(str_value);
		}
		catch(...)
		{
			exit(1);
		}

		SettingKey key = SettingsManager::GetSettingKeyFromString(str_key);

		if (key != SettingKey::None)
			this->Set(key, value);
	}
}

void SettingsManager::Reset() noexcept
{
	this->Data = { 0 };

	this->Set(SettingKey::PowerupMenuInterrupt, 1);
}
