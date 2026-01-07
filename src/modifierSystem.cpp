#include "modifierSystem.hpp"

#include <limits>

#include "commands.hpp"
#include "constants.hpp"
#include "modifiers.hpp"
#include "messageSystem.hpp"

void ModifierSystem::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto const& command : message_system.ModifierSystemCommands)
	{
		const size_t index = static_cast<size_t>(command.Type);
		auto command_handler = this->CommandHandlers[index];

		(this->*command_handler)();
	}

	message_system.ModifierSystemCommands.clear();
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

	this->SetAttribute(Attribute::SlideSpeedMultiplier, 2.0f);

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

// Handlers
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
	if (this->GetAttribute(Attribute::AuraSize) > 0.0f)
	{
		this->IncreaseAttribute(Attribute::AuraSize, 50.0f, static_cast<float>(REFERENCE_HEIGHT));
		this->IncreaseAttribute(Attribute::AuraDamage, 5.0f);
	}
	else
	{
		this->SetAttribute(Attribute::AuraSize, 100.0f);
		this->SetAttribute(Attribute::AuraDamage, 5.0f);
	}
}

void ModifierSystem::ApplyBuckshot() noexcept
{
	this->DecreaseAttribute(Attribute::BuckshotSpread, 0.02f, 5 * TO_RAD);

	this->IncreaseAttribute(Attribute::Buckshot, 2.0f);
}

void ModifierSystem::ApplyProjectile() noexcept
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
	//events[Event::AussieExpire] = ticks + SECONDS_TO_TICKS(1) * scale;
}

void ModifierSystem::ApplyPoison() noexcept
{
	this->ApplyEffect(Effect::Poison);

	//events[Event::PoisonTick] = ticks + SECONDS_TO_TICKS(1);
	//events[Event::PoisonExpire] = ticks + SECONDS_TO_TICKS(5) * scale;
}

void ModifierSystem::ApplyTrapped() noexcept
{
	this->ApplyEffect(Effect::Trapped);
}

void ModifierSystem::ApplyDrunk() noexcept
{
	this->ApplyEffect(Effect::Drunk);
	//events[Event::DrunkExpire] = ticks + SECONDS_TO_TICKS(5) * scale;
}


void ModifierSystem::RemoveMilk() noexcept
{
	this->RemoveEffect(Effect::Milk);
	// remove timer
}

void ModifierSystem::RemoveGreenbull() noexcept
{
	this->RemoveEffect(Effect::Greenbull);

	//PowerupMenu::ApplyEffect(Effect::Greenbull, Event::GreenbullExpire, SECONDS_TO_TICKS(120));
}

void ModifierSystem::RemoveMagnetism() noexcept
{
	this->RemoveEffect(Effect::Magnetism);
	//PowerupMenu::ApplyEffect(Effect::Magnetism, Event::MagnetismExpire, SECONDS_TO_TICKS(240));
}

void ModifierSystem::RemoveAussie() noexcept
{
	this->RemoveEffect(Effect::Aussie);
	//events[Event::AussieExpire] = ticks + SECONDS_TO_TICKS(1) * scale;
}

void ModifierSystem::RemovePoison() noexcept
{
	this->RemoveEffect(Effect::Poison);

	//events[Event::PoisonTick] = ticks + SECONDS_TO_TICKS(1);
	//events[Event::PoisonExpire] = ticks + SECONDS_TO_TICKS(5) * scale;
}

void ModifierSystem::RemoveTrapped() noexcept
{
	this->RemoveEffect(Effect::Trapped);
}

void ModifierSystem::RemoveDrunk() noexcept
{
	this->RemoveEffect(Effect::Drunk);
	//events[Event::DrunkExpire] = ticks + SECONDS_TO_TICKS(5) * scale;
}


void ModifierSystem::InsertLevelDebuff(MessageSystem& message_system) noexcept
{
	const int index = GetRandomValue(0, this->DebuffList.size() - 1);

	const Effect random_effect = this->DebuffList[index];

	this->ApplyEffect(random_effect);
	//global_data.CachedStrings[CachedString::LevelDebuff] = std::string(this->DebuffNames[index]);
}

void ModifierSystem::RemoveLevelDebuff(MessageSystem& message_system) noexcept
{
	for (auto const effect : this->DebuffList)
		this->RemoveEffect(effect);

	//global_data.CachedStrings[CachedString::LevelDebuff] = "";
}

