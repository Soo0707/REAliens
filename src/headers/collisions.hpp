#pragma once

#include <vector>
#include <memory>
#include "gameObjects.hpp"
#include "player.hpp"

namespace Collisions
{
	void ResolveCollisionsX(std::vector<std::shared_ptr<MovableGameObject>>movable_objects, std::vector<std::shared_ptr<BasicGameObject>>collidables);
	void ResolveCollisionsY(std::vector<std::shared_ptr<MovableGameObject>>movable_objects, std::vector<std::shared_ptr<BasicGameObject>>collidables);


	void ResolveCollisionPlayerX(Player& player, std::vector<Wall> walls, std::vector<BasicGameObject> props);
	void ResolveCollisionPlayerY(Player& player, std::vector<Wall> walls, std::vector<BasicGameObject> props);
}
