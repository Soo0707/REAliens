#pragma once

#include <unordered_map>
#include <cstddef>

#include "game.hpp"

namespace GameEventHandler
{
	void HandleEvents(Game& game);

	void UpgradeCircle(Game& game);
	void SpawnCircle(Game& game);
	bool HandleEventExpiry(Event event, Effect effect, GlobalDataWrapper& global_data, size_t expiry, std::unordered_map<Event, size_t>& new_events_map);

	void AuraTick(Game& game, size_t next_tick, std::unordered_map<Event, size_t>& new_events_map);
	void PoisonTick(Game& game, size_t next_tick, std::unordered_map<Event, size_t>& new_events_map);
	void IncreasePlotArmour(Game& game);

	void PoisonExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map);
}
