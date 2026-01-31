#include "gameHelpers.hpp"

#include "raylib.h"

void GameHelper::LoopOverMap(const float ground_width, const float ground_height, Rectangle& m_obj) noexcept
{
	if (m_obj.x < 0.0f)
		m_obj.x = ground_width - m_obj.width;
	else if (m_obj.x + m_obj.width > ground_width)
		m_obj.x = 0.0f;

	if (m_obj.y < 0.0f)
		m_obj.y = ground_height - m_obj.height;
	else if (m_obj.y + m_obj.height > ground_height)
		m_obj.y = 0.0f;
/*
 * static collision
	if (m_obj.x < 0.0f)
		m_obj.x = 0.0f;
	else if (m_obj.x + m_obj.width > ground_width)
		m_obj.x = ground_width - m_obj.width;

	if (m_obj.y < 0.0f)
		m_obj.y = 0.0f;
	else if (m_obj.y + m_obj.height > ground_height)
		m_obj.y = ground_height - m_obj.height;
		*/
}
