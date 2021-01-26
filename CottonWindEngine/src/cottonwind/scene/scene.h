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
					sprite->sprite.rect.x = glm::round(transform->center.x);
					sprite->sprite.rect.y = glm::round(transform->center.y);
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
						Renderer2D::render_sprite(sprite->sprite);
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
			world->registerSystem(new SpriteRenderSystem());
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