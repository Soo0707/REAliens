#include "messageSystem.hpp"

MessageSystem::MessageSystem()
{
	this->ParticleSystemCommands.reserve(128);
	this->GameTextSystemCommands.reserve(128);
	this->ProjectileSystemCommands.reserve(128);
	this->XpSystemCommands.reserve(128);

	this->ModifierSystemCommands.reserve(128);
	this->TimerSystemCommands.reserve(16);
	this->StatSystemCommands.reserve(16);
}

void MessageSystem::Reset() noexcept
{
	this->ParticleSystemCommands.clear();
	this->GameTextSystemCommands.clear();
	this->ProjectileSystemCommands.clear();
	this->XpSystemCommands.clear();

	this->ModifierSystemCommands.clear();
	this->TimerSystemCommands.clear();
	this->StatSystemCommands.clear();

	this->EnemySystemSignals = { 0 };
	this->PlayerSignals = { 0 };
}
