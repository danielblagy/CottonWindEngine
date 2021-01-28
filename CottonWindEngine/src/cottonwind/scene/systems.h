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
			//world->each<TransformComponent, SpriteComponent>([&](
			//	Entity* ent, ECS::ComponentHandle<TransformComponent> transform, ECS::ComponentHandle<SpriteComponent> sprite
			//) {
			//	// TODO : add center offset to sprite for transform
			//	sprite->sprite.rect.x = glm::round(transform->center.x);
			//	sprite->sprite.rect.y = glm::round(transform->center.y);
			//});

			for (Entity* ent : world->each<TransformComponent, SpriteComponent>())
			{
				ent->with<TransformComponent, SpriteComponent>([&](ECS::ComponentHandle<TransformComponent> transform, ECS::ComponentHandle<SpriteComponent> sprite) {
					// TODO : add center offset to sprite for transform
					sprite->rect.x = glm::round(transform->center.x);
					sprite->rect.y = glm::round(transform->center.y);
				});
			}

			/*for (Entity* ent : world->each<Position>())
			{
			ent->with<Position>([&](ComponentHandle<Position> position) {
			position->y += gravityAmount * deltaTime;
			});
			}*/
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

				// TODO : change this to velocity, once TransfromSystem implements it
				if (Input::is_key_pressed(CW_KEY_A))
					transform->center.x -= 120.0f * deltaTime;
				else if (Input::is_key_pressed(CW_KEY_D))
					transform->center.x += 120.0f * deltaTime;
				// else transform->velocity.x = 0.0f;

				if (Input::is_key_pressed(CW_KEY_W))
					transform->center.y -= 120.0f * deltaTime;
				else if (Input::is_key_pressed(CW_KEY_S))
					transform->center.y += 120.0f * deltaTime;
				// else transform->velocity.y = 0.0f;
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
				ent->with<SpriteComponent>([&](ECS::ComponentHandle<SpriteComponent> sprite) {
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
							
							// convert sprite->rect to rect with left, top, right, bottom
							glm::ivec4 sprite_rect = {
								sprite->rect[0],
								sprite->rect[1],
								sprite->rect[0] + sprite->rect[2],
								sprite->rect[1] + sprite->rect[3]
							};
							
							if (render_camera.captures(sprite_rect))
							{
								int camera_offset_x = sprite->rect.x + sprite->rect[2] / 2 - camera_transform->center.x;
								int camera_offset_y = sprite->rect.y + sprite->rect[3] / 2 - camera_transform->center.y;

								glm::ivec4 relative_rect = {
									sprite->rect.x + camera_offset_x,
									sprite->rect.y + camera_offset_y,
									sprite->rect[2],
									sprite->rect[3]
								};
								
								Renderer2D::render_texture(sprite->texture, sprite->texture_rect, relative_rect);
								sprites_drawn++;
							}
						}
						else
						{
							Renderer2D::render_texture(sprite->texture, sprite->texture_rect, sprite->rect);
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