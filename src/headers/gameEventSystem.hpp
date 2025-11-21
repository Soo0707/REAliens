#pragma once

#include <array>
#include <cstddef>

#include "game.hpp"

namespace GameEventSystem
{
	void HandleEvents(Game& game) noexcept;

	bool HandleEventExpiry(Event event, Effect effect, GlobalDataWrapper& global_data, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;

	void AuraTick(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void PoisonTick(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;

	void IncreasePlotArmour(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void PoisonExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void GreenbullExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;

	void MilkExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void DrunkExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void AussieExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;

	void MagnetismExpire(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;
	void IncreasePlayerSpeed(Game& game, size_t expiry, std::unordered_map<Event, size_t>& new_events_map) noexcept;


	// The ordering here matters as it mimics the order of the definition of Event
	static inline constexpr std::array<void(*)(Game&, size_t, std::unordered_map<Event, size_t>&), static_cast<size_t>(Event::COUNT)> EventHandlers =
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
		&AuraTick
	};
};
