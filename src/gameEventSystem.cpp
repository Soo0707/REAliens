#include "gameEventSystem.hpp"

#include <unordered_map>
#include <cmath>

#include "globalDataWrapper.hpp"
#include "constants.hpp"
#include "collisions.hpp"
#include "game.hpp"


void GameEventSystem::HandleEvents(Game& game) noexcept
{
	static std::unordered_map<Event, size_t> new_events_map;

	for (auto &pair : game.GlobalData->Events)
	{
		GameEventSystem::EventHandlers[static_cast<size_t>(pair.first)](game, pair.second, new_events_map);
	}

	game.GlobalData->Events.swap(new_events_map);

	new_events_map.clear();
}


bool GameEventSystem::HandleEventExpiry(Event event, Effect effect, GlobalDataWrapper& global_data, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	if (expiry >= global_data.Ticks)
	{
		new_events_map[event] = expiry;
		return false;
	}
	else
	{
		global_data.Effects.erase(effect);
		return true;
	}
}


void GameEventSystem::AuraTick(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	if (expiry >= game.GlobalData->Ticks)
		new_events_map[Event::AuraTick] = expiry;
	else
	{
		game.GlobalData->TotalDamage += Collisions::Aura(game);
		new_events_map[Event::AuraTick] = expiry + game.GlobalData->Attributes.at(Attribute::AuraCooldown);
	}
}

void GameEventSystem::PoisonTick(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	if (expiry >= game.GlobalData->Ticks)
		new_events_map[Event::PoisonTick] = expiry;
	else
	{
		game.PlayerInstance->Health -= 2;
		new_events_map[Event::PoisonTick] = expiry + SECONDS_TO_TICKS(1);
	}
}

void GameEventSystem::IncreasePlotArmour(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	game.PlayerInstance->HealthMax *= 1.5 * game.GlobalData->Events.at(Event::IncreasePlotArmour);
	game.PlayerInstance->Health = game.PlayerInstance->HealthMax;
}

void GameEventSystem::PoisonExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	if (GameEventSystem::HandleEventExpiry(Event::PoisonExpire, Effect::Poison, *game.GlobalData, expiry, new_events_map))
		game.GlobalData->Events.erase(Event::PoisonTick);
}

void GameEventSystem::GreenbullExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::GreenbullExpire, Effect::Greenbull, *game.GlobalData, expiry, new_events_map);
}

void GameEventSystem::MilkExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::MilkExpire, Effect::Milk, *game.GlobalData, expiry, new_events_map);
}

void GameEventSystem::DrunkExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::DrunkExpire, Effect::Drunk, *game.GlobalData, expiry, new_events_map);
}

void GameEventSystem::AussieExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::AussieExpire, Effect::Aussie, *game.GlobalData, expiry, new_events_map);
}

void GameEventSystem::MagnetismExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	GameEventSystem::HandleEventExpiry(Event::MagnetismExpire, Effect::Magnetism, *game.GlobalData, expiry, new_events_map);
}

void GameEventSystem::IncreasePlayerSpeed(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	game.PlayerInstance->Speed *= pow(1.2, game.GlobalData->Events.at(Event::IncreasePlayerSpeed));
}
