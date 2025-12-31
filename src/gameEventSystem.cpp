#include "gameEventSystem.hpp"

#include <unordered_map>
#include <cmath>

#include "gameState.hpp"
#include "gameHelpers.hpp"
#include "assetManager.hpp"
#include "constants.hpp"
#include "collisions.hpp"


void GameEventSystem::HandleEvents(GameState& game_state, const AssetManager& assets) noexcept
{
	static std::unordered_map<Event, size_t> new_events_map;

	for (auto &pair : game_state.Events)
	{
		GameEventSystem::EventHandlers[static_cast<size_t>(pair.first)](game_state, assets, pair.second, new_events_map);
	}

	game_state.Events.swap(new_events_map);

	new_events_map.clear();
}


bool GameEventSystem::HandleEventExpiry(Event event, Effect effect, GameState& game_state, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	if (expiry >= game_state.Ticks)
	{
		new_events_map[event] = expiry;
		return false;
	}
	else
	{
		game_state.Effects.erase(effect);
		return true;
	}
}


void GameEventSystem::AuraTick(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	const size_t ticks = game_state.Ticks;

	if (expiry >= ticks)
		new_events_map[Event::AuraTick] = expiry;
	else
	{
		unsigned int total_hit = 0;

		const float aura_damage = game_state.Attributes.at(Attribute::AuraDamage);
		const float aura_size = game_state.Attributes.at(Attribute::AuraSize);

		const Rectangle aura = {
			game_state.Player->Centre.x - aura_size / 2.0f,
			game_state.Player->Centre.y - aura_size / 2.0f,
			aura_size,
			aura_size
		};

		for (auto &enemy : game_state.Enemies)
		{
			const bool hit = Collisions::Aura(enemy, aura, aura_damage, ticks);

			if (hit)
			{
				game_state.GameTexts.emplace_back(
						enemy.Rect.x, enemy.Rect.y, 64.0f, std::to_string(static_cast<unsigned int>(aura_damage)), 
						42, MAGENTA, ticks,	ticks + TICK_RATE / 4
						);

				for (int i = 0; i < 15; i++)
				{
					float size = static_cast<float>(GetRandomValue(10, 20));
					float rotation = static_cast<float>(GetRandomValue(0, 90));
					size_t expiry = ticks + static_cast<size_t>(GetRandomValue(60, TICK_RATE));
					Vector2 velocity = { static_cast<float>(GetRandomValue(-192, 192)), static_cast<float>(GetRandomValue(-192, 192)) } ;

					game_state.Particles.emplace_back(
							enemy.Rect.x, enemy.Rect.y, size, rotation, ticks,
							expiry, velocity, PURPLE, RED, assets
							);
				}

				total_hit++;
			}
		}

		game_state.Stats[static_cast<size_t>(Stat::TotalDamage)] += static_cast<unsigned int>(total_hit * aura_damage);

		if (game_state.Effects.count(Effect::LifeSteal))
			game_state.Player->IncreaseHealth(total_hit * aura_damage * game_state.Attributes.at(Attribute::LifeStealMultiplier));

		new_events_map[Event::AuraTick] = expiry + game_state.Attributes.at(Attribute::AuraCooldown);
	}
}

void GameEventSystem::PoisonTick(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	if (expiry >= game_state.Ticks)
		new_events_map[Event::PoisonTick] = expiry;
	else
	{
		game_state.Player->Health -= 2;
		new_events_map[Event::PoisonTick] = expiry + SECONDS_TO_TICKS(1);
	}
}

void GameEventSystem::IncreasePlotArmour(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	game_state.Player->HealthMax *= pow(2.0, game_state.Events.at(Event::IncreasePlotArmour));
	game_state.Player->Health = game_state.Player->HealthMax;
}

void GameEventSystem::PoisonExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	if (GameEventSystem::HandleEventExpiry(Event::PoisonExpire, Effect::Poison, game_state, expiry, new_events_map))
		game_state.Events.erase(Event::PoisonTick);
}

void GameEventSystem::GreenbullExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::GreenbullExpire, Effect::Greenbull, game_state, expiry, new_events_map);
}

void GameEventSystem::MilkExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::MilkExpire, Effect::Milk, game_state, expiry, new_events_map);
}

void GameEventSystem::DrunkExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::DrunkExpire, Effect::Drunk, game_state, expiry, new_events_map);
}

void GameEventSystem::AussieExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::AussieExpire, Effect::Aussie, game_state, expiry, new_events_map);
}

void GameEventSystem::MagnetismExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::MagnetismExpire, Effect::Magnetism, game_state, expiry, new_events_map);
}

void GameEventSystem::IncreasePlayerSpeed(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	game_state.Player->Speed *= pow(1.2, game_state.Events.at(Event::IncreasePlayerSpeed));
}

void GameEventSystem::SpawnEnemies(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	const size_t ticks = game_state.Ticks;

	if (expiry >= ticks)
		new_events_map[Event::SpawnEnemies] = expiry;
	else
	{
		const size_t level = game_state.Level;
		const size_t spawn_count = level * 15;

		game_state.Enemies.reserve(game_state.Enemies.size() + spawn_count);

		std::vector<EnemyType> types;
		types.reserve(spawn_count);

		GameHelper::SpawnTypeFunctions[GetRandomValue(0, 1)](spawn_count, types);

		std::vector<BehaviourModifier> modifiers;
		modifiers.reserve(spawn_count);

		GameHelper::GenerateModifierFunctions[GetRandomValue(0, 2)](spawn_count, level, modifiers);

		const float level_scale = 1 + static_cast<float>(level) / 10.0f;

		for (size_t i = 0; i < spawn_count; i++)
			game_state.Enemies.emplace_back(game_state.SpawnLocations[i].x, game_state.SpawnLocations[i].y, level_scale, assets, types[i], modifiers[i]);

		game_state.SpawnLocations.clear();
	}
}
