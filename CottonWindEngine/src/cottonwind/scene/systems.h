#pragma once

#include "../vendor/ecs/ECS.h"

#include "components.h"

#include "../util/logger.h"

#include "../graphics/render_camera_2d.h"

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

			for (Entity* ent : world->each<SpriteComponent>())
			{
				ent->with<SpriteComponent>([&](ECS::ComponentHandle<SpriteComponent> sprite) {
					if (sprite->active)
					{
						if (camera_entity)
						{
							ComponentHandle<TransformComponent> transform = camera_entity->get<TransformComponent>();
							ComponentHandle<CameraComponent> camera = camera_entity->get<CameraComponent>();

							RenderCamera render_camera(
								transform->center.x - camera->bounds.x / 2,
								transform->center.y - camera->bounds.y / 2,
								transform->center.x + camera->bounds.x / 2,
								transform->center.y + camera->bounds.y / 2
							);
							
							if (
								render_camera.captures(
									sprite->rect[0],
									sprite->rect[1],
									sprite->rect[0] + sprite->rect[2],
									sprite->rect[1] + sprite->rect[3]
									)
								)
							{
								Renderer2D::render_texture(sprite->texture, sprite->texture_rect, sprite->rect);
							}
						}
						else
						{
							Renderer2D::render_texture(sprite->texture, sprite->texture_rect, sprite->rect);
						}
					}
				});
			}
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