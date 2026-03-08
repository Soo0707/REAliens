#include "messageSystem.hpp"

MessageSystem::MessageSystem()
{
	this->ParticleSystemCommands.reserve(32);
	this->ProjectileSystemCommands.reserve(32);
	this->XpSystemCommands.reserve(32);
	this->PlayerCommands.reserve(32);
	this->EnemySystemCommands.reserve(32);
	this->TimerSystemCommands.reserve(16);
	this->StatSystemCommands.reserve(16);
	this->StateManagerCommands.reserve(8);
}

void MessageSystem::Reset() noexcept
{
	this->ParticleSystemCommands.clear();
	this->ProjectileSystemCommands.clear();
	this->XpSystemCommands.clear();
	this->PlayerCommands.clear();
	this->EnemySystemCommands.clear();
	this->TimerSystemCommands.clear();
	this->StatSystemCommands.clear();
	this->StateManagerCommands.clear();

	this->EnemySystemSignals = { 0 };
	this->PlayerSignals = { 0 };
	this->ModifierSystemSignals = { 0 };
	this->CollisionSystemSignals = { 0 };
	this->GameSignals = { 0 };
}
