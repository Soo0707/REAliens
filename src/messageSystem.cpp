/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
