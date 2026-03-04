#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

#include "raylib.h" 
#include "messageSystem.hpp"
#include "assetManager.hpp"

class ParticleSystem
{
	public:
		ParticleSystem();
		~ParticleSystem() = default;
		
		void Update(MessageSystem& message_system, const AssetManager& assets, const Rectangle& update_area, const size_t ticks) noexcept;

		void Reset() noexcept;

		void ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept;
		
		void Draw(const AssetManager& assets, const size_t ticks) const noexcept;

	private:
		void VisibilityCheck(const Rectangle& update_area) noexcept;

		void CreateParticle(
				const float x, const float y, const float scale, const float rotation, const size_t creation,
				const size_t expiry, const Vector2 velocity, const Color start_colour, const Color end_colour
				) noexcept;

		void RemoveParticles(const size_t ticks) noexcept;
		void MoveAndScaleParticles(const size_t ticks) noexcept;

		std::vector<uint8_t> ParticleIsVisible;

		std::vector<size_t> ParticleCreation;
		std::vector<size_t> ParticleExpiry;

		std::vector<float> ParticleRotation;
		std::vector<float> ParticleScale;

		std::vector<Vector2> ParticleVelocity;

		std::vector<Color> ParticleStartColour;
		std::vector<Color> ParticleEndColour;

		std::vector<Rectangle> ParticleRect;
};

