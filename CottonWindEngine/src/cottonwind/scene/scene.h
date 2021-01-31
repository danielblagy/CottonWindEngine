#pragma once

#include "../vendor/entt/entt.hpp"

#include "components.h"

#include "../util/logger.h"

// NOTE : at least for now systems are in scene.update

#include "../input/input.h"
#include "../input/keycodes.h"

#include "../graphics/renderer.h"
#include "../graphics/render_camera_2d.h"

// TODO : for sprites drawn text, remove later
#include "../resource_manager/resource_manager.h"

#include <vector>
#include "../physics/2d/collision.h"


namespace cotwin
{
	class Scene
	{
	public:
		class Entity
		{
		private:
			entt::entity entity_handle = entt::entity(0);
			Scene* scene = NULL;

		public:
			Entity()
			{}

			Entity(entt::entity s_entity_handle, Scene* s_scene)
				: entity_handle(s_entity_handle), scene(s_scene)
			{}

			template<typename T, typename... Args>
			T& add_component(Args&&... args)
			{
				if (has_component<T>())
					Logger::Debug("CottonWind \tEntity already has this component!");

				return scene->registry.emplace<T>(entity_handle, std::forward<Args>(args)...);
			}

			template<typename T>
			T& get_component()
			{
				if (!has_component<T>())
					Logger::Debug("CottonWind \tEntity doesn't have this component!");

				return scene->registry.get<T>(entity_handle);
			}

			template<typename T>
			void remove_component()
			{
				if (!has_component<T>())
					Logger::Debug("CottonWind \tEntity doesn't have this component!");

				return scene->registry.remove<T>(entity_handle);
			}

			template<typename T>
			bool has_component()
			{
				return scene->registry.has<T>(entity_handle);
			}
		};
	
	private:
		entt::registry registry;
		
		// used for collision querying from CollisionSystem
		std::vector<std::pair<Entity, Entity>> collisions;

		// collision system (in here, since system context doesn't work, it.param() gives garbage values)
		/*void CollisionSystem(flecs::iter& it, TransformComponent* transform, ColliderComponent* collider)
		{
			for (auto i : it)
			{
				glm::vec2 collider_origin = transform[i].center + collider[i].offset;
				glm::vec4 collider_rect(
					collider_origin.x,
					collider_origin.y,
					collider[i].size.x,
					collider[i].size.y
				);

				// TODO : start from i + 1 index (so don't iterate over the same collisions twice)
				size_t end = it.count();
				for (size_t j = i + 1; j < it.count(); j++)
				{
					//if (i == j)
					//	continue;

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
		}*/

	public:
		Scene()
		{
			//world.system<TransformComponent>().each(TransformSystem);
			//world.system<TransformComponent, MovementControlComponent>().each(MovementControlSystem);
			//world.system<SpriteComponent, AnimationComponent>().each(AnimationSystem);
			//world.system<ScriptComponent>().each(ScriptSystem);
			//world.system<TransformComponent, CameraComponent>().each(CameraControllerSystem);
			//world.system<AudioEffectComponent>().each(AudioSystem);
			//
			//world.system<TransformComponent, SpriteComponent>().iter(SpriteRenderSystem);
			//
			//world.system<TransformComponent, ColliderComponent>().iter(&Scene::CollisionSystem);
		}

		~Scene()
		{}

		Entity create_entity(std::string tag)
		{
			Entity entity(registry.create(), this);
			entity.add_component<TagComponent>(tag);
			return entity;
		}
		
