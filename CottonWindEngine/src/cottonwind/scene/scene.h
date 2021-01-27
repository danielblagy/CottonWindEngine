#pragma once

#include "../vendor/ecs/ECS.h"

#include "components.h"

#include "../util/logger.h"

// TODO : remove this (for SpriteComponent test)
#include "../graphics/sdl2/renderer_2d.h"


namespace cotwin
{
	// just do that for now to conceal implementation details
	typedef ECS::Entity Entity;
	
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

			for (Entity* ent : world->each<SpriteComponent>())
			{
				ent->with<SpriteComponent>([&](ECS::ComponentHandle<SpriteComponent> sprite) {
					if (sprite->active)
					{
						Renderer2D::render_texture(sprite->texture, sprite->texture_rect, sprite->rect);
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
	
	class Scene
	{
	private:
		ECS::World* world;
	
	public:
		Scene()
		{
			world = ECS::World::createWorld();
			world->registerSystem(new TransformSpriteSystem());
			world->registerSystem(new AnimationSystem());
			world->registerSystem(new SpriteRenderSystem());
			world->registerSystem(new AudioSystem());
		}

		~Scene()
		{
			world->destroyWorld();
		}

		Entity* create_entity(std::string tag)
		{
			ECS::Entity* entity = world->create();

			entity->assign<TagComponent>(tag);
			
			return entity;
		}
		
		void on_update(float delta)
		{
			world->tick(delta);
		}
	};
}