#include "modifierSystem.hpp"

#include <limits>
#include <atomic>

#include "signals.hpp"
#include "commands.hpp"
#include "constants.hpp"
#include "modifiers.hpp"
#include "messageSystem.hpp"

void ModifierSystem::PollSignals(MessageSystem& message_system) noexcept
{
	for (size_t i = 0; i < static_cast<size_t>(ModifierSystemSignal::COUNT); i++)
	{
		const uint16_t times = message_system.ModifierSystemSignals[i].exchange(0);
		
		auto signal_handler = this->SignalHandlers[i];

		for (uint16_t i = times; i > 0; i--)
			(this->*signal_handler)();
	}
}

void ModifierSystem::Reset() noexcept
{
	this->Attributes = { 0 };

	this->SetAttribute(Attribute::BulletCooldown, 150.0f);
	this->SetAttribute(Attribute::BulletDamage, 25.0f);
	this->SetAttribute(Attribute::BulletSpeed, 1000.0f);

	this->SetAttribute(Attribute::Buckshot, 3.0f);
	this->SetAttribute(Attribute::BuckshotSpread, PI / 8);

	this->SetAttribute(Attribute::LazerCooldown, 450.0f);
	this->SetAttribute(Attribute::LazerDamage, 25.0f);
	this->SetAttribute(Attribute::LazerScale, 1.0f);
	this->SetAttribute(Attribute::LazerSpeed, 3000.0f);
	this->SetAttribute(Attribute::LazerMaxHit, 5.0f);

	this->SetAttribute(Attribute::BallSpeed, 500.0f);
	this->SetAttribute(Attribute::BallScale, 0.5f);
	this->SetAttribute(Attribute::BallDamage, 5.0f);

	this->IncreaseAttribute(Attribute::AuraSize, 50.0f, static_cast<float>(REFERENCE_HEIGHT));
	this->IncreaseAttribute(Attribute::AuraDamage, 5.0f);

	this->SetAttribute(Attribute::SlideSpeedMultiplier, 2.0f);

	this->SetAttribute(Attribute::LuckBottomLimit, 0.0f);
	this->SetAttribute(Attribute::LuckUpperLimit, 99.0f);
	//TODO: set this to be random and find a way to incorparate it
	this->SetAttribute(Attribute::LuckBoundary, 49.0f);

	this->Effects = static_cast<Effect>(0);
}


bool ModifierSystem::EffectStatus(const Effect effect) const noexcept
{
	return ((this->Effects & effect) == effect);
}

void ModifierSystem::ApplyEffect(const Effect effect) noexcept
{
	this->Effects |= effect;
}

void ModifierSystem::RemoveEffect(const Effect effect) noexcept
{
	this->Effects = this->Effects & ~effect;
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
	this->RemoveEffect(Effect::Trapped);

	this->RemoveEffect(Effect::Poison);
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
	this->SetAttribute(Attribute::AuraSize, 100.0f);
	this->SetAttribute(Attribute::AuraDamage, 5.0f);
}

void ModifierSystem::ApplyBuckshot() noexcept
{
	this->DecreaseAttribute(Attribute::BuckshotSpread, 0.04f, 5 * TO_RAD);

	this->IncreaseAttribute(Attribute::Buckshot, 2.0f);
}

void ModifierSystem::ApplyBullet() noexcept
{
	this->DecreaseAttribute(Attribute::BulletCooldown, 25.0f, 80.0f);

	this->IncreaseAttribute(Attribute::BulletDamage, 5.0f);
	this->IncreaseAttribute(Attribute::BulletSpeed, 100.0f);
}

void ModifierSystem::ApplyLazer() noexcept
{
	this->DecreaseAttribute(Attribute::LazerCooldown, 50.0f, 60.0f);

	this->IncreaseAttribute(Attribute::LazerDamage, 10.0f);
	this->IncreaseAttribute(Attribute::LazerScale, 0.5f);
	this->IncreaseAttribute(Attribute::LazerSpeed, 10.0f);
	this->IncreaseAttribute(Attribute::LazerMaxHit, 5.0f);
}

void ModifierSystem::ApplyBall() noexcept
{
	this->IncreaseAttribute(Attribute::BallSpeed, 100.0f);
	this->IncreaseAttribute(Attribute::BallScale, 0.25f);
	this->IncreaseAttribute(Attribute::BallDamage, 5.0f);
}

void ModifierSystem::ApplyLifeSteal() noexcept
{
	if (this->EffectStatus(Effect::LifeSteal))
		this->IncreaseAttribute(Attribute::LifeStealMultiplier, 0.2f);
	else
	{
		this->SetAttribute(Attribute::LifeStealMultiplier, 0.1f);
		this->ApplyEffect(Effect::LifeSteal);
	}
}

void ModifierSystem::ApplyBabyOil() noexcept
{
	this->IncreaseAttribute(Attribute::SlideSpeedMultiplier, 1.0f);
}

void ModifierSystem::ApplyAussie() noexcept
{
	this->ApplyEffect(Effect::Aussie);
}

void ModifierSystem::ApplyPoison() noexcept
{
	this->ApplyEffect(Effect::Poison);
}

void ModifierSystem::ApplyTrapped() noexcept
{
	this->ApplyEffect(Effect::Trapped);
}

void ModifierSystem::ApplyDrunk() noexcept
{
	this->ApplyEffect(Effect::Drunk);
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

void ModifierSystem::RemoveTrapped() noexcept
{
	this->RemoveEffect(Effect::Trapped);
}

void ModifierSystem::RemoveDrunk() noexcept
{
	this->RemoveEffect(Effect::Drunk);
}


void ModifierSystem::InsertLevelDebuff() noexcept
{
	const int index = GetRandomValue(0, this->DebuffList.size() - 1);

	const Effect random_effect = this->DebuffList[index];

	this->ApplyEffect(random_effect);
}

void ModifierSystem::RemoveLevelDebuff() noexcept
{
	for (auto const effect : this->DebuffList)
		this->RemoveEffect(effect);
}

