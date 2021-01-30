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

	void TransformSystem(flecs::entity entity, TransformComponent* transform)
	{
		transform->center += transform->velocity;
	}

	void CameraSystem(Entity entity, TransformComponent& transform, CameraComponent& camera)
	{
		
	}

	/*class CameraSystem : public ECS::EntitySystem
	{
	private:
		bool window_size_updated = false;
		glm::ivec2 window_size;

		Entity * camera_entity = 0;
		ComponentHandle<CameraComponent> camera;
	
	public:
		CameraSystem()
		{}

		virtual ~CameraSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			if (window_size_updated)
			{
				if (camera_entity == NULL)
				{
					auto camera_entity_iterator = world->each<CameraComponent>().begin();

					// check if the registry has an entity with a CameraComponent
					if (camera_entity_iterator.isEnd() != true)
					{
						camera_entity = world->getByIndex(camera_entity_iterator.getIndex());

						camera = camera_entity->get<CameraComponent>();
					}
				}

				else
				{
					// update camera scale
					camera->scale.x = (float) window_size.x / (float) camera->bounds.x;
					camera->scale.y = (float) window_size.y / (float) camera->bounds.y;
				}

				window_size_updated = false;
			}
		}

		void on_window_resize(const glm::ivec2& new_window_size)
		{
			window_size_updated = true;
			window_size = new_window_size;
		}
	};*/

	void CameraControllerSystem(Entity entity, TransformComponent& transform, CameraComponent& camera)
	{
		
	}
	
	/*class CameraControllerSystem : public ECS::EntitySystem
	{
	private:
		Entity * camera_entity = 0;
		ComponentHandle<TransformComponent> transform;
		ComponentHandle<CameraComponent> camera;
	
	public:
		CameraControllerSystem()
		{}

		virtual ~CameraControllerSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			if (camera_entity == NULL)
			{
				auto camera_entity_iterator = world->each<TransformComponent, CameraComponent>().begin();

				// check if the registry has an entity with a CameraComponent
				if (camera_entity_iterator.isEnd() != true)
				{
					camera_entity = world->getByIndex(camera_entity_iterator.getIndex());

					transform = camera_entity->get<TransformComponent>();
					camera = camera_entity->get<CameraComponent>();
				}
			}

			else
			{
				if (Input::is_key_pressed(CW_KEY_A))
					transform->velocity.x = -120.0f * deltaTime;
				else if (Input::is_key_pressed(CW_KEY_D))
					transform->velocity.x = 120.0f * deltaTime;
				else
					transform->velocity.x = 0.0f;

				if (Input::is_key_pressed(CW_KEY_W))
					transform->velocity.y = -120.0f * deltaTime;
				else if (Input::is_key_pressed(CW_KEY_S))
					transform->velocity.y = 120.0f * deltaTime;
				else
					transform->velocity.y = 0.0f;
			}
		}
	};*/

	//flecs::iter& it,
	void SpriteRenderSystem(Entity entity, TransformComponent& transform, SpriteComponent& sprite)
	{
		int sprites_drawn = 0;
		Renderer2D::render_texture(sprite.texture, sprite.texture_rect, transform.center, sprite.size);
		sprites_drawn++;
	}
	
	/*class SpriteRenderSystem : public ECS::EntitySystem
	{
	private:
		Entity* camera_entity = 0;
		ComponentHandle<TransformComponent> camera_transform;
		ComponentHandle<CameraComponent> camera;
	
	public:
		SpriteRenderSystem()
		{}

		virtual ~SpriteRenderSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			if (camera_entity == NULL)
			{
				auto camera_entity_iterator = world->each<TransformComponent, CameraComponent>().begin();

				// check if the registry has an entity with a CameraComponent
				if (camera_entity_iterator.isEnd() != true)
				{
					camera_entity = world->getByIndex(camera_entity_iterator.getIndex());

					camera_transform = camera_entity->get<TransformComponent>();
					camera = camera_entity->get<CameraComponent>();
				}
			}

			int sprites_drawn = 0;
			
			for (Entity* ent : world->each<TransformComponent, SpriteComponent>())
			{
				ent->with<TransformComponent, SpriteComponent>([&](
					ECS::ComponentHandle<TransformComponent> sprite_transform, ECS::ComponentHandle<SpriteComponent> sprite
				) {
					if (sprite->active)
					{
						if (camera_entity)
						{
							glm::ivec2 camera_half_size = camera->bounds / 2;
							RenderCamera render_camera(
								camera_transform->center.x - camera_half_size.x,
								camera_transform->center.y - camera_half_size.y,
								camera_transform->center.x + camera_half_size.x,
								camera_transform->center.y + camera_half_size.y
							);

							//Logger::Debug("%d  %d  %d  %d", render_camera.left, render_camera.top, render_camera.right, render_camera.bottom);
							
							// convert rect with left, top, right, bottom
							glm::ivec2 sprite_center = {
								sprite_transform->center.x + sprite->center_offset.x,
								sprite_transform->center.y + sprite->center_offset.y
							};
							glm::ivec4 sprite_rect = {
								sprite_center.x,
								sprite_center.y,
								sprite_center.x + sprite->size.x,
								sprite_center.y + sprite->size.y
							};
							
							if (render_camera.captures(sprite_rect))
							{
								glm::ivec2 sprite_relative_position = {
									camera->scale.x * (sprite_rect.x - render_camera.left),
									camera->scale.y * (sprite_rect.y - render_camera.top)
								};

								glm::ivec2 sprite_relative_size = {
									sprite->size.x * camera->scale.x,
									sprite->size.y * camera->scale.y
								};
								
								Renderer2D::render_texture(sprite->texture, sprite->texture_rect, sprite_relative_position, sprite_relative_size);
								sprites_drawn++;
							}
						}
						else
						{
							Renderer2D::render_texture(sprite->texture, sprite->texture_rect, sprite_transform->center, sprite->size);
							sprites_drawn++;
						}
					}
				});
			}

			static Font& main_font = cotwin::ResourceManager::load_font("src/test/resources/fonts/Lato/Lato-Regular.ttf", 28);
			Text sprites_drawn_text(std::to_string(sprites_drawn), main_font, { 200, 200, 200, 255 }, { 200, 0 });
			Renderer2D::render_text(sprites_drawn_text);
		}
	};*/

	void AnimationSystem(Entity entity, SpriteComponent& sprite, AnimationComponent& animation)
	{
		if (sprite.active)
		{
			animation.count += entity.delta_time();

			if (animation.count >= animation.frequency)
			{
				if (animation.i >= animation.frames->size())
					animation.i = 0;

				sprite.texture_rect = animation.frames->at(animation.i);
				animation.i++;

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
	}

	void ScriptSystem(Entity entity, ScriptComponent& script)
	{
		script.script(entity, entity.delta_time());
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