#pragma once

#include <vector>
#include <cstddef>

#include "raylib.h" 
#include "messageSystem.hpp"
#include "assetManager.hpp"
#include "particle.hpp"

class ParticleSystem
{
	public:
		ParticleSystem();
		~ParticleSystem() = default;

		void Reset() noexcept;

		void ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept;
		void UpdateParticles(const Rectangle update_area, const size_t ticks) noexcept;
		
		void Draw(const Rectangle update_area, const size_t ticks) const noexcept;

	private:
		std::vector<Particle> Particles;
};

