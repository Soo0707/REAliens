#include "settingsManager.hpp"

#include <filesystem>
#include <fstream>
#include <string>

SettingsManager::SettingsManager()
{
	std::string config_file_path = "config.txt";

	if (!std::filesystem::exists(config_file_path))
		return;

	std::ifstream config_file(config_file_path);

	if (!config_file.is_open())
		return;

	std::string line;

	std::string str_key;
	std::string str_value;

	while (std::getline(config_file, line))
	{
		size_t comment = line.find("#");

		if (comment != std::string::npos)
			continue;

		size_t delimiter_location = line.find("=");

		if (delimiter_location == std::string::npos)
			exit(1);

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
			this->Data[key] = value;
	}
}

SettingKey SettingsManager::GetSettingKeyFromString(const std::string& key) const noexcept
{
	static const std::unordered_map<std::string, SettingKey> lookup =
	{
		{ "POWERUP_MENU_STEALS_FOCUS", SettingKey::PowerupMenuInterrupt },
		{ "AUTO_CLICK", SettingKey::AutoClick },
		{ "DISABLE_HEALTH_CHECK", SettingKey::DisableHealthCheck },

		{ "UNLIMITED_POWERUPS", SettingKey::UnlimitedPowerups }
	};

	return lookup.count(key) ? lookup.at(key) : SettingKey::None;
}
