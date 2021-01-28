#pragma once

#include "../vendor/ecs/ECS.h"

#include "components.h"

#include "../util/logger.h"

#include "../graphics/render_camera_2d.h"

#include "../input/input.h"
#include "../input/keycodes.h"

// TODO : (for SpriteComponent test), make this graphics api-independent
#include "../graphics/sdl2/renderer_2d.h"


namespace cotwin
{
	// just do that for now to conceal implementation details
	typedef ECS::Entity Entity;

	template<typename T>
	using ComponentHandle = ECS::ComponentHandle<T>; // typedef for ECS::ComponentHandle

	// update sprite's position with transform position
	class TransformSpriteSystem : public ECS::EntitySystem
	{
	public:
		TransformSpriteSystem()
		{}

		virtual ~TransformSpriteSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{
			for (Entity* ent : world->each<TransformComponent>())
			{
				ent->with<TransformComponent>([&](ECS::ComponentHandle<TransformComponent> transform) {
					transform->center += transform->velocity;
				});
			}
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
			//world->each<SpriteComponent>([&](Entity* ent, ECS::ComponentHandle<SpriteComponent> sprite) {
			//	if (sprite->active)
			//	{
			//		Renderer2D::render_sprite(sprite->sprite);
			//	}
			//});

			Entity* camera_entity = 0;
			auto camera_entity_iterator = world->each<TransformComponent, CameraComponent>().begin();
			
			// check if the registry has an entity with a CameraComponent
			if (camera_entity_iterator.isEnd() != true)
			{
				camera_entity = world->getByIndex(camera_entity_iterator.getIndex());
			}

			int sprites_drawn = 0;
			
			for (Entity* ent : world->each<SpriteComponent>())
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

							RenderCamera render_camera(
								camera_transform->center.x - camera->bounds.x / 2,
								camera_transform->center.y - camera->bounds.y / 2,
								camera_transform->center.x + camera->bounds.x / 2,
								camera_transform->center.y + camera->bounds.y / 2
							);

							//Logger::Debug("%d  %d  %d  %d", render_camera.left, render_camera.top, render_camera.right, render_camera.bottom);
							
							// convert sprite->rect to rect with left, top, right, bottom
							glm::ivec4 sprite_rect = {
								sprite_transform->center.x,
								sprite_transform->center.y,
								sprite_transform->center.x + sprite->size.x,
								sprite_transform->center.y + sprite->size.y,
							};
							
							if (render_camera.captures(sprite_rect))
							{
								glm::ivec2 sprite_relative_position = {
									sprite_rect.x - render_camera.left,
									sprite_rect.y - render_camera.top
								};

								// TODO : scale sprites based on the camera bounds
								glm::ivec2 sprite_relative_size = {
									sprite->size.x,
									sprite->size.y
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

			Logger::Debug("SpriteRenderSystem: %d sprites drawn", sprites_drawn);
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
							sprite->texture_rect = animation->frames.at(animation->i);
							animation->i++;
							if (animation->i >= animation->frames.size())
								refresh(animation, sprite);
							animation->count -= animation->frequency;
						}
					}
				});
			}
		}

		void refresh(ECS::ComponentHandle<AnimationComponent> animation, ECS::ComponentHandle<SpriteComponent> sprite)
		{
			sprite->texture_rect = animation->frames.at(0);
			animation->i = 1;
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
}