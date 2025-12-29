#pragma once

#include <array>
#include <cstddef>
#include <unordered_map>

#include "gameState.hpp"
#include "assetManager.hpp"

namespace GameEventSystem
{
	void HandleEvents(GameState& game_state, const AssetManager& assets) noexcept;

	bool HandleEventExpiry(Event event, Effect effect, GameState& game_state, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;

	void AuraTick(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void PoisonTick(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void IncreasePlotArmour(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;

	void PoisonExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void GreenbullExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void MilkExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;

	void DrunkExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void AussieExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void MagnetismExpire(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;

	void IncreasePlayerSpeed(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void SpawnEnemies(GameState& game_state, const AssetManager& assets, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;

	// The ordering here matters as it mimics the order of the definition of Event
	static inline constexpr std::array<void(*)(GameState&, const AssetManager&, size_t, std::unordered_map<Event, size_t>&), static_cast<size_t>(Event::COUNT)> EventHandlers =
	{
		&GreenbullExpire,
		&MilkExpire,

		&PoisonTick,
		&PoisonExpire,
		&DrunkExpire,

		&AussieExpire,
		&MagnetismExpire,
		&IncreasePlayerSpeed,

		&IncreasePlotArmour,
		&AuraTick,
		&SpawnEnemies
	};
};
