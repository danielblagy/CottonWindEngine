#pragma once

#include "../vendor/glm/glm.hpp"

#include "../graphics/texture.h"
#include "../audio/audio.h"

#include <string>
#include <vector>
#include <functional>

//#include "../vendor/entt/entt.hpp"


namespace cotwin
{
	struct TagComponent
	{
		std::string tag;

		TagComponent(std::string s_tag)
			: tag(s_tag)
		{}
	};

	struct AudioEffectComponent
	{
		Audio audio;
		bool play;

		AudioEffectComponent(Audio& s_audio)
			: audio(s_audio), play(false)
		{}
	};
	
	// TODO : label these components as 2D ?? (e.g. Transform2DComponent)
	
	struct TransformComponent
	{
		glm::vec2 center;
		glm::vec2 velocity;

		TransformComponent(const glm::vec2& s_center, const glm::vec2& s_velocity)
			: center(s_center), velocity(s_velocity)
		{}
	};

	struct CameraComponent
	{
		// width & height of 2D camera
		glm::ivec2 bounds;
		// scale based on the game's window
		glm::vec2 scale;

		CameraComponent(const glm::ivec2& s_bounds, const glm::ivec2& window_size)
			: bounds(s_bounds), scale(window_size / s_bounds)
		{}
	};

	struct SpriteComponent
	{
		Texture texture;
		glm::ivec4 texture_rect; // x, y, area_width, area_height
		glm::ivec2 size; // width, height
		glm::ivec2 center_offset;	// an offset of position from transform->center

		bool active;

		SpriteComponent(Texture s_texture, const glm::ivec4& s_texture_rect, const glm::ivec2& s_size)
			: texture(s_texture), texture_rect(s_texture_rect), size(s_size), center_offset(0, 0), active(true)
		{}
		
		SpriteComponent(Texture s_texture, const glm::ivec4& s_texture_rect, const glm::ivec2& s_size, const glm::ivec2& s_center_offset)
			: texture(s_texture), texture_rect(s_texture_rect), size(s_size), center_offset(s_center_offset), active(true)
		{}
	};

	struct AnimationComponent
	{
		float frequency;
		float count;
		std::vector<glm::ivec4>* frames;
		std::size_t frame;

		// use that, since the get
		//std::vector<glm::ivec4>* new_frames;

		// NOTE: count in initialized to frequency to force resfresh() in AnimationSystem on init, so that
		//	sprite component is initialized to a proper texture_rect
		
		AnimationComponent(float s_frequency)
			: frequency(s_frequency), count(s_frequency), frames(0), frame(0)
		{}

		AnimationComponent(float s_frequency, std::vector<glm::ivec4>* s_frames)
			: frequency(s_frequency), count(s_frequency), frames(s_frames), frame(0)
		{}
		
		void set_animation(std::vector<glm::ivec4>* s_frames)
		{
			if (frames != s_frames)
			{
				frames = s_frames;
				frame = 0;
				count = frequency;

				Logger::Debug("Animation updated");
			}
		}
	};

	struct MovementControlComponent
	{
		std::function<void(glm::vec2&, float)> controller;

		MovementControlComponent(std::function<void(glm::vec2&, float)> s_controller)
			: controller(s_controller)
		{}
	};
	
	/*class Entity;
	
	struct ScriptComponent
	{
		std::function<void(Scene::Entity, float)> script;

		ScriptComponent()
		{}
		
		ScriptComponent(std::function<void(Scene::Entity, float)> s_script)
			: script(s_script)
		{}
	};*/

	struct ColliderComponent
	{
		glm::vec2 size;
		glm::vec2 offset;	// an offset from transform->position

		ColliderComponent(const glm::vec2& s_size)
			: size(s_size), offset(0.0f, 0.0f)
		{}

		ColliderComponent(const glm::vec2& s_size, const glm::vec2& s_offset)
			: size(s_size), offset(s_offset)
		{}
	};
}