		void update(float delta)
		{
			// TODO : Script System

			// TODO : maybe move Script & MovementControl to the top here
			
			// Transform System
			for (auto [entity, transform] : registry.view<TransformComponent>().each())
			{
				transform.center += transform.velocity;
			}

			// Movement Control System
			for (auto [entity, transform, movement_control] : registry.view<TransformComponent, MovementControlComponent>().each())
			{
				movement_control.controller(transform.velocity, delta);
			}

			// Animation System
			for (auto [entity, sprite, animation] : registry.view<SpriteComponent, AnimationComponent>().each())
			{
				if (sprite.active)
				{
					animation.count += delta;

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

			// Camera Controller System
			for (auto [entity, transform, camera] : registry.view<TransformComponent, CameraComponent>().each())
			{
				if (Input::is_key_pressed(CW_KEY_A))
					transform.velocity.x = -120.0f * delta;
				else if (Input::is_key_pressed(CW_KEY_D))
					transform.velocity.x = 120.0f * delta;
				else
					transform.velocity.x = 0.0f;

				if (Input::is_key_pressed(CW_KEY_W))
					transform.velocity.y = -120.0f * delta;
				else if (Input::is_key_pressed(CW_KEY_S))
					transform.velocity.y = 120.0f * delta;
				else
					transform.velocity.y = 0.0f;
			}

			// Audio System
			for (auto [entity, audio_effect] : registry.view<AudioEffectComponent>().each())
			{
				if (audio_effect.play)
				{
					audio_effect.audio.play();
					audio_effect.play = false;
				}
			}
			
			int sprites_drawn = 0;
			
			// Sprite Render System
			auto view = registry.view<TransformComponent, SpriteComponent>();
			for (auto [entity, transform, sprite] : view.each()) {
				//Renderer2D::render_texture(sprite.texture, sprite.texture_rect, transform.center, sprite.size);
				
				TransformComponent camera_transform(glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
				CameraComponent camera_info(glm::ivec2{1, 1}, glm::vec2{1.0f, 1.0f});

				// TODO : support multiple cameras in a scene (primary flag), but here determine and use the primary one
				// this loop is expected to iterate only once (until multiple cameras will be supported)
				for (auto [entity, c_transform, c_camera] : registry.view<TransformComponent, CameraComponent>().each())
				{
					camera_transform = c_transform;
					camera_info = c_camera;
				}
				
				if (sprite.active)
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
						transform.center.x + sprite.center_offset.x,
						transform.center.y + sprite.center_offset.y
					};
					glm::ivec4 sprite_rect = {
						sprite_center.x,
						sprite_center.y,
						sprite_center.x + sprite.size.x,
						sprite_center.y + sprite.size.y
					};

					if (render_camera.captures(sprite_rect))
					{
						glm::ivec2 sprite_relative_position = {
							camera_info.scale.x * (sprite_rect.x - render_camera.left),
							camera_info.scale.y * (sprite_rect.y - render_camera.top)
						};

						glm::ivec2 sprite_relative_size = {
							sprite.size.x * camera_info.scale.x,
							sprite.size.y * camera_info.scale.y
						};

						Renderer2D::render_texture(sprite.texture, sprite.texture_rect, sprite_relative_position, sprite_relative_size);
						sprites_drawn++;
					}
				}
			}

			static Font& main_font = cotwin::ResourceManager::get_font("src/test/resources/fonts/Lato/Lato-Regular.ttf");
			Text sprites_drawn_text(std::to_string(sprites_drawn), main_font, { 200, 200, 200, 255 }, { 200, 0 });
			Renderer2D::render_text(sprites_drawn_text);

			// Collision System
			collisions.clear();
			
			for (auto [entity, transform, collider] : registry.view<TransformComponent, ColliderComponent>().each())
			{
				glm::vec2 collider_origin = transform.center + collider.offset;
				glm::vec4 collider_rect(
					collider_origin.x,
					collider_origin.y,
					collider.size.x,
					collider.size.y
				);

				// TODO : start from i + 1 index (so don't iterate over the same collisions twice)
				for (auto [other, other_transform, other_collider] : registry.view<TransformComponent, ColliderComponent>().each())
				{
					if (entity == other)
						continue;

					glm::vec2 other_collider_origin = other_transform.center + other_collider.offset;
					glm::vec4 other_collider_rect(
						other_collider_origin.x,
						other_collider_origin.y,
						other_collider.size.x,
						other_collider.size.y
					);

					if (physics::collide_aabb(collider_rect, other_collider_rect))
					{
						collisions.push_back(std::make_pair(Entity(entity, this), Entity(other, this)));
					}
				}
			}
		}

		void on_window_resize_event(const glm::ivec2& new_size)
		{
			auto view = registry.view<CameraComponent>();
			for (auto [entity, camera] : view.each())
			{
				// update camera scale
				camera.scale.x = (float)new_size.x / (float)camera.bounds.x;
				camera.scale.y = (float)new_size.y / (float)camera.bounds.y;
			}
		}

		// a conveniance function that returns a sub-vector of collisions of entities with two specified tags
		std::vector<std::pair<Entity, Entity>> get_collisions(std::string t1, std::string t2)
		{
			std::vector<std::pair<Entity, Entity>> collisions_of_interest;
			
			for (std::pair<Entity, Entity>& collision : collisions)
			{
				// TODO : order of tags doesn't matter , so check each tag for both t1 & t2
				if (collision.first.get_component<TagComponent>().tag == t1 && collision.second.get_component<TagComponent>().tag == t2)
					collisions_of_interest.push_back(collision);
			}

			return collisions_of_interest;
		}

		friend Entity;
	};
}