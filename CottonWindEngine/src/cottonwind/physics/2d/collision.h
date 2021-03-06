#pragma once

#include "../../vendor/glm/glm.hpp"


namespace cotwin
{
	//static int collision_checks = 0;
	
	// test for collision of two rects, rect is left, top, width, height
	bool collide_aabb(const glm::vec4& a, const glm::vec4& b)
	{
		//collision_checks++;
		
		float a_right = a.x + a.z;
		float a_bottom = a.y + a.w;
		float b_right = b.x + b.z;
		float b_bottom = b.y + b.w;
		
		//return a.x <= b_right && a_right >= b.x && a.y <= b_bottom && a_bottom >= b.y;

		// SAT

		// if there's a horizontal gap
		if (a_right < b.x || b_right < a.x)
			return false;

		// if there's a vertical gap
		if (a_bottom < b.y || b_bottom < a.y)
			return false;

		return true;
	}
}