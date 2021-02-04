#pragma once

#include "../../vendor/glm/glm.hpp"

#include "collision.h"

// TODO : or use TransformComponent directly ??
//#include "../../scene/components.h"


namespace cotwin
{
	// reset entity's position to provide "solidity" effect, rect is left, top, width, height
	void apply_solidity(glm::vec2& entity_position, const glm::vec4& entity_rect, const glm::vec4& solid_rect)
	{
		
	}
}