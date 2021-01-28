#pragma once

#include "../vendor/ecs/ECS.h"

#include "components.h"
#include "systems.h"

#include "../util/logger.h"


namespace cotwin
{
	class Scene
	{
	private:
		ECS::World* world;
	
	public:
		Scene()
		{
			world = ECS::World::createWorld();
			world->registerSystem(new TransformSystem());
			world->registerSystem(new CameraControllerSystem());
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