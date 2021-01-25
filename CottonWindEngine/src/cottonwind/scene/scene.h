#pragma once

#include <utility>

#include "../vendor/ecs/ECS.h"

#include "../util/logger.h"


namespace cotwin
{
	// just do that for now to conceal implementation details
	typedef ECS::Entity Entity;
	
	class SpriteRenderSystem : public ECS::EntitySystem
	{
	public:
		SpriteRenderSystem()
		{}

		virtual ~SpriteRenderSystem()
		{}

		virtual void tick(ECS::World* world, float deltaTime) override
		{

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
			world->registerSystem(new SpriteRenderSystem());
		}

		~Scene()
		{}

		// TODO : add base components to entity by default
		Entity* create_entity()
		{
			ECS::Entity* entity = world->create();
			
			return entity;
		}
		
		void on_update(double delta)
		{}
	};
}