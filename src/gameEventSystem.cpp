#include "gameEventSystem.hpp"

#include <unordered_map>

#include "globalDataWrapper.hpp"
#include "constants.hpp"
#include "collisions.hpp"
#include "game.hpp"


void GameEventSystem::HandleEvents(Game& game) noexcept
{
	std::unordered_map<Event, size_t> new_events_map;

	for (auto &pair : game.GlobalData->Events)
	{
		GameEventSystem::EventHandlers[static_cast<size_t>(pair.first)](game, pair.second, new_events_map);
	}

	game.GlobalData->Events.swap(new_events_map);
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


void GameEventSystem::SpawnAndUpgradeBall(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	game.Projectiles.emplace_back(
			game.PlayerInstance->Centre.x,
			game.PlayerInstance->Centre.y,
			(Vector2) { 0, 0 },
			ProjectileType::Ball,
			*game.GlobalData,
			*game.Assets
			);
	
	for (auto &proj : game.Projectiles)
	{
		if (proj.Type == ProjectileType::Ball)
		{
			proj.Scale = game.GlobalData->Attributes.at(Attribute::BallScale);
			proj.Speed = game.GlobalData->Attributes.at(Attribute::BallAngularSpeed);
			proj.Rotation = game.GlobalData->Attributes.at(Attribute::BallAngularSpeed) * TO_DEG;
			proj.Radius = game.GlobalData->Attributes.at(Attribute::BallRadius);
		}
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
		// damage may be -1 if poison expired, destruction of PoisonDamage and PoisonTick both happen here
		float damage = game.GlobalData->Attributes.at(Attribute::PoisonDamage);

		if (damage > 0)
		{
			game.PlayerInstance->Health -= damage;
			new_events_map[Event::PoisonTick] = expiry + SECONDS_TO_TICKS(1);
		}
		else
			game.GlobalData->Attributes.erase(Attribute::PoisonDamage);
	}
}

void GameEventSystem::IncreasePlotArmour(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	game.PlayerInstance->HealthMax *= 1.5;
	game.PlayerInstance->Health = game.PlayerInstance->HealthMax;
}

void GameEventSystem::PoisonExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	/*
	 we cannot delete they key immediately as we might not have iterated through poison tick
	 which needs this. so we just don't copy PoisonExpire into the new map,
	 then signal using -1 so that PoisonTick can handle PoisonDamage's deletion
	*/
	if (GameEventSystem::HandleEventExpiry(Event::PoisonExpire, Effect::Poison, *game.GlobalData, expiry, new_events_map))
	{
		game.GlobalData->Attributes[Attribute::PoisonDamage] = -1.0f;
	}
}

void GameEventSystem::IncreaseAura(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept
{
	game.PlayerInstance->Aura.width = game.GlobalData->Attributes.at(Attribute::AuraSize);
	game.PlayerInstance->Aura.height = game.GlobalData->Attributes.at(Attribute::AuraSize);
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
	game.PlayerInstance->Speed *= 1.2;
}
