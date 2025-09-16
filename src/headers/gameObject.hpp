#pragma once

class GameObject
{
	public:
		virtual GameObject();
		virtual ~GameObject() = default;

		virtual void update() = 0;
		virtual void draw() = 0;
};
