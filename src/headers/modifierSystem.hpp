#pragma once

#include <cstddef>
#include <array>
#include <limits>

#include "modifiers.hpp"
#include "messageSystem.hpp"

class ModifierSystem
{
	public:
		ModifierSystem() = default;
		~ModifierSystem() = default;

		void PollSignals(MessageSystem& message_system) noexcept;

		bool EffectStatus(const Effect effect) const noexcept;
		float GetAttribute(const Attribute attribute) const noexcept;

		void Reset() noexcept;

	private:
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
		void ApplyProjectile() noexcept;
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

		std::array<float, static_cast<size_t>(Attribute::COUNT)> Attributes;

		static constexpr std::array<void(ModifierSystem::*)() noexcept, static_cast<size_t>(ModifierSystemSignal::COUNT)> SignalHandlers =
		{
			&ApplyMilk,
			&ApplyGreenbull,
			&ApplyMagnetism,
			&ApplyAura,
			&ApplyBuckshot,
			&ApplyProjectile,
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
			&RemoveLevelDebuff
		};

		static constexpr std::array<Effect, 4> DebuffList = { 
			Effect::Microscope,
			Effect::Earthquake,
			Effect::Stinky,
			Effect::Invisible
		};
		
		Effect Effects;
};
