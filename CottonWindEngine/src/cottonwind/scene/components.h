#pragma once

#include "../vendor/glm/glm.hpp"

#include "../graphics/texture.h"
#include "../audio/audio.h"

#include "../physics/2d/physics.h"

#include "../resource_manager/resource_manager.h"

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <sstream>


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
	
	// 2D COMPONENT
	struct TransformComponent
	{
		glm::vec2 center;
		glm::vec2 velocity;

		TransformComponent(const glm::vec2& s_center, const glm::vec2& s_velocity)
			: center(s_center), velocity(s_velocity)
		{}

		TransformComponent()
			: center(0.0f, 0.0f), velocity(0.0f, 0.0f)
		{}
	};

	// 2D COMPONENT
	struct CameraComponent
	{
		// width & height of 2D camera
		glm::vec2 bounds;
		// scale based on the game's window
		glm::vec2 scale;
		// primary means that the camera will be used to render stuff in sprite render system
		// (this allows having multiple cameras and switch between them at runtime)
		bool primary;

		CameraComponent(const glm::vec2& s_bounds, const glm::vec2& window_size, bool s_primary)
			: bounds(s_bounds), scale(window_size / s_bounds), primary(s_primary)
		{}

		CameraComponent(const glm::vec2& s_bounds, const glm::vec2& window_size)
			: bounds(s_bounds), scale(window_size / s_bounds), primary(false)
		{}

		CameraComponent()
			: bounds(1, 1), scale(1.0f, 1.0f), primary(false)
		{}
	};

	// 2D COMPONENT
	struct SpriteComponent
	{
		Texture texture;
		glm::ivec4 texture_rect; // x, y, area_width, area_height
		glm::ivec2 size; // width, height
		glm::ivec2 center_offset;	// an offset of position from transform->center

		bool active;

		enum RenderLayer
		{
			RenderLayerA, RenderLayerB, RenderLayerC, RenderLayerD, RenderLayerE
		};

		RenderLayer layer;

		SpriteComponent(Texture s_texture, const glm::ivec4& s_texture_rect, const glm::ivec2& s_size)
			: texture(s_texture), texture_rect(s_texture_rect), size(s_size), center_offset(0, 0), active(true), layer(RenderLayerA)
		{}
		
		SpriteComponent(Texture s_texture, const glm::ivec4& s_texture_rect, const glm::ivec2& s_size, const glm::ivec2& s_center_offset)
			: texture(s_texture), texture_rect(s_texture_rect), size(s_size), center_offset(s_center_offset), active(true), layer(RenderLayerA)
		{}

		SpriteComponent(Texture s_texture, const glm::ivec4& s_texture_rect, const glm::ivec2& s_size, RenderLayer s_layer)
			: texture(s_texture), texture_rect(s_texture_rect), size(s_size), center_offset(0, 0), active(true), layer(s_layer)
		{}

		SpriteComponent(Texture s_texture, const glm::ivec4& s_texture_rect, const glm::ivec2& s_size, const glm::ivec2& s_center_offset, RenderLayer s_layer)
			: texture(s_texture), texture_rect(s_texture_rect), size(s_size), center_offset(s_center_offset), active(true), layer(s_layer)
		{}
	};

	// 2D COMPONENT
	struct AnimationComponent
	{
		float frequency;
		float count;
		std::vector<glm::ivec4>* frames;
		std::size_t frame;

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

	// 2D COMPONENT
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

	// TODO : move to ResourceManager
	struct TilemapComponent
	{
		glm::ivec2 origin;
		glm::ivec2 tiles_count;
		int tile_size;

		std::vector<SpriteComponent> tiles;

		TilemapComponent(
			Texture& tileset,
			// TODO : store texture rect id not as char but as uint (tile world file format)
			const char* texture_table_str,
			const char* tiles_str,
			const glm::ivec2& s_origin,
			const glm::ivec2& s_tiles_count,
			int s_tile_size
		)
			: origin(s_origin), tiles_count(s_tiles_count), tile_size(s_tile_size)
		{
			std::unordered_map<char, glm::ivec4> texture_table;

			std::istringstream texture_table_stream(texture_table_str);
			// the format of a line in the textures table is "char texture_path"
			std::string texture_line;
			std::string rect_value;
			while (std::getline(texture_table_stream, texture_line, '\n'))
			{
				// TODO : check to see if i is > 5 (this indicated an invalid tilemap file / data)
				int i = 0;
				glm::ivec4 rect;
				std::istringstream rect_values(texture_line.c_str() + 2);
				while (std::getline(rect_values, rect_value, ','))
				{
					rect[i] = std::atoi(rect_value.c_str());
					i++;
				}
				texture_table[texture_line[0]] = rect;
			}

			glm::ivec2 size(tile_size, tile_size);

			tiles.reserve(tiles_count.x * tiles_count.y);
			
			for (int y = 0; y < tiles_count.y; y++)
				for (int x = 0; x < tiles_count.x; x++)
				{
					glm::ivec4& texture_rect = texture_table[tiles_str[x + y * tiles_count.x]];
					tiles.push_back(
						SpriteComponent(
							tileset,
							texture_rect,
							size
						)
						);

					// TODO	: along with static solid objects, support generic colliders (tile world file format)
					/*if (collision_map[x + y * tiles_count.x] == '1')
					{
						physics_colliders.push_back(
							PhysicsObjectComponent(StaticSolidBody, collider_size)
						);
					}*/
				}
		}
	};
}