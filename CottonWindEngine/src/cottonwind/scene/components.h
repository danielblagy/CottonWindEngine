#pragma once

#include "../vendor/glm/glm.hpp"

#include "../graphics/texture.h"

#include <string>
#include <vector>


namespace cotwin
{
	struct TagComponent
	{
		std::string tag;

		TagComponent(std::string s_tag)
			: tag(s_tag)
		{}
	};
	
	// TODO : label these components as 2D ?? (e.g. Transform2DComponent)
	
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

	struct AnimationComponent
	{
		float frequency;
		float count;
		std::vector<glm::ivec4> frames;
		std::size_t i = 0;

		// NOTE: count in initialized to frequency to force resfresh() in AnimationSystem on init, so that
		//	sprite component is initialized to a proper texture_rect
		
		AnimationComponent(float s_frequency)
			: frequency(s_frequency), count(s_frequency)
		{}

		AnimationComponent(float s_frequency, const std::vector<glm::ivec4>& s_frames)
			: frequency(s_frequency), count(s_frequency), frames(s_frames)
		{}
	};
}