#pragma once

#include "../vendor/glm/glm.hpp"

#include "../graphics/texture.h"

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
		
		TransformComponent(const glm::vec2& s_center, const glm::vec2& s_velocity)
			: center(s_center), velocity(s_velocity)
		{}
	};

	struct SpriteComponent
	{
		// TODO : maybe make this a pointer
		Texture texture;
		glm::ivec4 texture_rect; // x, y, area_width, area_height;
		glm::ivec4 rect; // x, y, width, height;

		bool active;

		// TODO : add center offset for transform

		SpriteComponent(Texture s_texture, const glm::ivec4& s_texture_rect, const glm::ivec4& s_rect)
			: texture(s_texture), texture_rect(s_texture_rect), rect(s_rect), active(true)
		{}
	};
}