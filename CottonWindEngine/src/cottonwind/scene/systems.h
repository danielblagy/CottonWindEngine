#pragma once

#include "../vendor/ecs/ECS.h"

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
	typedef ECS::Entity Entity;

	template<typename T>
	using ComponentHandle = ECS::ComponentHandle<T>; // typedef for ECS::ComponentHandle

	class TransformSystem : public ECS::EntitySystem
	{
	public:
		TransformSystem()
		{}

		virtual ~TransformSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			for (Entity* ent : world->each<TransformComponent>())
			{
				ent->with<TransformComponent>([&](ECS::ComponentHandle<TransformComponent> transform) {
					// update transform's position with velocity
					transform->center += transform->velocity;
				});
			}
		}
	};

	class CameraSystem : public ECS::EntitySystem
	{
	private:
		bool window_size_updated = false;
		glm::ivec2 window_size;
	
	public:
		CameraSystem()
		{}

		virtual ~CameraSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			if (window_size_updated)
			{
				Entity* camera_entity = 0;
				auto camera_entity_iterator = world->each<TransformComponent, CameraComponent>().begin();

				// check if the registry has an entity with a CameraComponent
				if (camera_entity_iterator.isEnd() != true)
				{
					camera_entity = world->getByIndex(camera_entity_iterator.getIndex());
				}

				if (camera_entity)
				{
					ComponentHandle<CameraComponent> camera = camera_entity->get<CameraComponent>();
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
	};

	class CameraControllerSystem : public ECS::EntitySystem
	{
	public:
		CameraControllerSystem()
		{}

		virtual ~CameraControllerSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			Entity* camera_entity = 0;
			auto camera_entity_iterator = world->each<TransformComponent, CameraComponent>().begin();

			// check if the registry has an entity with a CameraComponent
			if (camera_entity_iterator.isEnd() != true)
			{
				camera_entity = world->getByIndex(camera_entity_iterator.getIndex());
			}

			if (camera_entity)
			{
				ComponentHandle<TransformComponent> transform = camera_entity->get<TransformComponent>();

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
	};

	class SpriteRenderSystem : public ECS::EntitySystem
	{
	public:
		SpriteRenderSystem()
		{}

		virtual ~SpriteRenderSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			Entity* camera_entity = 0;
			auto camera_entity_iterator = world->each<TransformComponent, CameraComponent>().begin();
			
			// check if the registry has an entity with a CameraComponent
			if (camera_entity_iterator.isEnd() != true)
			{
				camera_entity = world->getByIndex(camera_entity_iterator.getIndex());
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
							ComponentHandle<TransformComponent> camera_transform = camera_entity->get<TransformComponent>();
							ComponentHandle<CameraComponent> camera = camera_entity->get<CameraComponent>();

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
	};

	class AnimationSystem : public ECS::EntitySystem
	{
	public:
		AnimationSystem()
		{}

		virtual ~AnimationSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			for (Entity* ent : world->each<SpriteComponent, AnimationComponent>())
			{
				// TODO : maybe get entities with AnimationComponent, and then check if it has SpriteComponent,
				//			and if not, log error message
				ent->with<SpriteComponent, AnimationComponent>([&](
					ECS::ComponentHandle<SpriteComponent> sprite, ECS::ComponentHandle<AnimationComponent> animation
					) {
					if (sprite->active)
					{
						animation->count += deltaTime;

						if (animation->count >= animation->frequency)
						{
							if (animation->i >= animation->frames->size())
								animation->i = 0;
							
							sprite->texture_rect = animation->frames->at(animation->i);
							animation->i++;
							
							animation->count -= animation->frequency;
						}
					}
				});
			}
		}
	};

	class AudioSystem : public ECS::EntitySystem
	{
	public:
		AudioSystem()
		{}

		virtual ~AudioSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			for (Entity* ent : world->each<AudioEffectComponent>())
			{
				ent->with<AudioEffectComponent>([&](ECS::ComponentHandle<AudioEffectComponent> audio_effect) {
					if (audio_effect->play)
					{
						audio_effect->audio.play();
						audio_effect->play = false;
					}
				});
			}
		}
	};

	class MovementControlSystem : public ECS::EntitySystem
	{
	public:
		MovementControlSystem()
		{}

		virtual ~MovementControlSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			for (Entity* ent : world->each<TransformComponent, MovementControlComponent>())
			{
				ent->with<TransformComponent, MovementControlComponent>([&](
					ECS::ComponentHandle<TransformComponent> transform, ECS::ComponentHandle<MovementControlComponent> movement_control) {
					movement_control->controller(transform->velocity, deltaTime);
				});
			}
		}
	};

	class ScriptSystem : public ECS::EntitySystem
	{
	public:
		ScriptSystem()
		{}

		virtual ~ScriptSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			for (Entity* ent : world->each<ScriptComponent>())
			{
				ent->with<ScriptComponent>([&](ECS::ComponentHandle<ScriptComponent> script) {
					script->script(ent, deltaTime);
				});
			}
		}
	};

	// TODO : rename this to collision system
	class ColliderSystem : public ECS::EntitySystem
	{
	public:
		std::vector<std::pair<Entity*, Entity*>> collisions;
		
		ColliderSystem()
		{}

		virtual ~ColliderSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			collisions.clear();
			
			for (Entity* ent : world->each<ColliderComponent>())
			{
				if (ent->has<TagComponent>() && ent->has<TransformComponent>())
				{
					ComponentHandle<TransformComponent> transform = ent->get<TransformComponent>();
					ComponentHandle<ColliderComponent> collider = ent->get<ColliderComponent>();

					glm::vec2 collider_origin = transform->center + collider->offset;
					glm::vec4 collider_rect(
						collider_origin.x,
						collider_origin.y,
						collider->size.x,
						collider->size.y
					);

					for (Entity* other : world->each<ColliderComponent>())
					{
						if (ent != other && other->has<TagComponent>() && other->has<TransformComponent>())
						{
							ComponentHandle<TransformComponent> other_transform = other->get<TransformComponent>();
							ComponentHandle<ColliderComponent> other_collider = other->get<ColliderComponent>();

							glm::vec2 other_collider_origin = other_transform->center + other_collider->offset;
							glm::vec4 other_collider_rect(
								other_collider_origin.x,
								other_collider_origin.y,
								other_collider->size.x,
								other_collider->size.y
							);

							if (physics::collide_aabb(collider_rect, other_collider_rect))
							{
								collisions.push_back(std::make_pair(ent, other));
							}
						}
					}
				}
			}
		}
	};
}