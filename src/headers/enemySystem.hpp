#pragma once

#include <vector>
#include <array>
#include <cstddef>
#include <cstdint>

#include "raylib.h"
#include "constants.hpp"
#include "messageSystem.hpp"
#include "assetManager.hpp"

enum class EnemyType : uint8_t
{
	Australian,
	Drunkard,
	Pleb,
	Poison,
	Trapper,
	COUNT
};

enum class BehaviourModifier : uint8_t
{
	None = 0,
	Big = 1 << 0,
	IncreasedSpeed = 1 << 1
};

struct EnemyAttackComponent
{
	size_t LastLeAttack;
	float Damage;
	bool CanLeAttack;
};

struct EnemyDisplayComponent
{
	size_t LastFlash;
	TextureKey Texture;
	bool Flash;
};

struct EnemyAnimationComponent
{
	size_t LastAnimationUpdate;
	uint8_t ImageIndex;
	uint8_t AnimationInterval;
	uint8_t AnimationFrames;
};

struct EnemyData
{
	const TextureKey Texture;
	const float Damage;
	const float Speed;
	const float Health;

	const uint8_t AnimationInterval;
	/*
	animation_frames = number of actual frames -1

	case index = 0:
		0 * TEXTURE_TILE_SIZE = 0, first frame
	
	so everything is off by 1
	*/
	const uint8_t AnimationFrames;
};

inline BehaviourModifier operator| (BehaviourModifier l, BehaviourModifier r) noexcept
{
	return static_cast<BehaviourModifier>(static_cast<size_t>(l) | static_cast<size_t>(r));
}

inline BehaviourModifier operator& (BehaviourModifier l, BehaviourModifier r) noexcept
{
	return static_cast<BehaviourModifier>(static_cast<size_t>(l) & static_cast<size_t>(r));
}

class EnemySystem
{
	public:
		EnemySystem();
		~EnemySystem() = default;

		void Reset() noexcept;

		void PollSignals(MessageSystem& message_system, const AssetManager& assets, const size_t level) noexcept;

		void UpdateEnemies(
				const size_t ticks, const Rectangle update_area, const Vector2 player_centre,
				const float map_width, const float map_height, const bool is_stinky, MessageSystem& message_system) noexcept;

		void Draw(const AssetManager& assets) const noexcept;

	private:
		static constexpr std::array<EnemyData, static_cast<size_t>(EnemyType::COUNT)> EnemyAttributes = 
		{
			(EnemyData) { TextureKey::Australian, 5, 87.5, 75, 30, 2 },
			(EnemyData) { TextureKey::Drunkard, 5, 125, 50, 15,	4 },
			(EnemyData) { TextureKey::Pleb, 5, 75,	75,	30,	2 },
			(EnemyData) { TextureKey::Poison, 5, 62.5, 100, 30, 3 },
			(EnemyData) { TextureKey::Trapper, 10, 0, 100, 30, 3 }
		};

		void GenerateLocations(const size_t spawn_count, const float map_width, const float map_height) noexcept;
		void GenerateTypes(const size_t spawn_count) noexcept;
		void GenerateModifiers(const size_t spawn_count, const size_t level) noexcept;
		
		void NoModifiers(const size_t spawn_count, const size_t level) noexcept;
		void SameModifiers(const size_t spawn_count, const size_t level) noexcept;
		void RandomModifiers(const size_t spawn_count, const size_t level) noexcept;

		static constexpr std::array<void(EnemySystem::*)(const size_t, const size_t) noexcept, 3> GenerateModifierFunctions =
		{
			&NoModifiers,
			&SameModifiers,
			&RandomModifiers,
		};

		void EmitParticlesFromLocations(const size_t ticks, MessageSystem& message_system) noexcept;
		void PrepareSpawnEnemies(const size_t level, const float map_width, const float map_height) noexcept;
		void SpawnEnemies(const size_t level, const float map_width, const float map_height) noexcept;

		static constexpr std::array<void(EnemySystem::*)(const size_t, const float, const float), static_cast<size_t>(EnemySystemSignal::COUNT)> SignalHandlers =
		{
			&PrepareSpawnEnemies,
			&SpawnEnemies
		};

		static constexpr std::array<uint8_t, static_cast<size_t>(EnemyType::COUNT)> EnemyXpValues = 
		{
			1, 2, 1, 2, 2
		};

		void CreateEnemy(const float x, const float y, const float level_scale, const EnemyType type, const BehaviourModifier modifier) noexcept;
		void KillEnemies(MessageSystem& message_system) noexcept;

		void VisibilityCheck(const Rectangle update_area) noexcept;
		void MoveEnemies(const float map_width, const float map_height) noexcept;
		void EnemiesSetDirection(const Vector2 player_centre, const bool is_stinky) noexcept;
		void AnimateEnemies(const size_t ticks) noexcept;
		void EnemiesUpdateTimers(const size_t ticks) noexcept;
		void FlashSprite(const size_t ticks) noexcept;

		std::vector<float> EnemyHealth;
		std::vector<float> EnemyScale;
		std::vector<float> EnemySpeed;
		// std::vector<bool> is a special overload of dynamic bit fields,
		// it will not work in our case when we're trying to SIMD
		std::vector<uint8_t> EnemyIsVisible;
		std::vector<Vector2> EnemyDirection;
		std::vector<Rectangle> EnemyRect;
		std::vector<EnemyType> EnemyTypes;
		std::vector<BehaviourModifier> EnemyBehaviourModifiers;
		std::vector<EnemyAttackComponent> EnemyAttackComponents;
		std::vector<EnemyDisplayComponent> EnemyDisplayComponents;
		std::vector<EnemyAnimationComponent> EnemyAnimationComponents;


		std::vector<EnemyType> FutureEnemyTypes;
		std::vector<BehaviourModifier> FutureEnemyModifiers;
		std::vector<Vector2> FutureSpawnLocations;	
};
