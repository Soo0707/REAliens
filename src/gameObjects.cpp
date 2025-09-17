#include "gameObjects.hpp"
#include "raylib.h"

BasicGameObject::BasicGameObject(float pos_x, float pos_y, Texture2D initial_image):
	rect({pos_x, pos_y, (float) initial_image.width, (float) initial_image.height}),image(initial_image) 
{}

UpdatableGameObject::UpdatableGameObject(float pos_x, float pos_y, Texture2D initial_image):
	BasicGameObject(pos_x, pos_y, initial_image)
{}
