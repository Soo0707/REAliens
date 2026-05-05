/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "modifierSystem.hpp"

#include <limits>
#include <variant>

#include "stringCache.hpp"
#include "signals.hpp"
#include "commands.hpp"
#include "constants.hpp"
#include "modifiers.hpp"
#include "settingsManager.hpp"
#include "messageSystem.hpp"

void ModifierSystem::Update(MessageSystem& message_system, StringCache& string_cache, const SettingsManager& settings) noexcept
{
	this->PollSignals(message_system);

	while (this->CollectedXp >= this->LevelUpThreshold)
	{
		const size_t distance = this->CollectedXp - this->LevelUpThreshold;

		this->LevelUp(message_system, string_cache, settings);
		this->CollectedXp = distance;
	}
}

void ModifierSystem::PollSignals(MessageSystem& message_system) noexcept
{
	for (size_t i = 0; i < static_cast<size_t>(ModifierSystemSignal::COUNT); i++)
	{
		const uint16_t times = message_system.ModifierSystemSignals[i];
		
		auto signal_handler = this->SignalHandlers[i];

		for (uint16_t i = times; i > 0; i--)
			(this->*signal_handler)();

		message_system.ModifierSystemSignals[i] = 0;
	}
}

void ModifierSystem::Reset() noexcept
{
	this->Attributes = { 0 };

	this->SetAttribute(Attribute::BulletDamage, 25.0f);
	this->SetAttribute(Attribute::BulletSpeed, 1000.0f);
	this->SetAttribute(Attribute::BulletPenetration, 1.0f);

	this->SetAttribute(Attribute::Buckshot, 3.0f);
	this->SetAttribute(Attribute::BuckshotSpread, PI / 8);

	this->SetAttribute(Attribute::LazerDamage, 25.0f);
	this->SetAttribute(Attribute::LazerSpeed, 3000.0f);
	this->SetAttribute(Attribute::LazerPenetration, 5.0f);

	this->SetAttribute(Attribute::BallDamage, 25.0f);
	this->SetAttribute(Attribute::BallSpeed, 500.0f);
	this->SetAttribute(Attribute::BallPenetration, 1.0f);
	this->SetAttribute(Attribute::BallSplitLazers, 3.0f);

	this->SetAttribute(Attribute::GlueDamage, 5.0f);
	this->SetAttribute(Attribute::GlueSpeed, 750.0f);
	this->SetAttribute(Attribute::GluePenetration, 1.0f);

	this->SetAttribute(Attribute::PlebifierDamage, 5.0f);
	this->SetAttribute(Attribute::PlebifierSpeed, 1000.0f);
	this->SetAttribute(Attribute::PlebifierPenetration, 1.0f);

	this->SetAttribute(Attribute::AuraSize, 100.0f);
	this->SetAttribute(Attribute::AuraDamage, 25.0f);

	this->SetAttribute(Attribute::SlideSpeedMultiplier, 2.0f);

	this->SetAttribute(Attribute::LuckBoundary, 79.0f);
	this->SetAttribute(Attribute::GreedMultiplier, 0.0f);

	this->Effects = static_cast<Effect>(0);
	
	this->Level = 1;
	this->CollectedXp = 0;
	this->LevelUpThreshold = 5;
	this->UnclaimedPowerups = 0;
}

bool ModifierSystem::EffectStatus(const Effect effect) const noexcept
{
	return ((this->Effects & effect) == effect);
}

bool ModifierSystem::IsLucky() const noexcept
{
	return GetRandomValue(1, 100) < static_cast<int>(this->GetAttribute(Attribute::LuckBoundary));
}

void ModifierSystem::ApplyEffect(const Effect effect) noexcept
{
	this->Effects = this->Effects | effect;
}

void ModifierSystem::RemoveEffect(const Effect effect) noexcept
{
	this->Effects = this->Effects & ~effect;
}

size_t ModifierSystem::GetLevel() const noexcept
{
	return this->Level;
}

size_t ModifierSystem::GetCollectedXp() const noexcept
{
	return this->CollectedXp;
}

size_t ModifierSystem::GetLevelUpThreshold() const noexcept
{
	return this->LevelUpThreshold;
}

size_t ModifierSystem::GetUnclaimedPowerups() const noexcept
{
	return this->UnclaimedPowerups;
}

