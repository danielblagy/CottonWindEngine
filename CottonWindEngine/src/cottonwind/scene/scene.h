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
		CameraSystem* camera_system;
	
	public:
		Scene()
		{
			world = ECS::World::createWorld();
			world->registerSystem(new TransformSystem());
			camera_system = new CameraSystem();
			world->registerSystem(camera_system);
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

		void on_window_resize_event(const glm::ivec2& new_size)
		{
			camera_system->on_window_resize(new_size);
		}
	};
}