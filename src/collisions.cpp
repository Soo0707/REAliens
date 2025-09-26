#include "collisions.hpp"
#include "gameObjects.hpp"
#include "projectiles.hpp"
#include "raylib.h"

void Collisions::ResolveCollisionsX(std::vector<std::shared_ptr<MovableGameObject>>movable_objects, std::vector<std::shared_ptr<BasicGameObject>>collidables)
{
	for (auto const& m_obj : movable_objects)
	{
		bool collided = false;
		for (auto const& static_obj : collidables)
		{
			if (CheckCollisionRecs(m_obj->NextRect, static_obj->Rect))
			{
				m_obj->Direction.x = 0;
				collided = true;
				break;
			}
		}
		if (!collided)
			m_obj->Rect.x = m_obj->NextRect.x;
	}
}

void Collisions::ResolveCollisionsY(std::vector<std::shared_ptr<MovableGameObject>>movable_objects, std::vector<std::shared_ptr<BasicGameObject>>collidables)
{
	for (auto const& m_obj : movable_objects)
	{
		bool collided = false;
		for (auto const& static_obj : collidables)
		{
			if (CheckCollisionRecs(m_obj->NextRect, static_obj->Rect))
			{
				m_obj->Direction.y = 0;
				collided = true;
				break;
			}
		}
		if (!collided)
			m_obj->Rect.y = m_obj->NextRect.y;
	}
}


void Collisions::ResolveCollisionPlayerX(Player& player, std::vector<Wall> walls, std::vector<BasicGameObject> props)
{
	bool collided = false;
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(player.NextRect, wall.Rect))
		{
			collided = true;
			break;
		}
	}
	
	if (!collided)
	{
		for (auto const& prop : props)
		{
			if (CheckCollisionRecs(player.NextRect, prop.Rect))
				collided = true;
		}
	}

	(collided) ? (player.Direction.x = 0) : (player.Rect.x = player.NextRect.x);
}


void Collisions::ResolveCollisionPlayerY(Player& player, std::vector<Wall> walls, std::vector<BasicGameObject> props)
{
	bool collided = false;
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(player.NextRect, wall.Rect))
		{
			collided = true;
			break;
		}
	}
	
	if (!collided)
	{
		for (auto const& prop : props)
		{
			if (CheckCollisionRecs(player.NextRect, prop.Rect))
				collided = true;
		}
	}

	(collided) ? (player.Direction.y = 0) : (player.Rect.y = player.NextRect.y);
}
