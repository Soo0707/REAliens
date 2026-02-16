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
	DisableLevelDebuffs,

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

