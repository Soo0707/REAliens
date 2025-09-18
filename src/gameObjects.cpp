#include "gameObjects.hpp"
#include "raylib.h"

BasicGameObject::BasicGameObject(float pos_x, float pos_y, Texture2D initial_image):
	Rect({pos_x, pos_y, (float) initial_image.width, (float) initial_image.height}), Image(initial_image)
{}

UpdatableGameObject::UpdatableGameObject(float pos_x, float pos_y, Texture2D initial_image):
	BasicGameObject(pos_x, pos_y, initial_image)
{}
