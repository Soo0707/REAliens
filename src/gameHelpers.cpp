#include "gameHelpers.hpp"

#include "raylib.h"

void GameHelper::LoopOverMap(float ground_width, float ground_height, Rectangle& m_obj) noexcept
{
	if (m_obj.x < 0)
		m_obj.x = ground_width - m_obj.width;
	else if (m_obj.x + m_obj.width > ground_width)
		m_obj.x = 0;

	if (m_obj.y < 0)
		m_obj.y = ground_height - m_obj.height;
	else if (m_obj.y + m_obj.height > ground_height)
		m_obj.y = 0;
}
