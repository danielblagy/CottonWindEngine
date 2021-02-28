#pragma once

#include "../../vendor/glm/glm.hpp"

#include <vector>

#include "../../util/logger.h"


namespace cotwin
{
	enum PhysicsObjectType
	{
		StaticBody,			// a non-solid body, unaffected by forces (basically doesn't interact with the physics environment)
		StaticSolidBody,	// a solid body unaffected by forces
		DynamicBody,		// a non-solid body affected by forces
		DynamicSolidBody	// a solid body affected by forces
	};
	
	// TODO : add accounting for mass & restitution
	struct PhysicsObjectComponent
	{
		PhysicsObjectType type;
		
		glm::vec2 size;
		glm::vec2 offset;	// an offset from transform->position

		PhysicsObjectComponent(PhysicsObjectType s_type, const glm::vec2& s_size)
			: type(s_type), size(s_size), offset(0.0f, 0.0f)
		{}

		PhysicsObjectComponent(PhysicsObjectType s_type, const glm::vec2& s_size, const glm::vec2& s_offset)
			: type(s_type), size(s_size), offset(s_offset)
		{}

		PhysicsObjectComponent(const PhysicsObjectComponent& physics_object)
			: type(physics_object.type), size(physics_object.size), offset(physics_object.offset)
		{}
	};
}