void ModifierSystem::DecrementUnclaimedPowerups() noexcept
{
	this->UnclaimedPowerups--;
}

void ModifierSystem::LevelUp(MessageSystem& message_system, StringCache& string_cache, const SettingsManager& settings) noexcept
{
	if (!settings.Get(SettingKey::UnlimitedPowerups))
	{
		this->UnclaimedPowerups++;
		string_cache.CacheString(
				"Unclaimed Poweurps: " + std::to_string(this->UnclaimedPowerups),
				GameString::UnclaimedPowerups
				);
	}

	this->Level++;
	string_cache.CacheString("Level: " + std::to_string(this->Level), GameString::LevelText);

	this->CollectedXp = 0;
	this->LevelUpThreshold += 5;
	
	if (settings.Get(SettingKey::PowerupMenuInterrupt))
		message_system.StateManagerCommands.emplace_back(std::in_place_type<SetState>, State::PowerupMenu);

	if (this->Level % 5 == 0)
	{
		this->InsertLevelDebuff();
		message_system.InventorySystemSignals[static_cast<size_t>(InventorySystemSignal::GrantItem)]++;
	}
	else
		this->RemoveLevelDebuff();
}


float ModifierSystem::GetAttribute(const Attribute attribute) const noexcept
{
	const size_t index = static_cast<size_t>(attribute);

	return this->Attributes[index];
}

void ModifierSystem::SetAttribute(const Attribute attribute, const float value) noexcept
{
	size_t index = static_cast<size_t>(attribute);

	this->Attributes[index] = value;
}


void ModifierSystem::IncreaseAttribute(const Attribute attribute, const float amount, const float custom_max) noexcept
{
	const size_t index = static_cast<size_t>(attribute);

	const float max = std::min(custom_max, std::numeric_limits<float>::max());
	const float distance_from_max = max - this->Attributes[index];

	if (amount < distance_from_max)
		this->Attributes[index] += amount;
	else
		this->Attributes[index] = max;
}

void ModifierSystem::DecreaseAttribute(const Attribute attribute, const float amount, const float custom_min) noexcept
{
	const size_t index = static_cast<size_t>(attribute);

	const float min = std::max(custom_min, 0.0f);
	const float new_value = this->Attributes[index] - amount;

	if (new_value <= min)
		this->Attributes[index] = min;
	else
		this->Attributes[index] = new_value;
}


void ModifierSystem::ApplyMilk() noexcept
{
	this->ApplyEffect(Effect::Milk);

	this->RemoveEffect(Effect::Aussie);
	this->RemoveEffect(Effect::Drunk);

	this->RemoveEffect(Effect::Poison);
	this->RemoveEffect(Effect::Weakness);
	this->RemoveEffect(Effect::Tariffs);
}

void ModifierSystem::ApplyGreenbull() noexcept
{
	this->ApplyEffect(Effect::Greenbull);
}

void ModifierSystem::ApplyMagnetism() noexcept
{
	this->ApplyEffect(Effect::Magnetism);
}

void ModifierSystem::ApplyAura() noexcept
{
	this->IncreaseAttribute(Attribute::AuraSize, 100.0f, static_cast<float>(REFERENCE_HEIGHT));
	this->IncreaseAttribute(Attribute::AuraDamage, 15.0f);
}

void ModifierSystem::ApplyBuckshot() noexcept
{
	this->DecreaseAttribute(Attribute::BuckshotSpread, 0.04f, 5 * TO_RAD);

	this->IncreaseAttribute(Attribute::Buckshot, 2.0f);
}

void ModifierSystem::ApplyBullet() noexcept
{
	this->IncreaseAttribute(Attribute::BulletDamage, 50.0f);
	this->IncreaseAttribute(Attribute::BulletSpeed, 100.0f);
}

void ModifierSystem::ApplyLazer() noexcept
{
	this->IncreaseAttribute(Attribute::LazerDamage, 25.0f);
	this->IncreaseAttribute(Attribute::LazerSpeed, 75.0f);
	this->IncreaseAttribute(Attribute::LazerPenetration, 10.0f);
}

