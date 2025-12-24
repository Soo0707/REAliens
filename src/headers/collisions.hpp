#pragma once

#include <unordered_map>
#include <array>
#include <cstddef>

#include "player.hpp"
#include "enemy.hpp"
#include "projectiles.hpp"
#include "gameState.hpp"
#include "raylib.h"

namespace Collisions
{
	unsigned int ProjectileCollision(Projectile& proj, std::vector<Enemy>& enemies, size_t ticks, const std::unordered_map<Attribute, float>& attributes) noexcept;
	void LeAttack(Player& player, Enemy& enemy, size_t ticks, std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events) noexcept;
	long long SlideAttack(Player& player, Enemy& enemy) noexcept;

	bool Aura(Enemy& enemy, Rectangle aura, float aura_damage, size_t ticks) noexcept;

	void ApplyAustralian(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events, size_t ticks, unsigned int scale) noexcept;
	void ApplyPoison(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events, size_t ticks, unsigned int scale) noexcept;
	void ApplyTrapped(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events, size_t ticks, unsigned int scale) noexcept;

	void ApplyDrunk(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events, size_t ticks, unsigned int scale) noexcept;
	void ApplyNone(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events, size_t ticks, unsigned int scale) noexcept;

	using EffectPrototype = void(*)(std::unordered_set<Effect>&, std::unordered_map<Event, size_t>&, size_t, unsigned int) noexcept;
	static inline constexpr std::array<EffectPrototype, static_cast<size_t>(EnemyType::COUNT)> LeAttackHooks =
	{
		&ApplyAustralian,
		&ApplyDrunk,
		&ApplyNone,
		&ApplyPoison,
		&ApplyTrapped
	};
}

