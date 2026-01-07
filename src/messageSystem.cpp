#include "messageSystem.hpp"

MessageSystem::MessageSystem()
{
	this->ParticleSystemCommands.reserve(256);
	this->GameTextSystemCommands.reserve(256);
	this->ProjectileSystemCommands.reserve(256);
	this->XpSystemCommands.reserve(256);

	this->ModifierSystemCommands.reserve(256);
	this->TimerSystemCommands.reserve(256);
	this->StatSystemCommands.reserve(256);
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