void ModifierSystem::ApplyBall() noexcept
{
	this->IncreaseAttribute(Attribute::BallDamage, 50.0f);
	this->IncreaseAttribute(Attribute::BallSpeed, 100.0f);
	this->IncreaseAttribute(Attribute::BallSplitLazers, 2.0f);
}

void ModifierSystem::ApplyGlue() noexcept
{
	this->IncreaseAttribute(Attribute::GlueDamage, 10.0f);
	this->IncreaseAttribute(Attribute::GlueSpeed, 50.0f);
}

void ModifierSystem::ApplyPlebifier() noexcept
{
	this->IncreaseAttribute(Attribute::PlebifierDamage, 10.0f);
	this->IncreaseAttribute(Attribute::PlebifierSpeed, 50.0f);
}

void ModifierSystem::ApplyLifeSteal() noexcept
{
	if (this->EffectStatus(Effect::LifeSteal))
		this->IncreaseAttribute(Attribute::LifeStealMultiplier, 0.2f);
	else
	{
		this->SetAttribute(Attribute::LifeStealMultiplier, 0.4f);
		this->ApplyEffect(Effect::LifeSteal);
	}
}

void ModifierSystem::ApplyBabyOil() noexcept
{
	this->IncreaseAttribute(Attribute::SlideSpeedMultiplier, 1.0f);
}

void ModifierSystem::ApplyLuck() noexcept
{
	this->DecreaseAttribute(Attribute::LuckBoundary, 10.0f, 10.0f);
}

void ModifierSystem::ApplyGreed() noexcept
{
	this->IncreaseAttribute(Attribute::GreedMultiplier, 1.0f);
}

void ModifierSystem::ApplyAussie() noexcept
{
	this->ApplyEffect(Effect::Aussie);
}

void ModifierSystem::ApplyPoison() noexcept
{
	this->ApplyEffect(Effect::Poison);
}

void ModifierSystem::ApplyDrunk() noexcept
{
	this->ApplyEffect(Effect::Drunk);
}

void ModifierSystem::ApplyTariffs() noexcept
{
	this->ApplyEffect(Effect::Tariffs);
}

void ModifierSystem::ApplyWeakness() noexcept
{
	this->ApplyEffect(Effect::Weakness);
}

void ModifierSystem::ApplyAlcoholism() noexcept
{
	this->ApplyEffect(Effect::Alcoholism);
}

void ModifierSystem::RemoveMilk() noexcept
{
	this->RemoveEffect(Effect::Milk);
}

void ModifierSystem::RemoveGreenbull() noexcept
{
	this->RemoveEffect(Effect::Greenbull);
}

void ModifierSystem::RemoveMagnetism() noexcept
{
	this->RemoveEffect(Effect::Magnetism);
}

void ModifierSystem::RemoveAussie() noexcept
{
	this->RemoveEffect(Effect::Aussie);
}

void ModifierSystem::RemovePoison() noexcept
{
	this->RemoveEffect(Effect::Poison);
}

void ModifierSystem::RemoveDrunk() noexcept
{
	this->RemoveEffect(Effect::Drunk);
}

void ModifierSystem::RemoveTariffs() noexcept
{
	this->RemoveEffect(Effect::Tariffs);
}

void ModifierSystem::RemoveWeakness() noexcept
{
	this->RemoveEffect(Effect::Weakness);
}

void ModifierSystem::RemoveAlcoholism() noexcept
{
	this->RemoveEffect(Effect::Alcoholism);
}

void ModifierSystem::InsertLevelDebuff() noexcept
{
	const size_t index = GetRandomValue(0, this->DebuffList.size() - 1);

	const Effect random_effect = this->DebuffList[index];
	this->ApplyEffect(random_effect);
}

void ModifierSystem::RemoveLevelDebuff() noexcept
{
	for (auto const effect : this->DebuffList)
		this->RemoveEffect(effect);
}

void ModifierSystem::IncrementCollectedXp() noexcept
{
	const uint32_t has_tariffs = static_cast<uint32_t>(!this->EffectStatus(Effect::Tariffs));
	const uint32_t bonus = static_cast<uint32_t>(this->IsLucky()) * static_cast<uint32_t>(this->GetAttribute(Attribute::GreedMultiplier));

	this->CollectedXp += static_cast<size_t>(has_tariffs * (1 + bonus));
}
