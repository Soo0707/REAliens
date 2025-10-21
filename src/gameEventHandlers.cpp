#include "gameEventHandlers.hpp"
#include "globalDataWrapper.hpp"
#include "constants.hpp"
#include "collisions.hpp"
#include "game.hpp"

void GameEventHandler::HandleEvents(Game& game)
{
	std::unordered_map<Event, size_t> new_events_map;

	for (auto &pair : game.GlobalData->Events)
	{
		switch (pair.first)
		{
			case Event::UpgradeCircle:
				GameEventHandler::UpgradeCircle(game);
				break;

			case Event::SpawnCircle:
				GameEventHandler::SpawnCircle(game);
				break;

			case Event::GreenbullExpire:
				GameEventHandler::HandleEventExpiry(Event::GreenbullExpire, Effect::Greenbull, *game.GlobalData, pair.second, new_events_map);
				break;

			case Event::MilkExpire:
				GameEventHandler::HandleEventExpiry(Event::MilkExpire, Effect::Milk, *game.GlobalData, pair.second, new_events_map);
				break;

			case Event::DrunkExpire:
				GameEventHandler::HandleEventExpiry(Event::DrunkExpire, Effect::Drunk, *game.GlobalData, pair.second, new_events_map);
				break;

			case Event::AussieExpire:
				GameEventHandler::HandleEventExpiry(Event::AussieExpire, Effect::Aussie, *game.GlobalData, pair.second, new_events_map);
				break;

			case Event::MagnetismExpire:
				GameEventHandler::HandleEventExpiry(Event::MagnetismExpire, Effect::Magnetism, *game.GlobalData, pair.second, new_events_map);
				break;
			
			case Event::PoisonExpire:
				GameEventHandler::PoisonExpire(game, pair.second, new_events_map);
				break;

			case Event::PoisonTick:
				GameEventHandler::PoisonTick(game, pair.second, new_events_map);
				break;

			case Event::AuraTick:
				GameEventHandler::AuraTick(game, pair.second, new_events_map);
				break;

			case Event::IncreasePlayerSpeed:
				game.PlayerInstance->Speed *= 1.2;
				break;

			case Event::IncreaseAura:
				game.PlayerInstance->Aura.width = game.GlobalData->Attributes.at(Attribute::AuraSize);
				game.PlayerInstance->Aura.height = game.GlobalData->Attributes.at(Attribute::AuraSize);
				break;

			case Event::IncreasePlotArmour:
				GameEventHandler::IncreasePlotArmour(game);
				break;
		}
	}
	game.GlobalData->Events.swap(new_events_map);
}

void GameEventHandler::UpgradeCircle(Game& game)
{
	for (auto &proj : game.Projectiles)
	{
		if (proj.Type == ProjectileType::Circle)
		{
			proj.Scale = game.GlobalData->Attributes.at(Attribute::CircleScale);
			proj.Speed = game.GlobalData->Attributes.at(Attribute::CircleAngularSpeed);
			proj.Rotation = game.GlobalData->Attributes.at(Attribute::CircleAngularSpeed) * TO_DEG;
			proj.Radius = game.GlobalData->Attributes.at(Attribute::CircleRadius);
		}
	}
}

void GameEventHandler::SpawnCircle(Game& game)
{
	game.Projectiles.emplace_back(
			game.PlayerInstance->Centre.x,
			game.PlayerInstance->Centre.y,
			(Vector2) { 0, 0 },
			ProjectileType::Circle,
			*game.GlobalData,
			*game.Assets
			);
}

bool GameEventHandler::HandleEventExpiry(Event event, Effect effect, GlobalDataWrapper& global_data, size_t expiry, std::unordered_map<Event, size_t>& new_events_map)
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

void GameEventHandler::AuraTick(Game& game, size_t next_tick, std::unordered_map<Event, size_t>& new_events_map)
{
	if (next_tick >= game.GlobalData->Ticks)
		new_events_map[Event::AuraTick] = next_tick;
	else
	{
		float damage;
		unsigned int enemies_hit = 0;

		damage = game.GlobalData->Attributes.at(Attribute::AuraDamage);

		for (auto &enemy : game.Enemies)
		{
			enemies_hit += Collisions::Aura(damage, game.GlobalData->Ticks, game.PlayerInstance->Aura, enemy);
		}

		if (game.GlobalData->Effects.count(Effect::LifeSteal))
			game.PlayerInstance->IncreaseHealth( enemies_hit * damage * game.GlobalData->Attributes.at(Attribute::LifeStealMultiplier) );

		new_events_map[Event::AuraTick] = game.GlobalData->Ticks + game.GlobalData->Attributes.at(Attribute::AuraCooldown);
	}
}

void GameEventHandler::PoisonTick(Game& game, size_t next_tick, std::unordered_map<Event, size_t>& new_events_map)
{
	if (next_tick >= game.GlobalData->Ticks)
		new_events_map[Event::PoisonTick] = next_tick;
	else
	{
		// damage may be -1 if poison expired, destruction of PoisonDamage and PoisonTick both happen here
		float damage = game.GlobalData->Attributes.at(Attribute::PoisonDamage);

		if (damage > 0)
		{
			game.PlayerInstance->Health -= damage;
			new_events_map[Event::PoisonTick] = next_tick + SECONDS_TO_TICKS(1);
		}
		else
			game.GlobalData->Attributes.erase(Attribute::PoisonDamage);
	}
}

void GameEventHandler::IncreasePlotArmour(Game& game)
{
	game.PlayerInstance->HealthMax *= 1.5;
	game.PlayerInstance->Health = game.PlayerInstance->HealthMax;
}

void GameEventHandler::PoisonExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map)
{
	/*
	 we cannot delete they key immediately as we might not have iterated through poison tick
	 which needs this. so we just don't copy PoisonExpire into the new map,
	 then signal using -1 so that PoisonTick can handle PoisonDamage's deletion
	*/
	if (GameEventHandler::HandleEventExpiry(Event::PoisonExpire, Effect::Poison, *game.GlobalData, expiry, new_events_map))
	{
		game.GlobalData->Attributes[Attribute::PoisonDamage] = -1.0f;
	}
}
