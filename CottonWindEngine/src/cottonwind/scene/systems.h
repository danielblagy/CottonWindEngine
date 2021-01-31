#pragma once

//#include "../vendor/entt/entt.hpp"

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
	void TransformSystem(TransformComponent& transform)
	{
		transform.center += transform.velocity;
	}

	/*void CameraControllerSystem(TransformComponent& transform, CameraComponent& camera)
	{
		if (Input::is_key_pressed(CW_KEY_A))
			transform.velocity.x = -120.0f * entity.delta_time();
		else if (Input::is_key_pressed(CW_KEY_D))
			transform.velocity.x = 120.0f * entity.delta_time();
		else
			transform.velocity.x = 0.0f;

		if (Input::is_key_pressed(CW_KEY_W))
			transform.velocity.y = -120.0f * entity.delta_time();
		else if (Input::is_key_pressed(CW_KEY_S))
			transform.velocity.y = 120.0f * entity.delta_time();
		else
			transform.velocity.y = 0.0f;
	}

	void SpriteRenderSystem(flecs::iter& it, TransformComponent* transform, SpriteComponent* sprite)
	{
		TransformComponent camera_transform;
		CameraComponent camera_info;
		
		it.world().query<TransformComponent, CameraComponent>().iter(
			[&](flecs::iter& it, TransformComponent* transform, CameraComponent* camera) {
			// TODO : support multiple cameras in a scene (primary flag), but here determine and use the primary one
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

	void AnimationSystem(Entity entity, SpriteComponent& sprite, AnimationComponent& animation)
	{
		if (sprite.active)
		{
			animation.count += entity.delta_time();

			if (animation.count >= animation.frequency)
			{
				if (animation.frame >= animation.frames->size())
					animation.frame = 0;

				sprite.texture_rect = animation.frames->at(animation.frame);
				animation.frame++;

				animation.count -= animation.frequency;
			}
		}
	}

	void AudioSystem(Entity entity, AudioEffectComponent& audio_effect)
	{
		if (audio_effect.play)
		{
			audio_effect.audio.play();
			audio_effect.play = false;
		}
	}

	void MovementControlSystem(Entity entity, TransformComponent& transform, MovementControlComponent& movement_control)
	{
		movement_control.controller(transform.velocity, entity.delta_time());
	}*/

	//void ScriptSystem(Entity entity, ScriptComponent& script)
	//{
	//	script.script(entity, entity.delta_time());
	//}
}