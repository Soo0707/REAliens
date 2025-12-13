#pragma once

#include <unordered_map>
#include <cstddef>
#include <string>

enum class SettingKey : size_t
{
	AutoClick,
	DisableHealthCheck,
	PowerupMenuInterrupt,

	UnlimitedPowerups,
	TargetFramerate,
	DisableLevelDebuffs,

	None
};

class SettingsManager
{
	public:
		SettingsManager();
		~SettingsManager() = default;

		std::unordered_map<SettingKey, unsigned int> Data =
		{
			{ SettingKey::PowerupMenuInterrupt, 1 },
			{ SettingKey::AutoClick, 0 },
			{ SettingKey::DisableHealthCheck, 0 },

			{ SettingKey::TargetFramerate, 0 },
			{ SettingKey::UnlimitedPowerups, 0 },

			{ SettingKey::DisableLevelDebuffs, 0 }
		};

	private:
		SettingKey GetSettingKeyFromString(const std::string& key) const noexcept;
};

