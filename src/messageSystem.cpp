#include "messageSystem.hpp"

MessageSystem::MessageSystem()
{
	this->ParticleSystemCommandsWrite.reserve(128);
	this->ParticleSystemCommandsRead.reserve(128);

	this->ProjectileSystemCommandsWrite.reserve(128);
	this->ProjectileSystemCommandsRead.reserve(128);

	this->XpSystemCommandsWrite.reserve(128);
	this->XpSystemCommandsRead.reserve(128);

	this->PlayerCommandsWrite.reserve(128);
	this->PlayerCommandsRead.reserve(128);

	this->EnemySystemCommandsWrite.reserve(128);
	this->EnemySystemCommandsRead.reserve(128);

	this->TimerSystemCommandsWrite.reserve(16);
	this->TimerSystemCommandsRead.reserve(16);

	this->StatSystemCommandsWrite.reserve(16);
	this->StatSystemCommandsRead.reserve(16);
}

void MessageSystem::Reset() noexcept
{
	this->ParticleSystemCommandsWrite.clear();
	this->ParticleSystemCommandsRead.clear();

	this->ProjectileSystemCommandsWrite.clear();
	this->ProjectileSystemCommandsRead.clear();

	this->XpSystemCommandsWrite.clear();
	this->XpSystemCommandsRead.clear();

	this->PlayerCommandsWrite.clear();
	this->PlayerCommandsRead.clear();

	this->EnemySystemCommandsWrite.clear();
	this->EnemySystemCommandsRead.clear();

	this->TimerSystemCommandsWrite.clear();
	this->TimerSystemCommandsRead.clear();

	this->StatSystemCommandsWrite.clear();
	this->StatSystemCommandsRead.clear();

	for (auto& value : this->EnemySystemSignals)
		value.store(0);

	for (auto& value : this->PlayerSignals)
		value.store(0);

	for (auto& value : this->ModifierSystemSignals)
		value.store(0);

	for (auto& value : this->CollisionSystemSignals)
		value.store(0);
}
