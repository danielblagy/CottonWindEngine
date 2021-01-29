#pragma once

#include "../../vendor/glm/glm.hpp"


namespace cotwin { namespace physics
{
	// test for collision of two rects, rect is left, top, width, height
	bool collide_aabb(const glm::vec4& a, const glm::vec4& b)
	{
		float a_right = a.x + a.z;
		float a_bottom = a.y + a.w;
		float b_right = b.x + b.z;
		float b_bottom = b.y + b.w;
		
		return a.x <= b_right && a_right >= b.x && a.y <= b_bottom && a_bottom >= b.y;
	}
} }