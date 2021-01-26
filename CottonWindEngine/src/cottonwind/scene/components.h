#pragma once

#include "../vendor/glm/glm.hpp"

#include "../graphics/sprite.h"

#include <string>


namespace cotwin
{
	struct TagComponent
	{
		std::string tag;

		TagComponent(std::string s_tag)
			: tag(s_tag)
		{}
	};
	
	// TODO : these components for now are only for 2D
	
	struct TransformComponent
	{
		glm::vec2 center;
		glm::vec2 velocity;

		TransformComponent()
		{}
		
		TransformComponent(glm::vec2 s_center, glm::vec2 s_velocity)
			: center(s_center), velocity(s_velocity)
		{}
	};

	struct SpriteComponent
	{
		// TODO : move sprite data from graphics/sprite.h to this component
		Sprite sprite;
		bool active;

		// TODO : add center offset for transform

		SpriteComponent(Sprite s_sprite)
			: sprite(s_sprite), active(true)
		{}
	};
}