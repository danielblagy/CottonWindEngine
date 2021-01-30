#pragma once

#include "../vendor/flecs/flecs.h"

#include "components.h"

#include "../util/logger.h"

#include "../graphics/render_camera_2d.h"

#include "../input/input.h"
#include "../input/keycodes.h"

#include "../graphics/renderer.h"

#include "../physics/2d/collision.h"

// TODO : for sprites drawn text, remove later
#include "../resource_manager/resource_manager.h"


namespace cotwin
{
	// just do that for now to conceal implementation details
	typedef flecs::entity Entity;

	void TransformSystem(flecs::iter& it, TransformComponent* transform)
	{
		for (auto i : it)
		{
			transform[i].center += transform[i].velocity;
		}
	}

	struct CameraSystemContext
	{
		bool window_size_updated = false;
		glm::ivec2 window_size;
	};
	
	void CameraSystem(flecs::iter& it, TransformComponent* transform, CameraComponent* camera)
	{
		//CameraSystemContext* context = static_cast<CameraSystemContext*>(it.param());
		//int* context = (int*)it.param();
		int* context = static_cast<int*>(it.param());
		Logger::Debug("%d", *context);
		
		/*for (auto i : it)
		{
			if (context->window_size_updated)
			{
				// update camera scale
				camera[i].scale.x = (float)context->window_size.x / (float)camera[i].bounds.x;
				camera[i].scale.y = (float)context->window_size.y / (float)camera[i].bounds.y;

				context->window_size_updated = false;
			}
		}*/
	}

	void CameraControllerSystem(flecs::iter& it, TransformComponent* transform, CameraComponent* camera)
	{
		for (auto i : it)
		{
			if (Input::is_key_pressed(CW_KEY_A))
				transform[i].velocity.x = -120.0f * it.delta_time();
			else if (Input::is_key_pressed(CW_KEY_D))
				transform[i].velocity.x = 120.0f * it.delta_time();
			else
				transform[i].velocity.x = 0.0f;

			if (Input::is_key_pressed(CW_KEY_W))
				transform[i].velocity.y = -120.0f * it.delta_time();
			else if (Input::is_key_pressed(CW_KEY_S))
				transform[i].velocity.y = 120.0f * it.delta_time();
			else
				transform[i].velocity.y = 0.0f;
		}
	}

	struct SpriteRenderSystemContext
	{
		TransformComponent camera_transform;
		CameraComponent camera;
		int a;
	};
	
	void SpriteRenderSystem(flecs::iter& it, TransformComponent* transform, SpriteComponent* sprite)
	{
		//SpriteRenderSystemContext* context = static_cast<SpriteRenderSystemContext*>(it.param());
		
		TransformComponent camera_transform;
		CameraComponent camera_info;
		
		it.world().query<TransformComponent, CameraComponent>().iter(
			[&](flecs::iter& it, TransformComponent* transform, CameraComponent* camera) {
			// this loop is expected to iterate only once
			for (auto i : it)
			{
				camera_transform = transform[i];
				camera_info = camera[i];
			}
		}
		);
		
		int sprites_drawn = 0;
		
		for (auto i : it)
		{
			//Renderer2D::render_texture(sprite[i].texture, sprite[i].texture_rect, transform[i].center, sprite[i].size);
			//sprites_drawn++;

			if (sprite[i].active)
			{
				glm::ivec2 camera_half_size = camera_info.bounds / 2;
				RenderCamera render_camera(
					camera_transform.center.x - camera_half_size.x,
					camera_transform.center.y - camera_half_size.y,
					camera_transform.center.x + camera_half_size.x,
					camera_transform.center.y + camera_half_size.y
				);

				// convert rect with left, top, right, bottom
				glm::ivec2 sprite_center = {
					transform[i].center.x + sprite[i].center_offset.x,
					transform[i].center.y + sprite[i].center_offset.y
				};
				glm::ivec4 sprite_rect = {
					sprite_center.x,
					sprite_center.y,
					sprite_center.x + sprite[i].size.x,
					sprite_center.y + sprite[i].size.y
				};

				if (render_camera.captures(sprite_rect))
				{
					glm::ivec2 sprite_relative_position = {
						camera_info.scale.x * (sprite_rect.x - render_camera.left),
						camera_info.scale.y * (sprite_rect.y - render_camera.top)
					};

					glm::ivec2 sprite_relative_size = {
						sprite[i].size.x * camera_info.scale.x,
						sprite[i].size.y * camera_info.scale.y
					};

					Renderer2D::render_texture(sprite[i].texture, sprite[i].texture_rect, sprite_relative_position, sprite_relative_size);
					sprites_drawn++;
				}
			}
		}

		Font& main_font = cotwin::ResourceManager::get_font("src/test/resources/fonts/Lato/Lato-Regular.ttf");
		Text sprites_drawn_text(std::to_string(sprites_drawn), main_font, { 200, 200, 200, 255 }, { 200, 0 });
		Renderer2D::render_text(sprites_drawn_text);
	}

	void AnimationSystem(flecs::iter& it, SpriteComponent* sprite, AnimationComponent* animation)
	{
		for (auto i : it)
		{
			if (sprite[i].active)
			{
				animation[i].count += it.delta_time();

				if (animation[i].count >= animation[i].frequency)
				{
					if (animation[i].frame >= animation[i].frames->size())
						animation[i].frame = 0;

					sprite[i].texture_rect = animation[i].frames->at(animation[i].frame);
					animation[i].frame++;

					animation[i].count -= animation[i].frequency;
				}
			}
		}
	}

	void AudioSystem(flecs::iter& it, AudioEffectComponent* audio_effect)
	{
		for (auto i : it)
		{
			if (audio_effect[i].play)
			{
				audio_effect[i].audio.play();
				audio_effect[i].play = false;
			}
		}
	}

	void MovementControlSystem(flecs::iter& it, TransformComponent* transform, MovementControlComponent* movement_control)
	{
		for (auto i : it)
		{
			movement_control[i].controller(transform[i].velocity, it.delta_time());
		}
	}

	void ScriptSystem(flecs::iter& it, ScriptComponent* script)
	{
		for (auto i : it)
		{
			script[i].script(it.entity(i), it.delta_time());
		}
	}

	void CollisionSystem(flecs::iter& it, TransformComponent* transform, ColliderComponent* collider)
	{
		static std::vector<std::pair<Entity, Entity>> collisions;
		
		collisions.clear();
		
		for (auto i : it)
		{
			glm::vec2 collider_origin = transform[i].center + collider[i].offset;
			glm::vec4 collider_rect(
				collider_origin.x,
				collider_origin.y,
				collider[i].size.x,
				collider[i].size.y
			);
			
			for (auto j : it)
			{
				if (i == j)
					continue;

				glm::vec2 other_collider_origin = transform[j].center + collider[j].offset;
				glm::vec4 other_collider_rect(
					other_collider_origin.x,
					other_collider_origin.y,
					collider[j].size.x,
					collider[j].size.y
				);

				if (physics::collide_aabb(collider_rect, other_collider_rect))
				{
					collisions.push_back(std::make_pair(it.entity(i), it.entity(j)));
				}
			}
		}
	}
}