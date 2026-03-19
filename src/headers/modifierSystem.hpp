/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>
#include <array>
#include <limits>

#include "modifiers.hpp"
#include "settingsManager.hpp"
#include "messageSystem.hpp"

class ModifierSystem
{
	public:
		ModifierSystem() = default;
		~ModifierSystem() = default;

		void Update(MessageSystem& message_system, const SettingsManager& settings) noexcept;

		void PollSignals(MessageSystem& message_system) noexcept;

		bool EffectStatus(const Effect effect) const noexcept;
		float GetAttribute(const Attribute attribute) const noexcept;

		size_t GetLevel() const noexcept;
		size_t GetCollectedXp() const noexcept;
		size_t GetLevelUpThreshold() const noexcept;
		size_t GetUnclaimedPowerups() const noexcept;
		void DecrementUnclaimedPowerups() noexcept;

		void Reset() noexcept;

	private:
		void LevelUp(MessageSystem& message_system, const SettingsManager& settings) noexcept;

		void ApplyEffect(const Effect effect) noexcept;
		void RemoveEffect(const Effect effect) noexcept;

		void SetAttribute(const Attribute attribute, const float value) noexcept;
		void IncreaseAttribute(const Attribute attribute, const float amount, const float custom_max = std::numeric_limits<float>::max()) noexcept;
		void DecreaseAttribute(const Attribute attribute, const float amount, const float custom_min = 0.0f) noexcept;

		void ApplyMilk() noexcept;
		void ApplyGreenbull() noexcept;
		void ApplyMagnetism() noexcept;
		void ApplyAura() noexcept;
		void ApplyBuckshot() noexcept;
		void ApplyBullet() noexcept;
		void ApplyLazer() noexcept;
		void ApplyBall() noexcept;
		void ApplyLifeSteal() noexcept;
		void ApplyBabyOil() noexcept;
		void ApplyAussie() noexcept;
		void ApplyPoison() noexcept;
		void ApplyTrapped() noexcept;
		void ApplyDrunk() noexcept;


		void RemoveMilk() noexcept;
		void RemoveGreenbull() noexcept;
		void RemoveMagnetism() noexcept;
		void RemoveAussie() noexcept;
		void RemovePoison() noexcept;
		void RemoveTrapped() noexcept;
		void RemoveDrunk() noexcept;

		void InsertLevelDebuff() noexcept;
		void RemoveLevelDebuff() noexcept;

		void IncrementCollectedXp() noexcept;

		std::array<float, static_cast<size_t>(Attribute::COUNT)> Attributes;

		static constexpr std::array<void(ModifierSystem::*)() noexcept, static_cast<size_t>(ModifierSystemSignal::COUNT)> SignalHandlers =
		{
			&ApplyMilk,
			&ApplyGreenbull,
			&ApplyMagnetism,
			&ApplyAura,
			&ApplyBuckshot,
			&ApplyBullet,
			&ApplyLazer,
			&ApplyBall,
			&ApplyLifeSteal,
			&ApplyBabyOil,
			&ApplyAussie,
			&ApplyPoison,
			&ApplyTrapped,
			&ApplyDrunk,

			&RemoveMilk,
			&RemoveGreenbull,
			&RemoveMagnetism,
			&RemoveAussie,
			&RemovePoison,
			&RemoveTrapped,
			&RemoveDrunk,

			&InsertLevelDebuff,
			&RemoveLevelDebuff,

			&IncrementCollectedXp
		};

		static constexpr std::array<Effect, 5> DebuffList = { 
			Effect::Microscope,
			Effect::Earthquake,
			Effect::Stinky,
			Effect::Invisible,
			Effect::DiddySlide
		};
		
		size_t Level;
		size_t CollectedXp;
		size_t UnclaimedPowerups;
		size_t LevelUpThreshold;
		
		Effect Effects;
};
